//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "math/mathIO.h"
#include "math/mRandom.h"
#include "sceneGraph/sceneState.h"
#include "audio/audioDataBlock.h"
#include "game/fx/weatherLightning.h"

IMPLEMENT_CO_CLIENTEVENT_V1(WeatherLightningStrikeEvent);
IMPLEMENT_CO_DATABLOCK_V1(WeatherLightningData);
IMPLEMENT_CO_NETOBJECT_V1(WeatherLightning);

MRandomLCG sgRandomGen;

S32 QSORT_CALLBACK cmpWLSounds(const void* p1, const void* p2)
{
   U32 i1 = *((const S32*)p1);
   U32 i2 = *((const S32*)p2);

   if (i1 < i2) {
      return 1;
   } else if (i1 > i2) {
      return -1;
   } else {
      return 0;
   }
}

S32 QSORT_CALLBACK cmpWLTextures(const void* t1, const void* t2)
{
   StringTableEntry ta = *(StringTableEntry*)t1;
   StringTableEntry tb = *(StringTableEntry*)t2;
   
   if(ta && ta[0] != '\0')
   {
      if(tb && tb[0] != '\0')
         return dStricmp(ta, tb);
      else
         return -1;
   }
   else
   {
      if(tb && tb[0] != '\0')
         return 1;
      else
         return 0;
   }
}

WeatherLightningStrikeEvent::WeatherLightningStrikeEvent()
{
   mLightning = NULL;
}

WeatherLightningStrikeEvent::~WeatherLightningStrikeEvent()
{
   
}

void WeatherLightningStrikeEvent::pack(NetConnection* con, BitStream* stream)
{
   if(!mLightning)
   {
      stream->writeFlag(false);
      return;
   }
   
   S32 ghostIndex = con->getGhostIndex(mLightning);
   if(ghostIndex == -1)
   {
      stream->writeFlag(false);
      return;
   }
   
   stream->writeFlag(true);
   stream->writeRangedU32(U32(ghostIndex), 0, NetConnection::MaxGhostCount);
   stream->writeFloat(mStart.x, PositionalBits);
   stream->writeFloat(mStart.y, PositionalBits);
}

void WeatherLightningStrikeEvent::unpack(NetConnection* con, BitStream* stream)
{
   if(!stream->readFlag())
      return;
   S32 ghostIndex = stream->readRangedU32(0, NetConnection::MaxGhostCount);
   mLightning = NULL;
   NetObject* pObject = con->resolveGhost(ghostIndex);
   if(pObject)
      mLightning = dynamic_cast<WeatherLightning*>(pObject);

   mStart.x = stream->readFloat(PositionalBits);
   mStart.y = stream->readFloat(PositionalBits);
}

void WeatherLightningStrikeEvent::process(NetConnection*)
{
   if (mLightning)
      mLightning->processEvent(this);
}

//--------------------------------------------------------------------------

WeatherLightningData::WeatherLightningData()
{
   dMemset(strikeTextureNames, 0, sizeof(strikeTextureNames));
   dMemset(flashTextureNames,  0, sizeof(flashTextureNames));
   dMemset(fuzzyTextureNames,  0, sizeof(fuzzyTextureNames));
   
   dMemset(strikeTextures, 0, sizeof(strikeTextures));
   dMemset(flashTextures,  0, sizeof(flashTextures));
   dMemset(fuzzyTextures,  0, sizeof(fuzzyTextures));
   
   strikeSoundId = -1;
   strikeSound = NULL_AUDIOHANDLE;
   for(U32 i = 0; i < MaxSounds; i++)
   {
      thunderSoundIds[i] = -1;
      thunderSounds[i] = NULL_AUDIOHANDLE;
   }
}

WeatherLightningData::~WeatherLightningData()
{
   //
};

