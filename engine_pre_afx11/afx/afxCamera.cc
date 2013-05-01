//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// afxCamera implements a modified camera for demonstrating a third person camera style
// which is more common to RPG games than the standard FPS style camera. For the most part,
// it is a hybrid of the standard TGE camera and the third person mode of the Advanced Camera
// resource, authored by Thomas "Man of Ice" Lund. This camera implements the bare minimum
// required for demonstrating an RPG style camera and leaves tons of room for improvement. 
// It should be replaced with a better camera if possible.
//
// Advanced Camera Resource by Thomas "Man of Ice" Lund:
//   http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=5471
//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/gameBase.h"
#include "dgl/dgl.h"
#include "math/mathUtils.h"
#include "game/gameConnection.h"
#include "game/camera.h"
#include "game/player.h"
#include "math/mathIO.h"
#include "editor/editor.h"

#include "afx/afxCamera.h"

#define MaxPitch      1.3962
#define CameraRadius  0.05;

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxCameraData

IMPLEMENT_CO_DATABLOCK_V1(afxCameraData);

void afxCameraData::initPersistFields()
{
  Parent::initPersistFields();
}

void afxCameraData::packData(BitStream* stream)
{
  Parent::packData(stream);
}

void afxCameraData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxCamera

IMPLEMENT_CO_NETOBJECT_V1(afxCamera);

afxCamera::afxCamera()
{
  mNetFlags.clear(Ghostable);
  mTypeMask |= CameraObjectType;
  delta.pos = Point3F(0,0,100);
  delta.rot = Point3F(0,0,0);
  delta.posVec = delta.rotVec = VectorF(0,0,0);
  mObjToWorld.setColumn(3,delta.pos);
  mRot = delta.rot;
  
  mMinOrbitDist = 0;
  mMaxOrbitDist = 0;
  mCurOrbitDist = 0;
  mOrbitObject = NULL;
  mPosition.set(0.f, 0.f, 0.f);
  mObservingClientObject = false;
  mode = FlyMode;
  
  cam_subject = NULL;
  coi_offset.set(0, 0, 2);
  cam_offset.set(0, 0, 0);
      
  flymode_saved = false;
  third_person_snap_s = true;
  third_person_snap_c = true;
  flymode_saved_pos.zero();

  blockers_mask_3pov = InteriorObjectType | TerrainObjectType;
}

afxCamera::~afxCamera()
{
}

//----------------------------------------------------------------------------



void afxCamera::cam_update(F32 dt, bool on_server) 
{
  if (mode == ThirdPersonMode && cam_subject)
    cam_update_3pov(dt, on_server);
}

void afxCamera::set_cam_pos(const Point3F& pos,const Point3F& rot)
{
   MatrixF xRot, zRot;
   xRot.set(EulerF(rot.x, 0, 0));
   zRot.set(EulerF(0, 0, rot.z));
   MatrixF temp;
   temp.mul(zRot, xRot);
   temp.setColumn(3, pos);
   Parent::setTransform(temp);
   mRot = rot;
}


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

Point3F &afxCamera::getPosition()
{
   static Point3F position;
   mObjToWorld.getColumn(3, &position);
   return position;
}

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//    NEW Observer Code
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void afxCamera::setFlyMode()
{
  mode = FlyMode;
  if (flymode_saved)
    snapToPosition(flymode_saved_pos);
  
  if (bool(mOrbitObject)) 
  {
    clearProcessAfter();
    clearNotify(mOrbitObject);
  }
  mOrbitObject = NULL;
}

void afxCamera::setOrbitMode(GameBase *obj, Point3F &pos, AngAxisF &rot, F32 minDist, F32 maxDist, F32 curDist, bool ownClientObject)
{
   mObservingClientObject = ownClientObject;

   rot;
   if(bool(mOrbitObject)) {
      clearProcessAfter();
      clearNotify(mOrbitObject);
   }
   mOrbitObject = obj;
   if(bool(mOrbitObject))
   {
      processAfter(mOrbitObject);
      deleteNotify(mOrbitObject);
      mOrbitObject->getWorldBox().getCenter(&mPosition);
      mode = OrbitObjectMode;
   }
   else
   {
      mode = OrbitPointMode;
      mPosition = pos;
   }

   QuatF q(rot);
   MatrixF tempMat(true);
   q.setMatrix(&tempMat);
   Point3F dir;
   tempMat.getColumn(1, &dir);

   set_cam_pos(mPosition, dir);

   mMinOrbitDist = minDist;
   mMaxOrbitDist = maxDist;
   mCurOrbitDist = curDist;
}


