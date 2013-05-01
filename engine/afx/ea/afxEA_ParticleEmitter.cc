
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include <typeinfo>
#include "afx/arcaneFX.h"

#if defined(STOCK_TGE_PARTICLES)
#include "game/fx/particleEngine.h"
#else
#include "afx/ce/afxParticleEmitter.h"
#endif

#include "afx/afxEffectDefs.h"
#include "afx/afxEffectWrapper.h"
#include "afx/ea/afxEA_ParticleEmitter.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxEA_ParticleEmitter

afxEA_ParticleEmitter::afxEA_ParticleEmitter()
{
  emitter_data = 0;
  emitter = 0;
  do_bbox_update = false;
}

afxEA_ParticleEmitter::~afxEA_ParticleEmitter()
{
  if (emitter)
  {
    emitter->deleteWhenEmpty();
  }
}

void afxEA_ParticleEmitter::ea_set_datablock(SimDataBlock* db)
{
   emitter_data = dynamic_cast<ParticleEmitterData*>(db);
}

bool afxEA_ParticleEmitter::ea_start()
{
  if (!emitter_data)
  {
    Con::errorf("afxEA_ParticleEmitter::ea_start() -- missing or incompatible datablock.");
    return false;
  }

#if defined(STOCK_TGE_PARTICLES)
  emitter = new ParticleEmitter();
  emitter->onNewDataBlock(emitter_data);
#else
   afxParticleEmitterData* afx_emitter_db = dynamic_cast<afxParticleEmitterData*>(emitter_data);
   if (afx_emitter_db)
   {
      if (dynamic_cast<afxParticleEmitterVectorData*>(emitter_data))
      {
         afxParticleEmitterVector* pe = new afxParticleEmitterVector();
         pe->onNewDataBlock(afx_emitter_db);
         emitter = pe;
      }
      else if (dynamic_cast<afxParticleEmitterConeData*>(emitter_data))
      {
         afxParticleEmitterCone* pe = new afxParticleEmitterCone();
         pe->onNewDataBlock(afx_emitter_db);
         emitter = pe;
      }
      else if (dynamic_cast<afxParticleEmitterPathData*>(emitter_data))
      {
         afxParticleEmitterPath* pe = new afxParticleEmitterPath();
         pe->onNewDataBlock(afx_emitter_db);
         emitter = pe;
      }
      else if (dynamic_cast<afxParticleEmitterDiscData*>(emitter_data))
      {
         afxParticleEmitterDisc* pe = new afxParticleEmitterDisc();
         pe->onNewDataBlock(afx_emitter_db);
         emitter = pe;
      }
   }
   else
   {
      emitter = new ParticleEmitter();
      emitter->onNewDataBlock(emitter_data);
   }
#endif

  if (!emitter->registerObject())
  {
    delete emitter;
    emitter = NULL;
    Con::errorf("afxEA_ParticleEmitter::ea_start() -- effect failed to register.");
    return false;
  }

  if (datablock->forced_bbox.isValidBox())
  {
    do_bbox_update = true;
  }

#if !defined(TGEA_ENGINE)
  emitter->setSortPriority(datablock->sort_priority);
#endif

  return true;
}

bool afxEA_ParticleEmitter::ea_update(F32 dt)
{
  if (emitter && in_scope)
  {
    if (do_bbox_update)
    {
      Box3F bbox = emitter->getObjBox();

      bbox.min = updated_pos + datablock->forced_bbox.min; 
      bbox.max = updated_pos + datablock->forced_bbox.max; 
      emitter->setForcedObjBox(bbox);
      emitter->setTransform(emitter->getTransform());

      if (!datablock->update_forced_bbox)
        do_bbox_update = false;
    }

    if (do_fades)
      emitter->setFadeAmount(fade_value);
    
    emitter->emitParticlesExt(updated_xfm, updated_pos, Point3F(0.0,0.0,0.0), (U32)(dt*1000));
  }

  return true;
}

void afxEA_ParticleEmitter::ea_finish(bool was_stopped)
{
  if (emitter)
  {
    // make sure particles are fully faded.
    //   note - fully faded particles are not always
    //     invisible, so they are still kept alive and 
    //     deleted via deleteWhenEmpty().
    if (ew_timing.fade_out_time > 0.0f)
      emitter->setFadeAmount(0.0f);
    emitter->deleteWhenEmpty();
    emitter = 0;
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxEA_ParticleEmitterDesc : public afxEffectAdapterDesc, public afxEffectDefs 
{
  static afxEA_ParticleEmitterDesc desc;

public:
  virtual bool  testEffectType(const SimDataBlock*) const;
  virtual bool  requiresStop(const afxEffectWrapperData*) const;
  virtual bool  runsOnServer(const afxEffectWrapperData*) const { return false; }
  virtual bool  runsOnClient(const afxEffectWrapperData*) const { return true; }

  virtual afxEffectWrapper* create() const { return new afxEA_ParticleEmitter; }
};

afxEA_ParticleEmitterDesc afxEA_ParticleEmitterDesc::desc;

bool afxEA_ParticleEmitterDesc::testEffectType(const SimDataBlock* db) const
{
#if defined(STOCK_TGE_PARTICLES)
  return (typeid(ParticleEmitterData) == typeid(*db));
#else
  if (typeid(ParticleEmitterData) == typeid(*db))
     return true;
  if (typeid(afxParticleEmitterVectorData) == typeid(*db))
     return true;
  if (typeid(afxParticleEmitterConeData) == typeid(*db))
     return true;
  if (typeid(afxParticleEmitterPathData) == typeid(*db))
     return true;
  if (typeid(afxParticleEmitterDiscData) == typeid(*db))
     return true;

  return false;
#endif
}

bool afxEA_ParticleEmitterDesc::requiresStop(const afxEffectWrapperData* ew) const
{
  return (ew->timing.lifetime < 0);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//