
//(c) 2002 Joshua Ritter (ritterjoshua@msn.com) see license.txt for details

//!!!!!!!ON WIN32
//!!!!!!!MAKE SURE TO COMPILE THIS WITH MULTITHREADED DLL and DEBUG MULTITHREADED DLL CODE GENERATION
//!!!!!!!ALSO CHANGE ALL PROJECTS TORQUEDEMO IS DEPENDANT ON TO THIS CODE GENERATION!!!!


//guard against _DEBUG so that we don't have to build a debug python to link against
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG 1
#else
#include <Python.h>
#endif

#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "console/simbase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct	{
    PyObject_HEAD
	PyObject*    functionname;  // function call 
	PyObject*    ownedBy;       // if non NULL SimObject was created from this Python class instance
	SimObject*   simObject;     // the bound SimObject
	
	
} tgePyObject;

//from tgePython.cc
extern PyObject* gTGEDelete;
extern PyObject* gTGEError;


//Helper class to catch when TGE deletes a python created SimObject (works via Notify System)
class SimObjectNotifyPython : public SimObject
{
public:
	tgePyObject* m_tgePyObject;

	SimObjectNotifyPython(){m_tgePyObject=NULL;}
	virtual ~SimObjectNotifyPython(){}

	virtual void onDeleteNotify(SimObject *object);
};

void SimObjectNotifyPython::onDeleteNotify(SimObject* o)
{
	
	m_tgePyObject->simObject=NULL; //clear this...the below code is gonna blow up the tgePyObject

	PyObject* args=PyTuple_New(1);
	PyObject* pi=PyInt_FromLong(o->getId());
	PyTuple_SetItem(args,0,pi);
	PyObject_CallObject((PyObject*)gTGEDelete,args);
	Py_DECREF(args);


	
	delete this;
}

#define tgePyObject_Check(v) ((v)->ob_type == &gTGEPyObjectType)




//type helpers
const char* tgePythonBuildStringValue(PyObject* value);
void tgePythonBuildStringArgs(PyObject* tuple,char** argv, S32& argc);
PyObject* tgePythonBuildPythonObject(U32 ftype,const char* value);

StringTableEntry tgePythonParseIndex(StringTableEntry s, S32& index)
{
	static char buf[256];
	index=-1;
	
	S32 len=strlen(s);
	if (len<4)
		return s;

	AssertFatal(len<256,"rather large variable name!");

	StringTableEntry r=s;

	
	while (--len>0)
	{
		if (s[len]=='_' && s[len-1]=='_')
		{
			index=strtol(&s[len+1],NULL,10);
			dStrcpy(buf,s);
			buf[len-1]=0;
			r=buf;
			break;
		}

	}


	return r; // not an array
}


PyObject* tgePyObject_getattro(tgePyObject *self, PyObject *name)
{
	

	const char* cname=PyString_AsString(name);

	

	//cname may reference an array .. index == -1 if not
	S32 index=-1;
	cname=tgePythonParseIndex(cname,index);

	StringTableEntry stname=StringTable->insert(cname);

	SimObject* o=self->simObject;
	if (!o)
	{
		PyErr_SetString(gTGEError, ("GETATTRO: NULL SimObject: accessing a deleted object?"));
		return NULL;
	}


	const AbstractClassRep::Field* f;
	const char* dfieldval=NULL;

	//this doesn't handle dynamic fields
	if (f=o->getClassRep()->findField(stname))
	{
		if (f->elementCount>1 && index==-1)
		{
			PyErr_SetString(gTGEError, ("GETATTRO: Array variable must be accessed with Tuple or __index notation"));
			return NULL;
		}

		if (index==-1)
			index=0;

		
		return tgePythonBuildPythonObject(f->type,Con::getData(f->type, (void *) (S32(o) + f->offset),index,  f->table, f->flag));
		
	}//this handles dynamic fields
	else if(o->getFieldDictionary() &&(dfieldval=o->getFieldDictionary()->getFieldValue(stname)))
	{

		return PyString_FromString(dfieldval);
	}
	else 
	{
		Namespace* ns=o->getClassRep()->getNameSpace();
		Namespace::Entry *ent=NULL;
		if (ns)
		{
			ent= ns->lookup(stname);
		}

		if(ent || !dStrcmp(stname,"registerObject"))
		{

			self->functionname=name;

			Py_INCREF(self);  // needed as we are using ourselves for the function call

			Py_INCREF(self->functionname);   // need to do this one..

			return (PyObject *)self;


		}
		else
		{
            char errorstr[64];
            dSprintf(errorstr,64,"GETATTRO: function/variable %s doesn't exist",stname);
			PyErr_SetString(gTGEError, errorstr);
			return NULL;

		}

	}

	Py_XINCREF(Py_None);
	return Py_None;
		

}

