#include "platform/platform.h"
#include "clientSideTSStatic.h"
#include "ts/tsShape.h"
#include "ts/tsShapeInstance.h"
#include "sceneGraph/sceneGraph.h"

//----------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(ClientSideTSStatic);

ClientSideTSStatic::ClientSideTSStatic()
{
   mTypeMask |= StaticShapeObjectType | StaticObjectType;
   mNetFlags.clear(Ghostable | ScopeAlways);
   mNetFlags.set(IsGhost);
   mHidden = false;
   
   // animation mod
   S32 i;   
   for (i = 0; i < ShapeBase::MaxScriptThreads; i++) 
   {      
	   mScriptThread[i].sequence = -1;      
	   mScriptThread[i].thread = 0;      
	   mScriptThread[i].sound = 0;      
	   mScriptThread[i].state = Thread::Stop;      
	   mScriptThread[i].atEnd = false;      
	   mScriptThread[i].forward = true;   
   }   
   lastrenderms = Sim::getCurrentTime();	
   TargetPos = oldpos = Point3F();	
   TargetRot = oldrot = QuatF();	
   interpolateTargetTick = interpolateStartTick = 0;
}

ClientSideTSStatic::~ClientSideTSStatic()
{
}

//--------------------------------------------------------------------------
void ClientSideTSStatic::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("Media");
   addField("hidden", TypeBool, Offset(mHidden, ClientSideTSStatic));
   endGroup("Media");
}

bool ClientSideTSStatic::onAdd()
{
   // Skip TSStatic onAdd
   if(!SceneObject::onAdd())
      return false;

   if (!mShapeName || mShapeName[0] == '\0') 
   {
      Con::errorf("TSStatic::onAdd: no shape name!");
      return false;
   }
   mShapeHash = _StringTable::hashString(mShapeName);

   mShape = ResourceManager->load(mShapeName);

   if (bool(mShape) == false) 
   {
      Con::errorf("TSStatic::onAdd: unable to load shape: %s", mShapeName);
      return false;
   }

/*   if(isClientObject() && !mShape->preloadMaterialList() && NetConnection::filesWereDownloaded())
      return false; */

   mObjBox = mShape->bounds;
   resetWorldBox();
   setRenderTransform(mObjToWorld);

   mShapeInstance = new TSShapeInstance(mShape, true);

   // Scan out the collision hulls...
   /* ============= TUAN remove 12.5
   U32 i;
   for (i = 0; i < 8; i++) 
   {
      char buff[128];
      dSprintf(buff, sizeof(buff), "Collision-%d", i + 1);
      mCollisionDetails[i] = mShape->findDetail(buff);
      mLOSDetails[i]       = mCollisionDetails[i];
   }

   // Compute the hull accelerators (actually, just force the shape to compute them)
   for (i = 0; i < 8; i++) 
   {
      if (mCollisionDetails[i] != -1) {
         mShapeInstance->getShape()->getAccelerator(mCollisionDetails[i]);
      }
   }
   TUAN remove 12.5 ============== */
   //=== Tuan add 12.5   
   U32 i;   
   for (i = 0; i < mShape->details.size(); i++)   
   {      
	   char* name = (char*)mShape->names[mShape->details[i].nameIndex];      
	   if (dStrstr((const char*)dStrlwr(name), "collision-"))      
	   {         
		   mCollisionDetails.push_back(i);         
		   // The way LOS works is that it will check to see if there is a LOS detail that matches         
		   // the the collision detail + 1 + MaxCollisionShapes (this variable name should change in         
		   // the future). If it can't find a matching LOS it will simply use the collision instead.         
		   // We check for any "unmatched" LOS's further down         
		   mLOSDetails.increment();      
		   char buff[128];         
		   dSprintf(buff, sizeof(buff), "LOS-%d", i + 1);         
		   U32 los = mShape->findDetail(buff);         
		   if (los == -1)            
			   mLOSDetails.last() = i;         
		   else            
			   mLOSDetails.last() = los;      
	   }   
   }   
   // Snag any "unmatched" LOS details   
   for (i = 0; i < mShape->details.size(); i++)   
   {      
	   char* name = (char*)mShape->names[mShape->details[i].nameIndex];      
	   if (dStrstr((const char*)dStrlwr(name), "los-"))      
	   {         
		   // See if we already have this LOS         
		   bool found = false;         
		   for (U32 j = 0; j < mLOSDetails.size(); j++)         
		   {            
			   if (mLOSDetails[j] == i)            
			   {               
				   found = true;               
				   break;            
			   }         
		   }         
		   if (!found)            
			   mLOSDetails.push_back(i);      
	   }   
   }   
   // Compute the hull accelerators (actually, just force the shape to compute them)   
   for (i = 0; i < mCollisionDetails.size(); i++)         
	   mShapeInstance->getShape()->getAccelerator(mCollisionDetails[i]);	
   //=== Tuan end add.      
   gClientContainer.addObject(this);   
   gClientSceneGraph->addObjectToScene(this);   
   return true;
}