void WeatherLightningData::initPersistFields()
{
   Parent::initPersistFields();
   
   addField("strikeTextures", TypeFilename, Offset(strikeTextureNames, WeatherLightningData), MaxStrikeTextures);
   addField("flashTextures",  TypeFilename, Offset(flashTextureNames,  WeatherLightningData), MaxFlashTextures);
   addField("fuzzyTextures",  TypeFilename, Offset(fuzzyTextureNames,  WeatherLightningData), MaxFuzzyTextures);
   
   addField("strikeSound",    TypeAudioProfilePtr, Offset(strikeSound,   WeatherLightningData));
   addField("thunderSounds",  TypeAudioProfilePtr, Offset(thunderSounds, WeatherLightningData), MaxSounds);
}

bool WeatherLightningData::onAdd()
{
   if(!Parent::onAdd())
      return false;
   
   if(!strikeSound && strikeSoundId != -1)
   {
      if(Sim::findObject(strikeSoundId, strikeSound) == false)
         Con::errorf(ConsoleLogEntry::General, "WeatherLightningData::onAdd: Invalid packet, bad datablockId(sound: %d", strikeSound);
   }
   
   for(U32 i = 0; i < MaxSounds; i++)
   {
      if(!thunderSounds[i] && thunderSoundIds[i] != -1)
      {
         if(Sim::findObject(thunderSoundIds[i], thunderSounds[i]) == false)
            Con::errorf(ConsoleLogEntry::General, "WeahterLightningData::onAdd: Invalid packet, bad datablockId(sound: %d", thunderSounds[i]);
      }
   }
   
   return true;
}

bool WeatherLightningData::preload(bool server, char errorBuffer[256])
{
   if(Parent::preload(server, errorBuffer) == false)
      return false;
   
   dQsort(strikeTextureNames, MaxStrikeTextures, sizeof(StringTableEntry), cmpWLTextures);
   dQsort(flashTextureNames,  MaxFlashTextures,  sizeof(StringTableEntry), cmpWLTextures);
   dQsort(fuzzyTextureNames,  MaxFuzzyTextures,  sizeof(StringTableEntry), cmpWLTextures);

   if(!server) 
   {
      for(numStrikes = 0; numStrikes < MaxStrikeTextures; numStrikes++)
      {
         if(strikeTextureNames[numStrikes] && strikeTextureNames[numStrikes][0] != '\0')
            strikeTextures[numStrikes] = TextureHandle(strikeTextureNames[numStrikes], MeshTexture);
         else
            break;
      }     
      for(numFlashes = 0; numFlashes < MaxFlashTextures && flashTextureNames[numFlashes] != NULL; numFlashes++)
      {
         if(flashTextureNames[numFlashes] && flashTextureNames[numFlashes][0] != '\0')
            flashTextures[numFlashes] = TextureHandle(flashTextureNames[numFlashes], MeshTexture);
         else
            break;
      }
      for(numFuzzes = 0; numFuzzes < MaxFuzzyTextures && fuzzyTextureNames[numFuzzes] != NULL; numFuzzes++)
      {
         if(fuzzyTextureNames[numFuzzes] && fuzzyTextureNames[numFuzzes][0] != '\0')
            fuzzyTextures[numFuzzes] = TextureHandle(fuzzyTextureNames[numFuzzes], MeshTexture);
         else
            break;
      }
   }
   
   dQsort(thunderSounds, MaxSounds, sizeof(AudioProfile*), cmpWLSounds);
   for(numSounds = 0; numSounds < MaxSounds && thunderSounds[numSounds] != NULL_AUDIOHANDLE; numSounds++) {
      //
   }

   return true;
}

void WeatherLightningData::packData(BitStream* stream)
{
   Parent::packData(stream);

   U32 i;
   for (i = 0; i < MaxStrikeTextures; i++)
      stream->writeString(strikeTextureNames[i]);
   for(i = 0; i < MaxFlashTextures; i++)
      stream->writeString(flashTextureNames[i]);
   for(i = 0; i < MaxFuzzyTextures; i++)
      stream->writeString(fuzzyTextureNames[i]);
   
   if(stream->writeFlag(strikeSound != NULL_AUDIOHANDLE))
      stream->writeRangedU32(strikeSound->getId(), DataBlockObjectIdFirst,
                                                   DataBlockObjectIdLast);
   
   for(i = 0; i < MaxSounds; i++) 
   {
      if(stream->writeFlag(thunderSounds[i] != NULL_AUDIOHANDLE)) 
         stream->writeRangedU32(thunderSounds[i]->getId(), DataBlockObjectIdFirst,
                                                           DataBlockObjectIdLast);
   }
}

