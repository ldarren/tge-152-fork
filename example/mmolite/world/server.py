# MMO Lite 2007
# World Server

#register "current working folder as system path"
#there is probably a better way of doing this (change from Python 2.4->2.5)
import sys, os
sys.path.append(os.getcwd())

from datetime import datetime, timedelta

from twisted.spread import pb
from twisted.cred import checkers, credentials, portal, error as credError
from twisted.internet import reactor, defer
from twisted.python import log, logfile
from zope.interface import Interface, implements

from mmolite.common.define import *
from mmolite.common.exceptions import *
from mmolite.common.util import getOptions
from mmolite.common.db_serv import Clients
from mmolite.world.avatars import *
from mmolite.world.db_serv import WorldDBObject
from mmolite import pytorque
from mmolite.pytorque import TorqueObject

class WorldPasswordChecker(object):
    implements(checkers.ICredentialsChecker)
    credentialInterfaces = (credentials.IUsernamePassword, 
                            credentials.IUsernameHashedPassword)
                            
    def __init__(self):
        return
        
    def requestAvatarId(self, credentials):
        name = credentials.username
        
        try:
            row = Clients.byUsername(name)
        except:
            print "Login Failed: username[%s]"%(name)
            return failure.Failure(credError.UnauthorizedLogin("Bad username"))
        
        print "Received client: username[%s] password[%s]"%(name, row.authenticID)
        return defer.maybeDeferred(credentials.checkPassword, row.authenticID).addCallback(self._checkedPassword, name)
            
    def _checkedPassword(self, matched, name):
        if matched: return name
        else: raise credError.UnauthorizedLogin("Bad password")
            
class WorldRealm(object):
    implements(portal.IRealm)
    
    def __init__(self):
        return
        
    def requestAvatar(self, avatarId, mind, *interfaces):
        if not pb.IPerspective in interfaces:
            raise NotImplementedError, "No supported avatar Interface."
        else:
            avatar = ZonePerspective(avatarId, mind, self)
                
            return pb.IPerspective, avatar, avatar.logout
            
    def logout(self, avatar):
        return

class WorldPortal(object):
    def __init__(self):
        self.realm = WorldRealm()
        
    def run(self, port):
        print "World Server Listening to port: %d"%port
        
        p = portal.Portal(self.realm)
        p.registerChecker(WorldPasswordChecker())
        reactor.listenTCP(port, pb.PBServerFactory(p))
        
class WorldServer(pb.Referenceable):
    Name = "Primary World"
    def __init__(self, ip, port, listenPort, type, credentials, portal):
        self.credentials = credentials
        self.serverIP = ip      # master server ip
        self.serverPort = port  # master server port
        self.serverType = type
        self.listenPort = listenPort
        self.server = None      # master server
        self.portal = portal    # received connection from client
        
    def run(self):
        self.connect().addCallback(self._register).addErrback(self._catchFailure)
        
    def connect(self):
        factory = pb.PBClientFactory()
        reactor.connectTCP(self.serverIP, self.serverPort, factory)
        return factory.login(self.credentials, client=self).addCallback(self._connected)

    def _register(self, result):
        self.server.callRemote("register", WorldServer.Name, self.listenPort, self.remote_getLoad(), self.serverType).addCallback(self._startPortal)
    
    def _startPortal(self, message):
        print "World portal running"
        self.portal.run(self.listenPort)
        
    def remote_getLoad(self):
        rows = Clients.selectBy(location = WorldServer.Name)
        return rows.count()
        
    def remote_newClient(self, accountID, username, authenticID, mode):
        
        client = Clients(accountID=accountID, 
            username=username, 
            authenticID=authenticID, 
            location=WorldServer.Name, 
            status=Clients.Hopping,
            mode=mode,
            expiryTime=datetime.now()+timedelta(minutes=5))
        
    def _connected(self, rootObj):
        self.server = rootObj

    def _catchFailure(self, failure):
        print "Error:", failure.getErrorMessage()
        reactor.stop()

def reactorTick():
    reactor.runUntilCurrent()
    reactor.doIteration(0)
        
def main():
    OPTIONS, argv = getOptions('mmolite/config/servers.cfg', 'world server', sys.argv)
    
    if OPTIONS.username=="-": OPTIONS.username = raw_input("Username: ")
    if OPTIONS.password=="-": OPTIONS.password = raw_input("Password: ")

    dbconn = WorldDBObject('world.db', 'mmolite/data')
    
    if OPTIONS.setup: 
        print "Setting up World Server..."
        
        dbconn.resetDB()
        print "Successfully"
        return
    else: 
        print "Initialize World Server database..."
        dbconn.startDB()
        Clients.dropTable(ifExists=True); Clients.createTable()
        
        print "Run TGE dedicated server"
        argv.extend(['-dedicated', '-game', 'test.mmo', '-mission', 'test.mmo/data/missions/chatspot.mis'])
        pytorque.initialize(len(argv),argv)

        print "World server connecting to master server: ip[%s] port[%d]"%(OPTIONS.master_ip, OPTIONS.master_port)
        
        creds = credentials.UsernamePassword(OPTIONS.username, OPTIONS.password)
        world = WorldServer(OPTIONS.master_ip, OPTIONS.master_port, OPTIONS.world_port, PeerType.World, creds, WorldPortal())
        world.run()
        #reactor.run()
        reactor.startRunning()

        #the main loop is broken out and can be combined with other frameworks rather easily
        while pytorque.tick():
            reactorTick()

        #cleanup pytorque.. goodbye!
        pytorque.shutdown()

if __name__ == '__main__':
    main()