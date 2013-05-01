# MMO Lite 2007
# Character Server

#register "current working folder as system path"
#there is probably a better way of doing this (change from Python 2.4->2.5)
import sys, os
sys.path.append(os.getcwd())

from datetime import datetime, timedelta

from twisted.spread import pb
from twisted.cred import checkers, credentials, portal, error as credError
from twisted.internet import reactor, defer, protocol
from twisted.python import log, logfile
from zope.interface import Interface, implements

from mmolite.common.define import *
from mmolite.common.exceptions import *
from mmolite.common.util import getOptions
from mmolite.common.db_serv import RaceStats, FractionModifier, GenderModifier, Characters
from mmolite.character.db_serv import CharacterDBObject
"""
class CharClientFactory(pb.PBClientFactory):
    protocol = protocol.Protocol
    def __init__(self, list):
        self.list = list
        
    def clientConnectionLost(self, connector, reason):
        pb.PBClientFactory.clientConnectionLost(self, connector, reason, 0)
        if connector in self.list:
            self.list.remove(connector)
"""        
class CharacterServer(pb.Referenceable):
    Name = "Primary Character"
    def __init__(self, username, type):
        self.username = username
        self.serverType = type
        self.masterServer = None    # master server
        self.worldServers = []      # world servers list.
    
    def run(self, password, ip, port):
        # first task: connect to master server
        self.connectMaster(password, ip, port)
        
    def login(self, cred, ip, port):
        factory = pb.PBClientFactory()
        reactor.connectTCP(ip, port, factory)
        return factory.login(cred, client=self)
    
    def connectMaster(self, password, ip, port):
        cred = credentials.UsernamePassword(self.username, password)
        self.login(cred, ip, port).addCallback(self._register).addErrback(self._catchFailure)

    def _register(self, root):
        self.masterServer = root
        self.masterServer.callRemote("register", CharacterServer.Name, -1, 0, self.serverType)
        
    def remote_connectWorld(self, password, ip, port):
        cred = credentials.UsernamePassword(self.username, password)
        self.login(cred, ip, port).addCallback(self._connectWorld).addErrback(self._catchFailure)

    def remote_connectWorld(self, root):
        self.worldservers.append(root);
        
    def remote_newCharacter(self, **props):
        char = Characters(
            name = props['name'],
            race = props['race'],
            fraction = props['frac'],
            gender = props['gen'],
            ownername = props['own'],
            exp = 0,
            world = 'Primary World',
            zone = 'Primary World',
            posX = 10.0,
            posY = 10.0,
            posZ = 11.0,
            rotZ = 0.0,
            face = props['face'],
            #helm = props['helm'],
            weapon = props['weap'],
            upperArmor = props['uarm'],
            lowerArmor = props['larm'],
            strength = props['str'],
            dexterity = props['dex'],
            constitution = props['con'],
            intelligence = props['int'],
            isUse = True
        )
        
    def remote_getCharacter(self, name):
        try:
            row = Characters.byName(name)
        except:
            failure.Failure(AvatarNoFound('Avatar not found'))
            return
        
        ret = {
            name: row.name,
            race: row.race,
            frac: row.fraction,
            gen: row.gender,
            ownername: row.ownername,
            exp: row.exp,
            world: row.world,
            zone: row.zone,
            x: row.posX,
            y: row.posY,
            z: row.posZ,
            w: row.rotZ,
            face: row.face,
            #helm: row.helm,
            uarm: row.upperArmor,
            larm: row.lowerArmor,
            str: row.strength,
            dex: row.dexterity,
            con: row.constitution,
            int: row.intelligence,
        }
        row.isUse = True
        return ret
        
    def remote_updateCharacter(self, **props):
        try:
            row = Characters.byName(props['name'])
        except:
            failure.Failure(AvatarNoFound('Avatar not found'))
            return
        
        if 'name' in props: row.name = props['name']
        if 'race' in props: row.race = props['race']
        if 'frac' in props: row.fraction = props['frac']
        if 'gen' in props: row.gender = props['gen']
        if 'own' in props: row.ownername = props['own']
        if 'exp' in props: row.exp = props['exp']
        if 'world' in props: row.world = props['world']
        if 'zone' in props: row.zone = props['zone']
        if 'x' in props: row.posX = props['x']
        if 'y' in props: row.posY = props['y']
        if 'z' in props: row.posZ = props['z']
        if 'w' in props: row.rotZ = props['w']
        if 'face' in props: row.face = props['face']
        if 'uarm' in props: row.upperArmor = props['uarm']
        if 'larm' in props: row.name = props['larm']
        if 'str' in props: row.strength = props['str']
        if 'dex' in props: row.dexterity = props['dex']
        if 'con' in props: row.constitution = props['con']
        if 'int' in props: row.intelligence = props['int']
        row.modifiedDate = datetime.now
        row.isUse = False

    def _catchFailure(self, failure):
        print "Error:", failure.getErrorMessage()
        reactor.stop()
        