void WeatherLightningData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   U32 i;
   for(i = 0; i < MaxStrikeTextures; i++)
      strikeTextureNames[i] = stream->readSTString();
   for(i = 0; i < MaxFlashTextures; i++)
      flashTextureNames[i] = stream->readSTString();
   for(i = 0; i < MaxFuzzyTextures; i++)
      fuzzyTextureNames[i] = stream->readSTString();
     
   if(stream->readFlag())
      strikeSoundId = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
   else
      strikeSoundId = -1;   
   
   for(i = 0; i < MaxSounds; i++)
   {
      if(stream->readFlag())
         thunderSoundIds[i] = stream->readRangedU32(DataBlockObjectIdFirst, DataBlockObjectIdLast);
      else
         thunderSoundIds[i] = -1;
   }
}

//--------------------------------------------------------------------------

WeatherLightning::WeatherLightning()
{
   mNetFlags.set(Ghostable | ScopeAlways);
   mTypeMask |= StaticObjectType|EnvironmentObjectType;
   
   lastThink = 0;
   strikesPerMinute = 9;
   boltDeathAge = 1.5;
}

WeatherLightning::~WeatherLightning()
{
   //
}

void WeatherLightning::initPersistFields()
{
   Parent::initPersistFields();
   
   addNamedField(strikesPerMinute, TypeS32, WeatherLightning);
   addNamedField(boltDeathAge,     TypeF32, WeatherLightning);
}

bool WeatherLightning::onAdd()
{
   if(!Parent::onAdd())
      return false;
   
   mObjBox.min.set( -0.5, -0.5, -0.5 );
   mObjBox.max.set(  0.5,  0.5,  0.5 );
   resetWorldBox();
   
   addToScene();
   return true;
}

void WeatherLightning::onRemove()
{
   while(mActiveBolts.size())
   {
      WeatherLightningBolt* bolt = mActiveBolts[0];
      delete bolt;
      mActiveBolts.erase_fast(U32(0));
   }
   while(mSoundEvents.size())
      mSoundEvents.erase_fast(U32(0));
   
   removeFromScene();
   Parent::onRemove();
}

bool WeatherLightning::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<WeatherLightningData*>(dptr);
   if(!mDataBlock || !Parent::onNewDataBlock(dptr))
      return false;

   scriptOnNewDataBlock();
   return true;
}

bool WeatherLightning::prepRenderImage(SceneState* state, const U32 stateKey, const U32, const bool)
{
   if(isLastState(state, stateKey))
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if(state->isObjectRendered(this))
   {
      SceneRenderImage* image = new SceneRenderImage;
      image->obj = this;
      image->isTranslucent = true;
      image->sortType = SceneRenderImage::EndSort;
      state->insertRenderImage(image);
   }

   return false;
}

