# MMO Lite 2007
# World Server's Database Services

import os
from datetime import datetime
from sqlobject import *

from mmolite.common.db_serv import Persistent, Clients, Peers, Characters, DBObject
    
class WorldDBObject(DBObject):
    def __init__(self, dbname, dbpath):
        DBObject.__init__(self, dbname, dbpath)

    def resetDB(self):
        self.startDB() #Persistent._connection = db.connection()
        TABLES = [Clients, Peers]

        #for now we'll drop and recreate the tables every time
        for t in TABLES:
            t.dropTable(ifExists=True)
            t.createTable()
        