int tgePyObject_setattro(tgePyObject *self, PyObject *name, PyObject* pyValue)
{
	//we could access arrays here with name convention? like _0 for first index... does TGE do multiple dimension arrays?  I think not

	if (pyValue==Py_None)
		return NULL;


	const char* cname=PyString_AsString(name);

	//cname may reference an array .. index == -1 if not
	S32 index=-1;
	cname=tgePythonParseIndex(cname,index);


	StringTableEntry stname=StringTable->insert(cname);


	SimObject* o=self->simObject;
	if (!o)
	{
		PyErr_SetString(gTGEError, ("SETATTRO: NULL SimObject: accessing a deleted object?"));
		return -1;
	}



	const AbstractClassRep::Field* f;

	if (f=o->getClassRep()->findField(stname))
	{
		
		const char* svalue=tgePythonBuildStringValue(pyValue);

		if (f->elementCount>1 && index==-1)
		{
			PyErr_SetString(gTGEError, ("SETATTRO: Array variable must be accessed with Tuple or __index notation"));
			return -1;
		}

		if (index==-1)
			index=0;


		Con::setData(f->type, (void *) (S32(o) + f->offset),index,1,&svalue,  f->table, f->flag);
	}
	else
	{
		//dynamic field!
		//eval(%this.object @ "." @ firstWord(InspectAddFieldName.getValue()) @ " = " @ InspectAddFieldValue.getValue() @ ";");

		const char* svalue=tgePythonBuildStringValue(pyValue);
		o->setDataField(StringTable->insert(stname),NULL,StringTable->insert(svalue));

		
		return 0;

	}

	
	return 0;
		

}