void WeatherLightningBolt::render(const Point3F &camPos)
{
   Point3F perpVec;
   Point3F lightUp = startPoint - endPoint;
   mCross(camPos - endPoint, lightUp, &perpVec);
   perpVec.normalize();
   
   Point3F frontVec;
   mCross(perpVec, lightUp, &frontVec);
   frontVec.normalize();
   
   glEnable(GL_DEPTH_TEST);
   glDepthMask(GL_FALSE);
   glDepthFunc(GL_LEQUAL);
   
   //
   // strike texture
   //
   
      // setup alpha value
      F32 strikeAlpha;
      if(currentAge < (strikeTime / 3.0))
      {
         strikeAlpha = currentAge / (strikeTime / 3.0);
         strikeAlpha = mPow(strikeAlpha, F32(1.0 / 3.0));
      }
      else if(currentAge < (2.0 * strikeTime / 3.0))
         strikeAlpha = 1.0;
      else
         strikeAlpha = 1.0 - ((currentAge - (2.0 * strikeTime / 3.0)) / (strikeTime / 3.0));
      glColor4f(1.0f, 1.0f, 1.0f, strikeAlpha);
      
      // generate texture coords
      Point3F points[4];
      F32 width = ((startPoint.z - endPoint.z) * 0.125f);
      points[0] = startPoint - perpVec * width;
      points[1] = startPoint + perpVec * width;
      points[2] = endPoint   + perpVec * width;
      points[3] = endPoint   - perpVec * width;
      
      // bind and draw texture
      glBindTexture(GL_TEXTURE_2D, strikeTexture->getGLName());
      glBegin(GL_TRIANGLE_FAN);
         glTexCoord2f(0, 0); glVertex3fv(points[0]);
         glTexCoord2f(0, 1); glVertex3fv(points[1]);
         glTexCoord2f(1, 1); glVertex3fv(points[2]);
         glTexCoord2f(1, 0); glVertex3fv(points[3]);
         //glTexCoord2f(0, 0); glVertex3fv(points[0]);
         //glTexCoord2f(1, 0); glVertex3fv(points[1]);
         //glTexCoord2f(1, 1); glVertex3fv(points[2]);
         //glTexCoord2f(0, 1); glVertex3fv(points[3]);
      glEnd();
   
   //
   // fuzzy texture
   //
   
      // setup alpha value
      F32 constAlpha;
      if(currentAge < strikeTime / 2.0)
         constAlpha = currentAge / (strikeTime / 2.0);
      else if(currentAge < (2.0 * strikeTime / 3.0))
         constAlpha = 1.0 - ((currentAge - (strikeTime / 2.0)) / (strikeTime / 6.0));
      else
         constAlpha = 0.0;
      glColor4f(1.0, 1.0, 1.0, constAlpha);
      
      // generate texture coords
      width *= 4;
      points[0] = startPoint - perpVec * width;
      points[1] = startPoint + perpVec * width;
      points[2] = endPoint   + perpVec * width;
      points[3] = endPoint   - perpVec * width;
      
      if(constAlpha != 0.0)
      {
         // bind and draw texture
         glBindTexture(GL_TEXTURE_2D, fuzzyTexture->getGLName());
         glBegin(GL_TRIANGLE_FAN);
            glTexCoord2f(0, 0); glVertex3fv(points[0]);
            glTexCoord2f(0, 1); glVertex3fv(points[1]);
            glTexCoord2f(1, 1); glVertex3fv(points[2]);
            glTexCoord2f(1, 0); glVertex3fv(points[3]);
            //glTexCoord2f(0, 0); glVertex3fv(points[0]);
            //glTexCoord2f(1, 0); glVertex3fv(points[1]);
            //glTexCoord2f(1, 1); glVertex3fv(points[2]);
            //glTexCoord2f(0, 1); glVertex3fv(points[3]);
         glEnd();
         
         glDepthMask(GL_TRUE);
      }
   
   //
   // flash texture
   //
   
      // setup alpha value
      glColor4f(1.0f, 1.0f, 1.0f, strikeAlpha);
      
      // generate texture coords
      points[0] = startPoint - perpVec * width + frontVec * width;
      points[1] = startPoint - perpVec * width - frontVec * width;
      points[2] = startPoint + perpVec * width - frontVec * width;
      points[3] = startPoint + perpVec * width + frontVec * width;

      // bind and draw texture
      glBindTexture(GL_TEXTURE_2D, flashTexture->getGLName());
      glBegin(GL_TRIANGLE_FAN);
         glTexCoord2f(0, 0); glVertex3fv(points[0]);
         glTexCoord2f(0, 1); glVertex3fv(points[1]);
         glTexCoord2f(1, 1); glVertex3fv(points[2]);
         glTexCoord2f(1, 0); glVertex3fv(points[3]);
         //glTexCoord2f(0, 0); glVertex3fv(points[0]);
         //glTexCoord2f(1, 0); glVertex3fv(points[1]);
         //glTexCoord2f(1, 1); glVertex3fv(points[2]);
         //glTexCoord2f(0, 1); glVertex3fv(points[3]);
      glEnd();
      
   //
   // finished
   //
   glDepthMask(GL_TRUE);
}

