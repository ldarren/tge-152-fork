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

#include "rpg/game/rpgAIPlayer.h"
#include "console/consoleInternal.h"
#include "core/realComp.h"
#include "math/mMatrix.h"
#include "game/moveManager.h"
#include "core/bitStream.h"  // <-- MMO Kit

IMPLEMENT_CO_NETOBJECT_V1(rpgAIPlayer);

/**
 * Constructor
 */
rpgAIPlayer::rpgAIPlayer()
{
   mMoveDestination.set( 0.0f, 0.0f, 0.0f );
   mLastLocation.set(-99999.f,-99999.f,-99999.f);  // <-- MMO Kit
   mMoveSpeed = 1.0f;
   mMoveTolerance = 3.25f;  // <-- MMO Kit, changed from 0.25 to 3.25
   mMoveSlowdown = true;
   mMoveState = ModeStop;

   // AFX CODE BLOCK (anim-clip) <<
   // This new member saves the movement state of the AI so that
   // it can be restored after a substituted animation is finished.
   mMoveState_saved = -1;
   // AFX CODE BLOCK (anim-clip) >>

   mFollowObject = 0;  // <-- MMO Kit
   mAimObject = 0;
   mAimLocationSet = false;
   mTargetInLOS = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);

   mTypeMask |= AIObjectType;
   // Begin MMO Kit
   mTypeMask |= PlayerObjectType;
//DARREN_MMO   mTypeMask &= ~HumanObjectType;
   
   mCollisionMoveMask = (TerrainObjectType | InteriorObjectType |
                                       // WaterObjectType       |
                                          VehicleObjectType     |
                                          PhysicalZoneObjectType);
   
   mServerCollisionContactMask = (mCollisionMoveMask |
                                  (ItemObjectType    |
                                   TriggerObjectType |
                                   CorpseObjectType));
   
   mClientCollisionContactMask = mCollisionMoveMask | PhysicalZoneObjectType;
   
   mAvoidFollowObject = false;
   
   mStuckCount = 125;
   
   mZombie = true;
   mSimZombie = true;
   // End MMO Kit
}

/**
 * Destructor
 */
rpgAIPlayer::~rpgAIPlayer()
{
}

// Begin MMO Kit
void rpgAIPlayer::updateDamageState()
{
   // Become a corpse when we're disabled (dead).
   if (mDamageState == Enabled)
      mTypeMask |= AIObjectType;
   else
      mTypeMask &= ~AIObjectType;
   
   Parent::updateDamageState();
}
// End MMO Kit

/**
 * Sets the speed at which this AI moves
 *
 * @param speed Speed to move, default player was 10
 */
void rpgAIPlayer::setMoveSpeed( F32 speed )
{
   mMoveSpeed = getMax(0.0f, getMin( 1.0f, speed ));
}

/**
 * Stops movement for this AI
 */
void rpgAIPlayer::stopMove()
{
   mMoveState = ModeStop;
}

/**
 * Sets how far away from the move location is considered
 * "on target"
 *
 * @param tolerance Movement tolerance for error
 */
void rpgAIPlayer::setMoveTolerance( const F32 tolerance )
{
   mMoveTolerance = getMax( 0.1f, tolerance );
}

/**
 * Sets the location for the bot to run to
 *
 * @param location Point to run to
 */
void rpgAIPlayer::setMoveDestination( const Point3F &location, bool slowdown )
{
   mMoveDestination = location;
   mMoveState = ModeMove;
   mMoveSlowdown = slowdown;
}

// Begin MMO Kit
/**
 * Sets the object for the bot to follow
 *
 * @param object to follow
 */
void rpgAIPlayer::setFollowObject( GameBase *followObject )
{
   mFollowObject = followObject;
   mAimObject = followObject;
}
// End MMO Kit

/**
 * Sets the object the bot is targeting
 *
 * @param targetObject The object to target
 */
void rpgAIPlayer::setAimObject( GameBase *targetObject )
{
   mAimObject = targetObject;
   mTargetInLOS = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}

/**
 * Sets the object the bot is targeting and an offset to add to target location
 *
 * @param targetObject The object to target
 * @param offset       The offest from the target location to aim at
 */
void rpgAIPlayer::setAimObject( GameBase *targetObject, Point3F offset )
{
   mAimObject = targetObject;
   mTargetInLOS = false;
   mAimOffset = offset;
}

