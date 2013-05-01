# MMO Lite 2007
# Master's Avatars or server process classes

from datetime import datetime

from twisted.spread import pb
from twisted.cred import credentials, error as credError

from mmolite.common.define import *
from mmolite.common.exceptions import *
from mmolite.common.db_serv import Clients, Peers
from mmolite.master.db_serv import Accounts

class ClientPerspective(pb.Avatar):
    def __init__(self, mind, realm):
        self.uniqId = id(self)
        self.mind = mind
        self.realm = realm
        self.name = ""
        
    def perspective_verifyUsername(self, username):
        query = Accounts.selectBy(username=username)
        if query.count() > 0: return False
        else: return True
    
    # register new user, not register itself to Peer db
    def perspective_register(self, username, password, role):
        role = role & UserType.GameMaster #  no registration for admin
        if self.perspective_verifyUsername(username):
            newUser = Accounts(username = username, password = password, role = role)
            return True
        return False
        
    def perspective_getWorldList(self):
        worldList = {}
        rows = Peers.selectBy(role = PeerType.World)
        for row in rows:
            worldList[row.uniqId] = row.name
        return worldList
        
    # for future use?
    def updateLoad(self):
        pass
        
    def logout(self):
        self.realm.logout(self)

class PeerPerspective(pb.Avatar):
    def __init__(self, mind, realm):
        self.uniqId = id(self)
        self.mind = mind
        self.realm = realm
        self.peerDB = None
        self.name = ""
    
    def perspective_register(self, name, port, load, role):
        self.name = name
        peer = Peers.selectBy(name = name)
        if peer.count() != 0: raise PeerRegistrationError("Peer Existed")
        info = self.mind.broker.transport.getPeer()
        if not info: raise PeerRegistrationError("Bad peer information")
        self.peerDB = Peers(name=name, ipAddress=info.host, port=port, load=load, role=role, uniqId=self.uniqId)
        return True
    
    def updateLoad(self):
        print "[%s] updating load..."%(self.name)
        self.mind.callRemote("getLoad").addCallback(self._updateLoad).addErrback(self._remoteCallFailed, "updateLoad")
        
    def _updateLoad(self, load):
        self.peerDB.load = load
        print "[%s] load[%d]"%(self.name, load)

    def _remoteCallFailed(self, msg, func):
        print "[%s] func[%s] %s"%(self.name, func, msg)
        self.logout()
        self.disconnect()
    
    def disconnect(self):
        self.mind.broker.transport.loseConnection()
    
    def logout(self):
        if self.peerDB:
            Peers.delete(self.peerDB.id)
            self.peerDB = None
        self.realm.logout(self)
    
class CharacterPerspective(PeerPerspective):
    def __init__(self, mind, realm):
        PeerPerspective.__init__(self, mind, realm)
    
    def perspective_quit(self):
        self.logout()
        return True

class WorldPerspective(PeerPerspective):
    def __init__(self, mind, realm):
        PeerPerspective.__init__(self, mind, realm)
    
    # new client hopping
    def getInfo(self):
        return self.peerDB.ipAddress, self.peerDB.port
        
    def newClient(self, id, name, authenticID, mode):
        self.mind.callRemote("newClient", id, name, authenticID, mode).addErrback(self._remoteCallFailed, "newClient")
        
    def newPeer(self, id, name, authenticID, mode):
        self.mind.callRemote("newPeer", id, name, authenticID, mode).addErrback(self._remoteCallFailed, "newPeer")
    
    def perspective_quit(self):
        self.logout()
        return True
    
class MemberPerspective(pb.Avatar):
    def __init__(self, name, mind, realm):
        # must explicitly call __init__() and pass "self" to initialized parent, if not it becomes create a new instance
        self.mind = mind
        self.realm = realm
        self.name = name
        self.uniqId = id(self)

        result = Accounts.selectBy(username=self.name)
        if (result.count() != 1):
            raise UserGetInfoError("Bad username")
        else:
            self.user = result[0]
            self.user.loginDate = datetime.now()
        
    def perspective_changeRole(self, newRole):
        if newRole == 0: return False # only admin can remove account
        newRole = newRole & UserType.GameMaster #  no registration for admin
        self.user.role = newRole
        self.user.modifiedDate = datetime.now()
    
    def perspective_changeServer(self, zone):
        return self._changeServer(zone, Clients.Grunt)
        
    def _changeServer(self, zone, mode):
        authenticID = self.user.password+"zone"+str(zone)
        ip, port = self.realm.changeServer(zone, self.user.id, self.name, authenticID, mode)
        print "_changeServer: ip[%s] port[%d] id[%s]"%(ip, port, authenticID)
        return ip, port, authenticID
        
    def logout(self):
        self.realm.logout(self)
        self.user.logoutDate = datetime.now()
    
    def perspective_quit(self):
        self.logout()
        return True
   
class PremiumMemberPerspective(MemberPerspective):
    def __init__(self, name, mind, realm):
        MemberPerspective.__init__(self, name, mind, realm)
        
    def perspective_payment(self):
        return
    
    def perspective_changeServer(self, zone):
        return self._changeServer(zone, Clients.Noble)
    
class GameMasterPerspective(PremiumMemberPerspective):
    def __init__(self, name, mind, realm):
        PremiumMemberPerspective.__init__(self, name, mind, realm)
        self.mode = Clients.Noble+Clients.God+Clients.Invincible
        
    def perspective_chooseMode(self, mode):
        self.mode = mode
        return True
    
    def perspective_changeServer(self, zone):
        return self._changeServer(zone, self.mode)
    
class AdminPerspective(GameMasterPerspective):
    def __init__(self, name, mind, realm):
        GameMasterPerspective.__init__(self, name, mind, realm)
        
    def perspective_updateDB(self):
        return
    
    def perspective_changeServer(self, zone):
        return self._changeServer(zone, Clients.Noble+Clients.God+Clients.Invincible+Clients.Stealth)
