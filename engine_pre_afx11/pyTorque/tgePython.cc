
void tgePyInit(void)
{
   // Install a mechanism to redirect stdout and the TGECall python entry point
    PyRun_SimpleString("import sys\n"
                       "import tgenative\n"
					         "import mud.tgepython.console as TGE\n"
       

                       "def main_is_frozen():\n"
                       "  return (hasattr(sys, \"frozen\") or # new py2exe\n"
                       "  hasattr(sys, \"importers\") # old py2exe\n"
                       "  or imp.is_frozen(\"__main__\")) # tools/freeze\n"

                       "sys.oldstdout = sys.stdout\n"
                       "sys.oldstderr = sys.stderr\n"
                       "class TGEWriter:\n"
                       "  def write(self, text):\n"
                       "    tgenative.TGEPrint(text)\n"
                       "  def flush(self):\n"
                       "    pass\n"
                       "  def __del__(self):\n"
                       "    sys.stdout = sys.oldstdout\n"
                       "    sys.stderr = sys.oldstderr\n"
                       "if not main_is_frozen():\n"
                       "  sys.stdout = TGEWriter()\n"
                       "  sys.stderr = TGEWriter()\n"		
					   "sys.stdout.write(\"Python \"+sys.version+\"\\n\")\n"	
					   "tgenative.TGEInit(TGE.TGECallback,TGE.TGEDelete)\n"					   
					  
                      );


	
	Con::addCommand("PyExec", cTGEPyExec, "PyExec(\"scriptname.py\"[,functionToCall().. defaults to PyExec()\")", 2, 3);
	Con::addCommand("PySimpleString", cTGEPySimpleString, "PySimpleString(\"script\")", 2, 2);

	//these should be valid after TGEInit call above
	AssertFatal(gTGECall,"Invalid TGECallback");
	AssertFatal(gTGEDelete,"Invalid TGEDelete");
}

ConsoleFunction( PyInit, void, 1, 1, "PyInit()" )
{
   argc;
   tgePyInit();
}
