# MMO Lite 2007
# Master Server - handle user registration, authentication

#register "current working folder as system path"
#there is probably a better way of doing this (change from Python 2.4->2.5)
import sys, os
sys.path.append(os.getcwd())

from twisted.spread import pb
from twisted.cred import checkers, credentials, portal, error as credError
from twisted.internet import reactor, defer
from twisted.python import failure, log, logfile
from zope.interface import Interface, implements

from mmolite.common.define import *
from mmolite.common.exceptions import *
from mmolite.master.avatars import *
from mmolite.master.db_serv import Accounts, Peers, PeersInfo, MasterDBObject
from mmolite.common.util import getOptions
from mmolite.common.db_serv import Clients

class MasterPasswordChecker(object):
    implements(checkers.ICredentialsChecker)
    credentialInterfaces = (credentials.IUsernamePassword, 
                            credentials.IUsernameHashedPassword)
                            
    def __init__(self):
        return
        
    def requestAvatarId(self, credentials):
        type = ObjectType.Peer
        name = credentials.username
        
        row = PeersInfo.selectBy(peername = name)
        if row.count() == 0: 
            type = ObjectType.User
            row = Accounts.selectBy(username = name)  
            if row.count() == 0:
                print "Login Failed: username[%s]"%(name)
                return failure.Failure(credError.UnauthorizedLogin("Bad username"))
        peer = row[0]
        return defer.maybeDeferred(credentials.checkPassword, peer.password).addCallback(
            self._checkedPassword, name, peer.role, type)
            
    def _checkedPassword(self, matched, name, role, type):
        if matched:
            return (name, role, type)
        else:
            print "Login Failed: username[%s] role[%s] peer[%s]"%(name, role, type==ObjectType.Peer)
            return failure.Failure(credError.UnauthorizedLogin("Bad password"))
 
class MasterRealm(object):
    implements(portal.IRealm)
    
    def __init__(self):
        self.userList = {}
        self.peerList = {}
        return
        
    def requestAvatar(self, avatarId, mind, *interfaces):
        if not pb.IPerspective in interfaces:
            raise KeyError("No supported avatar Interface")
            return
            
        name, role, type = avatarId[0], avatarId[1], avatarId[2]

        if type == ObjectType.Peer:
            if role & PeerType.Master == PeerType.Master:   # e.g. secondary master
                avatar = MasterPerspective(mind, self)
            elif role & PeerType.Character == PeerType.Character:
                avatar = CharacterPerspective(mind, self)
            elif role & PeerType.World == PeerType.World:
                avatar = WorldPerspective(mind, self)
            elif role & PeerType.Cluster == PeerType.Cluster:
                avatar = ClusterPerspective(mind, self)
            elif role & PeerType.Zone == PeerType.Zone:
                avatar = ZonePerspective(mind, self)
            else: # PeerType.Client, dun need store into peerList ?
                avatar = ClientPerspective(mind, self)
            self.peerList[avatar.uniqId] = avatar
        else: # user
            if role & UserType.Admin == UserType.Admin:
                avatar = AdminPerspective(name, mind, self)
            elif role & UserType.GameMaster == UserType.GameMaster:
                avatar = GameMasterPerspective(name, mind, self)
            elif role & UserType.PremiumMember == UserType.PremiumMember:
                avatar = PremiumMemberPerspective(name, mind, self)
            else: # role & UserType.Member == UserType.Member:
                avatar = MemberPerspective(name, mind, self)
            self.userList[avatar.uniqId] = avatar
        print "User Login: username[%s] role[%s] uniq[%d] peer[%s]"%(name, role, avatar.uniqId, type==ObjectType.Peer)
            
        return pb.IPerspective, avatar, avatar.logout
            
    # keep alive + load balancing
    def updateLoads(self):
        for key in self.peerList:
            self.peerList[key].updateLoad()
            
    # TODO: proper peer matching, currently always switch to peer 0
    def changeServer(self, worldId, id, name, authenticID, mode):
        if worldId not in self.peerList : raise ServerNotFoundError("Server not found")
        peer = self.peerList[worldId]
        peer.newClient(id, name, authenticID, mode)
        return peer.getInfo()
            
    def logout(self, avatar):
        key = avatar.uniqId
        if key in self.userList:
            del self.userList[key]
        elif key in self.peerList:
            del self.peerList[key]

def keepAlive(realm):
    reactor.callLater(300, keepAlive, realm) # call every 5 mins
    realm.updateLoads()

def main():
    OPTIONS, argv = getOptions('mmolite/config/servers.cfg', 'master server', sys.argv)
    
    dbconn = MasterDBObject('master.db', 'mmolite/data')
    
    if OPTIONS.setup: 
        print "Setting up Master Server..."
        
        dbconn.resetDB()
        user = PeersInfo(peername = "Master", password = "retsam", role = PeerType.Master)
        user = PeersInfo(peername = "Character", password = "retcarahc", role = PeerType.Character)
        user = PeersInfo(peername = "World", password = "dlrow", role = PeerType.World)
        user = PeersInfo(peername = "Cluster", password = "retsulc", role = PeerType.Cluster)
        user = PeersInfo(peername = "Zone", password = "enoz", role = PeerType.Zone)
        user = PeersInfo(peername = "Client", password = "tneilc", role = PeerType.Client)
        user = Accounts(username = "darren", password = "nerrad", role = UserType.Admin)
        print "Successfully"
        return
    else: 
        print "Initialize Master Server database..."
        dbconn.startDB()
        Peers.dropTable(ifExists=True); Peers.createTable()
        print "Master Server Listening to port: %d"%OPTIONS.master_port
        
        realm = MasterRealm()
        p = portal.Portal(realm)
        p.registerChecker(MasterPasswordChecker())
        reactor.listenTCP(OPTIONS.master_port, pb.PBServerFactory(p))

        keepAlive(realm)
        reactor.run()

if __name__ == '__main__':
    main()