void ClientSideTSStatic::onRemove()
{
   removeFromScene();
   Parent::onRemove();
}

void ClientSideTSStatic::setHidden(bool isHidden) {
	mHidden = isHidden;
}

bool ClientSideTSStatic::prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState) {
	if (mHidden) {
		return false;
	}
	return Parent::prepRenderImage(state, stateKey, startZone, modifyBaseZoneState);

	// animation mod
	U32 curms = Sim::getCurrentTime();advanceThreads(curms);	
	if(interpolateTargetTick > 0)		
		if(interpolateTargetTick >= curms)		
		{            
			F32 t = (float)(curms-interpolateStartTick)/(interpolateTargetTick-interpolateStartTick);			
			interpolate(t);		
		}		
		else		
		{			
			MatrixF mat;			
			TargetRot.setMatrix(&mat);			
			mat.setColumn(3,TargetPos);            
			setTransform(mat);		
		}
}

// animation mod
bool ClientSideTSStatic::setThreadSequence(U32 slot,S32 seq,bool reset)
{   
	Thread& st = mScriptThread[slot];   
	if (st.thread && st.sequence == seq && st.state == Thread::Play)      
		return true;   
	
	if (seq < ShapeBase::MaxSequenceIndex) 
	{	   
		setMaskBits(ShapeBase::ThreadMaskN << slot);      
		st.sequence = seq;      
		if (reset) 
		{         
			st.state = Thread::Play;         
			st.atEnd = false;         
			st.forward = true;      
		}      
		
		if (mShapeInstance) 
		{         
			if (!st.thread)            
				st.thread = mShapeInstance->addThread();         
			mShapeInstance->setSequence(st.thread,seq,0);         
			updateThread(st);      
		}      
		
		return true;   
	}   
	
	return false;
}

void ClientSideTSStatic::updateThread(Thread& st)
{   
	switch (st.state) 
	{      
	case Thread::Stop:         
		mShapeInstance->setTimeScale(st.thread,1);         
		mShapeInstance->setPos(st.thread,0);         
		// Drop through to pause state      
	case Thread::Pause:         
		mShapeInstance->setTimeScale(st.thread,0);         
		break;      
	case Thread::Play:         
		if (st.atEnd) 
		{            
			mShapeInstance->setTimeScale(st.thread,1);            
			mShapeInstance->setPos(st.thread,st.forward? 1: 0);            
			mShapeInstance->setTimeScale(st.thread,0);         
		}         
		else 
		{            
			mShapeInstance->setTimeScale(st.thread,st.forward? 1: -1);         
		}         
		break;   
	}
}

bool ClientSideTSStatic::stopThread(U32 slot)
{   
	Thread& st = mScriptThread[slot];   
	if (st.sequence != -1 && st.state != Thread::Stop) 
	{	   
		setMaskBits(ShapeBase::ThreadMaskN << slot);      
		st.state = Thread::Stop;      
		updateThread(st);      
		return true;   
	}   
	return false;
}

bool ClientSideTSStatic::pauseThread(U32 slot)
{   
	Thread& st = mScriptThread[slot];   
	if (st.sequence != -1 && st.state != Thread::Pause) 
	{	   
		setMaskBits(ShapeBase::ThreadMaskN << slot);      
		st.state = Thread::Pause;      
		updateThread(st);      
		return true;   
	}   
	return false;
}

bool ClientSideTSStatic::playThread(U32 slot)
{   
	Thread& st = mScriptThread[slot];   
	
	if (st.sequence != -1 && st.state != Thread::Play) 
	{	   
		setMaskBits(ShapeBase::ThreadMaskN << slot);      
		st.state = Thread::Play;      
		updateThread(st);      
		return true;   
	}   
	
	return false;
}

bool ClientSideTSStatic::setThreadDir(U32 slot,bool forward)
{   
	Thread& st = mScriptThread[slot];   
	if (st.sequence != -1) 
	{      
		if (st.forward != forward) 
		{		  
			setMaskBits(ShapeBase::ThreadMaskN << slot);         
			st.forward = forward;         
			st.atEnd = false;         
			updateThread(st);      
		}      
		return true;   
	}   
	return false;
}

