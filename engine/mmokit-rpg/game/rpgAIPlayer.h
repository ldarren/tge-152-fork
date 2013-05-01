//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

/*

MMO Kit
-------



*/

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//        anim-clip -- sequence selection by afx effects
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _RPG_AIPLAYER_H_
#define _RPG_AIPLAYER_H_

#ifndef _RPG_PLAYER_H_
#include "rpg/game/rpgPlayer.h"
#endif


class rpgAIPlayer : public rpgPlayer {

	typedef rpgPlayer Parent;

public:
	enum MoveState {
		ModeStop,
		ModeMove,
		ModeStuck,
	};

private:
		MoveState mMoveState;
		F32 mMoveSpeed;
		F32 mMoveTolerance;                 // Distance from destination before we stop
		Point3F mMoveDestination;           // Destination for movement
      Point3F mLastLocation;              // For stuck check
      S32     mStuckCount;  // <-- MMO Kit
      bool mMoveSlowdown;                 // Slowdown as we near the destination

		SimObjectPtr<GameBase> mAimObject; // Object to point at, overrides location
		bool mAimLocationSet;               // Has an aim location been set?
		Point3F mAimLocation;               // Point to look at
      bool mTargetInLOS;                  // Is target object visible?
      
      // Begin MMO Kit
      SimObjectPtr<GameBase> mFollowObject;
      bool mAvoidFollowObject;
      // End MMO Kit

      Point3F mAimOffset;

      // Utility Methods
      void throwCallback( const char *name );
public:
		DECLARE_CONOBJECT( rpgAIPlayer );

		rpgAIPlayer();
      ~rpgAIPlayer();
      
      virtual void updateDamageState();  // <-- MMO Kit

		virtual bool getAIMove( Move *move );

		// Targeting and aiming sets/gets
		void setAimObject( GameBase *targetObject );
      void setAimObject( GameBase *targetObject, Point3F offset );
		GameBase* getAimObject() const  { return mAimObject; }
		void setAimLocation( const Point3F &location );
		Point3F getAimLocation() const { return mAimLocation; }
		void clearAim();

		// Movement sets/gets
		void setMoveSpeed( const F32 speed );
		F32 getMoveSpeed() const { return mMoveSpeed; }
		void setMoveTolerance( const F32 tolerance );
		F32 getMoveTolerance() const { return mMoveTolerance; }
		void setMoveDestination( const Point3F &location, bool slowdown );
		Point3F getMoveDestination() const { return mMoveDestination; }
		void stopMove();
      
      // Begin MMO Kit
      void setAvoidFollowObject(bool yes) { mAvoidFollowObject = yes; }
      void setFollowObject( GameBase *followObject );
      GameBase* getFollowObject() const  { return mFollowObject; }
      
      U32  packUpdate  (NetConnection *conn, U32 mask, BitStream *stream);
      void unpackUpdate(NetConnection *conn,           BitStream *stream);
      // End MMO Kit

    // AFX CODE BLOCK (anim-clip) <<
    // New method, restartMove(), restores the AIPlayer to its normal move-state
    // following animation overrides from AFX. The tag argument is used to match
    // the latest override and prevents interruption of overlapping animation
    // overrides.
    // New method, saveMoveState(), stores the current movement state
    // so that it can be restored when restartMove() is called.
    // See related anim-clip changes in Player.[h,cc].
private:
    S32 mMoveState_saved;
public:
    void restartMove(U32 tag, bool is_death_anim);
    void saveMoveState();
    // AFX CODE BLOCK (anim-clip) >>
};

#endif
