
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"
#include "math/mathIO.h"
#include "sceneGraph/sceneGraph.h"
#include "afx/util/afxPath.h"
#include "afx/util/afxPath3D.h"
#include "afx/ce/afxParticleEmitter.h"

IMPLEMENT_CO_DATABLOCK_V1(afxParticleEmitterData);

afxParticleEmitterData::afxParticleEmitterData()
{
  fade_velocity = false;  // coordinate velocity amount with fade amout
  fade_offset   = false;  // coordinate ejection-offset amount with fade amount
  pe_vector.set(0.0,0.0,0.0);
  pe_vector_is_world = false;
  tpaths_string = ST_NULLSTRING;
  tPathDataBlocks.clear();
  tPathDataBlockIds.clear();
}

void afxParticleEmitterData::initPersistFields()
{
  Parent::initPersistFields();

  addField("fadeVelocity",         TypeBool,    Offset(fade_velocity,      afxParticleEmitterData));
  addField("fadeOffset",           TypeBool,    Offset(fade_offset,        afxParticleEmitterData));
  addField("vector",               TypePoint3F, Offset(pe_vector,          afxParticleEmitterData));
  addField("vectorIsWorld",        TypeBool,    Offset(pe_vector_is_world, afxParticleEmitterData));
  addField("pathsTransform",       TypeString,  Offset(tpaths_string,      afxParticleEmitterData));
}

IMPLEMENT_CONSOLETYPE(afxParticleEmitterData)
IMPLEMENT_GETDATATYPE(afxParticleEmitterData)
IMPLEMENT_SETDATATYPE(afxParticleEmitterData)

void afxParticleEmitterData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeFlag(fade_velocity);
  stream->writeFlag(fade_offset);
  mathWrite(*stream, pe_vector);
  stream->writeFlag(pe_vector_is_world);

  stream->write(tPathDataBlockIds.size());
  for (int i = 0; i < tPathDataBlockIds.size(); i++)
    stream->write(tPathDataBlockIds[i]);
}

void afxParticleEmitterData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  fade_velocity = stream->readFlag();
  fade_offset   = stream->readFlag();
  mathRead(*stream, &pe_vector);
  pe_vector_is_world = stream->readFlag();

  U32 n_db; stream->read(&n_db);
  tPathDataBlockIds.setSize(n_db);
  for (U32 i = 0; i < n_db; i++)
    stream->read(&tPathDataBlockIds[i]);
}

bool afxParticleEmitterData::onAdd()
{
  if( Parent::onAdd() == false )
    return false;

  if (tpaths_string != ST_NULLSTRING && tpaths_string[0] == '\0')
  {
    Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) empty transform paths string.", getName());
    return false;
  }
  
  if (tpaths_string != ST_NULLSTRING && dStrlen(tpaths_string) > 255) 
  {
    Con::errorf(ConsoleLogEntry::General, "ParticleEmitterData(%s) transform paths string too long [> 255 chars].", getName());
    return false;
  }
  
  if (tpaths_string != ST_NULLSTRING) 
  {
    Vector<char*> dataBlocks(__FILE__, __LINE__);
    char* tokCopy = new char[dStrlen(tpaths_string) + 1];
    dStrcpy(tokCopy, tpaths_string);
    
    char* currTok = dStrtok(tokCopy, " \t");
    while (currTok != NULL) 
    {
      dataBlocks.push_back(currTok);
      currTok = dStrtok(NULL, " \t");
    }
    if (dataBlocks.size() == 0) 
    {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) invalid transform paths string.  No datablocks found", getName());
      delete [] tokCopy;
      return false;
    }
    tPathDataBlocks.clear();
    tPathDataBlockIds.clear();
    
    for (U32 i = 0; i < dataBlocks.size(); i++) 
    {
      afxPathData* pData = NULL;
      if (Sim::findObject(dataBlocks[i], pData) == false) 
      {
        Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find transform path datablock: %s", getName(), dataBlocks[i]);
      } 
      else 
      {
        tPathDataBlocks.push_back(pData);
        tPathDataBlockIds.push_back(pData->getId());
      }
    }
    delete [] tokCopy;
    if (tPathDataBlocks.size() == 0) 
    {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find any transform path datablocks", getName());
      return false;
    }
  }

  return true;
}

bool afxParticleEmitterData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;

  tPathDataBlocks.clear();
  for (U32 i = 0; i < tPathDataBlockIds.size(); i++) 
  {
    afxPathData* pData = NULL;
    if (Sim::findObject(tPathDataBlockIds[i], pData) == false)
    {
      Con::warnf(ConsoleLogEntry::General, 
                 "ParticleEmitterData(%s) unable to find transform path datablock: %d", 
                 getName(), tPathDataBlockIds[i]);
    }
    else
      tPathDataBlocks.push_back(pData);
  }

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// VECTOR

