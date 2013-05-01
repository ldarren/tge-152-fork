# MMO Lite 2007
# Master Server's Database Services

import os
from datetime import datetime
from sqlobject import *

from mmolite.common.define import *
from mmolite.common.db_serv import Persistent, Clients, Peers, DBObject
    
# Persistent database for real user's login info
class Accounts(Persistent):
    username = StringCol(alternateID = True, length=Persistent.UniqueNameLen)
    password = StringCol(length=Persistent.PasswordLen)
    #registrationKey = StringCol(alternateID = True, length=20)
    role = IntCol()
    #email = StringCol(alternateID = True, length=45)
    creationDate = DateTimeCol(default = datetime.now)
    modifiedDate = DateTimeCol(default = datetime.now)
    loginDate = DateTimeCol(default = datetime.now)
    logoutDate = DateTimeCol(default = datetime.now)

# peers login info
class PeersInfo(Persistent):
    peername = StringCol(alternateID = True, length=Persistent.UniqueNameLen)
    password = StringCol(length=Persistent.PasswordLen)
    role = IntCol()
    
class MasterDBObject(DBObject):
    def __init__(self, dbname, dbpath):
        DBObject.__init__(self, dbname, dbpath)

    def resetDB(self):
        self.startDB() #Persistent._connection = db.connection()
        TABLES = [Accounts, PeersInfo, Peers]

        #for now we'll drop and recreate the tables every time
        for t in TABLES:
            t.dropTable(ifExists=True)
            t.createTable()
        