void WeatherLightning::renderObject(SceneState* state, SceneRenderImage*)
{
   if(mActiveBolts.size())
   {
      AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

      RectI viewport;
      F64 farPlane;
      
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      
      dglGetViewport(&viewport);
      farPlane = state->getFarPlane();
      const Point3F &camPos = state->getCameraPosition();
    
      // adjust far clip plane
      F64 distance = (getPosition() - camPos).lenSquared();
      state->setFarPlane(getMax(farPlane, distance));
      
      state->setupObjectProjection(this);

      glDisable(GL_CULL_FACE);
      glEnable(GL_TEXTURE_2D);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
         
      for(U32 i = 0; i < mActiveBolts.size(); i++)
         mActiveBolts[i]->render(camPos);

      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glDisable(GL_BLEND);
      glDisable(GL_TEXTURE_2D);

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
      
      state->setFarPlane(farPlane);
      dglSetViewport(viewport);

      AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
   }
}

void WeatherLightning::processTick(const Move *move)
{
   Parent::processTick(move);
   
   if (isServerObject()) 
   {
      S32 msBetweenStrikes = (S32)(60.0 / strikesPerMinute * 1000.0);
      
      lastThink += TickMs;
      if( lastThink > msBetweenStrikes )
      {
         strikeRandomPoint();
         lastThink -= msBetweenStrikes;
      }
   }
}

void WeatherLightning::advanceTime(F32 dt)
{
   Parent::advanceTime(dt);
   
   U32 i;
   
   // loop through and erase any dead bolts
   for(i = 0; i < mActiveBolts.size();)
   {
      WeatherLightningBolt* bolt = mActiveBolts[i];
      bolt->currentAge += dt;
      if(bolt->currentAge > bolt->deathAge)
      {
         delete bolt;
         mActiveBolts.erase_fast(i);
         continue;
      }
      
      i++;
   }
   
   // loop through and find any pending sound events
   for(i = 0; i < mSoundEvents.size();)
   {
      SoundEvent *sEvent = &mSoundEvents[i];
      sEvent->time -= dt;
      if(sEvent->time <= 0.0)
      {
         // fire off the sound
         if(sEvent->soundBlockId != -1)
            alxPlay(mDataBlock->thunderSounds[sEvent->soundBlockId], &sEvent->position);
         mSoundEvents.erase_fast(i);
         continue;
      }
      
      i++;
   } 
}

void WeatherLightning::strikeRandomPoint()
{  
   // choose random strike point within object bounds
   Point2F strikePoint;
   strikePoint.x = sgRandomGen.randF( 0.0, 1.0 );
   strikePoint.y = sgRandomGen.randF( 0.0, 1.0 );
   
   SimGroup* pClientGroup = Sim::getClientGroup();
   for (SimGroup::iterator itr = pClientGroup->begin(); itr != pClientGroup->end(); itr++)
   {
      NetConnection* nc = static_cast<NetConnection*>(*itr);

      WeatherLightningStrikeEvent* wlEvent = new WeatherLightningStrikeEvent;
      wlEvent->mLightning = this;
      wlEvent->mStart = strikePoint;

      nc->postNetEvent(wlEvent);
   }
}

TextureHandle* WeatherLightning::getRandomStrike()
{
   U32 strike = (U32)(mCeil(mDataBlock->numStrikes * sgRandomGen.randF()) - 1.0f);
   return &mDataBlock->strikeTextures[strike]; 
}

TextureHandle* WeatherLightning::getRandomFlash()
{
   U32 flash = (U32)(mCeil(mDataBlock->numFlashes * sgRandomGen.randF()) - 1.0f);
   return &mDataBlock->flashTextures[flash];
}

TextureHandle* WeatherLightning::getRandomFuzzy()
{
   U32 fuzzy = (U32)(mCeil(mDataBlock->numFuzzes * sgRandomGen.randF()) - 1.0f);
   return &mDataBlock->fuzzyTextures[fuzzy];
}