IMPLEMENT_CO_DATABLOCK_V1(afxParticleEmitterVectorData);

afxParticleEmitterVectorData::afxParticleEmitterVectorData()
{
}

void afxParticleEmitterVectorData::initPersistFields()
{
  Parent::initPersistFields();
}

IMPLEMENT_CONSOLETYPE(afxParticleEmitterVectorData)
IMPLEMENT_GETDATATYPE(afxParticleEmitterVectorData)
IMPLEMENT_SETDATATYPE(afxParticleEmitterVectorData)

void afxParticleEmitterVectorData::packData(BitStream* stream)
{
  Parent::packData(stream);
}

void afxParticleEmitterVectorData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);
}

bool afxParticleEmitterVectorData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;
  return true;
}

bool afxParticleEmitterVectorData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;
  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// CONE

IMPLEMENT_CO_DATABLOCK_V1(afxParticleEmitterConeData);

afxParticleEmitterConeData::afxParticleEmitterConeData()
{
  spread_min = 0.0f;
  spread_max = 90.0f;
}

void afxParticleEmitterConeData::initPersistFields()
{
  Parent::initPersistFields();

  addField("spreadMin",   TypeF32,    Offset(spread_min, afxParticleEmitterConeData));
  addField("spreadMax",   TypeF32,    Offset(spread_max, afxParticleEmitterConeData));
}

IMPLEMENT_CONSOLETYPE(afxParticleEmitterConeData)
IMPLEMENT_GETDATATYPE(afxParticleEmitterConeData)
IMPLEMENT_SETDATATYPE(afxParticleEmitterConeData)

void afxParticleEmitterConeData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeRangedU32((U32)spread_min, 0, 180);
  stream->writeRangedU32((U32)spread_max, 0, 180);
}

void afxParticleEmitterConeData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  spread_min = stream->readRangedU32(0, 180);
  spread_max = stream->readRangedU32(0, 180);
}

bool afxParticleEmitterConeData::onAdd()
{
  if( Parent::onAdd() == false )
    return false;

  if (spread_min < 0.0f) 
  {
    Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) spreadMin < 0.0", getName());
    spread_min = 0.0f;
  }
  if (spread_max > 180.0f) 
  {
    Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) spreadMax > 180.0f", getName());
    spread_max = 180.0f;
  }
  if (spread_max > 179.5f) 
  {
    spread_max = 179.5f;
  }
  if (spread_min > spread_max) 
  {
    Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) spreadMin > spreadMax", getName());
    spread_min = spread_max;
  }

  return true;
}

bool afxParticleEmitterConeData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;
  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// PATH

IMPLEMENT_CO_DATABLOCK_V1(afxParticleEmitterPathData);

afxParticleEmitterPathData::afxParticleEmitterPathData()
{
  epaths_string = ST_NULLSTRING;
  epathDataBlocks.clear();
  epathDataBlockIds.clear();
  path_origin_type = PATHEMIT_ORIGIN;
}

static EnumTable::Enums PathOriginEnums_[] =
{
  { afxParticleEmitterPathData::PATHEMIT_ORIGIN,  "origin"  },
  { afxParticleEmitterPathData::PATHEMIT_POINT,   "point"   },
  { afxParticleEmitterPathData::PATHEMIT_VECTOR,  "vector"  },
  { afxParticleEmitterPathData::PATHEMIT_TANGENT, "tangent" },
};
static EnumTable PathOriginTable_(4, PathOriginEnums_);

void afxParticleEmitterPathData::initPersistFields()
{
  Parent::initPersistFields();

  addField("paths",         TypeString,  Offset(epaths_string,      afxParticleEmitterPathData));
  addField("pathOrigin",    TypeEnum,    Offset(path_origin_type,  afxParticleEmitterPathData), 1, &PathOriginTable_);
}

IMPLEMENT_CONSOLETYPE(afxParticleEmitterPathData)
IMPLEMENT_GETDATATYPE(afxParticleEmitterPathData)
IMPLEMENT_SETDATATYPE(afxParticleEmitterPathData)

void afxParticleEmitterPathData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->write(epathDataBlockIds.size());
  for (int i = 0; i < epathDataBlockIds.size(); i++)
    stream->write(epathDataBlockIds[i]);
  stream->write(path_origin_type);
}

void afxParticleEmitterPathData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  U32 n_db; stream->read(&n_db);
  epathDataBlockIds.setSize(n_db);
  for (U32 i = 0; i < n_db; i++)
    stream->read(&epathDataBlockIds[i]);
  stream->read(&path_origin_type);
}