void afxCamera::validateEyePoint(F32 pos, MatrixF *mat)
{
   if (pos != 0) {
      // Use the eye transform to orient the camera
      Point3F dir;
      mat->getColumn(1, &dir);
      pos *= mMaxOrbitDist - mMinOrbitDist;
      // Use the camera node's pos.
      Point3F startPos;
      Point3F endPos;
      mObjToWorld.getColumn(3,&startPos);

      // Make sure we don't extend the camera into anything solid
      if(mOrbitObject)
         mOrbitObject->disableCollision();
      disableCollision();
      RayInfo collision;
      U32 mask = TerrainObjectType |
                 InteriorObjectType |
                 WaterObjectType |
                 StaticShapeObjectType |
                 PlayerObjectType |
                 ItemObjectType |
                 VehicleObjectType;

      Container* pContainer = isServerObject() ? &gServerContainer : &gClientContainer;
      if (!pContainer->castRay(startPos, startPos - dir * 2.5 * pos, mask, &collision))
         endPos = startPos - dir * pos;
      else
      {
         float dot = mDot(dir, collision.normal);
         if(dot > 0.01)
         {
            float colDist = mDot(startPos - collision.point, dir) - (1 / dot) * CameraRadius;
            if(colDist > pos)
               colDist = pos;
            if(colDist < 0)
               colDist = 0;
            endPos = startPos - dir * colDist;
         }
         else
            endPos = startPos - dir * pos;
      }
      mat->setColumn(3,endPos);
      enableCollision();
      if(mOrbitObject)
         mOrbitObject->enableCollision();
   }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//





// Sets the position and calculates rotation
void afxCamera::snapToPosition(const Point3F& tPos) 
{
  MatrixF transMat;
  if (cam_subject) 
  {
    MatrixF objToWorld = cam_subject->getRenderTransform();
    Point3F objPos;
    objToWorld.mulP(coi_offset, &objPos);

    VectorF dirVec = objPos - tPos;
    dirVec.normalize();

    MathUtils::getAnglesFromVector(dirVec, mRot.z, mRot.x);
    mRot.x = 0 - mRot.x;

    transMat = MathUtils::createOrientFromDir(dirVec);
  } 

  transMat.setColumn(3, tPos);
  Parent::setTransform(transMat);
}

void afxCamera::setCameraSubject(SceneObject* new_subject)
{
  // cleanup any existing chase subject
  if (cam_subject) 
  {
    if (dynamic_cast<GameBase*>(cam_subject))
      clearProcessAfter();
    clearNotify(cam_subject);
  }
  
  cam_subject = new_subject;
  
  // set associations with new chase subject 
  if (cam_subject) 
  {
    if (dynamic_cast<GameBase*>(cam_subject))
      processAfter((GameBase*)cam_subject);
    deleteNotify(cam_subject);
  }

  mode = (cam_subject) ? ThirdPersonMode : FlyMode;
  setMaskBits(SubjectMask);
}

void afxCamera::setThirdPersonOffset(Point3F offset) 
{
  if (offset.y != cam_offset.y && isClientObject())
  {
#ifdef AFX_TGE_1_3
    // this auto switches to/from first person 
    if (GameConnection::isFirstPersonMode())
    {
      if (offset.y <= -1.0f)
        GameConnection::setFirstPersonMode(false);
    }
    else
    {
      if (offset.y > -1.0f)
        GameConnection::setFirstPersonMode(true);
    }
#else // (TORQUE_GAME_ENGINE >= 1400)
    GameConnection* conn = GameConnection::getConnectionToServer();
    if (conn)
    {
      // this auto switches to/from first person 
      if (conn->isFirstPerson())
      {
        if (offset.y <= -1.0f)
          conn->setFirstPerson(false);
      }
      else
      {
        if (offset.y > -1.0f)
          conn->setFirstPerson(true);
      }
    }
#endif
  }

  cam_offset = offset;
}

void afxCamera::setThirdPersonMode()
{
  mode = ThirdPersonMode;
  flymode_saved_pos = getPosition();
  flymode_saved = true;
  third_person_snap_s = true;
}

void afxCamera::setThirdPersonSnap()
{
  if (mode == ThirdPersonMode)
    third_person_snap_s = true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Console Methods

static char buffer[100];

ConsoleMethod(afxCamera, setOrbitMode, void, 7, 8, 
  "(GameBase orbitObject, transform mat, float minDistance, float maxDistance, float curDistance, bool ownClientObject)"
  "Set the camera to orbit around some given object.\n\n"
  "@param   orbitObject  Object we want to orbit.\n"
  "@param   mat          A set of fields: posX posY posZ aaX aaY aaZ aaTheta\n"
  "@param   minDistance  Minimum distance to keep from object.\n"
  "@param   maxDistance  Maximum distance to keep from object.\n"
  "@param   curDistance  Distance to set initially from object.\n"
  "@param   ownClientObj Are we observing an object owned by us?")
{
  Point3F pos;
  AngAxisF aa;
  F32 minDis, maxDis, curDis;
  
  GameBase *orbitObject = NULL;
  if(Sim::findObject(argv[2],orbitObject) == false)
  {
    Con::warnf("Cannot orbit non-existing object.");
    object->setFlyMode();
    return;
  }
  
  dSscanf(argv[3],"%f %f %f %f %f %f %f",
    &pos.x,&pos.y,&pos.z,&aa.axis.x,&aa.axis.y,&aa.axis.z,&aa.angle);
  minDis = dAtof(argv[4]);
  maxDis = dAtof(argv[5]);
  curDis = dAtof(argv[6]);
  
  object->setOrbitMode(orbitObject, pos, aa, minDis, maxDis, curDis, (argc == 8) ? dAtob(argv[7]) : false);
}

ConsoleMethod( afxCamera, setFlyMode, void, 2, 2, "()" "Set the camera to be able to fly freely.")
{
  object->setFlyMode();
}

ConsoleMethod( afxCamera, getPosition, const char *, 2, 2, "()"
              "Get the position of the camera.\n\n"
              "@returns A string of form \"x y z\".")
{ 
  Point3F& pos = object->getPosition();
  dSprintf(buffer, sizeof(buffer),"%f %f %f",pos.x,pos.y,pos.z);
  return buffer;
}

ConsoleMethod(afxCamera, setCameraSubject, bool, 3, 3, "") 
{   
  SceneObject* subject;
  if (!Sim::findObject(argv[2], subject))
  {
    Con::errorf("Camera subject \"%s\" not found.", argv[2]);
    return false;
  }
  
  object->setCameraSubject(subject);
  
  return true;
}

ConsoleMethod(afxCamera, setThirdPersonOffset, void, 3, 3, "(Point3F offset)") 
{
  Point3F pos; dSscanf(argv[2], "%f %f %f", &pos.x, &pos.y, &pos.z);
  object->setThirdPersonOffset(pos);
}

ConsoleMethod(afxCamera, getThirdPersonOffset, const char *, 2, 2, "()")
{
  Point3F& pos = object->getThirdPersonOffset();
  dSprintf(buffer, sizeof(buffer),"%f %f %f",pos.x,pos.y,pos.z);
  return buffer;
}

ConsoleMethod(afxCamera, setThirdPersonMode, void, 2, 2, "()")
{
  object->setThirdPersonMode();
}

ConsoleMethod(afxCamera, setThirdPersonSnap, void, 2, 2, "()")
{
  object->setThirdPersonSnap();
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// 3POV SECTION

void afxCamera::cam_update_3pov(F32 dt, bool on_server) 
{
  Point3F	goal_pos;
  Point3F curr_pos = getRenderPosition();
  MatrixF	xfm = cam_subject->getRenderTransform();
  Point3F coi = cam_subject->getRenderPosition() + coi_offset;

  // for player subjects, pitch is adjusted
  Player*	player_subj =	dynamic_cast<Player*>(cam_subject);
  if (player_subj) 
  {
    // rotate xfm by amount of head pitch in player
    F32	head_pitch = player_subj->getHeadRotation().x;
    MatrixF	head_pitch_mtx(EulerF(head_pitch,0,0));
    xfm.mul(head_pitch_mtx);

    VectorF	behind_vec(0, cam_offset.y, 0);
    xfm.mulP(behind_vec, &goal_pos);
    goal_pos.z += cam_offset.z;
  }
  // for non-player subjects, camera will follow, but pitch won't adjust.
  else 
  {
    xfm.mulP(cam_offset, &goal_pos);
  }

  // avoid view occlusion
  if (avoid_blocked_view(coi, goal_pos, goal_pos) && !on_server)
  {
    // snap to final position if path to goal is blocked
    if (test_blocked_line(curr_pos, goal_pos))
      third_person_snap_c = true;
  }

  // place camera into its final position	

  // speed factor values
  //   15 -- tight
  //   10 -- normal
  //    5 -- loose
  //    1 -- very loose
  F32 speed_factor = 8.0f;
  F32 time_inc = 1.0f/speed_factor;

  // snap to final position
  if (on_server || (third_person_snap_c || dt > time_inc))
  {
    snapToPosition(goal_pos);
    third_person_snap_c = false;
    return;
  }
  // interpolate to final position
  else
  {
    // interpretation: always move a proportion of the distance
    // from current location to destination that would cover the
    // entire distance in time_inc duration at constant velocity.
    F32 t = (dt >= time_inc) ? 1.0f : dt*speed_factor;
    snapToPosition(goal_pos*t + curr_pos*(1.0-t));
  }
}

// See if the camera view is occluded by certain objects, 
// and move the camera closer to the subject in that case
bool afxCamera::avoid_blocked_view(const Point3F& startpos, const Point3F& endpos, Point3F& newpos) 
{ 
  // cast ray to check for intersection with potential blocker objects
  RayInfo hit_info;
  if (!getContainer()->castRay(startpos, endpos, blockers_mask_3pov, &hit_info)) 
  {
    // no hit: just return original endpos
    newpos = endpos;
    return false;
  }

	// did hit: return the hit location nudged forward slightly
  // to avoid seeing clipped portions of blocking object.
	Point3F sight_line = startpos - hit_info.point;
  sight_line.normalize();
  newpos = hit_info.point + sight_line*0.4f;

  return true;
}

bool afxCamera::test_blocked_line(const Point3F& startpos, const Point3F& endpos) 
{ 
  RayInfo hit_info;
  return getContainer()->castRay(startpos, endpos, blockers_mask_3pov, &hit_info);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// STD OVERRIDES SECTION

bool afxCamera::onAdd()
{
  if(!Parent::onAdd())
    return false;

  mObjBox.max = mObjScale;
  mObjBox.min = mObjScale;
  mObjBox.min.neg();
  resetWorldBox();

  addToScene();

  return true;
}

void afxCamera::onRemove()
{
  removeFromScene();

  Parent::onRemove();
}

void afxCamera::onDeleteNotify(SimObject *obj)
{
  Parent::onDeleteNotify(obj);

  if (obj == (SimObject*)mOrbitObject)
  {
    mOrbitObject = NULL;
    if (mode == OrbitObjectMode)
      mode = OrbitPointMode;
  }

  if (obj == cam_subject)
  {
    cam_subject = NULL;
  }
}

void afxCamera::advanceTime(F32 dt) 
{
  Parent::advanceTime(dt);

  cam_update(dt, false);
}

void afxCamera::processTick(const Move* move)
{
  Parent::processTick(move);
  Point3F vec,pos;

  // move will be NULL unless camera becomes the control object as in FlyMode
  if (move) 
  {
    // MODE SWITCH //
    if (gEditingMission && mode != FlyMode)
      setFlyMode();

    // UPDATE ORIENTATION //
    delta.rotVec = mRot;
    mObjToWorld.getColumn(3, &delta.posVec);
    mRot.x = mClampF(mRot.x + move->pitch, -MaxPitch, MaxPitch);
    mRot.z += move->yaw;

    // ORBIT MODE // 
    if (mode == OrbitObjectMode || mode == OrbitPointMode)
    {
      if(mode == OrbitObjectMode && bool(mOrbitObject)) 
      {
        // If this is a shapebase, use its render eye transform
        // to avoid jittering.
        GameBase *castObj = mOrbitObject;
        ShapeBase* shape = dynamic_cast<ShapeBase*>(castObj);
        if( shape != NULL ) {
          MatrixF ret;
          shape->getRenderEyeTransform( &ret );
          mPosition = ret.getPosition();
        } 
        else 
        {
          // Hopefully this is a static object that doesn't move,
          // because the worldbox doesn't get updated between ticks.
          mOrbitObject->getWorldBox().getCenter(&mPosition);
        }
      }
      set_cam_pos(mPosition, mRot);
      validateEyePoint(1.0f, &mObjToWorld);
      pos = mPosition;
    }

    // NON-ORBIT MODE (FLY MODE) //
    else // if (mode == FlyMode)
    {
      // Update pos
      bool faster = move->trigger[0] || move->trigger[1];
      F32 scale = Camera::getMovementSpeed() * (faster + 1);

      mObjToWorld.getColumn(3,&pos);
      mObjToWorld.getColumn(0,&vec);
      pos += vec * move->x * TickSec * scale;
      mObjToWorld.getColumn(1,&vec);
      pos += vec * move->y * TickSec * scale;
      mObjToWorld.getColumn(2,&vec);
      pos += vec * move->z * TickSec * scale;
      set_cam_pos(pos,mRot);
    }

    // If on the client, calc delta for backstepping
    if (isClientObject()) 
    {
      delta.pos = pos;
      delta.rot = mRot;
      delta.posVec = delta.posVec - delta.pos;
      delta.rotVec = delta.rotVec - delta.rot;
    }
    else
    {
      setMaskBits(MoveMask);
    }
  }
  else // if (!move)
  {
    if (isServerObject())
      cam_update(1.0/32.0, true);
  }

  if (getControllingClient() && mContainer)
    updateContainer();
}

void afxCamera::interpolateTick(F32 dt)
{
  Parent::interpolateTick(dt);

  if (mode == ThirdPersonMode)
    return;

  Point3F rot = delta.rot + delta.rotVec * dt;

  if(mode == OrbitObjectMode || mode == OrbitPointMode)
  {
    if(mode == OrbitObjectMode && bool(mOrbitObject))
    {
      // If this is a shapebase, use its render eye transform
      // to avoid jittering.
      GameBase *castObj = mOrbitObject;
      ShapeBase* shape = dynamic_cast<ShapeBase*>(castObj);
      if( shape != NULL ) 
      {
        MatrixF ret;
        shape->getRenderEyeTransform( &ret );
        mPosition = ret.getPosition();
      } 
      else 
      {
        // Hopefully this is a static object that doesn't move,
        // because the worldbox doesn't get updated between ticks.
        mOrbitObject->getWorldBox().getCenter(&mPosition);
      }
    }
    set_cam_pos(mPosition, rot);
    validateEyePoint(1.0f, &mObjToWorld);
  }
  else 
  {
    // NOTE - posVec is 0,0,0 unless cam is control-object and process tick is
    // updating the delta
    Point3F pos = delta.pos + delta.posVec * dt;
    set_cam_pos(pos,rot);
  }
}

void afxCamera::writePacketData(GameConnection *connection, BitStream *bstream)
{
  // Update client regardless of status flags.
  Parent::writePacketData(connection, bstream);

  Point3F pos; mObjToWorld.getColumn(3, &pos);
  bstream->setCompressionPoint(pos);                                      // SET COMPRESSION POINT
  mathWrite(*bstream, pos);                                               // SND POS
  bstream->write(mRot.x);                                                 // SND X ROT
  bstream->write(mRot.z);                                                 // SND Z ROT
  mathWrite(*bstream, cam_offset);                                        // SND CAM_OFFSET

  U32 writeMode = mode;
  Point3F writePos = mPosition;
  S32 gIndex = -1;
  if (mode == OrbitObjectMode)
  {
    gIndex = bool(mOrbitObject) ? connection->getGhostIndex(mOrbitObject): -1;
    if(gIndex == -1)
    {
      writeMode = OrbitPointMode;
      mOrbitObject->getWorldBox().getCenter(&writePos);
    }
  }

  bstream->writeRangedU32(writeMode, CameraFirstMode, CameraLastMode);    // SND MODE
  if (writeMode == ThirdPersonMode)
  {
    bstream->write(third_person_snap_s);                                  // SND SNAP
    third_person_snap_s = false;
  }

  if (writeMode == OrbitObjectMode || writeMode == OrbitPointMode)
  {
    bstream->write(mMinOrbitDist);                                        // SND ORBIT MIN DIST
    bstream->write(mMaxOrbitDist);                                        // SND ORBIT MAX DIST
    bstream->write(mCurOrbitDist);                                        // SND ORBIT CURR DIST
    if(writeMode == OrbitObjectMode)
    {
      bstream->writeFlag(mObservingClientObject);                         // SND OBSERVING CLIENT OBJ
      bstream->writeInt(gIndex, NetConnection::GhostIdBitSize);           // SND ORBIT OBJ
    }
    if (writeMode == OrbitPointMode)
      bstream->writeCompressedPoint(writePos);                            // WRITE COMPRESSION POINT
  }
}

void afxCamera::readPacketData(GameConnection *connection, BitStream *bstream)
{
  Parent::readPacketData(connection, bstream);

  Point3F pos,rot;
  mathRead(*bstream, &pos);                                               // RCV POS
  bstream->setCompressionPoint(pos);
  bstream->read(&rot.x);                                                  // RCV X ROT
  bstream->read(&rot.z);                                                  // RCV Z ROT

  Point3F new_cam_offset;
  mathRead(*bstream, &new_cam_offset);                                    // RCV CAM_OFFSET
  setThirdPersonOffset(new_cam_offset);

  GameBase* obj = 0;
  mode = bstream->readRangedU32(CameraFirstMode,                          // RCV MODE
    CameraLastMode);
  if (mode == ThirdPersonMode)
  {
    bool snap; bstream->read(&snap);
    if (snap)
      third_person_snap_c = true;
  }

  mObservingClientObject = false;
  if (mode == OrbitObjectMode || mode == OrbitPointMode) {
    bstream->read(&mMinOrbitDist);
    bstream->read(&mMaxOrbitDist);
    bstream->read(&mCurOrbitDist);

    if(mode == OrbitObjectMode)
    {
      mObservingClientObject = bstream->readFlag();
      S32 gIndex = bstream->readInt(NetConnection::GhostIdBitSize);
      obj = static_cast<GameBase*>(connection->resolveGhost(gIndex));
    }
    if (mode == OrbitPointMode)
      bstream->readCompressedPoint(&mPosition);
  }
  if (obj != (GameBase*)mOrbitObject) {
    if (mOrbitObject) {
      clearProcessAfter();
      clearNotify(mOrbitObject);
    }
    mOrbitObject = obj;
    if (mOrbitObject) {
      processAfter(mOrbitObject);
      deleteNotify(mOrbitObject);
    }
  }

  if (mode == ThirdPersonMode)
    return;

  set_cam_pos(pos,rot);
  delta.pos = pos;
  delta.rot = rot;
  delta.rotVec.set(0,0,0);
  delta.posVec.set(0,0,0);
}

U32 afxCamera::packUpdate(NetConnection* conn, U32 mask, BitStream *bstream)
{
  U32 retMask = Parent::packUpdate(conn,mask,bstream);

  // The rest of the data is part of the control object packet update.
  // If we're controlled by this client, we don't need to send it.
  //if(bstream->writeFlag(getControllingClient() == conn && !(mask & InitialUpdateMask)))
  //   return 0;

  if (bstream->writeFlag(mask & MoveMask)) {
    Point3F pos;
    mObjToWorld.getColumn(3,&pos);
    bstream->write(pos.x);
    bstream->write(pos.y);
    bstream->write(pos.z);
    bstream->write(mRot.x);
    bstream->write(mRot.z);
  }

  if (bstream->writeFlag(mask & SubjectMask)) 
  {
    S32 ghost_id = (cam_subject) ? conn->getGhostIndex(cam_subject) : -1;
    if (bstream->writeFlag(ghost_id != -1))
      bstream->writeRangedU32(U32(ghost_id), 0, NetConnection::MaxGhostCount);
    else if (cam_subject)
      retMask |= SubjectMask;
  }

  return retMask;
}

void afxCamera::unpackUpdate(NetConnection *conn, BitStream *bstream)
{
  Parent::unpackUpdate(conn,bstream);

  // controlled by the client?
  //if(bstream->readFlag())
  //   return;

  if (bstream->readFlag()) {
    Point3F pos,rot;
    bstream->read(&pos.x);
    bstream->read(&pos.y);
    bstream->read(&pos.z);
    bstream->read(&rot.x);
    bstream->read(&rot.z);
    set_cam_pos(pos,rot);

    // New delta for client side interpolation
    delta.pos = pos;
    delta.rot = rot;
    delta.posVec = delta.rotVec = VectorF(0,0,0);
  }

  if (bstream->readFlag()) 
  {
    if (bstream->readFlag())
    {
      S32 ghost_id = bstream->readRangedU32(0, NetConnection::MaxGhostCount);
      cam_subject = dynamic_cast<GameBase*>(conn->resolveGhost(ghost_id));
    }
    else
      cam_subject = NULL;
  }
}

void afxCamera::renderImage(SceneState*, SceneRenderImage*)
{
  if (gEditingMission)
  {
    glPushMatrix();
    dglMultMatrix(&mObjToWorld);
    glScalef(mObjScale.x,mObjScale.y,mObjScale.z);
    wireCube(Point3F(1, 1, 1),Point3F(0,0,0));
    glPopMatrix();
  }
}

// Override to ensure both are kept in scope
void afxCamera::onCameraScopeQuery(NetConnection* conn, CameraScopeQuery* query) 
{
  if (cam_subject)
    conn->objectInScope(cam_subject);
  Parent::onCameraScopeQuery(conn, query);
}

//----------------------------------------------------------------------------
// check if the object needs to be observed through its own camera...
void afxCamera::getCameraTransform(F32* pos, MatrixF* mat)
{
  // The camera doesn't support a third person mode,
  // so we want to override the default ShapeBase behavior.
  ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
  if (obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
    obj->getCameraTransform(pos, mat);
  else
    getEyeTransform(mat);
}

void afxCamera::setTransform(const MatrixF& mat)
{
  // This method should never be called on the client.

  // This currently converts all rotation in the mat into
  // rotations around the z and x axis.
  Point3F pos,vec;
  mat.getColumn(1,&vec);
  mat.getColumn(3,&pos);
  Point3F rot(-mAtan(vec.z, mSqrt(vec.x*vec.x + vec.y*vec.y)),0,-mAtan(-vec.x,vec.y));
  set_cam_pos(pos,rot);
}

void afxCamera::onEditorEnable()
{
  mNetFlags.set(Ghostable);
}

void afxCamera::onEditorDisable()
{
  mNetFlags.clear(Ghostable);
}

F32 afxCamera::getCameraFov()
{
  ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
  if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
    return(obj->getCameraFov());
  else
    return(Parent::getCameraFov());
}

F32 afxCamera::getDefaultCameraFov()
{
  ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
  if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
    return(obj->getDefaultCameraFov());
  else
    return(Parent::getDefaultCameraFov());
}

bool afxCamera::isValidCameraFov(F32 fov)
{
  ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
  if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
    return(obj->isValidCameraFov(fov));
  else
    return(Parent::isValidCameraFov(fov));
}

void afxCamera::setCameraFov(F32 fov)
{
  ShapeBase * obj = dynamic_cast<ShapeBase*>(static_cast<SimObject*>(mOrbitObject));
  if(obj && static_cast<ShapeBaseData*>(obj->getDataBlock())->observeThroughObject)
    obj->setCameraFov(fov);
  else
    Parent::setCameraFov(fov);
}

F32 afxCamera::getDamageFlash() const
{
  if (mode == OrbitObjectMode && isServerObject() && bool(mOrbitObject))
  {
    const GameBase *castObj = mOrbitObject;
    const ShapeBase* psb = dynamic_cast<const ShapeBase*>(castObj);
    if (psb)
      return psb->getDamageFlash();
  }

  return mDamageFlash;
}

F32 afxCamera::getWhiteOut() const
{
  if (mode == OrbitObjectMode && isServerObject() && bool(mOrbitObject))
  {
    const GameBase *castObj = mOrbitObject;
    const ShapeBase* psb = dynamic_cast<const ShapeBase*>(castObj);
    if (psb)
      return psb->getWhiteOut();
  }

  return mWhiteOut;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
