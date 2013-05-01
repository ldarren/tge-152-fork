// http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=6504
#ifndef _CLIENTSIDETSSTATIC_H_
#define _CLIENTSIDETSSTATIC_H_

#ifndef _TSSTATIC_H_
#include "game/tsStatic.h"
#endif
#include "game/shapeBase.h" // animation mod
//----------------------------------------------------------------------------

class ClientSideTSStatic : public TSStatic
{
	typedef TSStatic Parent;
	bool mHidden;
	U32 lastrenderms;	// animation mod
public:
	DECLARE_CONOBJECT(ClientSideTSStatic);
	static void initPersistFields();

	ClientSideTSStatic();
	~ClientSideTSStatic();

	bool onAdd();
	void onRemove();
	void setHidden(bool hidden);
	bool prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
	
	// animation mod
	struct Thread 
	{      
		/// State of the animation thread.      
		enum State {         Play, Stop, Pause      };      
		TSThread* thread;	///< Pointer to 3space data.      
		U32 state;			///< State of the thread            
							///                        
							///  @see Thread::State      
		S32 sequence;     ///< The animation sequence which is running in this thread.      
		U32 sound;        ///< Handle to sound.      
		bool atEnd;       ///< Are we at the end of this thread?      
		bool forward;     ///< Are we playing the thread forward? (else backwards)   
	};   
	Thread mScriptThread[ShapeBase::MaxScriptThreads];	
	TSShapeInstance* getShapeInstance(){return mShapeInstance;}   
	bool setThreadSequence(U32 slot,S32 seq,bool reset);   
	void updateThread(Thread& st);   
	bool stopThread(U32 slot);   
	bool pauseThread(U32 slot);   
	bool playThread(U32 slot);   
	bool setThreadDir(U32 slot,bool forward);   
	void advanceThreads(U32 curms);   
	void setTransform(const MatrixF &mat);   
	void setTransform2(const MatrixF &mat,U32 targettick = 32);   
	void interpolate(F32);   
	U32 interpolateTargetTick;   
	U32 interpolateStartTick;   
	Point3F TargetPos;   
	QuatF TargetRot;   
	Point3F oldpos;   
	QuatF oldrot;
};


#endif