bool afxParticleEmitterPathData::onAdd()
{
  if( Parent::onAdd() == false )
    return false;

  // path
  if (epaths_string != ST_NULLSTRING && epaths_string[0] == '\0')
  {
    Con::warnf(ConsoleLogEntry::General, "afxParticleEmitterPathData(%s) empty paths string.", getName());
    return false;
  }

  if (epaths_string != ST_NULLSTRING && dStrlen(epaths_string) > 255) 
  {
    Con::errorf(ConsoleLogEntry::General, "afxParticleEmitterPathData(%s) paths string too long [> 255 chars].", getName());
    return false;
  }

  if (epaths_string != ST_NULLSTRING) 
  {
    Vector<char*> dataBlocks(__FILE__, __LINE__);
    char* tokCopy = new char[dStrlen(epaths_string) + 1];
    dStrcpy(tokCopy, epaths_string);

    char* currTok = dStrtok(tokCopy, " \t");
    while (currTok != NULL) 
    {
      dataBlocks.push_back(currTok);
      currTok = dStrtok(NULL, " \t");
    }
    if (dataBlocks.size() == 0) 
    {
      Con::warnf(ConsoleLogEntry::General, "afxParticleEmitterPathData(%s) invalid paths string.  No datablocks found", getName());
      delete [] tokCopy;
      return false;
    }
    epathDataBlocks.clear();
    epathDataBlockIds.clear();

    for (U32 i = 0; i < dataBlocks.size(); i++) 
    {
      afxPathData* pData = NULL;
      if (Sim::findObject(dataBlocks[i], pData) == false) 
      {
        Con::warnf(ConsoleLogEntry::General, "afxParticleEmitterPathData(%s) unable to find path datablock: %s", getName(), dataBlocks[i]);
      } 
      else 
      {
        epathDataBlocks.push_back(pData);
        epathDataBlockIds.push_back(pData->getId());
      }
    }
    delete [] tokCopy;
    if (epathDataBlocks.size() == 0) 
    {
      Con::warnf(ConsoleLogEntry::General, "afxParticleEmitterPathData(%s) unable to find any path datablocks", getName());
      return false;
    }
  }

  return true;
}

bool afxParticleEmitterPathData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;

  epathDataBlocks.clear();
  for (U32 i = 0; i < epathDataBlockIds.size(); i++) 
  {
    afxPathData* pData = NULL;
    if (Sim::findObject(epathDataBlockIds[i], pData) == false)
    {
      Con::warnf(ConsoleLogEntry::General, 
                 "afxParticleEmitterPathData(%s) unable to find path datablock: %d", 
                 getName(), epathDataBlockIds[i]);
    }
    else
      epathDataBlocks.push_back(pData);
  }
  parts_per_eject = epathDataBlocks.size();

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// DISC

IMPLEMENT_CO_DATABLOCK_V1(afxParticleEmitterDiscData);

afxParticleEmitterDiscData::afxParticleEmitterDiscData()
{
  pe_radius_min = 0.0f;
  pe_radius_max = 1.0f;
}

void afxParticleEmitterDiscData::initPersistFields()
{
  Parent::initPersistFields();

  addField("radiusMin",   TypeF32,    Offset(pe_radius_min, afxParticleEmitterDiscData));
  addField("radiusMax",   TypeF32,    Offset(pe_radius_max, afxParticleEmitterDiscData));
}

IMPLEMENT_CONSOLETYPE(afxParticleEmitterDiscData)
IMPLEMENT_GETDATATYPE(afxParticleEmitterDiscData)
IMPLEMENT_SETDATATYPE(afxParticleEmitterDiscData)

void afxParticleEmitterDiscData::packData(BitStream* stream)
{
  Parent::packData(stream);

  stream->writeInt((S32)(pe_radius_min * 100), 16);
  stream->writeInt((S32)(pe_radius_max * 100), 16);
}

void afxParticleEmitterDiscData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  pe_radius_min = stream->readInt(16) / 100.0f;
  pe_radius_max = stream->readInt(16) / 100.0f;
}

bool afxParticleEmitterDiscData::onAdd()
{
  if( Parent::onAdd() == false )
    return false;

  return true;
}

