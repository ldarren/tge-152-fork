# MMO Lite 2007
# World's Avatars or server process classes

from datetime import datetime

from twisted.spread import pb
from twisted.cred import credentials, error as credError

#from mmolite.world.db_serv import 
from mmolite.common.db_serv import Clients

class ZonePerspective(pb.Avatar):
    def __init__(self, name, mind, realm):
        self.name = name
        self.mind = mind
        self.realm = realm
        if self.name:
            row = Clients.byUsername(self.name)
            row.status = Clients.Online
        return
        
    def perspective_getZonePort(self):
        return 47000
    
    def logout(self):
        if self.name:
            row = Clients.byUsername(self.name)
            Clients.delete(row.id)
            self.name = None
        self.realm.logout(self)
    
    def perspective_quit(self):
        self.logout()
        return True

class EchoPerspective(pb.Avatar):
    def __init__(self, name, mind, realm):
        self.name = name
        self.mind = mind
        self.realm = realm
        if self.name:
            row = Clients.byUsername(self.name)
            row.status = Clients.Online
        return
    
    def perspective_echo(self, message):
        return self.name +":"+ message
    
    def logout(self):
        if self.name:
            row = Clients.byUsername(self.name)
            Clients.delete(row.id)
            self.name = None
        self.realm.logout(self)
    
    def perspective_quit(self):
        self.logout()
        return True
  