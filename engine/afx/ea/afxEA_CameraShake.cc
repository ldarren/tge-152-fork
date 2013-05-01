
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#include "game/fx/cameraFXMgr.h"

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/ce/afxCameraShake.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_CameraShake 

class afxEA_CameraShake : public afxEffectWrapper
{
  typedef afxEffectWrapper Parent;

  afxCameraShakeData* shake_data;
  CameraShake*        camera_shake;

public:
  /*C*/             afxEA_CameraShake();
  /*D*/             ~afxEA_CameraShake();

  virtual void      ea_set_datablock(SimDataBlock*);
  virtual bool      ea_start();
  virtual bool      ea_update(F32 dt);
  virtual void      ea_finish(bool was_stopped);
};

//~~~~~~~~~~~~~~~~~~~~//

afxEA_CameraShake::afxEA_CameraShake()
{
  shake_data = 0;
  camera_shake = 0;
}

afxEA_CameraShake::~afxEA_CameraShake()
{
  delete camera_shake;
}

void afxEA_CameraShake::ea_set_datablock(SimDataBlock* db)
{
  shake_data = dynamic_cast<afxCameraShakeData*>(db);
}

bool afxEA_CameraShake::ea_start()
{
  if (!shake_data)
  {
    Con::errorf("afxEA_CameraShake::ea_start() -- missing or incompatible datablock.");
    return false;
  }

  afxConstraint* pos_constraint = getPosConstraint();
  afxConstraint* aim_constraint = getAimConstraint();

  if (aim_constraint && pos_constraint)
  {
    if (full_lifetime <= 0 || full_lifetime == INFINITE_LIFETIME)
    {
      Con::errorf("afxEA_CameraShake::ea_start() --  effect requires a finite lifetime.");
      return false;
    }

    ShapeBase* shaken = dynamic_cast<ShapeBase*>(aim_constraint->getObject());
    if (shaken)
    {
      Point3F pos; pos_constraint->getPosition(pos);
      VectorF diff = shaken->getPosition() - pos;
      F32 dist = diff.len();
      if (dist < shake_data->camShakeRadius)
      {
        camera_shake = new CameraShake;
        camera_shake->setDuration(full_lifetime);
        camera_shake->setFrequency(shake_data->camShakeFreq);

        F32 falloff =  dist/shake_data->camShakeRadius;
        falloff = 1 + falloff*10.0;
        falloff = 1.0 / (falloff*falloff);

        VectorF shakeAmp = shake_data->camShakeAmp*falloff;
        camera_shake->setAmplitude(shakeAmp);
        camera_shake->setFalloff(shake_data->camShakeFalloff);
        camera_shake->init();
      }
    }
  }

  return true;
}

bool afxEA_CameraShake::ea_update(F32 dt)
{
  afxConstraint* aim_constraint = getAimConstraint();
  if (camera_shake && aim_constraint)
  {
    camera_shake->update(dt);
    
    ShapeBase* shaken = dynamic_cast<ShapeBase*>(aim_constraint->getObject());
    if (shaken)
    {
      MatrixF fxTrans = camera_shake->getTrans();
      MatrixF curTrans = shaken->getRenderTransform();
      curTrans.mul(fxTrans);
      
      Point3F	cameraPosWorld;
      curTrans.getColumn(3,&cameraPosWorld);
      shaken->setPosition(cameraPosWorld);
    }
  }
  
  return true;
}

void afxEA_CameraShake::ea_finish(bool was_stopped)
{
  delete camera_shake;
  camera_shake = 0;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_CameraShakeDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_CameraShakeDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_CameraShake; }
};

afxEA_CameraShakeDesc afxEA_CameraShakeDesc::desc;

bool afxEA_CameraShakeDesc::testEffectType(const SimDataBlock* db) const
{
  return (typeid(afxCameraShakeData) == typeid(*db));
}

bool afxEA_CameraShakeDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//