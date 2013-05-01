class ObjectType:
    # Object type
    User = 0x1000000    # 0000 0001 0000 0000 0000 0000 0000 0000
    Peer = 0x2000000    # 0000 0010 0000 0000 0000 0000 0000 0000

class UserType:
    # UserType
    NewUser = 0         #0000 0000
    Member = 1          #0000 0001
    PremiumMember = 3   #0000 0011
    GameMaster = 7      #0000 0111
    Admin = 15          #0000 1111
    AI = 16             #0001 0000
    
class UserMode:
    Grunt = 1       # normal player
    Noble = 2       # high class client
    Invincible = 4  # super strength + super armor
    God = 8         # accessable to admin commands
    Stealth = 16    # invisible
    
class UserState:
    Hopping = 0
    Online = 1

class PeerType:
    # Peer Type
    Test = 1                #0000 0000 0000 0001, if production leave it to 0
    Secondary = 2           #0000 0000 0000 0010, if primary leave it to 0
    Character = 16          #0000 0000 0001 0000
    Supervisor = 32         #0000 0000 0010 0000
    Master = 64             #0000 0000 0100 0000
    World = 128             #0000 0000 1000 0000
    Cluster = 256           #0000 0001 0000 0000
    Zone = 512              #0000 0010 0000 0000
    Client = 1024           #0000 0100 0000 0000

class FractionType:
    Aral = 1            # High Constitution
    Juze = 2            # High Intelligence
    Neit = 4            # High Strength
    
class GenderType:
    Male = 1
    Female = 2
    Neuter = 4           # mix both male and female
    
class RaceType:
    Men = 1
    Elves = 2
    Orcs = 4
    Goblins = 8
    Dwarves = 16
    Undead = 32
    Drakes = 64
    Saurian = 128
    Merfolk = 256
    Nagas = 512
    Ogres = 1024
    Trolls = 2048
    Woses = 4096
    Monsters = 8192
    Alien = 16384
    
class UpperArmorType:
    Bare = 0
    Light = 1
    Spider = 2
    
class LowerArmorType:
    Bare = 0
    Light = 1
    Spider = 2
    
class WeaponType:
    Fist = 0
    Sword = 1
    MachineGun = 2
    
class FaceType:
    FemaleMen = 1
    MaleMen = 2
    Elven = 4
    Orc = 8
    Troll = 16
    
    
    