bool afxParticleEmitterDiscData::preload(bool server, char errorBuffer[256])
{
  if (Parent::preload(server, errorBuffer) == false)
    return false;

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

afxParticleEmitter::afxParticleEmitter()
{
  pe_vector.set(0,0,1);
  pe_vector_norm.set(0,0,1);
  tpaths.clear();
  tpath_mults.clear();
  n_tpath_points = 0;
  tpath_points = NULL;
}

afxParticleEmitter::~afxParticleEmitter()
{
}

bool afxParticleEmitter::onAdd()
{
  if( !Parent::onAdd() )
    return false;

  if (dynamic_cast<afxParticleEmitterData*>(mDataBlock))
    init_paths();

  return true;
}

void afxParticleEmitter::onRemove()
{
  if (dynamic_cast<afxParticleEmitterData*>(mDataBlock))
    cleanup_paths();

  Parent::onRemove();
}

void afxParticleEmitter::init_paths()
{
  if (!mDataBlock)
  {
    n_tpath_points = 0;
    tpath_points = NULL;
    return;
  }

  if (mDataBlock->tPathDataBlocks.size() < 1)
  {
    n_tpath_points = 0;
    tpath_points = NULL;
  }
  else
  {
    n_tpath_points = mDataBlock->tPathDataBlocks.size();
    tpath_points = new Point3F*[n_tpath_points];
  
    for (U32 i=0; i < n_tpath_points; i++)
    {
      afxPathData* pd = mDataBlock->tPathDataBlocks[i];      

      afxPath3D* path = new afxPath3D();
      //path->buildPath( pd->num_points, pd->points, 0.0f, 1.0f );
      if (pd->lifetime == 0)
        path->buildPath( pd->num_points, pd->points, pd->delay, 1.0f );
      else
        path->buildPath( pd->num_points, pd->points, pd->delay, pd->delay+pd->lifetime ); 
      path->setLoopType( pd->loop_type );
      tpaths.push_back(path);  
    
      tpath_mults.push_back( pd->mult );
    
      tpath_points[i] = new Point3F[pd->num_points];
      for (U32 j=0; j<pd->num_points; j++)
        tpath_points[i][j] = pd->points[j];
    }
  }
}

void afxParticleEmitter::cleanup_paths()
{
  if (n_tpath_points < 1)
    return;

  for (U32 i=0; i < tpaths.size(); i++)
  {
    if (tpaths[i])
      delete tpaths[i];
  }
  tpaths.clear();
  
  if (tpath_points)
  {
    if (mDataBlock)
    {
      for (U32 i=0; i < n_tpath_points; i++)
      {
        if (tpath_points[i])
          delete [] tpath_points[i];
      }
    }
    
    delete [] tpath_points;
    tpath_points = 0;
  }
}

void afxParticleEmitter::sub_particleUpdate(Particle* part)
{
   if (tpaths.size() < 1)
      return;

   F32 t = ((F32)part->currentAge)/((F32)part->totalLifetime);
   for (U32 i=0; i < tpaths.size(); i++)
   {
      F32 t_last = part->t_last;
      Point3F path_delta = (t_last <= 0.0f) ? tpaths[i]->evaluateAtTime(t) : tpaths[i]->evaluateAtTime(t_last, t);

      if (mDataBlock->tPathDataBlocks[i]->concentric)
      {
         // scale radial vector by x-component of path
         part->pos_local += part->radial_v*path_delta.x;
         // scale axis vector by z-component of path
         part->pos_local += pe_vector_norm*path_delta.z;
         // y-component is ignored
      }
      else
      {
         part->pos_local += path_delta;     
      }
   }

   part->t_last = t;
}

void afxParticleEmitter::preCompute(const MatrixF& mat)
{
   // Put vector into the space of the input matrix
   pe_vector = mDataBlock->pe_vector;
   if (!mDataBlock->pe_vector_is_world)
     mat.mulV(pe_vector);

   pe_vector_norm = pe_vector;
   pe_vector_norm.normalize();

   // Transform Paths: rebuild with current matrix
   for( U32 i=0; i < tpaths.size(); i++ )
   {
      for( U32 j=0; j < tpaths[i]->getNumPoints(); j++ )
      {
         Point3F p = tpath_points[i][j];
         mat.mulV(p);
         tpaths[i]->setPointPosition(j, p);
      }

      tpaths[i]->reBuildPath();
   }

   sub_preCompute(mat);
}

void afxParticleEmitter::afx_emitParticles(const Point3F& point, const bool useLastPosition, const Point3F& velocity, const U32 numMilliseconds)
{
  if (mDead) return;

  // lifetime over - no more particles
  if (mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS)
    return;

  Point3F realStart;
  if (useLastPosition && mHasLastPosition)
    realStart = mLastPosition;
  else
    realStart = point;

  afx_emitParticles(realStart, point, velocity, numMilliseconds);
}

void afxParticleEmitter::afx_emitParticles(const Point3F& start, const Point3F& end, const Point3F& velocity, const U32 numMilliseconds)
{
  if (mDead) return;

  // lifetime over - no more particles
  if (mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS)
    return;

  U32 currTime = 0;
  bool particlesAdded = false;

  if (mNextParticleTime != 0) 
  {
    // Need to handle next particle
    //
    if (mNextParticleTime > numMilliseconds) 
    {
      // Defer to next update
      //  (Note that this introduces a potential spatial irregularity if the owning
      //   object is accelerating, and updating at a low frequency)
      //
      mNextParticleTime -= numMilliseconds;
      mInternalClock += numMilliseconds;
      mLastPosition = end;
      mHasLastPosition = true;
      return;
    } 
    else 
    {
      currTime       += mNextParticleTime;
      mInternalClock += mNextParticleTime;
      // Emit particle at curr time

      // Create particle at the correct position
      Point3F pos;
      pos.interpolate(start, end, F32(currTime) / F32(numMilliseconds));

      for (S32 p = 0; p < mDataBlock->parts_per_eject; p++)
         {
            sub_addParticle(pos, velocity, numMilliseconds-currTime, p);
            particlesAdded = true;
         }
      mNextParticleTime = 0;
    }
  }

  while (currTime < numMilliseconds) 
  {
     S32 nextTime = mDataBlock->ejectionPeriodMS;
     if (mDataBlock->periodVarianceMS != 0) 
     {
        nextTime += S32(gRandGen.randI() % (2 * mDataBlock->periodVarianceMS + 1)) -
           S32(mDataBlock->periodVarianceMS);
     }
     AssertFatal(nextTime > 0, "Error, next particle ejection time must always be greater than 0");

     if (currTime + nextTime > numMilliseconds) 
     {
        mNextParticleTime = (currTime + nextTime) - numMilliseconds;
        mInternalClock   += numMilliseconds - currTime;
        AssertFatal(mNextParticleTime > 0, "Error, should not have deferred this particle!");
        break;
     }

     currTime       += nextTime;
     mInternalClock += nextTime;

     // Create particle at the correct position
     Point3F pos;
     pos.interpolate(start, end, F32(currTime) / F32(numMilliseconds));

     U32 advanceMS = numMilliseconds - currTime;
     if (mDataBlock->overrideAdvance == false && advanceMS != 0)
     {
        for (S32 p = 0; p < mDataBlock->parts_per_eject; p++)
        {
           sub_addParticle(pos, velocity, numMilliseconds-currTime, p);
           particlesAdded = true;

           Particle* last_part = part_list_head.next;
           if (advanceMS > last_part->totalLifetime) 
           {
              part_list_head.next = last_part->next;
              n_parts--;
              last_part->next = part_freelist;
              part_freelist = last_part;
           } 
           else 
           {
              F32 t = F32(advanceMS) / 1000.0;

              Point3F a = last_part->acc;
              a -= last_part->vel*last_part->dataBlock->dragCoefficient;
              a -= mWindVelocity*last_part->dataBlock->windCoefficient;
              //a += Point3F(0, 0, -9.81) * last_part->dataBlock->gravityCoefficient;
              a.z += -9.81f*last_part->dataBlock->gravityCoefficient; // as long as gravity is a constant, this is faster

              last_part->vel += a * t;
              last_part->pos_local += last_part->vel * t;

              // allow subclasses to adjust the particle params here
              sub_particleUpdate(last_part);

              if (last_part->dataBlock->constrain_pos)
                 last_part->pos = last_part->pos_local + this->pos_pe;
              else
                 last_part->pos = last_part->pos_local;

              updateKeyData(last_part);
           }
        }
     }
     else
     {
        for (S32 p = 0; p < mDataBlock->parts_per_eject; p++)
        {
           sub_addParticle(pos, velocity, numMilliseconds-currTime, p);
           particlesAdded = true;
        }
     }
  }

  if( particlesAdded == true )
     updateBBox();

  if( n_parts > 0 && mSceneManager == NULL )
  {
     gClientSceneGraph->addObjectToScene(this);
     gClientContainer.addObject(this);
     gClientProcessList.addObject(this);
  }

  mLastPosition = end;
  mHasLastPosition = true;
}

Particle* afxParticleEmitter::alloc_particle()
{
  n_parts++;

  // this should happen rarely
  if (n_parts > n_part_capacity)
  {
    Particle* store_block = new Particle[16];
    part_store.push_back(store_block);
    n_part_capacity += 16;
    for (S32 i = 0; i < 16; i++)
    {
      store_block[i].next = part_freelist;
      part_freelist = &store_block[i];
    }
    mDataBlock->allocPrimBuffer(n_part_capacity);
  }

  Particle* pNew = part_freelist;
  part_freelist = pNew->next;
  pNew->next = part_list_head.next;
  part_list_head.next = pNew;

  return pNew;
}

ParticleData* afxParticleEmitter::pick_particle_type()
{
  U32 dBlockIndex = (U32)(mCeil(gRandGen.randF() * F32(mDataBlock->particleDataBlocks.size())) - 1);
  return mDataBlock->particleDataBlocks[dBlockIndex];
}

bool afxParticleEmitter::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxParticleEmitterData*>(dptr);
  if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
    return false;

  scriptOnNewDataBlock();
  return true;
}

