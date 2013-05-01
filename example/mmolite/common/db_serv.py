# MMO Lite 2007
# World Server's Database Services

import os
from datetime import datetime
from sqlobject import *

class Persistent(SQLObject):
    _connection = None

    # String Length Constant
    PasswordLen = 16
    UniqueNameLen = 32
    DescLen = 64
    IPLen = 16

# only show online client info,
class Clients(Persistent):
    # status code
    Hopping = 0
    Online = 1
    
    # client mode
    Grunt = 1       # normal player
    Noble = 2       # high class client
    Invincible = 4  # super strength + super armor
    God = 8         # accessable to admin commands
    Stealth = 16    # invisible
    
    accountID = IntCol(alternateID = True) # no one-to-multiple, to make it standalone
    username = StringCol(alternateID = True, length=Persistent.UniqueNameLen)
    authenticID = StringCol(length=Persistent.PasswordLen) # temp password for server hopping
    location = StringCol(length=Persistent.UniqueNameLen) # server name
    status = IntCol()
    mode = IntCol()
    expiryTime = DateTimeCol(default = datetime.now) # prevent direct connection, bypassing master server

class RaceStats(Persistent):
    race = IntCol(alternateID = True)
    initStr = IntCol()
    initDex = IntCol()
    initCon = IntCol()
    initInt = IntCol()
    maxStr = IntCol()
    maxDex = IntCol()
    maxCon = IntCol()
    maxInt = IntCol()

class FractionModifier(Persistent):
    race = IntCol()
    fraction = IntCol()
    weapon = IntCol()
    upperArmor = IntCol()
    lowerArmor = IntCol()
    initStr = IntCol()
    initDex = IntCol()
    initCon = IntCol()
    initInt = IntCol()
    maxStr = IntCol()
    maxDex = IntCol()
    maxCon = IntCol()
    maxInt = IntCol()

class GenderModifier(Persistent):
    race = IntCol()
    gender = IntCol()
    face = IntCol()
    initStr = IntCol()
    initDex = IntCol()
    initCon = IntCol()
    initInt = IntCol()
    maxStr = IntCol()
    maxDex = IntCol()
    maxCon = IntCol()
    maxInt = IntCol()
    
# peer info, for keepalive, redirection. only show online server
class Peers(Persistent):
    name = StringCol(alternateID = True, length=Persistent.UniqueNameLen)
    # TODO: other fields like primary/secondary, test/production
    ipAddress = StringCol(length=Persistent.IPLen)
    port = IntCol()
    load = IntCol() # for keep alive as well
    role = IntCol() 
    uniqId = IntCol()
    lastActiveTime = DateTimeCol(default = datetime.now)
    
# Persistent database for game character info
class Characters(Persistent):
    name = StringCol(alternateID = True, length=Persistent.UniqueNameLen)
    race = IntCol()
    Fraction = IntCol()
    gender = IntCol()
    ownername = StringCol(length=Persistent.UniqueNameLen)
    exp = IntCol()
    world = StringCol(length=Persistent.UniqueNameLen)
    zone = StringCol(length=Persistent.UniqueNameLen)
    posX = FloatCol()
    posY = FloatCol()
    posZ = FloatCol()
    rotZ = FloatCol()
    face = IntCol()
    #helm = IntCol()
    weapon = IntCol()
    upperArmor = IntCol()
    lowerArmor = IntCol()
    strength = IntCol()
    dexterity = IntCol()
    constitution = IntCol()
    intelligence = IntCol()
    inUse = BoolCol()
    modifyDate = DateTimeCol(default = datetime.now)
    creationDate = DateTimeCol(default = datetime.now)
    
class DBObject(object):
    uri = None
    hub = dbconnection.ConnectionHub()
    hub.processConnection = None
    
    def __init__(self, dbname, dbpath):
        if not DBObject.uri:
            uri = self._generateURI(dbname, dbpath)

    def _generateURI(self, dbname=None, dbpath=None):
        if not os.path.exists(dbpath): os.makedirs(dbpath)
        DBObject.uri = 'sqlite:/%s/%s'%(dbpath,dbname)

        return DBObject.uri
        
    def _connection(self, finalize=False):
        if finalize:
            return dbconnection.connectionForURI(DBObject.uri)
            
        DBObject.hub.processConnection = dbconnection.connectionForURI(DBObject.uri)
        return DBObject.hub
        
    def startDB(self):
        Persistent._connection = self._connection(True)
