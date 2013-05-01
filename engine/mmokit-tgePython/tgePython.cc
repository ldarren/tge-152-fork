
//(c) 2002 Joshua Ritter (ritterjoshua@msn.com) see license.txt for details

//!!!!!!!ON WIN32
//!!!!!!!MAKE SURE TO COMPILE THIS WITH MULTITHREADED DLL and DEBUG MULTITHREADED DLL CODE GENERATION
//!!!!!!!ALSO CHANGE ALL PROJECTS TORQUEDEMO IS DEPENDANT ON TO THIS CODE GENERATION!!!!


//TGEPython Native Module
//guard against _DEBUG so that we don't have to build a debug python to link against
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG 1
#else
#include <Python.h>
#endif


#include "stdio.h"
#include "platform/platform.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "console/simbase.h"
#include "game/gamebase.h"
#include "game/shapebase.h"
#include "rpg/game/rpgPlayer.h"
#include "game/gameconnection.h"
#include "math/mathUtils.h"
#include "rpg/rpgWayPoint.h"
#include "rpg/rpgSpawnPoint.h"


#include "console/compiler.h"
#include "interior/interior.h"


#include <float.h>


#ifdef __cplusplus
extern "C" {
#endif

extern PyTypeObject gTGEPyObjectType;

//from tgePyObject.cc
const char* tgePythonBuildStringValue(PyObject* value);
void tgePythonBuildStringArgs(PyObject* tuple,char** argv, S32& argc);

//PyFunction sole entry point for TGE -> Python
PyObject* gTGECall=NULL;
PyObject* gTGEDelete=NULL;
PyObject* gTGEError=NULL;


//  Helper functions for directing output to Torque's console
PyObject* tgePyCmd_print( PyObject *self, PyObject *args)
{
  
	char     *text;

	if(PyArg_ParseTuple(args, "s:TGEPrint", &text)) 
	{ 
      if (dAtoi(Con::getVariable("$Server::Dedicated")))
		   Con::printf(text); 
      else
		   dPrintf("%s",text);
	}
	else 
		PyErr_SetString(gTGEError, "Unable to output text!");

	Py_INCREF(Py_None);
	return Py_None;

  
}


static const char*  cTGEToPythonCallback(SimObject *ptr, S32 argc, const char **argv)
{
	//need ptr -> PyObject (SimObject binding)

	GameBaseData* gbd=NULL;

	if (ptr)
		gbd = dynamic_cast<GameBaseData *>(ptr); 

	PyObject* mainargs=PyTuple_New(4);
	PyObject* fargs=PyTuple_New(argc);

	if (!ptr)
	{
		Py_INCREF(Py_None); 
		PyTuple_SetItem(mainargs,0,Py_None); //will be self eventually
		if (!CodeBlock::sfnNameSpace)
			PyTuple_SetItem(mainargs,1,PyString_FromString("Global")); //namespace Global
		else
			PyTuple_SetItem(mainargs,1,PyString_FromString(CodeBlock::sfnNameSpace)); //namespace Global
	}
	else
	{
		PyTuple_SetItem(mainargs,0,PyInt_FromLong(ptr->getId())); //selfid
		if (0)//gbd)
			PyTuple_SetItem(mainargs,1,PyString_FromString(gbd->getName())); //namespace = Datablock Name
		else
        {
            if (CodeBlock::sfnNameSpace)
                PyTuple_SetItem(mainargs,1,PyString_FromString(CodeBlock::sfnNameSpace)); //Namespace TGE C++ Base Class
            else
                PyTuple_SetItem(mainargs,1,PyString_FromString("Global")); //Namespace TGE C++ Base Class

        }
            
			// was, buggy?PyTuple_SetItem(mainargs,1,PyString_FromString(ptr->getNamespace()->mName)); //Namespace TGE C++ Base Class
	}
		


		
	PyTuple_SetItem(mainargs,2,PyString_FromString(argv[0])); //function name
	PyTuple_SetItem(mainargs,3,fargs); //function name

	S32 x;
	if (argc>1)
	{
		for (x=0;x<argc;x++)
			PyTuple_SetItem(fargs,x,PyString_FromString(argv[x])); //argv[1]==objectid if bound

	}
	else
	{
		Py_INCREF(Py_None); 
		PyTuple_SetItem(fargs,0,Py_None); //argv[1]==objectid if bound
		


	}
		
		

	PyObject* result = PyObject_CallObject((PyObject*)gTGECall,mainargs);

	//may need to prime prints?  They don't seem to be coming thru
	PyObject* po=PyErr_Occurred();
	if (po)
		PyErr_Print();

	const char* sresult="";

	//TODO return val -> string
	if (result && result!=Py_None)
	{
		sresult=tgePythonBuildStringValue(result);
	}

	
	Py_XDECREF(result);
	
	Py_XDECREF(mainargs);


	return sresult;
}
//export global and bound functions from Python to TGE
PyObject* tgePyCmd_export( PyObject *self, PyObject *args)
{
	int size=PyTuple_Size(args);
	if (size!=4 && size!=5)
	{
		PyErr_SetString(gTGEError, "TGEExport function takes 3/4 arguments (namespace),functionname, usage, minargs, maxargs");
		return NULL;
	}

	const char* znamespace=NULL;

	int index=0;
	if (size==5)
	  znamespace=PyString_AsString(PyTuple_GetItem(args,index++));//string

	const char* functionname= PyString_AsString(PyTuple_GetItem(args,index++));//string
	const char* usage=PyString_AsString(PyTuple_GetItem(args,index++)); //string
	S32   minargs=PyInt_AsLong(PyTuple_GetItem(args,index++)); //int
	S32   maxargs=PyInt_AsLong(PyTuple_GetItem(args,index++)); //int

	if(!(functionname && usage && minargs && maxargs))
	{
		PyErr_SetString(gTGEError, "TGEExport function problem with argument");
		return NULL;
	}

	if (znamespace)
		znamespace=StringTable->insert(znamespace);
	functionname=StringTable->insert(functionname);
	usage=StringTable->insert(usage);

	


	if (size==5)
		Con::addCommand(znamespace,functionname,cTGEToPythonCallback,usage,minargs,maxargs);
	else
		Con::addCommand(functionname,cTGEToPythonCallback,usage,minargs,maxargs);
	 
	Py_INCREF(Py_None);
	return Py_None;
}

//lets make a way for Python to evaluate .cs strings :)
PyObject* tgePyCmd_tgeeval(PyObject *self, PyObject *args)
{
  if (PyTuple_Size(args)!=1)
  {
		PyErr_SetString(gTGEError, "TGEEval: takes exactly one argument (a string)");
		return NULL;

  }

  PyObject* sarg=PyTuple_GetItem(args,0);
  if (!PyString_Check(sarg))
  {
		PyErr_SetString(gTGEError, "TGEEval: non TypeString passed to evaluate");
		return NULL;

  }

  const char* earg=PyString_AsString(sarg);

  return PyString_FromString(Con::evaluate(earg));

}

//Namespaces appear busted here, "Entity::spawnEntity" for instance
//check that this is still a problem
PyObject* tgePyCmd_tgecall( PyObject *self, PyObject *args)
{

	S32 argc=0;
	static const char* argv[32];

	
	//args will be a tuple first is the function name then the args
  
	if (PyTuple_Size(args)<1)
	{
		PyErr_SetString(gTGEError, "TGECall invoked without functionname argument!");
		return NULL;
	}

	//we can call a global function with a tuple of args...
	if(PyTuple_Check(PyTuple_GetItem(args,0)))
	{
		
		args=PyTuple_GetItem(args,0);
	}

	if(!PyString_Check(PyTuple_GetItem(args,0)))
	{
		PyErr_SetString(gTGEError, "TGECall invoked without TypeString functionname argument!");
		return NULL;
	}


	tgePythonBuildStringArgs(args,(char**)argv,argc);

	argv[0]=StringTable->insert(argv[0]);

	return PyString_FromString(Con::execute(argc,argv));

	

}







PyObject* tgePyCmd_getglobal( PyObject *self, PyObject *args)
{
	

	if (PyTuple_Size(args)!=1)
	{
		PyErr_SetString(gTGEError, "Error: GetGlobal - requires one arg (varname)");

		return NULL;
	}

	PyObject* pyvarname=PyTuple_GetItem(args,0);

	if (!PyString_Check(pyvarname))
	{
		PyErr_SetString(gTGEError, "Error: GetGlobal - argument must be type string -> $DedicatedServer for instance");
		return NULL;
	}

	char* varname = PyString_AsString(pyvarname);

	if (varname[0]!='$')
	{
		PyErr_SetString(gTGEError, "Error: GetGlobal - variable name must begin with $ -> $DedicatedServer for instance");
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


PyObject* tgePyCmd_setglobal( PyObject *self, PyObject *args)
{
	

	if (PyTuple_Size(args)!=2)
	{
		PyErr_SetString(gTGEError, "Error: SetGlobal - requires two args (varname,value)");

		return NULL;
	}

	PyObject* pyvarname=PyTuple_GetItem(args,0);

	if (!PyString_Check(pyvarname))
	{
		PyErr_SetString(gTGEError, "Error: SetGlobal - first argument must be type string -> $DedicatedServer for instance");
		return NULL;
	}

	char* varname = PyString_AsString(pyvarname);

	if (varname[0]!='$')
	{
		PyErr_SetString(gTGEError, "Error: SetGlobal - variable name must begin with $  -> $DedicatedServer for instance");
		return NULL;
	}

	const char* value=tgePythonBuildStringValue(PyTuple_GetItem(args,1));

	Con::setVariable(varname,value);

	
	

	Py_INCREF(Py_None);
	return Py_None;


}

PyObject* tgePyCmd_inittge( PyObject *self, PyObject *args)
{
  gTGECall=PyTuple_GetItem(args,0);
  gTGEDelete=PyTuple_GetItem(args,1);



  Py_INCREF(Py_None);
  return Py_None;
}


extern PyObject* new_tgePyObject(PyObject* self,StringTableEntry classname, StringTableEntry objectname, U32 id=0);

//Given a (classname, objectname) OR a (SimObjectID) either create a new TGEObject or wrap an existing one 
PyObject* tgePyObject_new( PyObject *self, PyObject *args)
{
	PyObject* result=NULL;

	if (PyTuple_Size(args)==3)
	{
		//self, classname, objectname flavor
		result= new_tgePyObject(PyTuple_GetItem(args,0),PyString_AsString(PyTuple_GetItem(args,1)),PyString_AsString(PyTuple_GetItem(args,2)));

	}
	else
	{
		PyObject* po=PyTuple_GetItem(args,0);
		if (PyInt_Check(po))
		{
			//by id
			result= new_tgePyObject(NULL,NULL,NULL,(U32)PyInt_AsLong(po));

		}
		else
		{
			//by id string
			S32 id=strtol(PyString_AsString(po),NULL,10);
			if (id)
				result= new_tgePyObject(NULL,NULL,NULL,id);
			else
				result= new_tgePyObject(NULL,NULL,PyString_AsString(po),0);

		}
		
	}

	return result;

}




PyObject* TGEGenerateCanSee(PyObject *self, PyObject *args); //game function
PyObject* TGEGetTrackingFrame(PyObject* self, PyObject* args);


//PyObject* TGEGenerateRanges(PyObject *self, PyObject *args);

static PyMethodDef tgePythonMethods[] = {
	{"TGEInit",				tgePyCmd_inittge, METH_VARARGS, NULL},
	{"TGEObject",           tgePyObject_new, METH_VARARGS, NULL},
   {"TGEPrint",			tgePyCmd_print, METH_VARARGS, NULL},  
	{"TGESetGlobal",		tgePyCmd_setglobal, METH_VARARGS, NULL},
	{"TGEGetGlobal",		tgePyCmd_getglobal, METH_VARARGS, NULL},
	{"TGECall",				tgePyCmd_tgecall, METH_VARARGS, NULL},
	{"TGEEval",				tgePyCmd_tgeeval, METH_VARARGS, NULL},
	{"TGENativeExport",		tgePyCmd_export, METH_VARARGS, NULL},
   {"TGEGenerateCanSee", TGEGenerateCanSee, METH_VARARGS, NULL}, //game function
   {"TGEGetTrackingFrame", TGEGetTrackingFrame, METH_VARARGS, NULL}, //game function
   
  // {"TGEGenerateRanges", TGEGenerateRanges, METH_VARARGS, NULL}, //game function
    {NULL,NULL,0,NULL}    /* Sentinel */
};


bool cTGEPyExec(SimObject *self, S32 argc, const char * argv[]);
bool cTGEPySimpleString(SimObject *self, S32 argc, const char * argv[]);


#ifdef __cplusplus
}
#endif


//this can take just a .py source file.. for which case PyExec() will be called
//otherwise it can be handed a .py source file + a function to call in that source
static bool cTGEPyExec(SimObject *self, S32 argc, const char * argv[])
{
	

	static char buffer[256];
	if (strlen(argv[1])>200)
	{
		AssertFatal(0,"cTGEPyExec buffer overrun");
	}



	Con::expandScriptFilename(buffer, sizeof(buffer), argv[1]);

	static char file[128];
	dStrcpy(file,buffer);
	S32 len=strlen(file);
	while (--len>=0)
	{
		char a=file[len];
		
		if (a=='.') //strip .py
			file[len]=0;
		if (a=='/' || a=='\\')
			file[len]='.';
		
	}



	if (argc==2)
		dSprintf(buffer,255,"import %s\n%s.PyExec()\n",file,file);
	else
	{
		//with or without ()
		if (argv[2][dStrlen(argv[2])]!=')')
			dSprintf(buffer,255,"import %s\n%s.%s()\n",file,file,argv[2]);
		else
			dSprintf(buffer,255,"import %s\n%s.%s\n",file,file,argv[2]);

	}
		

#ifdef TORQUE_OS_WIN32		
	
	_controlfp( _CW_DEFAULT, 0xfffff );

#endif
	int success=PyRun_SimpleString(buffer);



	return (!success);



}


//If you bomb in this function on Win32 -> you must change to Multithreaded DLL code generation!
static bool cTGEPyRunFile(SimObject *self, S32 argc, const char * argv[])
{

//!!!!!!!ON WIN32
//!!!!!!!MAKE SURE TO COMPILE THIS WITH MULTITHREADED DLL and DEBUG MULTITHREADED DLL CODE GENERATION
//!!!!!!!ALSO CHANGE ALL PROJECTS TORQUEDEMO IS DEPENDANT ON TO THIS CODE GENERATION!!!!

//!!!!!!!You will most like bomb here if you didn't :)
	
	Con::printf("Executing Python script %s\n",argv[1]);

	
	FILE* fp= fopen(argv[1],"r");
	int success=0;

    if (fp) {
      success = PyRun_SimpleFile(fp, (char *)argv[1]);

      fclose(fp);

      if (success == -1) {
         // interpreter exits due to an exception
		  Con::errorf(ConsoleLogEntry::Script,"Problem running Python script file %s!",argv[1]);
         
         return false;
      }
    }
    else {
		Con::errorf(ConsoleLogEntry::Script,"Python script %s doesn't exist", argv[1]);
      return false;
    }

	return true;


}
static bool cTGEPySimpleString(SimObject *self, S32 argc, const char * argv[])
{

	int success = PyRun_SimpleString((char *)argv[1]);
//	if (success)
//		PyErr_Print();

	return (!success);

}

static void GetYaw (const Point3F vec, F32& yawOut){

	float	forward;
	float	pitch, yaw;

	if (vec[1] == 0 && vec[0] == 0){
		yaw = 0;

		if (vec[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else {
		if (vec[0])
			yaw = mRadToDeg(atan2(vec[1], vec[0]));
		else if (vec[1] > 0)
			yaw = 90;
		else
			yaw = 270;

		if (yaw < 0)
			yaw += 360;

		forward = sqrt(vec[0]*vec[0] + vec[1]*vec[1]);

		pitch = mRadToDeg(atan2(vec[2], forward));
		if (pitch < 0)
			pitch += 360;
	}


	yawOut = yaw;

}


PyObject* TGEGetTrackingFrame(PyObject* self, PyObject* args)
{
   PyObject* pos = PyTuple_GetItem(args,0);
   F32 x = PyFloat_AsDouble(PyTuple_GetItem(pos,0));
   F32 y = PyFloat_AsDouble(PyTuple_GetItem(pos,1));
   F32 z = PyFloat_AsDouble(PyTuple_GetItem(pos,2));

   GameConnection* conn = GameConnection::getConnectionToServer();
   
   if (!conn)
   {
      return PyLong_FromLong(0);
   }

   ShapeBase* control = conn->getCameraObject();

   if (!control)
   {
      return PyLong_FromLong(0);
   }


   Point3F myPos = control->getPosition();
   Point3F tgtPos(x,y,z);

   myPos=tgtPos - myPos;

   myPos.normalize();

   MatrixF transform = control->getTransform();

   transform.setColumn(3,Point3F(0.0f,0.0f,0.0f));
   transform = transform.inverse();
   
   transform.mulP(myPos);
   

   F32 yaw;

   GetYaw(myPos,yaw);

   S32 sframe = 0;

   if (yaw > 67.5f && yaw <=112.5f)
      sframe = 0;

   else if (yaw > 112.5f && yaw <=157.5f)
      sframe = 7;
   
   else if (yaw > 157.5f && yaw <= 202.5f)
      sframe = 6;

   else if (yaw > 202.5f && yaw <= 247.5f)
      sframe = 5;
   
   else if (yaw > 247.5f && yaw <= 292.5f)
      sframe = 4;

   else if (yaw > 292.5f && yaw <= 337.5f)
      sframe = 3;

    else if (yaw > 337.5f || yaw <= 22.5f)
      sframe = 2;
    else 
      sframe = 1;

   return PyLong_FromLong(sframe);

}

/*

 def updateRanges(self):
        simObjects = self.simObjects
        numSimObjects = len(simObjects)
        for o1 in simObjects:
            o1.range = {}
        
        for xx in range(0,numSimObjects):
            o1 = simObjects[xx]
            o1.range[1]=0.01
            for yy in range(xx+1,numSimObjects):
                
                o2 = simObjects[yy]
                
                p1 = o1.position
                p2 = o2.position
                
                x = p1[0]-p2[0]
                y = p1[1]-p2[1]
                z = p1[2]-p2[2]
                
                dist = math.sqrt(x*x+y*y+z*z)
                dist = math.floor(dist)
                if dist < 1:
                    dist = 1
                
                o1.range[o2]=dist
                o2.range[o1]=dist

*/

//trying to save time ... passing informatino to world server is preemptive
/*
PyObject* TGEGenerateRanges(PyObject *self, PyObject *args)
{
   PyObject* soList = PyTuple_GetItem(args,0);
   int numSo = PyObject_Size(soList);

   int i,xx,yy;
   Point3F pos1,pos2;

   for (i=0;i<numSo;i++)
   {
      PyObject* o1 = PyList_GetItem(soList,i);
      PyObject* range = PyDict_New();
      PyObject_SetAttrString(o1,"range",range);
      Py_DECREF(range);
   }


   for (xx = 0;xx<numSo;xx++)
   {
      PyObject* o1 = PyList_GetItem(soList,xx);
      ShapeBase* shapeBase1;
      PyObject* pyid = PyObject_GetAttrString(o1,"id");
      S32 id = PyInt_AsLong(pyid);
      Py_DECREF(pyid);
      PyObject* range1 = PyObject_GetAttrString(o1,"range");
      Sim::findObject(id,shapeBase1);
      AssertISV(shapeBase1,"Missing Object");

      pos1 = shapeBase1->getPosition();

      for (yy=xx+1;yy<numSo;yy++)
      {
         PyObject* o2 = PyList_GetItem(soList,yy);
         ShapeBase* shapeBase2;
         PyObject* pyid2 = PyObject_GetAttrString(o2,"id");
         S32 id2 = PyInt_AsLong(pyid2);
         Py_DECREF(pyid2);
         PyObject* range2 = PyObject_GetAttrString(o2,"range");
         Sim::findObject(id2,shapeBase2);
         AssertISV(shapeBase2,"Missing Object");

         pos2 = shapeBase2->getPosition();

         pos2-=pos1;

         F32 dist = pos2.len();

         PyObject* pydist = PyFloat_FromDouble(dist);

         PyDict_SetItem(range1,o2,pydist);
         PyDict_SetItem(range2,o1,pydist);

         Py_DECREF(range2);
         Py_DECREF(pydist);
         

      }

      Py_DECREF(range1);

      

      
   }

   Py_INCREF(Py_None);

   return Py_None;
   
}
*/

ConsoleFunction( MyCastRay, bool, 3, 3, "(transform1,transform2)")
{

   U32 mask=   InteriorObjectType|TerrainObjectType;
   RayInfo ri;
   Point3F pos,pos2;
   
   dSscanf(argv[1], "%f %f %f", &pos.x, &pos.y, &pos.z);
   dSscanf(argv[2], "%f %f %f", &pos2.x, &pos2.y, &pos2.z);

   
#ifdef DARREN_MMO
   Interior::smIncludeClipHulls=true;
   if (gServerContainer.castRay(pos,pos2, mask, &ri))
      return false;
   Interior::smIncludeClipHulls=false;
#endif //DARREN_MMO
   return true;

}


static bool gFoundObject = false;
static void FoundObject( SceneObject* sceneObj, void * key )
{
    gFoundObject = true;
}

static bool CheckGroup_r( SimGroup* group, const Point3F& pos )
{
    int x;
    for (x=0;x<group->size();x++)
    {
        SimGroup* g2 = dynamic_cast<SimGroup*>((*group)[x]);
        if (g2)
        {
            if (!CheckGroup_r(g2,pos))
                return false;
        }
        else
        {
            rpgSpawnPoint* sp = dynamic_cast<rpgSpawnPoint*>((*group)[x]);
            if (sp)
            {
                F32 d = 48.f;
                //hack for battle system
                if (dStrstr(sp->mSpawnGroup,"B1_S"))
                {
                    d = 350.f;
                }
                Point3F p2 = sp->getPosition();
                p2-=pos;
                if (p2.len()<d)
                    return false;
            }
            rpgWayPoint* wp = dynamic_cast<rpgWayPoint*>((*group)[x]);
            if (wp)
            {
                Point3F p2 = wp->getPosition();
                p2-=pos;
                if (p2.len()<48.f)
                    return false;
            }

        }
    }
        


    return true;
}

MRandomLCG                       RandomGen;
ConsoleFunction( MyRandomRotation,const char *,1,1,"()")
{

    
    EulerF rot;
    rot.set(  0,0,mDegToRad(RandomGen.randF(0, 359)));


    QuatF quat;
    quat.set(rot);

    MatrixF mat;

    quat.setMatrix(&mat);
    AngAxisF aa(mat);

    char* buff = Con::getReturnBuffer(100);
    dSprintf(buff,100,"%g %g %g %g",aa.axis[0],aa.axis[1],aa.axis[2],aa.angle);
    return buff;

}

ConsoleFunction( MyCheckGridPoint, F32, 2, 2, "(point)")
{
    gFoundObject = false;
    Point3F pos,pos2;
    F32 b = 48.f;
    dSscanf(argv[1], "%f %f %f", &pos.x, &pos.y, &pos.z);

    //bah
    SimGroup* mission = dynamic_cast<SimGroup*>(Sim::findObject("MissionGroup"));
    if (!CheckGroup_r(mission,pos))
    {
        
        return 0;
    }

    

    Box3F box(pos[0]-b,pos[1]-b,pos[2]-1024.f,pos[0]+b,pos[1]+b,pos[2]+1024.f);
    
    gServerContainer.findObjects( box, InteriorObjectType, FoundObject, NULL );
    if (gFoundObject)
        return 0;

    

    pos.z = pos.z+1024.f;
    pos2 = pos;
    pos2.z-=2048.f;

    RayInfo ri;
    if (gServerContainer.castRay(pos,pos2, InteriorObjectType|TerrainObjectType|WaterObjectType|StaticTSObjectType, &ri))
    {
        if (ri.object)
            if (!(ri.object->getTypeMask()&TerrainObjectType))
                gFoundObject = true;
    }

    if (gFoundObject)
        return 0;


    F32 z;
    if (gServerContainer.castRay(pos,pos2, TerrainObjectType, &ri))
    {
        if (ri.normal.z < mSin(mDegToRad(90.0f-20.f)))
            gFoundObject = true;
        else
            z = ri.point.z+.1f;
    }
    else
        gFoundObject = true;

    if (gFoundObject)
        return 0;

    return z;
}



PyObject* TGEGenerateCanSee(PyObject *self, PyObject *args)
{
   U32 mask=   InteriorObjectType|StaticTSObjectType|TerrainObjectType;
   SimGroup* pPlayerGroup = dynamic_cast<SimGroup*>(Sim::findObject("PlayerGroup"));

   if (!pPlayerGroup)
   {
      Py_INCREF(Py_None);
      return Py_None;
   }

   Vector<Vector<S32>* > cansee;
   

   int x,y;

   for (x=0;x<pPlayerGroup->size();x++)
      cansee.push_back(new Vector<S32>);

   for (x=0;x<pPlayerGroup->size();x++)
   {
      rpgPlayer* sb1 = dynamic_cast<rpgPlayer*>((*pPlayerGroup)[x]);

      if (sb1->mSimZombie)
         continue;

      Point3F p1 = sb1->getBoxCenter();

      for (y=x+1;y<pPlayerGroup->size();y++)
      {
         rpgPlayer* sb2 = dynamic_cast<rpgPlayer*>((*pPlayerGroup)[y]);

         if (sb2->mSimZombie)
            continue;


         RayInfo ri;
         Point3F p2;
         p2 = sb2->getBoxCenter();

         Point3F dist = p1-p2;
         F32 ld = dist.len();
         F32 d = 100.f;
         if (d < sb1->mAggroRange)
            d = sb1->mAggroRange;
         if (d < sb2->mAggroRange)
            d = sb2->mAggroRange;

         if (ld > d+15.f)
            continue;


         if (!gServerContainer.castRay(p1,p2, mask, &ri))
         {
            cansee[x]->push_back(y);
            cansee[y]->push_back(x);
         }
      }

      
   }

   PyObject* pyCanSee = PyDict_New();

   for (x=0;x<pPlayerGroup->size();x++)
   {
      PyObject* id = PyInt_FromLong((*pPlayerGroup)[x]->getId());
      
      if (cansee[x]->size())
      {
         PyObject* tup = PyTuple_New(cansee[x]->size());
         for (y=0;y<cansee[x]->size();y++)
         {  
            PyObject* id2 = PyInt_FromLong((*pPlayerGroup)[(*cansee[x])[y]]->getId());
            PyTuple_SetItem(tup,y,id2);
         }

         PyDict_SetItem(pyCanSee,id,tup);
         Py_DECREF(tup);
         Py_DECREF(id);
      }
      else
      {
         PyObject* tup = PyTuple_New(0);
         PyDict_SetItem(pyCanSee,id,tup);
         Py_DECREF(tup);
         Py_DECREF(id);
      }
      
   }

   for (x=0;x<pPlayerGroup->size();x++)
      delete cansee[x];

   
   return pyCanSee;

   
}

/*

PyObject* TGEGenerateCanSee(PyObject *self, PyObject *args)
{
   //given a python list of ids 
   PyObject* idList = PyTuple_GetItem(args,0);
   int numIds = PyObject_Size(idList);

   PyObject* cansee = PyDict_New();


   int x,y;
   for (x=0;x<numIds;x++)
   {
      S32 id = PyInt_AsLong(PyList_GetItem(idList,x));
      PyObject* list = PyList_New(0);
      PyObject* pyid = PyInt_FromLong(id);
      PyDict_SetItem(cansee,pyid,list);
      Py_DECREF(list);
      Py_DECREF(pyid);


      for (y=x+1;y<numIds;y++)
      {
         
         S32 id2 = PyInt_AsLong(PyList_GetItem(idList,y));
         SimObject* sim1 = Sim::findObject(id);
         SimObject* sim2 = Sim::findObject(id2);

         if (!sim1 || !sim2)
            continue;

         ShapeBase* sb1 = dynamic_cast<ShapeBase*>(sim1);
         ShapeBase* sb2 = dynamic_cast<ShapeBase*>(sim2);

         if (!sb1 || !sb2)
            continue;

         //do a trace
			U32 mask=   EnvironmentObjectType|StaticShapeObjectType|TerrainObjectType;
    
         
         
         RayInfo ri;


         MatrixF mat;
         sb1->getEyeTransform(&mat);
         Point3F p1;
         mat.getColumn(3,&p1);

         sb2->getEyeTransform(&mat);
         Point3F p2;
         mat.getColumn(3,&p2);

         Point3F dist = p1-p2;
         if (dist.len() > 100.f)
            continue;


         if (!gServerContainer.castRay(p1,p2, mask, &ri))
         {
            PyObject* pyid2 = PyInt_FromLong(id2);
            PyList_Append(list,pyid2);
            Py_DECREF(pyid2);
         }

      }
   }

   return cansee;

}
*/


void tgePyInit(void)
{
   
   

	//AssertFatal(!Py_IsInitialized(),"Python already initialized"); 


	//Con::printf("\n");

	//Con::printf("----- Initializing Python -----");

   	//create the interpreter
    //Py_Initialize();

   // Install a mechanism to redirect stdout and the TGECall python entry point

	//XXX DEBUG temp direct to C:\dev for modules
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
	
	//For now lets keep this out
	//Con::addCommand("PyRunFile", cTGEPyRunFile, "PyRunFile(\"scriptname.py\")", 2, 2);
	

	//these should be valid after TGEInit call above
	AssertFatal(gTGECall,"Invalid TGECallback");
	AssertFatal(gTGEDelete,"Invalid TGEDelete");
     



 	//Con::printf("\n");

	
}

void tgePyExit(void)
{

	//AssertFatal(Py_IsInitialized(),"Python not initialized"); 

    
	//destroy the interpreter
    //Py_Finalize();
	
}

ConsoleFunction(PyGetWorkingDirectory,const char*,1,1,"PyGetWorkingDirectory")
{
	return Platform::getWorkingDirectory ()	;
}

ConsoleFunction( PyInit, void, 1, 1, "PyInit()" )
{
   argc;
   tgePyInit();
}

ConsoleFunction( PyExit, void, 1, 1, "PyExit()" )
{
   argc;
   tgePyExit();
}

#include "game/game.h"
#include "game/gameConnection.h"
#include "core/bitStream.h"
#include "game/gameConnectionEvents.h"




//PyTorque

#include "platform/gameInterface.h"

#ifdef TORQUE_OS_WIN32
//clean this up!
//win32
#include "./platformWin32/platformWin32.h"
void createFontInit();
void createFontShutdown();
extern bool LinkConsoleFunctions;

static void ShutdownWin32()
{

   createFontShutdown();

}

static void InitWin32()
{
   winState.appInstance = GetModuleHandle(NULL);


   // Console hack to ensure consolefunctions get linked in
   LinkConsoleFunctions=true;

   createFontInit();
   //windowSize.set(0,0); //jmr: this is static over in the windows platform stuff
   
   //S32 ret = Game->main(argc, argv);
   
   
}

#endif


PyObject* Py_TorqueInit(PyObject* self,PyObject* args)
{
#ifdef DARREN_MMO 
   int i;
   static char argvv[64][256];
   static const char* argv[64];

   dMemset(argvv,0,sizeof(64*256));
   dMemset(argv,0,sizeof(const char*)*64);

   for (i=0;i<64;i++)
      argv[i]=argvv[i];

#ifdef TORQUE_OS_WIN32
   InitWin32();
#endif

   int argc = PyInt_AsLong(PyTuple_GetItem(args,0));

   PyObject* pyargv = PyTuple_GetItem(args,1);

   for (i=0;i<argc;i++)
      dStrcpy(argvv[i],PyString_AsString(PyList_GetItem(pyargv,i)));

   int ret = Game->init(argc,argv);

   return PyLong_FromLong(ret);
#endif //DARREN_MMO
   return 0;
}

PyObject* Py_TorqueTick(PyObject* self,PyObject* args)
{
#ifdef DARREN_MMO
   int ret = Game->tick();

   return PyLong_FromLong(ret);
#endif // DARREN_MMO
   return 0;
}

PyObject* Py_TorqueShutdown(PyObject* self,PyObject* args)
{
#ifdef DARREN_MMO
   int ret = Game->shutdown();

#ifdef TORQUE_OS_WIN32
   ShutdownWin32();
#endif

   return PyLong_FromLong(ret);
#endif //DARREN_MMO
   return 0;
}

static PyMethodDef tgePyTorqueMethods[] = {
	{"Init",				Py_TorqueInit, METH_VARARGS, NULL},
	{"Tick",				Py_TorqueTick, METH_VARARGS, NULL},
	{"Shutdown",	   Py_TorqueShutdown, METH_VARARGS, NULL},
   {NULL,NULL,0,NULL}    /* Sentinel */
};

void TGEInstallWXSupport(void);

#ifdef TORQUE_OS_WIN32
extern "C" __declspec(dllexport) void initpytge_15(void)
#else
extern "C" void initpytge_15(void)
#endif
{

	//initialize the python module
	PyObject *m, *d;  

	m = Py_InitModule("pytge_15",  tgePyTorqueMethods);

	d = PyModule_GetDict(m);


	gTGEError = PyErr_NewException("pytge.error", NULL, NULL);
	PyDict_SetItemString(d, "error", gTGEError);


   //without this the module is being destructed!!!
   Py_INCREF(m); //keep from destruction


	if (PyErr_Occurred())
		PyErr_Print();

   //TGENATIVE

	//initialize the python module

    /* Initialize the type of the new type object here; doing it here
     * is required for portability to Windows without requiring C++. */
    gTGEPyObjectType.ob_type = &PyType_Type;

    m = Py_InitModule("tgenative",  tgePythonMethods);
    Py_INCREF(m);
    
	//TGEInstallWXSupport();

	
    //gTGEError = PyErr_NewException("tgenative.error", NULL, NULL);
    //PyDict_SetItemString(d, "error", gTGEError);
    
	 m=PyImport_ImportModule("tgenative");

    Py_INCREF(m);

    //TGEInstallWXSupport();


	 Py_INCREF((PyObject*)&gTGEPyObjectType); //keep from destruction
	 PyModule_AddObject(m,"TGEObjectType",(PyObject*)&gTGEPyObjectType);


	if (PyErr_Occurred())
		PyErr_Print();

	
}