void afxParticleEmitter::emitParticlesExt(const MatrixF& xfm, const Point3F& point, const Point3F& velocity, const U32 numMilliseconds)
{
   if (mDataBlock->use_emitter_xfm)
   {
      Point3F zero_point(0.0f, 0.0f, 0.0f);
      pos_pe = zero_point;
      setTransform(xfm);

      preCompute(xfm);
      afx_emitParticles(zero_point, true, velocity, numMilliseconds);
   }
   else
   {
      pos_pe = point;
      preCompute(xfm);
      afx_emitParticles(point, true, velocity, numMilliseconds);
   }
}  

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// VECTOR

afxParticleEmitterVector::afxParticleEmitterVector()
{
}

afxParticleEmitterVector::~afxParticleEmitterVector()
{
}

bool afxParticleEmitterVector::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxParticleEmitterVectorData*>(dptr);
  if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
    return false;

  scriptOnNewDataBlock();
  return true;
}

void afxParticleEmitterVector::sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx)
{
  Particle* pNew = alloc_particle();
  ParticleData* part_db = pick_particle_type();

  Point3F pos_start = pos;
  if (part_db->constrain_pos)
    pos_start.set(0,0,0);

  F32 initialVel = mDataBlock->ejectionVelocity;
  initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;
  if(mDataBlock->fade_velocity)
    initialVel *= fade_amt;

  F32 ejection_offset = mDataBlock->ejectionOffset;
  if(mDataBlock->fade_offset)
    ejection_offset *= fade_amt;

  pNew->pos = pos_start + (pe_vector_norm * ejection_offset);
  pNew->pos_local = pNew->pos;

  pNew->vel = pe_vector_norm * initialVel;
  pNew->orientDir = pe_vector_norm;
  pNew->acc.set(0, 0, 0);
  pNew->currentAge = age_offset;
  pNew->t_last = 0.0f;

  pNew->radial_v.set(0.0f, 0.0f, 0.0f);

  part_db->initializeParticle(pNew, vel);
  updateKeyData( pNew );
}

