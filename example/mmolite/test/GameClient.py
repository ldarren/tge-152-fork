# MMO Lite 2007
# MMOLite test Client

#register "current working folder as system path"
#there is probably a better way of doing this (change from Python 2.4->2.5)
import sys, os
sys.path.append(os.getcwd())

from twisted.spread import pb
from twisted.internet import reactor
from twisted.cred import credentials

from mmolite import pytorque
from mmolite.pytorque import TorqueObject
from mmolite.common.util import getOptions

class PbAuth(pb.Referenceable):
    def __init__(self):
        self.server = None      # server itself
        self.serverConn = None  # the connection to server
        
    def login(self, id, password, ip, port):
        factory = pb.PBClientFactory()
        self.serverConn = reactor.connectTCP(ip, port, factory)
        creds = credentials.UsernamePassword(id, password)
        return factory.login(creds).addCallback(self._connected).addErrback(self._catchFailure)
        
    def _connected(self, rootObj):
        self.server = rootObj

    def _catchFailure(self, failure):
        print "Error:", failure.getErrorMessage()
        self.server = None
        self.serverConn = None

class PbAuthClient(PbAuth):
    def __init__(self):
        self.updated = False

    def login(self, name, password, ip, port):
        pytorque.setglobal("$loginInfo::message","Checking client version")
        return PbAuth.login(self, name, password, ip, port).addCallback(self._update)
        
    # TODO: version update, right now always return true
    def _update(self, root):
        #print "_update root type [%s]"%type(root), why root always return NoneType?
        if self.server == None: 
            pytorque.setglobal("$loginInfo::message","Version checking failed!")
            return
        self.getWorldList()

    def register(self, username, password, roleId):
        self.server.callRemote("register", username, password, roleId).addErrback(self._registerFailure)

    def _registerFailure(self, failure):
        pytorque.setglobal("$loginInfo::message",failure.getErrorMessage())
        
    def getWorldList(self):
        self.server.callRemote("getWorldList").addCallback(self._getWorldList).addErrback(self._registerFailure)
        
    def _getWorldList(self, list):
        print "_getWorldList(self, list): %d"%len(list)
        WorldListBox = TorqueObject("WorldList")
        for key in list:
            print "key[%d] name[%s]"%(key, list[key])
            WorldListBox.addRow(key, list[key])
        loginButton = TorqueObject("LoginButton")
        signupButton = TorqueObject("SignupButton")
        loginButton.setActive(1)
        signupButton.setActive(1)
        pytorque.setglobal("$loginInfo::message","Version updated!")
        self.updated = True

class PbAuthUser(PbAuth):
    def __init__(self):
        self.name = ""
        self.ip = "127.0.0.1"

    def login(self, name, password, ip, port):
        self.name = name
        self.ip = ip
        pytorque.setglobal("$loginInfo::message","Login in to world")
        return PbAuth.login(self, name, password, ip, port).addCallback(self.changeServer)
    
    def changeServer(self, root):
        if self.server == None:
            pytorque.setglobal("$loginInfo::message","Login failed!")
            return
        WorldListBox = TorqueObject("WorldList")
        self.server.callRemote("changeServer", int(WorldListBox.getSelectedId())).addCallback(self._changeServer)
    
    # disconnect master server and connect to world server
    def _changeServer(self, argv):
        self.serverConn.disconnect()
        ip, port, authenticID = argv[0], argv[1], argv[2]
        #print "name[%s] id[%s] ip[%s] pott[%d]"%(self.name, authenticID, ip, port)
        PbAuth.login(self, self.name, authenticID, ip, port).addCallback(self.startGame)
        
    def startGame(self, root):
        self.server.callRemote("getZonePort").addCallback(self._startGame)
        
    def _startGame(self, port):
        print "port[%d]"%port
        #pytorque.evaluate('JoinServerGui.joinServer("%s", "", "IP:%s:%d");'%(self.name, self.ip, port))Canvas.setContent(PlayerSelection);
        pytorque.evaluate('Canvas.setContent(PlayerSelection);')

clientObj = None
userObj = None

def reactorTick():
    reactor.runUntilCurrent()
    reactor.doIteration(0)
    
def OnLoginPressed(ip, port):
    global clientObj
    global userObj
    if not clientObj.updated: return
    
    username = pytorque.getglobal("$loginInfo::username")
    password = pytorque.getglobal("$loginInfo::password1")
    print "name[%s] password[%s]" % (username, password)
    userObj.login(username, password, ip, int(port))

def OnSignupPressed(ip, port):
    global clientObj
    global userObj
    if not clientObj.updated: return

    username = pytorque.getglobal("$loginInfo::username")
    password1 = pytorque.getglobal("$loginInfo::password1")
    password2 = pytorque.getglobal("$loginInfo::password2")
    if password1 == "" or (password1 != password2):
        pytorque.setglobal("$loginInfo::message","Password Error!")
        return
    roleCtrl = TorqueObject("LoginRole")
    roleId = int(roleCtrl.getSelected())
    #print "name[%s] password[%s] confirm[%s] role[%d]" % (usernameCtrl.getText(), password1Ctrl.getText(), password2Ctrl.getText(), roleCtrl.getSelected())
    print "name[%s] password[%s] confirm[%s] role[%d]" % (username, password1, password2, roleId)
    clientObj.register(username, password1, roleId)

   
def main():
    global clientObj
    global userObj
    Running = False
    OPTIONS, argv = getOptions('mmolite/config/client.cfg', 'client', sys.argv)

    print "MMO test client initializing torque client"
    if '-game' not in argv:
        argv.extend(['-game', 'test.mmo'])
    pytorque.initialize(len(argv),argv)
    
    print "MMO test client connecting to: ip[%s] port[%d]"%(OPTIONS.master_ip, OPTIONS.master_port)
    clientObj = PbAuthClient()
    clientObj.login("Client", "tneilc", OPTIONS.master_ip, OPTIONS.master_port)
    userObj = PbAuthUser()
    
    # get torque object
    loginButton = TorqueObject("LoginButton")
    signupButton = TorqueObject("SignupButton")
    # set torque object's attribute
    loginButton.Command = 'LoginButton.OnButton("%s", %s);'%(OPTIONS.master_ip, OPTIONS.master_port)
    signupButton.Command = 'SignupButton.OnButton("%s", %s);'%(OPTIONS.master_ip, OPTIONS.master_port)
    # map python function to torque action
    pytorque.export(OnLoginPressed,"LoginButton","OnButton","Login button command",2,2)
    pytorque.export(OnSignupPressed,"SignupButton","OnButton","Signup button command",2,2)
    # call torque object method
    loginButton.setActive(0)
    signupButton.setActive(0)
    
    print "MMO test client running"
    reactor.startRunning()

    #the main loop is broken out and can be combined with other frameworks rather easily
    while pytorque.tick():
        reactorTick()

    #cleanup pytorque.. goodbye!
    pytorque.shutdown()
    
    print "MMO test client quit"

if __name__ == '__main__':
    main()