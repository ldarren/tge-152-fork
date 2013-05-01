# MMO Lite 2007
# Character Server's Database Services

import os
from datetime import datetime
from sqlobject import *

from mmolite.common.db_serv import Persistent, RaceStats, FractionModifier, GenderModifier, Characters, DBObject
    
class CharacterDBObject(DBObject):
    def __init__(self, dbname, dbpath):
        DBObject.__init__(self, dbname, dbpath)

    def resetDB(self):
        self.startDB() #Persistent._connection = db.connection()
        TABLES = [Characters,RaceStats,FractionModifier,GenderModifier]

        #for now we'll drop and recreate the tables every time
        for t in TABLES:
            t.dropTable(ifExists=True)
            t.createTable()
        