void afxParticleEmitterVector::sub_preCompute(const MatrixF& mat)
{
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// CONE

afxParticleEmitterCone::afxParticleEmitterCone()
{
  cone_v.set(0,0,1);
  cone_s0.set(0,0,1);
  cone_s1.set(0,0,1);
}

afxParticleEmitterCone::~afxParticleEmitterCone()
{
}

bool afxParticleEmitterCone::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxParticleEmitterConeData*>(dptr);
  if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
    return false;

  scriptOnNewDataBlock();
  return true;
}

void afxParticleEmitterCone::sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx)
{
  Particle* pNew = alloc_particle();
  ParticleData* part_db = pick_particle_type();

  Point3F pos_start = pos;
  if (part_db->constrain_pos)
    pos_start.set(0,0,0);

  F32 initialVel = mDataBlock->ejectionVelocity;
  initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;
  if(mDataBlock->fade_velocity)
    initialVel *= fade_amt;

  // Randomly choose a vector between cone_s0 and cone_s1 and normalize:
  Point3F vec;
  F32 t = mRandF(0.0f, 1.0f);   
  vec.interpolate(cone_s0, cone_s1, t);
  vec.normalize();

  // Randomly rotate about cone_v
  F32 theta = mRandF(0.0f, M_2PI_F);
  AngAxisF thetaRot(cone_v, theta);
  MatrixF temp(true);
  thetaRot.setMatrix(&temp);
  temp.mulP(vec);   

  F32 ejection_offset = mDataBlock->ejectionOffset;
  if(mDataBlock->fade_offset)
    ejection_offset *= fade_amt;

  pNew->pos = pos_start + (vec * ejection_offset);   
  pNew->pos_local = pNew->pos;

  pNew->vel = mDataBlock->ejectionInvert ? vec * -initialVel : vec * initialVel;
  pNew->orientDir = vec;
  pNew->acc.set(0, 0, 0);
  pNew->currentAge = age_offset;
  pNew->t_last = 0.0f;

  pNew->radial_v.set(0.0f, 0.0f, 0.0f);

  part_db->initializeParticle(pNew, vel);
  updateKeyData( pNew );
}