void ClientSideTSStatic::advanceThreads(U32 curms)
{	
	F32 dt;	
	dt = (curms - lastrenderms)*0.001;	
	lastrenderms = curms;	
	for (U32 i = 0; i < ShapeBase::MaxScriptThreads; i++) 
	{      
		Thread& st = mScriptThread[i];      
		if (st.thread) 
		{         
			if (!mShapeInstance->getShape()->sequences[st.sequence].isCyclic() && !st.atEnd &&             
				(st.forward? mShapeInstance->getPos(st.thread) >= 1.0 :              
				mShapeInstance->getPos(st.thread) <= 0)) 
			{            
				st.atEnd = true;            
				updateThread(st);            
				if (!isGhost()) 
				{               
					char slot[16];               
					dSprintf(slot,sizeof(slot),"%d",i);               
					Con::executef(this,3,"onEndSequence",scriptThis(),slot);            
				}         
			}         
			mShapeInstance->advanceTime(dt,st.thread);      
		}   
	}
}

ConsoleMethod( ClientSideTSStatic, playThread, bool, 3, 4, "(int slot, string sequenceName)")
{   
	U32 slot = dAtoi(argv[2]);   
	if (slot >= 0 && slot < ShapeBase::MaxScriptThreads) 
	{      
		if (argc == 4) 
		{         
			if (object->getShapeInstance()->getShape()) 
			{            
				S32 seq = object->getShapeInstance()->getShape()->findSequence(argv[3]);            
				if (seq != -1 && object->setThreadSequence(slot,seq,true))               
					return true;         
			}      
		}      
		else         
			if (object->playThread(slot))            
				return true;   
	}   
	return false;
}

ConsoleMethod( ClientSideTSStatic, setThreadDir, bool, 4, 4, "(int slot, bool isForward)")
{   
	int slot = dAtoi(argv[2]);   
	if (slot >= 0 && slot < ShapeBase::MaxScriptThreads) 
	{      
		if (object->setThreadDir(slot,dAtob(argv[3])))         
			return true;   
	}   
	return false;
}

ConsoleMethod( ClientSideTSStatic, stopThread, bool, 3, 3, "(int slot)")
{   
	int slot = dAtoi(argv[2]);   
	if (slot >= 0 && slot < ShapeBase::MaxScriptThreads) 
	{      
		if (object->stopThread(slot))         
			return true;   
	}   
	return false;
}

ConsoleMethod( ClientSideTSStatic, pauseThread, bool, 3, 3, "(int slot)")
{   
	int slot = dAtoi(argv[2]);   
	if (slot >= 0 && slot < ShapeBase::MaxScriptThreads) 
	{      
		if (object->pauseThread(slot))         
			return true;   
	}   
	return false;
}

ConsoleMethod( ClientSideTSStatic, setHidden, void, 3, 3, "(bool hidden)")
{
	object->setHidden(dAtob(argv[3]));
}

void ClientSideTSStatic::setTransform(const MatrixF &mat)
{	
	TargetPos = oldpos = Point3F();	
	TargetRot = oldrot = QuatF();	
	interpolateTargetTick = interpolateStartTick = 0;	
	Parent::setTransform(mat);
}

void ClientSideTSStatic::setTransform2(const MatrixF &mat,U32 targettick)
{	
	const MatrixF& oldmat = getTransform();	
	oldmat.getColumn(3,&oldpos);	
	oldrot = QuatF(oldmat);	
	interpolateStartTick = Sim::getCurrentTime();	
	interpolateTargetTick = interpolateStartTick+targettick;	
	mat.getColumn(3,&TargetPos);	
	TargetRot = AngAxisF(mat);
}

ConsoleMethod( ClientSideTSStatic, setTransform2, void, 3, 4, "Interpolated(Transform T,Optional TargetTickDelay = 32MS)")
{   
	Point3F pos;   
	const MatrixF& tmat = object->getTransform();   
	tmat.getColumn(3,&pos);   
	AngAxisF aa(tmat);   
	dSscanf(argv[2],"%g %g %g %g %g %g %g",           
		&pos.x,&pos.y,&pos.z,&aa.axis.x,&aa.axis.y,&aa.axis.z,&aa.angle);   
	MatrixF mat;   
	aa.setMatrix(&mat);   
	mat.setColumn(3,pos);   
	if(argc > 3)	   
		object->setTransform2(mat,dAtoi(argv[3]));   
	else	   
		object->setTransform2(mat);
}

void ClientSideTSStatic::interpolate(F32 d)
{	
	MatrixF mat;	
	QuatF newrot;	
	Point3F newpos;	
	newpos.interpolate(oldpos,TargetPos,d);	
	newrot.interpolate(oldrot,TargetRot,d);	
	newrot.setMatrix(&mat);	
	mat.setColumn(3,newpos);	
	Parent::setTransform(mat);
}