def main():
    OPTIONS, argv = getOptions('mmolite/config/servers.cfg', 'character server', sys.argv)
    
    if OPTIONS.username=="-": OPTIONS.username = raw_input("Username: ")
    if OPTIONS.password=="-": OPTIONS.password = raw_input("Password: ")

    dbconn = CharacterDBObject('character.db', 'mmolite/data')
    
    if OPTIONS.setup: 
        print "Setting up Character Server..."
        
        dbconn.resetDB()
        raceStat = RaceStats(race = RaceType.Men, initStr = 5, initDex = 5, initCon = 5, initInt = 8, maxStr = 800, maxDex = 800, maxCon = 800, maxInt = 850)
        raceStat = RaceStats(race = RaceType.Elves, initStr = 3, initDex = 8, initCon = 3, initInt = 9, maxStr = 650, maxDex = 800, maxCon = 650, maxInt = 1000)
        raceStat = RaceStats(race = RaceType.Orcs, initStr = 8, initDex = 7, initCon = 7, initInt = 3, maxStr = 1000, maxDex = 1000, maxCon = 900, maxInt = 500)
        raceStat = RaceStats(race = RaceType.Dwarves, initStr = 9, initDex = 5, initCon = 9, initInt = 4, maxStr = 1000, maxDex = 600, maxCon = 1000, maxInt = 750)
        fracMod = FractionModifier(race = RaceType.Men, fraction = FractionType.Aral, weapon = WeaponType.MachineGun, upperArmor = UpperArmorType.Light, lowerArmor = LowerArmorType.Light, initStr = 0, initDex = 0, initCon = 0, initInt = 5, maxStr = 0, maxDex = 0, maxCon = 0, maxInt = 80)
        fracMod = FractionModifier(race = RaceType.Men, fraction = FractionType.Juze, weapon = WeaponType.Sword, upperArmor = UpperArmorType.Spider, lowerArmor = LowerArmorType.Spider, initStr = 5, initDex = 0, initCon = 0, initInt = 0, maxStr = 100, maxDex = 0, maxCon = 0, maxInt = 0)
        fracMod = FractionModifier(race = RaceType.Men, fraction = FractionType.Neit, weapon = WeaponType.Fist, upperArmor = UpperArmorType.Bare, lowerArmor = LowerArmorType.Bare, initStr = 0, initDex = 0, initCon = 5, initInt = 0, maxStr = 0, maxDex = 0, maxCon = 60, maxInt = 0)
        genMod = GenderModifier(race = RaceType.Men, gender = GenderType.Male, face = FaceType.MaleMen, initStr = 2, initDex = 0, initCon = 1, initInt = 0, maxStr = 50, maxDex = 0, maxCon = 50, maxInt = 0)
        genMod = GenderModifier(race = RaceType.Men, gender = GenderType.Female, face = FaceType.FemaleMen, initStr = 0, initDex = 1, initCon = 0, initInt = 2, maxStr = 0, maxDex = 50, maxCon = 0, maxInt = 50)
        print "Successfully"
        return
    else: 
        print "Initialize Character Server database..."
        dbconn.startDB()

        print "Character server connecting to master server: ip[%s] port[%d]"%(OPTIONS.master_ip, OPTIONS.master_port)
        
        charSvr = CharacterServer(OPTIONS.username, PeerType.Character)
        charSvr.run(OPTIONS.password, OPTIONS.master_ip, OPTIONS.master_port)

        reactor.run()

if __name__ == '__main__':
    main()