void afxParticleEmitterCone::sub_preCompute(const MatrixF& mat)
{
  // Find vectors on the XZ plane corresponding to the inner and outer spread angles:
  //    (tan is infinite at PI/4 or 90 degrees)
  cone_v.set( 0.0f, 0.0f, 1.0f );

  cone_s0.x = mTan( mDegToRad( ((afxParticleEmitterConeData*)mDataBlock)->spread_min / 2.0f ));
  cone_s0.y = 0.0f;
  cone_s0.z = 1.0f;

  cone_s1.x = mTan( mDegToRad(((afxParticleEmitterConeData*)mDataBlock)->spread_max / 2.0f ));
  cone_s1.y = 0.0f;
  cone_s1.z = 1.0f;         

  Point3F axis;
  F32 theta = mAcos( mDot(cone_v, pe_vector_norm) );

  if( M_PI_F-theta < POINT_EPSILON )
  {
    cone_v.neg();
    cone_s0.neg();
    cone_s1.neg();
  }
  else if( theta > POINT_EPSILON )
  {
    mCross(pe_vector_norm, cone_v, &axis);
    axis.normalize();

    AngAxisF thetaRot(axis, theta);
    MatrixF temp(true);
    thetaRot.setMatrix(&temp);

    temp.mulP(cone_v);            
    temp.mulP(cone_s0);
    temp.mulP(cone_s1);                      
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// PATH

afxParticleEmitterPath::afxParticleEmitterPath()
{
  epaths.clear();
  epath_mults.clear();
  n_epath_points = 0;
  epath_points = NULL;
}

afxParticleEmitterPath::~afxParticleEmitterPath()
{
}

bool afxParticleEmitterPath::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxParticleEmitterPathData*>(dptr);
  if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
    return false;

  scriptOnNewDataBlock();
  return true;
}

bool afxParticleEmitterPath::onAdd()
{
  if( !Parent::onAdd() )
    return false;

  if (dynamic_cast<afxParticleEmitterPathData*>(mDataBlock))
    init_paths();

  return true;
}

void afxParticleEmitterPath::onRemove()
{
  if (dynamic_cast<afxParticleEmitterPathData*>(mDataBlock))
    cleanup_paths();

  Parent::onRemove();
}

void afxParticleEmitterPath::init_paths()
{
  if (!mDataBlock || ((afxParticleEmitterPathData*)mDataBlock)->epathDataBlocks.size() < 1)
  {
    n_epath_points = 0;
    epath_points = NULL;
    return;
  }

  n_epath_points = ((afxParticleEmitterPathData*)mDataBlock)->epathDataBlocks.size();
  epath_points = new Point3F*[n_epath_points];
  
  for (U32 i=0; i < n_epath_points; i++)
  {
    afxPathData* pd = ((afxParticleEmitterPathData*)mDataBlock)->epathDataBlocks[i];
    
    afxPath3D* path = new afxPath3D();
    path->buildPath( pd->num_points, pd->points, 0.0f, 1.0f );  
    epaths.push_back(path);  
    
    epath_mults.push_back( pd->mult );
    
    epath_points[i] = new Point3F[pd->num_points];
    for (U32 j=0; j<pd->num_points; j++)
      epath_points[i][j] = pd->points[j];
  }
}

void afxParticleEmitterPath::cleanup_paths()
{
  if (n_epath_points < 1)
    return;

  for (U32 i=0; i < epaths.size(); i++)
  {
    if (epaths[i])
      delete epaths[i];
  }
  epaths.clear();
  
  if (epath_points)
  {
    if (mDataBlock)
    {
      for (U32 i=0; i < n_epath_points; i++)
      {
        if (epath_points[i])
          delete [] epath_points[i];
      }
    }
    
    delete [] epath_points;
    epath_points = 0;
  }
}

void afxParticleEmitterPath::sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx)
{
   S32 i = part_idx;

   Particle* pNew = alloc_particle();
   ParticleData* part_db = pick_particle_type();

   Point3F pos_start = pos;
   if (part_db->constrain_pos)
      pos_start.set(0,0,0);

   F32 initialVel = mDataBlock->ejectionVelocity;
   initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;
   if(mDataBlock->fade_velocity)
      initialVel *= fade_amt;

   // Randomly choose a curve parameter between [0.0,1.0] and evaluate the curve there
   F32 param = mRandF(0.0f, 1.0f);
   Point3F curve_pos = epath_mults[i] * epaths[i]->evaluateAtTime(param);

   Point3F vec;
   switch (((afxParticleEmitterPathData*)mDataBlock)->path_origin_type)
   { 
   case afxParticleEmitterPathData::PATHEMIT_ORIGIN :
      vec = curve_pos;
      vec.normalize();
      break;
   case afxParticleEmitterPathData::PATHEMIT_POINT :
      vec = curve_pos-pe_vector;
      vec.normalize();
      break;
   case afxParticleEmitterPathData::PATHEMIT_VECTOR :
      vec = pe_vector_norm;
      break;
   case afxParticleEmitterPathData::PATHEMIT_TANGENT :
      vec = epaths[i]->evaluateTangentAtTime(param);
      vec.normalize();
      break;
   }

   F32 ejection_offset = mDataBlock->ejectionOffset;
   if(mDataBlock->fade_offset)
      ejection_offset *= fade_amt;

   pNew->pos = pos_start + curve_pos + (vec * ejection_offset);
   pNew->pos_local = pNew->pos;

   pNew->vel = mDataBlock->ejectionInvert ? vec * -initialVel : vec * initialVel;
   pNew->orientDir = vec;
   pNew->acc.set(0, 0, 0);
   pNew->currentAge = age_offset;
   pNew->t_last = 0.0f;

   pNew->radial_v.set(0.0f, 0.0f, 0.0f);

   part_db->initializeParticle(pNew, vel);
   updateKeyData( pNew );
}