/**
 * Sets the location for the bot to aim at
 *
 * @param location Point to aim at
 */
void rpgAIPlayer::setAimLocation( const Point3F &location )
{
   mAimObject = 0;
   mAimLocationSet = true;
   mAimLocation = location;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}

/**
 * Clears the aim location and sets it to the bot's
 * current destination so he looks where he's going
 */
void rpgAIPlayer::clearAim()
{
   mAimObject = 0;
   mAimLocationSet = false;
   mAimOffset = Point3F(0.0f, 0.0f, 0.0f);
}

/**
 * This method calculates the moves for the AI player
 *
 * @param movePtr Pointer to move the move list into
 */
bool rpgAIPlayer::getAIMove(Move *movePtr)
{
   *movePtr = NullMove;
   
   // Begin MMO Kit
   if (!mMoveSpeed)
      return true;
   // End MMO Kit

   // Use the eye as the current position.
   MatrixF eye;
   getEyeTransform(&eye);
   // Begin MMO Kit
   Point3F location = getPosition();
   /* TGE Original Code
   Point3F location = eye.getPosition();
   */
   // End MMO Kit
   Point3F rotation = getRotation();

   // Orient towards the aim point, aim object, or towards
   // our destination.
   if (mAimObject || mAimLocationSet || mMoveState == ModeMove) {

      // Update the aim position if we're aiming for an object
      if (mAimObject)
         mAimLocation = mAimObject->getPosition() + mAimOffset;
      else
         if (!mAimLocationSet)
            mAimLocation = mMoveDestination;

      F32 xDiff = mAimLocation.x - location.x;
      F32 yDiff = mAimLocation.y - location.y;
      
      // Begin MMO Kit
      if (mCanKite && mAvoidFollowObject && mFollowObject)
      {
         xDiff = location.x - mAimLocation.x;
         yDiff = location.y - mAimLocation.y;
      }
      // End MMO Kit
      
      if (!isZero(xDiff) || !isZero(yDiff)) {

         // First do Yaw
         // use the cur yaw between -Pi and Pi
         F32 curYaw = rotation.z;
         while (curYaw > M_2PI)
            curYaw -= M_2PI;
         while (curYaw < -M_2PI)
            curYaw += M_2PI;

         // find the yaw offset
         F32 newYaw = mAtan( xDiff, yDiff );
         F32 yawDiff = newYaw - curYaw;

         // make it between 0 and 2PI
         if( yawDiff < 0.0f )
            yawDiff += M_2PI;
         else if( yawDiff >= M_2PI )
            yawDiff -= M_2PI;

         // now make sure we take the short way around the circle
         if( yawDiff > M_PI )
            yawDiff -= M_2PI;
         else if( yawDiff < -M_PI )
            yawDiff += M_2PI;

         // Begin MMO Kit
         movePtr->yaw = yawDiff * 0.1f;
         /* TGE Original Code
         movePtr->yaw = yawDiff;

         // Next do pitch.
         if (!mAimObject && !mAimLocationSet) {
            // Level out if were just looking at our next way point.
            Point3F headRotation = getHeadRotation();
            movePtr->pitch = -headRotation.x;
         }
         else
         */
         // End MMO Kit
         {
            // This should be adjusted to run from the
            // eye point to the object's center position. Though this
            // works well enough for now.
            F32 vertDist = mAimLocation.z - location.z;
            F32 horzDist = mSqrt(xDiff * xDiff + yDiff * yDiff);
            F32 newPitch = mAtan( horzDist, vertDist ) - ( M_PI / 2.0f );
            if (mFabs(newPitch) > 0.01) {
               Point3F headRotation = getHeadRotation();
               movePtr->pitch = newPitch - headRotation.x;
            }
         }
         
         movePtr->pitch *= 0.1f;  // <-- MMO Kit
      }
   }
   else {
      // Level out if we're not doing anything else
      Point3F headRotation = getHeadRotation();
      movePtr->pitch = -headRotation.x;
   }

   // Move towards the destination
   // Begin MMO Kit
   if (mMoveState == ModeMove || mFollowObject)
   {
      if (mFollowObject)
         mMoveDestination = mFollowObject->getPosition();
   /* TGE Original Code
   if (mMoveState == ModeMove) {
   */
   // End MMO Kit
      F32 xDiff = mMoveDestination.x - location.x;
      F32 yDiff = mMoveDestination.y - location.y;

      // Check if we should mMove, or if we are 'close enough'
      // Begin MMO Kit
      Point3F diff = mMoveDestination - location;
      
      F32 tolerance = 1.0f;
      
      if (mFollowObject)
         tolerance = getScale().lenSquared() * 1.25f;
      else
         diff[2] = 0.f;
      
      if (!(mCanKite && mAvoidFollowObject) && diff.lenSquared() < tolerance)
      {
         mMoveState = ModeStop;
         if (!mFollowObject)
            Con::evaluatef("Py::OnReachDestination(%s);",scriptThis());
         else
            setVelocity(Point3F(0.0,0.0,0.0));
      }
      else
      {
         mMoveState = ModeMove;
      /* TGE Original Code
      if (mFabs(xDiff) < mMoveTolerance && mFabs(yDiff) < mMoveTolerance) {
         mMoveState = ModeStop;
         throwCallback("onReachDestination");
      }
      else {
      */
      // End MMO Kit
         // Build move direction in world space
         if (isZero(xDiff))
            movePtr->y = (location.y > mMoveDestination.y)? -1 : 1;
         else
            if (isZero(yDiff))
               movePtr->x = (location.x > mMoveDestination.x)? -1 : 1;
            else
               if (mFabs(xDiff) > mFabs(yDiff)) {
                  F32 value = mFabs(yDiff / xDiff);
                  movePtr->y = (location.y > mMoveDestination.y)? -value : value;
                  movePtr->x = (location.x > mMoveDestination.x)? -1 : 1;
               }
               else {
                  F32 value = mFabs(xDiff / yDiff);
                  movePtr->x = (location.x > mMoveDestination.x)? -value : value;
                  movePtr->y = (location.y > mMoveDestination.y)? -1 : 1;
               }

         // Rotate the move into object space (this really only needs
         // a 2D matrix)
         Point3F newMove;
         MatrixF moveMatrix;
         moveMatrix.set(EulerF(0, 0, -(rotation.z + movePtr->yaw)));
         moveMatrix.mulV( Point3F( movePtr->x, movePtr->y, 0 ), &newMove );
         movePtr->x = newMove.x;
         movePtr->y = newMove.y;
      
         // Begin MMO Kit
         F32 speed;
         F32 mod = mMoveModifier;
         
         if (mod > 1.f)
            mod = 1.f + ((mod - 1.0f) * .1f);
         
         if (mod < 1.f)
            mod = 1.f - (mod * .1f);
         
         if (mod > 1.20f)
            mod = 1.20f;
         
         if (!mFollowObject)
            if (mod > .5f)
               mod = .5f;
         
         // Clamp water movement
         if (movePtr->y > 0)
         {
            speed = getMax(mDataBlock->maxForwardSpeed * mod * movePtr->y,
                              mDataBlock->maxSideSpeed * mod * mFabs(movePtr->x));
         }
         else
         {
            speed = getMax(mDataBlock->maxBackwardSpeed * mod * mFabs(movePtr->y),
                               mDataBlock->maxSideSpeed * mod * mFabs(movePtr->x));
         }
         
         speed *= .09f;  // eyeballing to player
         
         movePtr->x *= speed * mMoveSpeed;
         movePtr->y *= speed * mMoveSpeed;
         
         if (mCanKite && mAvoidFollowObject && mFollowObject)
         {
            movePtr->x = -movePtr->x;
            movePtr->y = -movePtr->y;
         }
         
         // We should check to see if we are stuck...
         F32 m = mMoveSpeed * mMoveModifier;
         if (m && mDamageState == Enabled && !mSimZombie)
         {
            Point3F v, v1;
            v = location;
            v1 = mLastLocation;
            v.z = v1.z = 0.f;
            if ((v - v1).len() < .1f * m)
            {
               mStuckCount -= 3;
               if (mStuckCount <= 0)
               {
                  mStuckCount = 125;
                  if (mFollowObject)
                     setTransform(mFollowObject->getTransform());
                  else
                     SceneObject::setPosition(mMoveDestination);
                  //Con::evaluatef("Py::OnStuck(%s);",scriptThis());
                  //throwCallback("onMoveStuck");
                  //mMoveState = ModeStop;
               }
            }
            else
            {
               mStuckCount += 1;
               if (mStuckCount > 125)
                  mStuckCount = 125;
            }
         }
         /* TGE Original Code
         // Set movement speed.  We'll slow down once we get close
         // to try and stop on the spot...
         if (mMoveSlowdown) {
            F32 speed = mMoveSpeed;
            F32 dist = mSqrt(xDiff*xDiff + yDiff*yDiff);
            F32 maxDist = 5;
            if (dist < maxDist)
               speed *= dist / maxDist;
            movePtr->x *= speed;
            movePtr->y *= speed;
         }
		 else {
            movePtr->x *= mMoveSpeed;
            movePtr->y *= mMoveSpeed;
		 }

         // We should check to see if we are stuck...
         if (location == mLastLocation) {
            throwCallback("onMoveStuck");
            mMoveState = ModeStop;
         }
         */
         // End MMO Kit
      }
   }

   // Test for target location in sight if it's an object. The LOS is
   // run from the eye position to the center of the object's bounding,
   // which is not very accurate.
   // Begin MMO Kit
   /* TGE Original Code
   if (mAimObject) {
      MatrixF eyeMat;
      getEyeTransform(&eyeMat);
      eyeMat.getColumn(3,&location);
      Point3F targetLoc = mAimObject->getBoxCenter();

      // This ray ignores non-static shapes. Cast Ray returns true
      // if it hit something.
      RayInfo dummy;
      if (getContainer()->castRay( location, targetLoc,
            InteriorObjectType | StaticShapeObjectType | StaticObjectType |
            TerrainObjectType, &dummy)) {
         if (mTargetInLOS) {
            throwCallback( "onTargetExitLOS" );
            mTargetInLOS = false;
         }
      }
      else
         if (!mTargetInLOS) {
            throwCallback( "onTargetEnterLOS" );
            mTargetInLOS = true;
         }
   }
   */
   // End MMO Kit

   // Replicate the trigger state into the move so that
   // triggers can be controlled from scripts.
   for( int i = 0; i < MaxTriggerKeys; i++ )
      movePtr->trigger[i] = getImageTriggerState(i);

   mLastLocation = location;  // <-- MMO Kit
   
   return true;
}