/*FUNCTION CALLS*/
// handle call to the object..
// If functionname =! NULL then we know we are really inside an OPCODE loop in the interpreter
// and need to call into TGE.. Makes the object stateful for one internal interpreter cycle
PyObject* tgePyObject_call(tgePyObject *self, PyObject *args, PyObject *kw)
{
	static const char* argv[32];
	static char idBuf[16];

    PyObject * results;


	SimObject* o=self->simObject;

	if (!o)
	{
		PyErr_SetString(gTGEError, ("CALL: NULL SimObject: accessing a deleted object?"));
		return NULL;
	}


    PyObject* functionname = self->functionname;

    if ( functionname != NULL ) // should have been set previously by getattro call..
	{  
		Namespace* ns=o->getClassRep()->getNameSpace();
		if (ns)
		{
			

			//FUNCTION
			S32 argc = 2;
			argv[0]=PyString_AsString(functionname);

			//note this is the only special case we have... if more are added make sure to make getattro call above aware
			if (!dStrcmp(argv[0],"registerObject"))
			{
				SimDataBlock *dataBlock = dynamic_cast<SimDataBlock *>(o);
				if(dataBlock)
				{
				
					dataBlock->assignId();
					static char errorBuffer[256];
					if(!dataBlock->preload(true, errorBuffer))
					{
						Con::printf("preload failed for Python datablock %s",errorBuffer);
					}


				}
				bool r=o->registerObject();
				if (!r)
				{
					PyErr_SetString(gTGEError, ("SimObject Registration Error"));
					AssertFatal(r,"SimObject did not properly register... check the fields are properly setup... _including_ datablock");

				}
				

				


				
				Py_INCREF(Py_None);
				return Py_None;
			}
			else if (!dStrcmp(argv[0],"delete"))
			{
				if (!self->ownedBy)
				{
               self->simObject=NULL;
               o->deleteObject();
               Py_INCREF(Py_None);
				   return Py_None;

					//PyErr_SetString(gTGEError, "TGEPython Object attempting to delete nonowned SimObject!");
					//return NULL;
				}
					

				
				self->simObject=NULL;
			}





			

			dSprintf(idBuf, sizeof(idBuf), "%i", o->getId());
			argv[1] = idBuf;

			
			argv[0] = StringTable->insert(argv[0]);
			Namespace::Entry *ent = ns->lookup(argv[0]);

			if (ent)
			{

				
				//ent has minargs and maxargs.. we could check this here.

				if (argc<ent->mMinArgs)
				if (ent->mMaxArgs>0 && argc>ent->mMaxArgs)
				{
					Con::errorf(ConsoleLogEntry::Script,"CALL: %s invalid number of args for function",argv[0]);
					//this exception is being raised but not printed
					PyErr_SetString(gTGEError, ("CALL: invalid number of args for function"));
					
					return NULL;
				}

				if (PyTuple_Size(args))
				{

					tgePythonBuildStringArgs(args,(char**)argv,argc);

				}
				

				SimObject *save = gEvalState.thisObject;
				gEvalState.thisObject = o;
				const char *ret = ent->execute(argc, argv, &gEvalState);
				gEvalState.thisObject = save;

				Py_DECREF(functionname);

				self->functionname=NULL;

            if (!dStricmp(argv[0],"getTransform"))
            {
               F32 transform[7];
               dSscanf(ret,"%f %f %f %f %f %f %f",
                  &transform[0],&transform[1],&transform[2],&transform[3],&transform[4],&transform[5],
                  &transform[6]);

               PyObject* tuple = PyTuple_New(7);
               PyTuple_SetItem(tuple,0,PyFloat_FromDouble(transform[0]));
               PyTuple_SetItem(tuple,1,PyFloat_FromDouble(transform[1]));
               PyTuple_SetItem(tuple,2,PyFloat_FromDouble(transform[2]));
               PyTuple_SetItem(tuple,3,PyFloat_FromDouble(transform[3]));
               PyTuple_SetItem(tuple,4,PyFloat_FromDouble(transform[4]));
               PyTuple_SetItem(tuple,5,PyFloat_FromDouble(transform[5]));
               PyTuple_SetItem(tuple,6,PyFloat_FromDouble(transform[6]));

               return tuple;

            }

            if (!dStricmp(argv[0],"getVelocity"))
            {
               F32 vel[3];
               dSscanf(ret,"%f %f %f",
                  &vel[0],&vel[1],&vel[2]);

               PyObject* tuple = PyTuple_New(3);
               PyTuple_SetItem(tuple,0,PyFloat_FromDouble(vel[0]));
               PyTuple_SetItem(tuple,1,PyFloat_FromDouble(vel[1]));
               PyTuple_SetItem(tuple,2,PyFloat_FromDouble(vel[2]));

               return tuple;

            }

            if (!dStricmp(argv[0],"getPosition"))
            {
               F32 pos[3];
               dSscanf(ret,"%f %f %f",
                  &pos[0],&pos[1],&pos[2]);

               PyObject* tuple = PyTuple_New(3);
               PyTuple_SetItem(tuple,0,PyFloat_FromDouble(pos[0]));
               PyTuple_SetItem(tuple,1,PyFloat_FromDouble(pos[1]));
               PyTuple_SetItem(tuple,2,PyFloat_FromDouble(pos[2]));

               return tuple;

            }


				return PyString_FromString(ret);
			}

		}


		Py_DECREF(self->functionname);

		self->functionname=NULL;

    }
	else
	{
		AssertFatal(0,"functionname is NULL (should have been set previously)\n");

	}


	PyErr_SetString(gTGEError, ("Invalid function call... check spelling or (variable) attribute???"));

    return NULL;

}

void tgePyObject_dealloc(tgePyObject *self)
{
	

	if (self->ownedBy && self->simObject)
	{
		
		//this is to catch a common error... of losing the python object but the simobject is left dangling
		//would it be better to just delete the object here?
		PyErr_SetString(gTGEError, ("DEALLOC: Python created SimObject destruction without matching .delete() call"));
	}


	PyObject_Del(self);
}

/*
static PySequenceMethods gTGEPyObjectAsSequence = {
	(inquiry) 0,         //sq_length
	(binaryfunc) 0,      //sq_concat
	(intargfunc) 0,      //sq_repeat
	(intargfunc) 0,      //sq_item y=x[i]
	(intintargfunc) 0,   //sq_slice
	(intobjargproc) 0,   //sq_ass_setitem x[i]=y
	(intintobjargproc),0,//sq_ass_slice
}
*/