void afxParticleEmitterPath::sub_preCompute(const MatrixF& mat)
{
  for( U32 i=0; i < epaths.size(); i++ )
  {
    for( U32 j=0; j < epaths[i]->getNumPoints(); j++ )
    {
      Point3F p = epath_points[i][j];
      mat.mulV(p);
      epaths[i]->setPointPosition(j, p);
    }

    epaths[i]->reBuildPath();
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// DISC

afxParticleEmitterDisc::afxParticleEmitterDisc()
{
  disc_v.set(0,0,1);
  disc_r.set(1,0,0);
}

afxParticleEmitterDisc::~afxParticleEmitterDisc()
{
}

bool afxParticleEmitterDisc::onNewDataBlock(GameBaseData* dptr)
{
  mDataBlock = dynamic_cast<afxParticleEmitterDiscData*>(dptr);
  if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
    return false;
  return true;
}

void afxParticleEmitterDisc::sub_addParticle(const Point3F& pos, const Point3F& vel, const U32 age_offset, S32 part_idx)
{
  Particle* pNew = alloc_particle();
  ParticleData* part_db = pick_particle_type();

  Point3F pos_start = pos;
  if (part_db->constrain_pos)
    pos_start.set(0,0,0);

  F32 initialVel = mDataBlock->ejectionVelocity;
  initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;
  if(mDataBlock->fade_velocity)
    initialVel *= fade_amt;

  // Randomly choose a radius vector
  Point3F r( disc_r );
  F32 radius = mRandF(((afxParticleEmitterDiscData*)mDataBlock)->pe_radius_min, ((afxParticleEmitterDiscData*)mDataBlock)->pe_radius_max);   
  r *= radius;

  // Randomly rotate r about disc_v
  F32 theta = mRandF(0.0f, M_2PI_F);
  AngAxisF thetaRot(disc_v, theta);
  MatrixF temp(true);
  thetaRot.setMatrix(&temp);
  temp.mulP(r);   

  F32 ejection_offset = mDataBlock->ejectionOffset;
  if(mDataBlock->fade_offset)
    ejection_offset *= fade_amt;

  pNew->pos = pos_start + r + (disc_v * ejection_offset);
  pNew->pos_local = pNew->pos;

  pNew->vel = (mDataBlock->ejectionInvert) ? (disc_v * -initialVel) : (disc_v * initialVel);
  pNew->orientDir = disc_v;
  pNew->acc.set(0, 0, 0);
  pNew->currentAge = age_offset;
  pNew->t_last = 0.0f;

  pNew->radial_v = r;

  part_db->initializeParticle(pNew, vel);
  updateKeyData( pNew );
}

void afxParticleEmitterDisc::sub_preCompute(const MatrixF& mat)
{
  disc_v.set(0.0f, 0.0f, 1.0f);
  disc_r.set(1.0f, 0.0f, 0.0f);

  Point3F axis;
  F32 theta = mAcos( mDot(disc_v, pe_vector_norm) );

  if( M_PI_F-theta < POINT_EPSILON )
  {
    disc_v.neg();
  }
  else if( theta > POINT_EPSILON )
  {
    mCross(pe_vector_norm, disc_v, &axis);
    axis.normalize();

    AngAxisF thetaRot(axis, theta);
    MatrixF temp(true);
    thetaRot.setMatrix(&temp);

    temp.mulP(disc_v);            
    temp.mulP(disc_r);            
  }         
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

