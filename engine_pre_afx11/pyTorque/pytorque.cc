
//Python Support

#include "platform/platform.h"
#include "platform/event.h"
#include "platform/gameInterface.h"
#include "platformWin32/platformWin32.h"
#include "console/simBase.h"
#include "console/compiler.h"
#include "console/consoleInternal.h"
#include "core/tDictionary.h"

int Torque_Initialize(int argc, const char** argv);
int Torque_Tick();
int Torque_Shutdown();

bool gPyTorqueInitialized = false;

extern "C" {

#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG 
#else
#include <Python.h>
#endif

static PyObject* gTorqueException = NULL;
static HashTable<StringTableEntry,PyObject*> gCallableLookup;

//TorqueObject

typedef struct {
   PyObject_HEAD      
   SimObject* simObject;
} PyTorqueObject;

//PyTorqueObject Allocator (used when wrapping/finding an existing TorqueObject)
static PyObject* PyTorqueObject_new(PyTypeObject *typeobject,PyObject *pargs, PyObject *kwargs)
{
   PyObject* arg = NULL;
   SimObject* simObject = NULL;

   if (PyTuple_Size(pargs)!=1)
   {
      PyErr_SetString(gTorqueException, "TorqueObject(name|id)");
      return NULL;
   }

   arg = PyTuple_GetItem(pargs,0);

   if (PyString_Check(arg))
   {
      simObject = Sim::findObject(PyString_AsString(arg));
      if (!simObject)
      {
         PyErr_Format(gTorqueException, "NEW: Unable to find a TorqueObject with name '%s'",PyString_AsString(arg));
         return NULL;
      }
   }
   else if (PyInt_Check(arg))
   {
      simObject = Sim::findObject(PyInt_AsLong(arg));
      if (!simObject)
      {
         PyErr_Format(gTorqueException, "NEW: Unable to find a TorqueObject with id: %i",PyInt_AsLong(arg));
         return NULL;
      }

   }
   else
   {
      PyErr_SetString(gTorqueException, "TorqueObject(name|id)");
      return NULL;
   }

   PyTorqueObject* nobj = PyObject_New(PyTorqueObject, typeobject);   //new reference   
   nobj->simObject = simObject;
   Py_INCREF(nobj);
   return (PyObject*)nobj;
}

//PyTorqueObject Field Accessors

//Python to Torque function calling
static StringTableEntry gFunctionString;
static Namespace* gFunctionNameSpace;
static Namespace::Entry *gFunctionEntry;

static PyObject* PyTorqueObject_call(PyTorqueObject *self, PyObject *args, PyObject *kw)
{
   const char* argv[32];
   S32 argc = 2;

   argv[0]=gFunctionString;
   argv[1]=self->simObject->getIdString();

   for (int i =0;i<PyTuple_Size(args) && i<32;i++)
   {
      argc++;
      if (gFunctionEntry->mMaxArgs>0 && argc>gFunctionEntry->mMaxArgs)
      {
         PyErr_Format(gTorqueException, "TorqueObject of class %s function %s max arguments exceeded",self->simObject->getClassName(),gFunctionString);
         return NULL;
      }
      
      PyObject* o = PyTuple_GetItem(args,i);
      if (PyString_Check(o))
         argv[i+2]=PyString_AsString(o);
      else
      {
         PyObject* str = PyObject_Str(o);
         argv[i+2]=PyString_AsString(str);
         Py_DECREF(str);
      }
   }

   if (argc<gFunctionEntry->mMinArgs)
   {
      PyErr_Format(gTorqueException, "TorqueObject of class %s function %s not enough arguments",self->simObject->getClassName(),gFunctionString);
      return NULL;
   }

   SimObject *save = gEvalState.thisObject;
   gEvalState.thisObject = self->simObject;
   const char *ret = gFunctionEntry->execute(argc, argv, &gEvalState);
   gEvalState.thisObject = save;

   return PyString_FromString(ret);
}

static PyObject* PyTorqueObject_getattr(PyTorqueObject *self, char *attr)
{
   StringTableEntry fieldName = StringTable->insert(attr);

   const AbstractClassRep::Field* field = self->simObject->getClassRep()->findField(fieldName);

   if (field)
   {
      //XXX: Handle Arrays
      return PyString_FromString(self->simObject->getDataField(fieldName,NULL));
   }

   //not a field, could be a function
   gFunctionNameSpace=self->simObject->getClassRep()->getNameSpace();
   AssertISV(gFunctionNameSpace,"SimObject with no namespace");

   gFunctionString = fieldName;

   gFunctionEntry = gFunctionNameSpace->lookup(gFunctionString);
   if (!gFunctionEntry)
   {
      PyErr_Format(gTorqueException, "TorqueObject of class %s has no field or function %s",self->simObject->getClassName(),gFunctionString);
      return NULL;
   }

   //XXX: increase ref count on self here?
   Py_INCREF(self);
   return (PyObject *)self;
}

static int PyTorqueObject_setattr(PyTorqueObject* self, char* attr, PyObject* value)
{
   StringTableEntry fieldName = StringTable->insert(attr);

   const AbstractClassRep::Field* field = self->simObject->getClassRep()->findField(fieldName);

   if (!field)
   {
      PyErr_Format(gTorqueException, "TorqueObject of class %s has no field %s",self->simObject->getClassName(),fieldName);
      return NULL;
   }

   if (PyString_Check(value))
   {
      self->simObject->setDataField(fieldName,NULL,PyString_AsString(value));
      return 0;
   }

   PyObject* str = PyObject_Str(value);
   self->simObject->setDataField(fieldName,NULL,PyString_AsString(str));
   Py_DECREF(str);

   return 0;
}


static PyObject* PyTorqueObject_repr(PyTorqueObject* self)
{
   return PyString_FromString(self->simObject->getIdString());
}

static PyTypeObject PyTorqueObject_Type = {
   PyObject_HEAD_INIT(NULL)
      0,                         /*ob_size*/
      "pytorque.TorqueObject",         /*tp_name*/
      sizeof(PyTorqueObject),       /*tp_basicsize*/
      0,                         /*tp_itemsize*/
      0,                         /*tp_dealloc*/
      0,                         /*tp_print*/
      (getattrfunc) PyTorqueObject_getattr, /*tp_getattr*/
      (setattrfunc) PyTorqueObject_setattr,                         /*tp_setattr*/
      0,                         /*tp_compare*/
      (reprfunc) PyTorqueObject_repr,                          /*tp_repr*/
      0,                         /*tp_as_number*/
      0,                         /*tp_as_sequence*/
      0,    /*tp_as_mapping*/
      0,                         /*tp_hash */
      (ternaryfunc) PyTorqueObject_call,          /*tp_call*/
      0,                         /*tp_str*/
      0,                         /*tp_getattro*/
      0,                         /*tp_setattro*/
      0,                         /*tp_as_buffer*/
      Py_TPFLAGS_DEFAULT,        /*tp_flags*/
      "TorqueObject",               /* tp_doc */
};

//pytorque Module

//single point of entry from torque -> python
static const char*  cTorqueToPythonCallback(SimObject *ptr, S32 argc, const char **argv)
{
   HashTable<StringTableEntry,PyObject*>::Iterator iter;
   HashTable<StringTableEntry,PyObject*>::Pair pair;
   PyObject* callable = NULL;
   const char *look = NULL;
   char buffer[512];

   PyObject* args=NULL;

   //first look in namespace. DARREN: is this really nessary? let's see
   if (argc > 1)
   {
      dSprintf(buffer, 512, "%s::%s",argv[1],argv[0]);
      look = StringTable->insert(buffer);
      if (!gCallableLookup.count(look))
         look = NULL;
	  else
	  {
		args = PyTuple_New(argc-2);
		for (int i=2;i<argc;i++) PyTuple_SetItem(args,i-2,PyString_FromString(argv[i]));
	  }
   }
   else
   {
	   //next look in objects namespace
	   if (!look && ptr)
	   {
		  dSprintf(buffer, 512, "%s::%s",ptr->getName(),argv[0]);
		  look = StringTable->insert(buffer);
		  if (!gCallableLookup.count(look))
			 look = NULL;
	   }
	   
	   //finally look for a global export
	   if (!look)
	   {
		  look = argv[0]; //assumes argv[0,] which is the function name, is a stringtable entry
		  if (!gCallableLookup.count(look))
			 look = NULL;
	   }

	   if (!look)
	   {
		  Con::errorf("cTorqueToPythonCallback - missing export %s",argv[0]);
		  return "";
	   }
	   
	   args = PyTuple_New(argc-1);
	   for (int i=1;i<argc;i++) PyTuple_SetItem(args,i-1,PyString_FromString(argv[i]));
   }

   iter = gCallableLookup.find(look);

   pair =*iter;
   callable = pair.value;

   PyObject* result = PyObject_CallObject(callable,args);
   if (PyErr_Occurred())
      PyErr_Print();

   const char* r = "";

   if (result)
   {
      PyObject* str = PyObject_Str(result);
      r = PyString_AsString(str);
      Py_DECREF(str);
   }

   Py_XDECREF(args);
   Py_XDECREF(result);

   return r;
}

//export global and bound functions from Python to TGE
static PyObject* pytorque_export( PyObject *self, PyObject *args)
{
   //this can happen, for instance, when importing into an editor that supports module instrospection for call tips
   if(!gPyTorqueInitialized)
   {
      Py_INCREF(Py_None);
      return Py_None;
   }

   int size = PyTuple_Size(args);
   if (size!=5 && size!=6)
   {
      PyErr_SetString(gTorqueException,"pytorque.export(callback,[namespace],functionname,usagedoc,minargs,maxargs)");
      return NULL;
   }

   int index=0;
   PyObject* o, *callable;
   const char* ns,*fname,*usage;
   S32 minargs,maxargs;
   StringTableEntry lookup;

   o=PyTuple_GetItem(args,index++);
   if (!PyCallable_Check(o))
   {
      PyErr_SetString(gTorqueException,"pytorque.export() - non-callable callback");
      return NULL;
   }

   callable = o;

   if (size==6) //with namespace
   {
      o=PyTuple_GetItem(args,index++);
      if (!PyString_Check(o))
      {
         PyErr_SetString(gTorqueException,"pytorque.export() - non-string namespace");
         return NULL;
      }

      ns = PyString_AsString(o);
   }

   //function name
   o=PyTuple_GetItem(args,index++);
   if (!PyString_Check(o))
   {
      PyErr_SetString(gTorqueException,"pytorque.export() - non-string function name");
      return NULL;
   }

   fname = PyString_AsString(o);

   //usage
   o=PyTuple_GetItem(args,index++);
   if (!PyString_Check(o))
   {
      PyErr_SetString(gTorqueException,"pytorque.export() - non-string usage");
      return NULL;
   }

   usage = PyString_AsString(o);

   //min args
   o=PyTuple_GetItem(args,index++);
   if (!PyInt_Check(o))
   {
      PyErr_SetString(gTorqueException,"pytorque.export() - non integer min args");
      return NULL;
   }

   minargs = PyInt_AsLong(o)+1; //adjust for function name argument

   //max args
   o=PyTuple_GetItem(args,index++);
   if (!PyInt_Check(o))
   {
      PyErr_SetString(gTorqueException,"pytorque.export() - non integer max args");
      return NULL;
   }

   maxargs = PyInt_AsLong(o)+1; //adjust for function name argument

   if (size==6)
   {
      //with namespace
      char catname[512];
      dSprintf(catname,512,"%s::%s",ns,fname);
      lookup = StringTable->insert(catname);
      if (gCallableLookup.count(lookup))
      {
         PyErr_SetString(gTorqueException,"pytorque.export() - namespace::function already exported will not overwrite");
         return NULL;
      }

      gCallableLookup.insertEqual(lookup,callable);

      Con::addCommand(ns,fname,cTorqueToPythonCallback,usage,minargs+1,maxargs+1);
   }
   else
   {
      lookup = StringTable->insert(fname);
      if (gCallableLookup.count(lookup))
      {
         PyErr_SetString(gTorqueException,"pytorque.export() - function already exported will not overwrite.  Consider using a namespace");
         return NULL;
      }

      gCallableLookup.insertEqual(lookup,callable);

      Con::addCommand(fname,cTorqueToPythonCallback,usage,minargs,maxargs);
   }

   Py_INCREF(Py_None);
   return Py_None;
}


//evaluate arbitrary .cs script
PyObject* pytorque_evaluate(PyObject *self, PyObject *args)
{
   if(!gPyTorqueInitialized)
   {
      Py_INCREF(Py_None);
      return Py_None;
   }

   if (PyTuple_Size(args)!=1)
   {
      PyErr_SetString(gTorqueException, "pytorque.evaluate: takes exactly one argument (string)");
      return NULL;
   }

   PyObject* sarg=PyTuple_GetItem(args,0);
   if (!PyString_Check(sarg))
   {
      PyErr_SetString(gTorqueException, "pytorque.evaluate: passed a non-string");
      return NULL;
   }

   return PyString_FromString(Con::evaluate(PyString_AsString(sarg)));
}

PyObject* pytorque_getglobal( PyObject *self, PyObject *args)
{
   if (PyTuple_Size(args)!=1)
   {
      PyErr_SetString(gTorqueException, "pytorque.getglobal(varname - incorrect number of arguments)");
      return NULL;
   }

   PyObject* pyvarname=PyTuple_GetItem(args,0);

   if (!PyString_Check(pyvarname))
   {
      PyErr_SetString(gTorqueException, "pytorque.getglobal(varname) - passed non-string");
      return NULL;
   }

   char* varname = PyString_AsString(pyvarname);

   if (varname[0]!='$')
   {
      PyErr_SetString(gTorqueException, "pytorque.getglobal(varname) - variable must begin with a $ for instance $DedicatedServer");
      return NULL;
   }

   const char* v=Con::getVariable(varname);
   if(!strlen(v))
   {
      Py_INCREF(Py_None);
      return Py_None;
   }

   return PyString_FromString(v);
}


PyObject* pytorque_setglobal( PyObject *self, PyObject *args)
{
   if (PyTuple_Size(args)!=2)
   {
      PyErr_SetString(gTorqueException, "pytorque.setglobal(varname,value) - incorrect number of arguments)");
      return NULL;
   }

   PyObject* pyvarname=PyTuple_GetItem(args,0);

   if (!PyString_Check(pyvarname))
   {
      PyErr_SetString(gTorqueException, "pytorque.setglobal(varname,value) - varname is non-string)");
      return NULL;
   }

   char* varname = PyString_AsString(pyvarname);

   if (varname[0]!='$')
   {
      PyErr_SetString(gTorqueException, "pytorque.setglobal(varname,value) - variable must begin with a $ for instance $DedicatedServer");
      return NULL;
   }

   PyObject* str = PyObject_Str(PyTuple_GetItem(args,1));
   Con::setVariable(varname,PyString_AsString(str));
   Py_DECREF(str);

   Py_INCREF(Py_None);
   return Py_None;
}


static PyObject* pytorque_init(PyObject* self,PyObject* args)
{
   int i;
   static char argvv[64][256];
   static const char* argv[64];

   dMemset(argvv,0,sizeof(64*256));
   dMemset(argv,0,sizeof(const char*)*64);

   for (i=0;i<64;i++)
      argv[i]=argvv[i];

   int argc = PyInt_AsLong(PyTuple_GetItem(args,0));

   PyObject* pyargv = PyTuple_GetItem(args,1);

   for (i=0;i<argc;i++)
      dStrcpy(argvv[i],PyString_AsString(PyList_GetItem(pyargv,i)));

   Torque_Initialize(argc,argv);

   Py_INCREF(Py_None);
   return Py_None;
}

static PyObject* pytorque_tick(PyObject* self,PyObject* args)
{
   if (Torque_Tick())
   {
      //still going
      Py_INCREF(Py_True);
      return Py_True;
   }

   Py_INCREF(Py_False);
   return Py_False;
}

static PyObject* pytorque_shutdown(PyObject* self,PyObject* args)
{
   Torque_Shutdown();
   Py_INCREF(Py_None);
   return Py_None;
}


static PyMethodDef pytorque_methods[] = {
   {"initialize", pytorque_init, METH_VARARGS, NULL},
   {"tick", pytorque_tick, METH_VARARGS, NULL},
   {"shutdown", pytorque_shutdown, METH_VARARGS, NULL},
   {"export",pytorque_export,METH_VARARGS,NULL},
   {"evaluate",pytorque_evaluate,METH_VARARGS,NULL},
   {"setglobal",pytorque_setglobal,METH_VARARGS,NULL},
   {"getglobal",pytorque_getglobal,METH_VARARGS,NULL},

   {NULL,NULL,0,NULL}    /* Sentinel */
};

extern "C" __declspec(dllexport) void initpytorque(void)
{

   //initialize the python module
   PyObject *m;  

   m = Py_InitModule("pytorque",  pytorque_methods);
   Py_INCREF(m);

   //initialize the TorqueObject type
   PyTorqueObject_Type.tp_new = PyTorqueObject_new;
   if (PyType_Ready(&PyTorqueObject_Type) < 0)
      AssertISV(0,"There was an error reading the PyTorqueObject_Type");

   Py_INCREF(&PyTorqueObject_Type);

   PyModule_AddObject(m, "TorqueObject", (PyObject *)&PyTorqueObject_Type);

   gTorqueException = PyErr_NewException("pytorque.exception", NULL, NULL);
   Py_INCREF(gTorqueException);

   PyModule_AddObject(m, "TorqueException", gTorqueException);

}

} //extern "C" 

//Torque 

extern void createFontInit();
extern void createFontShutdown();
extern bool LinkConsoleFunctions;

//--------------------------------------
static int Torque_Initialize(int argc, const char** argv)
{
   //ensure that console functions link
   LinkConsoleFunctions = true;

   winState.appInstance = GetModuleHandle(NULL);

   //S32 retVal = run(argv.size(), (const char **) argv.address());

   createFontInit();

   S32 ret = Game->initialize(argc, argv);

   gPyTorqueInitialized = true;

   return 1;
}

static int Torque_Tick()
{
   return Game->tick();
}

static int Torque_Shutdown()
{
   int ret = Game->shutdown();
   createFontShutdown();

   gPyTorqueInitialized = false;

   return ret;
}