PyObject* tgePythonGetArrayIndexName(PyObject* key)
{
	if (!PyTuple_Check(key) || PyTuple_Size(key)!=2)
	{
		PyErr_SetString(gTGEError, ("ARRAYGETITEM: called with non-Tuple or len(Tuple)!=2... use (name,index)"));
		return NULL;
	}

	PyObject* pname=PyTuple_GetItem(key,0);
	if (!PyString_Check(pname))
	{
		PyErr_SetString(gTGEError, ("ARRAYGETITEM: first Tuple argument must be StringType"));
		return NULL;
	}

	char* name=PyString_AsString(pname);

	S32 index=-1;
	PyObject* pindex=PyTuple_GetItem(key,1);

	
	if (!PyInt_Check(pindex))
		index=PyInt_AsLong(pindex);
	else if (!PyFloat_Check(pindex))
		index=int(PyFloat_AsDouble(pindex));
	else
	{
		PyErr_SetString(gTGEError, ("ARRAYGETITEM: second Tuple argument must be IntType or FloatType"));
		return NULL;

	}

	if (pindex<0)
	{
		PyErr_SetString(gTGEError, ("ARRAYGETITEM: negative index!"));
		return NULL;

	}

	
	static char arrayget[256];
	dSprintf(arrayget,256,"%s__%i",name,index);

	return PyString_FromString(arrayget);

}

PyObject * tgePyObject_arraygetitem(tgePyObject* self, PyObject* key)
{ 

	PyObject* parrayget=tgePythonGetArrayIndexName(key);
	PyObject* ret= tgePyObject_getattro(self, parrayget);

	Py_DECREF(parrayget);
	
	return ret;
}

int tgePyObject_arraysetitem(tgePyObject* self, PyObject* key,PyObject* value)
{
	PyObject* parrayset=tgePythonGetArrayIndexName(key);
	int ret=tgePyObject_setattro(self, parrayset,value);


	Py_DECREF(parrayset);

	return ret;

}



static PyMappingMethods gTGEPyObjectAsMapping = {
	(inquiry) 0, //length
	(binaryfunc) tgePyObject_arraygetitem,
	(objobjargproc) tgePyObject_arraysetitem,
} ;

PyTypeObject gTGEPyObjectType = {

    PyObject_HEAD_INIT(NULL)

    0,						 /*ob_size*/

    "TGEObject",			 /*tp_name*/

    sizeof(gTGEPyObjectType),/*tp_basicsize*/

    0,						 /*tp_itemsize*/

    /* methods */

    (destructor)tgePyObject_dealloc,       /*tp_dealloc*/

    0,  	 /*tp_print*/

    0,		 /*tp_getattr*/

    0,		 /*tp_setattr*/

    0,       /*tp_compare*/

    0,       /*tp_repr*/

    0,       /*tp_as_number*/

    0,       /*tp_as_sequence*/

    &gTGEPyObjectAsMapping,       /*tp_as_mapping*/

    0,       /*tp_hash*/

    (ternaryfunc)tgePyObject_call,		 /*tp_call*/  

    0,       /*tp_str*/

    (getattrofunc)tgePyObject_getattro,		 /*tp_getattro*/

    (setattrofunc)tgePyObject_setattro,       /*tp_setattro*/

    0,       /*tp_as_buffer*/

    Py_TPFLAGS_DEFAULT, /*tp_flags*/

    0,       /*tp_doc*/

    0,       /*tp_traverse*/

    0,       /*tp_clear*/

    0,       /*tp_richcompare*/

    0,       /*tp_weaklistoffset*/

    0,       /*tp_iter*/

    0,       /*tp_iternext*/

    0,       /*tp_methods*/

    0,       /*tp_members*/

    0,       /*tp_getset*/

    0,       /*tp_base*/

    0,       /*tp_dict*/

    0,       /*tp_descr_get*/

    0,       /*tp_descr_set*/

    0,       /*tp_dictoffset*/

    0,       /*tp_init*/

    0,       /*tp_alloc*/

    0,       /*tp_new*/

    0,       /*tp_free*/

    0,       /*tp_is_gc*/

};