/**
 * Utility function to throw callbacks. Callbacks always occure
 * on the datablock class.
 *
 * @param name Name of script function to call
 */
void rpgAIPlayer::throwCallback( const char *name )
{
   Con::executef(getDataBlock(), 2, name, scriptThis());
}


// --------------------------------------------------------------------------------------------
// Console Functions
// --------------------------------------------------------------------------------------------

ConsoleMethod( rpgAIPlayer, stop, void, 2, 2, "()"
              "Stop moving.")
{
   object->stopMove();
}

ConsoleMethod( rpgAIPlayer, clearAim, void, 2, 2, "()"
              "Stop aiming at anything.")
{
   object->clearAim();
}

ConsoleMethod( rpgAIPlayer, setMoveSpeed, void, 3, 3, "( float speed )"
              "Sets the move speed for an AI object.")
{
   object->setMoveSpeed( dAtof( argv[2] ) );
}

ConsoleMethod( rpgAIPlayer, setMoveDestination, void, 3, 4, "(Point3F goal, bool slowDown=true)"
              "Tells the AI to move to the location provided.")
{
   Point3F v( 0.0f, 0.0f, 0.0f );
   dSscanf( argv[2], "%g %g %g", &v.x, &v.y, &v.z );
   bool slowdown = (argc > 3)? dAtob(argv[3]): true;
   object->setMoveDestination( v, slowdown);
}