S32 WeatherLightning::getRandomSound()
{
   U32 sound = (U32)(mCeil(mDataBlock->numSounds * sgRandomGen.randF()) - 1.0f);
   if(mDataBlock->thunderSounds[sound] != NULL_AUDIOHANDLE)
      return sound;
   
   return -1;
}

void WeatherLightning::processEvent(WeatherLightningStrikeEvent* wlEvent)
{
   AssertFatal(wlEvent->mStart.x >= 0 && wlEvent->mStart.x <= 1.0, "Out of bounds coord!");

   mActiveBolts.push_back(new WeatherLightningBolt);
   WeatherLightningBolt* bolt = mActiveBolts.last();
   
   Point3F strikePoint(0.0, 0.0, 0.0);
   strikePoint.x = wlEvent->mStart.x;
   strikePoint.y = wlEvent->mStart.y;
   strikePoint *= mObjScale;
   strikePoint += getPosition();
   strikePoint += Point3F( -mObjScale.x * 0.5, -mObjScale.y * 0.5, 0.0 );

   RayInfo rayInfo;
   Point3F start = strikePoint;
   start.z = mObjScale.z * 0.5 + getPosition().z;
   strikePoint.z += -mObjScale.z * 0.5;
   bool rayHit = gClientContainer.castRay(start, strikePoint, (STATIC_COLLISION_MASK | WaterObjectType), &rayInfo);
   if(rayHit)
      strikePoint.z = rayInfo.point.z;
   
   F32 height = mObjScale.z * 0.5 + getPosition().z;
   
   bolt->startPoint = Point3F(strikePoint.x, strikePoint.y, height);
   bolt->endPoint   = strikePoint;
   
   bolt->currentAge       = 0.0f;
   bolt->deathAge         = boltDeathAge;
   bolt->strikeTime       = 0.35;
   
   bolt->strikeTexture = getRandomStrike();
   bolt->flashTexture  = getRandomFlash();
   bolt->fuzzyTexture  = getRandomFuzzy();
   
   // setup a thunder sound event
   Point3F listener;
   alxGetListenerPoint3F(AL_POSITION, &listener);
   
   mSoundEvents.increment();
   SoundEvent& sEvent = mSoundEvents.last();
   
   // find the length to the closest point on the bolt
   Point3F dHat = bolt->startPoint - bolt->endPoint;
   F32 boltLength = dHat.len();
   dHat /= boltLength;
   F32 distAlong = mDot((listener - bolt->endPoint), dHat);
   
   Point3F contactPoint;
   if(distAlong >= boltLength)
      contactPoint = bolt->startPoint;
   else if(distAlong <= 0.0)
      contactPoint = bolt->endPoint;
   else
      contactPoint = bolt->endPoint + dHat * distAlong;
      
   F32 delayDist = (listener - contactPoint).len();
   U32 delayTime = U32((delayDist / 330.0f) * 100.0f);
   
   MatrixF trans(true);
   trans.setPosition(contactPoint);
   
   sEvent.soundBlockId = getRandomSound();
   sEvent.position = trans;
   sEvent.time = delayTime;
   
   // play strike sound
   trans.setPosition(strikePoint);
   if(mDataBlock->strikeSound)
      alxPlay(mDataBlock->strikeSound, &trans);
}

U32 WeatherLightning::packUpdate(NetConnection *conn, U32 mask, BitStream *stream)
{
   U32 retMask = Parent::packUpdate(conn, mask, stream);

   // Only write data if this is the initial packet or we've been inspected.  
   if (stream->writeFlag(mask & (InitialUpdateMask | ExtendedInfoMask))) 
   {
      // Initial update
      mathWrite(*stream, getPosition());
      mathWrite(*stream, mObjScale);
   }

   return retMask;
}

void WeatherLightning::unpackUpdate(NetConnection *conn, BitStream *stream)
{
   Parent::unpackUpdate(conn, stream);

   if (stream->readFlag())
   {
      // Initial update
      Point3F pos;
      mathRead(*stream, &pos);
      setPosition( pos );

      mathRead(*stream, &mObjScale);
   }
}