PyObject* new_tgePyObject(PyObject* self,StringTableEntry classname, StringTableEntry objectname, U32 id=0)
{
	char buf[1024];

	SimObject *o =NULL;

	tgePyObject* nobj = PyObject_New(tgePyObject, &gTGEPyObjectType);   //new reference
	nobj->ownedBy=NULL;
	nobj->functionname=NULL;
	
	

	if (self!=NULL)
	{
		//we need to create SimObject
		
		nobj->ownedBy=self;

		AssertFatal(classname,"NEW: NULL classname for new SimObject")

		o =(SimObject*) ConsoleObject::create(classname); 

		if (o) 
		{

			//this may have to go after object is registered
			SimObjectNotifyPython* sop=new SimObjectNotifyPython();
			sop->m_tgePyObject=nobj;
			sop->deleteNotify(o);

				
			o->assignName(StringTable->insert(objectname));


			

        }
		else
		{
			PyErr_Format(gTGEError, "NEW: Unable to create a TGEObject of type %s",classname);
			return NULL;
		}
			

		

	}
	else
	{
		if (!objectname)
			o=Sim::findObject(id);
		else
			o=Sim::findObject(objectname);

		if (!o)
		{
			if (!objectname)
				PyErr_Format(gTGEError, "NEW: Unable to find a TGEObject with ID %i",id);
			else
				PyErr_Format(gTGEError, "NEW: Unable to find a TGEObject with name %s",objectname);
				
			return NULL;
		}

	}


	AssertFatal(o,"Error Getting SimObject!");

	nobj->simObject=o;
	
	return (PyObject*)nobj;
	
}

const char* tgePythonBuildStringValue(PyObject* value)
{
	S32 i;
	static char retBuf[32768]; //no recursion please
	
	
	retBuf[0]=0;
	if (PyInstance_Check(value))
	{
		 PyInstanceObject* pi=( PyInstanceObject*)value;
		 PyObject* v=PyDict_GetItemString(pi->in_dict,"_tge");
		 if (v)
			 value=v;

	}

	if (tgePyObject_Check(value))
	{
		tgePyObject* tgep=(tgePyObject*)value;
		AssertFatal(tgep->simObject,"CALL: BUILDARGS: Invalid (deleted?) SimObject used in call");
		dSprintf(retBuf, 32768, "%i", tgep->simObject->getId());
		
		
	}
	else if (PyList_Check(value))
	{
		//unlikely spot, but possible error... 128 byte argument buffer overrun "possible"

		S32 i;
		
		for (i=0;i<PyList_Size(value);i++)
		{
			PyObject* lo=PyList_GetItem(value,i);
			PyObject* ps=PyObject_Str(lo);
			const char* ss=PyString_AsString(ps);
			if (!i)
				dStrcpy(retBuf,ss);
			else
				dSprintf(&retBuf[strlen(retBuf)],32768," %s",ss ); 

			

			Py_DECREF(ps);
			

		}


	}
	else
	{
		//vec3 along with anything else (matrix3 etc) handled here :)
		PyObject* ps=PyObject_Str(value);
		strcpy(retBuf,PyString_AsString(ps));
      if (retBuf[0]=='~')
      {
         char s[512];
         dSprintf(s,512,"%s%s",Con::getVariable("$defaultGame"),&retBuf[1]);
         dStrcpy(retBuf,s);
      }
		Py_DECREF(ps);

	}
		


	return retBuf;

}

void tgePythonBuildStringArgs(PyObject* tuple,char** argv, S32& argc)
{
	static char argbuf[32][32768];
	
	S32 len=PyTuple_Size(tuple);

	AssertFatal(len<32,"Too many arguments!");
	S32 i;
	for (i=0;i<len;i++)
	{
		PyObject* po=PyTuple_GetItem(tuple,i);
		dStrcpy(argbuf[i],tgePythonBuildStringValue(po));
		argv[argc]=(char*)StringTable->insert(argbuf[i],true);
		
		argc++;

	}

    
}

PyObject* tgePythonBuildPythonObject(U32 ftype,const char* value)
{
// TGE_1_5_1 OneST8: fix taken from Pascal's (Mar 25, 2006 at 18:17) reply
// http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=3345
	if(ftype == TypeS8 || ftype == TypeS32)
   {
      return PyInt_FromLong(strtol(value,NULL,10));
	}
	else if(ftype == TypeF32)
   {
      return PyFloat_FromDouble(strtod(value,NULL));
	}
	else if(ftype == TypeBool)
   {
      if (!dStricmp("1",value))
	      return PyInt_FromLong(1);
      else
	      return PyInt_FromLong(0);
	}
	else
   {
      return PyString_FromString(value);
	}
/*
	switch (ftype)
	{
		case TypeS8:
		case TypeS32:
			return PyInt_FromLong(strtol(value,NULL,10));
			break;
		case TypeF32:
			return PyFloat_FromDouble(strtod(value,NULL));
			break;
		case TypeBool:
			if (!dStricmp("1",value))
				return PyInt_FromLong(1);
			else
				return PyInt_FromLong(0);
			break;
		default:
			return PyString_FromString(value);
			break;
	}

	return NULL;
*/
// End TGE_1_5_1
}

#ifdef __cplusplus
}
#endif
