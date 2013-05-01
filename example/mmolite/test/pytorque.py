# MMO Lite 2007
# MMOLite test Python Torque Integration

#Torque as a standard Python extension (no longer a executable)

#register "current working folder as system path"
#there is probably a better way of doing this (change from Python 2.4->2.5)
import sys, os
sys.path.append(os.getcwd())

from mmolite import pytorque
from mmolite.pytorque import TorqueObject

#initialize pytorque, this also executes main.cs and the .cs packages
pytorque.initialize(len(sys.argv),sys.argv)

#example of executing a script file
#f = file("myscript.cs","rb")
#script = f.read()
#f.close()
#pytorque.evaluate(script)

#or, just generate the cs code right inside Python!

pytorque.evaluate("""
new GuiBitmapButtonCtrl(MyButton) {
 profile = "GuiButtonProfile";
 horizSizing = "right";
 vertSizing = "bottom";
 position = "404 361";
 extent = "285 85";
 minExtent = "8 2";
 visible = "1";
 text = "Button";
 groupNum = "-1";
 buttonType = "PushButton";
 bitmap = "./button";
 helpTag = "0";
};""")

#it's easy to grab a reference to the button we created
button = TorqueObject("MyButton")

#buttons are kind of worthless without commands.  Let's make one:
def OnMyButton(value):
    print "Button pushed with value",value
    
#export the function to the console system in much the same way the C++ system does...
#we also support optional namespaces, usage documentation, and min/max args
pytorque.export(OnMyButton,"MyButton","OnButton","Example button command",1,1)

#we can get and set fields (including dynamic fields).  We'll set our button's command:
button.command = "MyButton::OnButton(42);"

#we can call console methods on our TorqueObjects... So, let's simulate a button click.
#the OnMyButton function will be called with the value 42 :)
button.performClick()

#note that getting an object reference to the button and setting the command like this is 
#purely for illustration. You can also: command = "MyButton::OnButton(42);" in the evaluated code.

#moving on, we can get and set global variables
pytorque.setglobal("$MyVariable",42)
print pytorque.getglobal("$MyVariable")
pytorque.evaluate('echo ("*** Here is your variable:" @ $MyVariable);')

#the main loop is broken out and can be combined with other frameworks rather easily
while pytorque.tick():
    pass

#cleanup pytorque.. goodbye!
pytorque.shutdown()