ConsoleMethod( rpgAIPlayer, getMoveDestination, const char *, 2, 2, "()"
              "Returns the point the AI is set to move to.")
{
   Point3F movePoint = object->getMoveDestination();

   char *returnBuffer = Con::getReturnBuffer( 256 );
   dSprintf( returnBuffer, 256, "%g %g %g", movePoint.x, movePoint.y, movePoint.z );

   return returnBuffer;
}

ConsoleMethod( rpgAIPlayer, setAimLocation, void, 3, 3, "( Point3F target )"
              "Tells the AI to aim at the location provided.")
{
   Point3F v( 0.0f,0.0f,0.0f );
   dSscanf( argv[2], "%g %g %g", &v.x, &v.y, &v.z );

   object->setAimLocation( v );
}

ConsoleMethod( rpgAIPlayer, getAimLocation, const char *, 2, 2, "()"
              "Returns the point the AI is aiming at.")
{
   Point3F aimPoint = object->getAimLocation();

   char *returnBuffer = Con::getReturnBuffer( 256 );
   dSprintf( returnBuffer, 256, "%g %g %g", aimPoint.x, aimPoint.y, aimPoint.z );

   return returnBuffer;
}

ConsoleMethod( rpgAIPlayer, setAimObject, void, 3, 4, "( GameBase obj, [Point3F offset] )"
              "Sets the bot's target object. Optionally set an offset from target location.")
{
   Point3F off( 0.0f, 0.0f, 0.0f );

   // Find the target
   GameBase *targetObject;
   if( Sim::findObject( argv[2], targetObject ) )
   {
      if (argc == 4)
         dSscanf( argv[3], "%g %g %g", &off.x, &off.y, &off.z );

      object->setAimObject( targetObject, off );
   }
   else
      object->setAimObject( 0, off );
}

