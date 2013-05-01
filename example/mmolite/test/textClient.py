# MMO Lite 2007
# MMOLite test Client

#register "current working folder as system path"
#there is probably a better way of doing this (change from Python 2.4->2.5)
import sys, os
sys.path.append(os.getcwd())

from md5 import md5

from twisted.spread import pb
from twisted.internet import reactor
from twisted.cred import credentials

from mmolite.common.util import getOptions

class PbAuthTester(object):
    def __init__(self, name):
        self.username = name
        self.server = None
        self.serverConn = None
        
    def runTests(self, ip, port, password):
        self.connect(ip, port, password).addCallback(self._mainMenu).addErrback(self._catchFailure)
        
    def connect(self, ip, port, password):
        creds = credentials.UsernamePassword(self.username, password)
        factory = pb.PBClientFactory()
        self.serverConn = reactor.connectTCP(ip, port, factory)
        return factory.login(creds).addCallback(self._connected)

    def _mainMenu(self, gameType):
        print "gameType[%s]"%gameType
        print "********* Test MMO Main Menu *********"
        print "1) Start Game"
        print "2) Sign up"
        print "3) Select role (Game Master only"
        print "4) Get player list (Admin only)"
        print "5) Send message to world server"
        print "6) Quit"
        choice = int(raw_input("Choice: "))
        
        work = None
        if choice == 1:
            work = self.server.callRemote("changeServer", 1).addCallback(self._changeServer) # enter zone 1
        elif choice == 2:
            newName = raw_input("Username: ")
            newPassword = raw_input("Password: ")
            newRole = raw_input("Role: ")
            work = self.server.callRemote("register", newName, newPassword, int(newRole)).addCallback(self._mainMenu)
        elif choice == 3:
            work = self.server.callRemote("chooseMode", 1).addCallback(self._mainMenu)
        elif choice == 4:
            work = self.server.callRemote("listPlayer").addCallback(self._mainMenu)
        elif choice == 5:
            msg = raw_input("Message: ")
            work = self.server.callRemote("echo", msg).addCallback(self._gameAction)
        else:
            work = self.server.callRemote("quit").addCallback(lambda _: reactor.stop()) # use lambda to filter out return value from server
        
        #work.addErrback(self._catchFailure)
    
    def _changeServer(self, argv):
        #print "argv[%s]"%argv
        ip, port, authenticID = argv[0], argv[1], argv[2]
        self.serverConn.disconnect()
        self.runTests(ip, port, authenticID)
        
    def _gameAction(self, message):
        print message
        self._mainMenu(1)
        
    def _connected(self, rootObj):
        self.server = rootObj

    def _catchFailure(self, failure):
        print "Error:", failure.getErrorMessage()
        reactor.stop()
        
def main():
    OPTIONS, argv = getOptions('mmolite/config/client.cfg', 'client', sys.argv)
    
    if OPTIONS.username=="-": OPTIONS.username = raw_input("Username: ")
    else: print ("Username: %s"%OPTIONS.username)
    if OPTIONS.password=="-": OPTIONS.password = raw_input("Password: ")
    else: print ("Password: %s"%OPTIONS.password)
    #password = md5(OPTIONS.password).digest()
    
    print "MMO test client connecting to: ip[%s] port[%d]"%(OPTIONS.master_ip, OPTIONS.master_port)
    
    tester = PbAuthTester(OPTIONS.username)
    tester.runTests(OPTIONS.master_ip, OPTIONS.master_port, OPTIONS.password)
    reactor.run()

if __name__ == '__main__':
    main()