ConsoleMethod( rpgAIPlayer, getAimObject, S32, 2, 2, "()"
              "Gets the object the AI is targeting.")
{
   GameBase* obj = object->getAimObject();
   return obj? obj->getId(): -1;
}


// Begin MMO Kit
ConsoleMethod( rpgAIPlayer, setFollowObject, void, 3, 3, "( GameBase obj )"
               "Sets the bot's target object.")
{
   // Find the target
   GameBase *followObject;
   if( Sim::findObject( argv[2], followObject ) )
      object->setFollowObject( followObject );
   else
      object->setFollowObject( 0 );
}

ConsoleMethod( rpgAIPlayer, getFollowObject, S32, 2, 2, "()"
               "Gets the object the AI is targeting.")
{
   GameBase* obj = object->getFollowObject();
   return obj ? obj->getId() : -1;
}

ConsoleMethod( rpgAIPlayer, setAvoidFollowObject, void, 3, 3, "()"
               "Sets the object the AI should avoid.")
{
   object->setAvoidFollowObject(dAtob(argv[2]));
}
// End MMO Kit


// AFX CODE BLOCK (anim-clip) <<
// These changes coordinate with anim-clip mods to parent class, Player.

// New method, restartMove(), restores the AIPlayer to its normal move-state
// following animation overrides from AFX. The tag argument is used to match
// the latest override and prevents interruption of overlapping animation
// overrides. See related anim-clip changes in Player.[h,cc].
void rpgAIPlayer::restartMove(U32 tag, bool is_death_anim)
{
  if (tag != 0 && tag == last_anim_tag)
  {
    if (mMoveState_saved != -1)
    {
      mMoveState = (MoveState) mMoveState_saved;
      mMoveState_saved = -1;
    }
    last_anim_tag = 0;
    anim_clip_flags &= ~ANIM_OVERRIDDEN;
    
    // Begin MMO Kit
    /* ORIGINAL CODE
    if (mDamageState != Enabled)
    {
      if (!is_death_anim)
      {
        // this is a bit hardwired and desperate,
        // but if he's dead he needs to look like it.
        setActionThread("death10", false, false, false);
      }
    }
    */
    // End MMO Kit
  }
}

// New method, saveMoveState(), stores the current movement state
// so that it can be restored when restartMove() is called.
void rpgAIPlayer::saveMoveState()
{
  if (mMoveState_saved == -1)
    mMoveState_saved = (S32) mMoveState;
}
// AFX CODE BLOCK (anim-clip) >>

// Begin MMO Kit
U32 rpgAIPlayer::packUpdate(NetConnection *con, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(con, mask, stream);
   
   /*
   if (mMoveState == ModeStop && !(mTypeMask&CorpseObjectType))
      mTypeMask |= StaticShapeObjectType;
   else
      mTypeMask &= ~StaticShapeObjectType;
   */
   
   S32 moveState = mMoveState;
   stream->write(moveState);
   
   return retMask;
}

void rpgAIPlayer::unpackUpdate(NetConnection *con, BitStream *stream)
{
   Parent::unpackUpdate(con,stream);
   
   S32 moveState;
   stream->read(&moveState);
   
   mMoveState = (rpgAIPlayer::MoveState)moveState;
   
   /*
   if (mMoveState == ModeStop && !(mTypeMask&CorpseObjectType))
      mTypeMask |= StaticShapeObjectType;
   else
      mTypeMask &= ~StaticShapeObjectType;
   */
}
// End MMO Kit
