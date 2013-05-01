//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//        enhanced-emitter -- numerous enhancements to ParticleEmitter class.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "particleEmitter.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mRandom.h"
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
#include "gfx/gfxDevice.h"
#include "gfx/primBuilder.h"
#include "renderInstance/renderInstMgr.h"
#else
#include "platform/profiler.h"
#include "dgl/dgl.h"
#endif


static ParticleEmitterData gDefaultEmitterData;
Point3F ParticleEmitter::mWindVelocity( 0.0, 0.0, 0.0 );

IMPLEMENT_CO_DATABLOCK_V1(ParticleEmitterData);

//-----------------------------------------------------------------------------
// ParticleEmitterData
//-----------------------------------------------------------------------------
ParticleEmitterData::ParticleEmitterData()
{
   VECTOR_SET_ASSOCIATION(particleDataBlocks);
   VECTOR_SET_ASSOCIATION(dataBlockIds);

   ejectionPeriodMS = 100;    // 10 Particles Per second
   periodVarianceMS = 0;      // exactly

   ejectionVelocity = 2.0f;   // From 1.0 - 3.0 meters per sec
   velocityVariance = 1.0f;
   ejectionOffset   = 0.0f;   // ejection from the emitter point

   thetaMin         = 0.0f;   // All heights
   thetaMax         = 90.0f;

   phiReferenceVel  = 0.0f;   // All directions
   phiVariance      = 360.0f;

   lifetimeMS           = 0;
   lifetimeVarianceMS   = 0;

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   overrideAdvance  = true;
#else
   overrideAdvance  = false;
#endif
   orientParticles  = false;
   orientOnVelocity = true;
   useEmitterSizes  = false;
   useEmitterColors = false;
   particleString   = NULL;
   partListInitSize = 0;

#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   mixedInvAlpha =  false;   // not networked -- used to flag mixed InvAlpha settings in single emitter
   oldstyleAnimated = false; // not networked -- used to flag old style animated particles
#endif

   // These members added for support of user defined blend factors
   // and optional particle sorting.
   blendStyle = BlendUndefined;
   // AFX CODE BLOCK (to-tgea-from-tge) <<
   srcBlendFactor = BLEND_UNDEFINED;
   dstBlendFactor = BLEND_UNDEFINED;
   /* ORIGINAL CODE
   srcBlendFactor = GFXBlendUndefined;
   dstBlendFactor = GFXBlendUndefined;
   */
   // AFX CODE BLOCK (to-tgea-from-tge) >>
   sortParticles = false;
   reverseOrder = false;
   textureName = 0;
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   textureHandle = 0;
#endif
   // AFX CODE BLOCK (enhanced-emitter) <<
   ejectionInvert = false;
   fade_color    = false;
   fade_alpha    = false;
   fade_size     = false;
   parts_per_eject = 1;
   use_emitter_xfm = false;
   // AFX CODE BLOCK (enhanced-emitter) >>
}


IMPLEMENT_CONSOLETYPE(ParticleEmitterData)
IMPLEMENT_GETDATATYPE(ParticleEmitterData)
IMPLEMENT_SETDATATYPE(ParticleEmitterData)


// Enum tables used for fields blendStyle, srcBlendFactor, dstBlendFactor.
// Note that the enums for srcBlendFactor and dstBlendFactor are consistent
// with the blending enums used in Torque Game Builder.

static EnumTable::Enums blendStyleLookup[] =
{
    { ParticleEmitterData::BlendNormal,         "NORMAL"                },
    { ParticleEmitterData::BlendAdditive,       "ADDITIVE"              },
    { ParticleEmitterData::BlendSubtractive,    "SUBTRACTIVE"           },
    { ParticleEmitterData::BlendPremultAlpha,   "PREMULTALPHA"          },
    { ParticleEmitterData::BlendUser,           "USER"                  },
};
EnumTable blendStyleTable(sizeof(blendStyleLookup) / sizeof(EnumTable::Enums), &blendStyleLookup[0]);

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
static EnumTable::Enums srcBlendFactorLookup[] =
{
    { GFXBlendZero,                 "ZERO"                  },
    { GFXBlendOne,                  "ONE"                   },
    { GFXBlendDestColor,            "DST_COLOR"             },
    { GFXBlendInvDestColor,         "ONE_MINUS_DST_COLOR"   },
    { GFXBlendSrcAlpha,             "SRC_ALPHA"             },
    { GFXBlendInvSrcAlpha,          "ONE_MINUS_SRC_ALPHA"   },
    { GFXBlendDestAlpha,            "DST_ALPHA"             },
    { GFXBlendInvDestAlpha,         "ONE_MINUS_DST_ALPHA"   },
    { GFXBlendSrcAlphaSat,          "SRC_ALPHA_SATURATE"    },
};
EnumTable srcBlendFactorTable(sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums), &srcBlendFactorLookup[0]);

static EnumTable::Enums dstBlendFactorLookup[] =
{
    { GFXBlendZero,                 "ZERO"                  },
    { GFXBlendOne,                  "ONE"                   },
    { GFXBlendSrcColor,             "SRC_COLOR"             },
    { GFXBlendInvSrcColor,          "ONE_MINUS_SRC_COLOR"   },
    { GFXBlendSrcAlpha,             "SRC_ALPHA"             },
    { GFXBlendInvSrcAlpha,          "ONE_MINUS_SRC_ALPHA"   },
    { GFXBlendDestAlpha,            "DST_ALPHA"             },
    { GFXBlendInvDestAlpha,         "ONE_MINUS_DST_ALPHA"   },
};
EnumTable dstBlendFactorTable(sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums), &dstBlendFactorLookup[0]);
#else
static EnumTable::Enums srcBlendFactorLookup[] =
{
    { GL_ZERO,                      "ZERO"                  },
    { GL_ONE,                       "ONE"                   },
    { GL_DST_COLOR,                 "DST_COLOR"             },
    { GL_ONE_MINUS_DST_COLOR,       "ONE_MINUS_DST_COLOR"   },
    { GL_SRC_ALPHA,                 "SRC_ALPHA"             },
    { GL_ONE_MINUS_SRC_ALPHA,       "ONE_MINUS_SRC_ALPHA"   },
    { GL_DST_ALPHA,                 "DST_ALPHA"             },
    { GL_ONE_MINUS_DST_ALPHA,       "ONE_MINUS_DST_ALPHA"   },
    { GL_SRC_ALPHA_SATURATE,        "SRC_ALPHA_SATURATE"    },
};
EnumTable srcBlendFactorTable(sizeof(srcBlendFactorLookup) / sizeof(EnumTable::Enums), &srcBlendFactorLookup[0]);

static EnumTable::Enums dstBlendFactorLookup[] =
{
    { GL_ZERO,                      "ZERO"                  },
    { GL_ONE,                       "ONE"                   },
    { GL_SRC_COLOR,                 "SRC_COLOR"             },
    { GL_ONE_MINUS_SRC_COLOR,       "ONE_MINUS_SRC_COLOR"   },
    { GL_SRC_ALPHA,                 "SRC_ALPHA"             },
    { GL_ONE_MINUS_SRC_ALPHA,       "ONE_MINUS_SRC_ALPHA"   },
    { GL_DST_ALPHA,                 "DST_ALPHA"             },
    { GL_ONE_MINUS_DST_ALPHA,       "ONE_MINUS_DST_ALPHA"   },
};
EnumTable dstBlendFactorTable(sizeof(dstBlendFactorLookup) / sizeof(EnumTable::Enums), &dstBlendFactorLookup[0]);
#endif


//-----------------------------------------------------------------------------
// initPersistFields
//-----------------------------------------------------------------------------
void ParticleEmitterData::initPersistFields()
{
   Parent::initPersistFields();

   addField("ejectionPeriodMS",     TypeS32,    Offset(ejectionPeriodMS,   ParticleEmitterData));
   addField("periodVarianceMS",     TypeS32,    Offset(periodVarianceMS,   ParticleEmitterData));
   addField("ejectionVelocity",     TypeF32,    Offset(ejectionVelocity,   ParticleEmitterData));
   addField("velocityVariance",     TypeF32,    Offset(velocityVariance,   ParticleEmitterData));
   addField("ejectionOffset",       TypeF32,    Offset(ejectionOffset,     ParticleEmitterData));
   addField("thetaMin",             TypeF32,    Offset(thetaMin,           ParticleEmitterData));
   addField("thetaMax",             TypeF32,    Offset(thetaMax,           ParticleEmitterData));
   addField("phiReferenceVel",      TypeF32,    Offset(phiReferenceVel,    ParticleEmitterData));
   addField("phiVariance",          TypeF32,    Offset(phiVariance,        ParticleEmitterData));
   addField("overrideAdvance",      TypeBool,   Offset(overrideAdvance,    ParticleEmitterData));
   addField("orientParticles",      TypeBool,   Offset(orientParticles,    ParticleEmitterData));
   addField("orientOnVelocity",     TypeBool,   Offset(orientOnVelocity,   ParticleEmitterData));
   addField("particles",            TypeString, Offset(particleString,     ParticleEmitterData));
   addField("lifetimeMS",           TypeS32,    Offset(lifetimeMS,         ParticleEmitterData));
   addField("lifetimeVarianceMS",   TypeS32,    Offset(lifetimeVarianceMS, ParticleEmitterData));
   addField("useEmitterSizes",      TypeBool,   Offset(useEmitterSizes,    ParticleEmitterData));
   addField("useEmitterColors",     TypeBool,   Offset(useEmitterColors,   ParticleEmitterData));

   // These fields added for support of user defined blend factors and optional particle sorting.
   addField("blendStyle",         TypeEnum,     Offset(blendStyle,         ParticleEmitterData), 1, &blendStyleTable);
   addField("srcBlendFactor",     TypeEnum,     Offset(srcBlendFactor,     ParticleEmitterData), 1, &srcBlendFactorTable);
   addField("dstBlendFactor",     TypeEnum,     Offset(dstBlendFactor,     ParticleEmitterData), 1, &dstBlendFactorTable);
   addField("sortParticles",      TypeBool,     Offset(sortParticles,      ParticleEmitterData));
   addField("reverseOrder",       TypeBool,     Offset(reverseOrder,       ParticleEmitterData));
   addField("textureName",        TypeFilename, Offset(textureName,        ParticleEmitterData));

   // AFX CODE BLOCK (enhanced-emitter) <<
   addField("ejectionInvert",       TypeBool,    Offset(ejectionInvert,     ParticleEmitterData));
   addField("fadeColor",            TypeBool,    Offset(fade_color,         ParticleEmitterData));
   addField("fadeAlpha",            TypeBool,    Offset(fade_alpha,         ParticleEmitterData));
   addField("fadeSize",             TypeBool,    Offset(fade_size,          ParticleEmitterData));
   // useEmitterTransform currently does not work in TGEA
   addField("useEmitterTransform",  TypeBool,    Offset(use_emitter_xfm,    ParticleEmitterData));
   // AFX CODE BLOCK (enhanced-emitter) >>
}

//-----------------------------------------------------------------------------
// packData
//-----------------------------------------------------------------------------
void ParticleEmitterData::packData(BitStream* stream)
{
   Parent::packData(stream);

   stream->writeInt(ejectionPeriodMS, 10);
   stream->writeInt(periodVarianceMS, 10);
   stream->writeInt((S32)(ejectionVelocity * 100), 16);
   stream->writeInt((S32)(velocityVariance * 100), 14);
   if( stream->writeFlag(ejectionOffset != gDefaultEmitterData.ejectionOffset) )
      stream->writeInt((S32)(ejectionOffset * 100), 16);
   stream->writeRangedU32((U32)thetaMin, 0, 180);
   stream->writeRangedU32((U32)thetaMax, 0, 180);
   if( stream->writeFlag(phiReferenceVel != gDefaultEmitterData.phiReferenceVel) )
      stream->writeRangedU32((U32)phiReferenceVel, 0, 360);
   if( stream->writeFlag(phiVariance != gDefaultEmitterData.phiVariance) )
      stream->writeRangedU32((U32)phiVariance, 0, 360);
   stream->writeFlag(overrideAdvance);
   stream->writeFlag(orientParticles);
   stream->writeFlag(orientOnVelocity);
   stream->write( lifetimeMS );
   stream->write( lifetimeVarianceMS );
   stream->writeFlag(useEmitterSizes);
   stream->writeFlag(useEmitterColors);

   stream->write(dataBlockIds.size());
   for (U32 i = 0; i < dataBlockIds.size(); i++)
      stream->write(dataBlockIds[i]);

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   stream->writeInt(srcBlendFactor, 4);
   stream->writeInt(dstBlendFactor, 4);
#else
   stream->writeInt(srcBlendFactor, 16);
   stream->writeInt(dstBlendFactor, 16);
#endif

   stream->writeFlag(sortParticles);
   stream->writeFlag(reverseOrder);
   if (stream->writeFlag(textureName != 0))
     stream->writeString(textureName);

   // AFX CODE BLOCK (enhanced-emitter) <<
   stream->writeFlag(ejectionInvert);
   stream->writeFlag(fade_color);
   stream->writeFlag(fade_alpha);
   stream->writeFlag(fade_size);
   stream->writeFlag(use_emitter_xfm);
   // AFX CODE BLOCK (enhanced-emitter) >>
}

//-----------------------------------------------------------------------------
// unpackData
//-----------------------------------------------------------------------------
void ParticleEmitterData::unpackData(BitStream* stream)
{
   Parent::unpackData(stream);

   ejectionPeriodMS = stream->readInt(10);
   periodVarianceMS = stream->readInt(10);
   ejectionVelocity = stream->readInt(16) / 100.0f;
   velocityVariance = stream->readInt(14) / 100.0f;
   if( stream->readFlag() )
      ejectionOffset = stream->readInt(16) / 100.0f;
   else
      ejectionOffset = gDefaultEmitterData.ejectionOffset;

   thetaMin = (F32)stream->readRangedU32(0, 180);
   thetaMax = (F32)stream->readRangedU32(0, 180);
   if( stream->readFlag() )
      phiReferenceVel = (F32)stream->readRangedU32(0, 360);
   else
      phiReferenceVel = gDefaultEmitterData.phiReferenceVel;

   if( stream->readFlag() )
      phiVariance = (F32)stream->readRangedU32(0, 360);
   else
      phiVariance = gDefaultEmitterData.phiVariance;

   overrideAdvance = stream->readFlag();
   orientParticles = stream->readFlag();
   orientOnVelocity = stream->readFlag();
   stream->read( &lifetimeMS );
   stream->read( &lifetimeVarianceMS );
   useEmitterSizes = stream->readFlag();
   useEmitterColors = stream->readFlag();

   U32 size; stream->read(&size);
   dataBlockIds.setSize(size);
   for (U32 i = 0; i < dataBlockIds.size(); i++)
      stream->read(&dataBlockIds[i]);

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   srcBlendFactor = stream->readInt(4);
   dstBlendFactor = stream->readInt(4);
#else
   srcBlendFactor = stream->readInt(16);
   dstBlendFactor = stream->readInt(16);
#endif

   sortParticles = stream->readFlag();
   reverseOrder = stream->readFlag();
   textureName = (stream->readFlag()) ? stream->readSTString() : 0;

   // AFX CODE BLOCK (enhanced-emitter) <<
   ejectionInvert = stream->readFlag();
   fade_color    = stream->readFlag();
   fade_alpha    = stream->readFlag();
   fade_size     = stream->readFlag();
   use_emitter_xfm = stream->readFlag();
   // AFX CODE BLOCK (enhanced-emitter) >>
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleEmitterData::onAdd()
{
   if( Parent::onAdd() == false )
      return false;

//   if (overrideAdvance == true) {
//      Con::errorf(ConsoleLogEntry::General, "ParticleEmitterData: Not going to work.  Fix it!");
//      return false;
//   }

   return loadParameters();
}

bool ParticleEmitterData::loadParameters()
{
   // Validate the parameters...
   //
   if( ejectionPeriodMS < 1 )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) period < 1 ms", getName());
      ejectionPeriodMS = 1;
   }
   if( periodVarianceMS >= ejectionPeriodMS )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) periodVariance >= period", getName());
      periodVarianceMS = ejectionPeriodMS - 1;
   }
   if( ejectionVelocity < 0.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) ejectionVelocity < 0.0f", getName());
      ejectionVelocity = 0.0f;
   }
   if( velocityVariance > ejectionVelocity )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) velocityVariance > ejectionVelocity", getName());
      velocityVariance = ejectionVelocity;
   }
   if( ejectionOffset < 0.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) ejectionOffset < 0", getName());
      ejectionOffset = 0.0f;
   }
   if( thetaMin < 0.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) thetaMin < 0.0", getName());
      thetaMin = 0.0f;
   }
   if( thetaMax > 180.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) thetaMax > 180.0", getName());
      thetaMax = 180.0f;
   }
   if( thetaMin > thetaMax )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) thetaMin > thetaMax", getName());
      thetaMin = thetaMax;
   }
   if( phiVariance < 0.0f || phiVariance > 360.0f )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) invalid phiVariance", getName());
      phiVariance = phiVariance < 0.0f ? 0.0f : 360.0f;
   }

   if (particleString == NULL && dataBlockIds.size() == 0) 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) no particleString, invalid datablock", getName());
      return false;
   }
   if (particleString && particleString[0] == '\0') 
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) no particleString, invalid datablock", getName());
      return false;
   }
   if (particleString && dStrlen(particleString) > 255) 
   {
      Con::errorf(ConsoleLogEntry::General, "ParticleEmitterData(%s) particle string too long [> 255 chars]", getName());
      return false;
   }

   if( lifetimeMS < 0 )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) lifetimeMS < 0.0f", getName());
      lifetimeMS = 0;
   }
   if( lifetimeVarianceMS > lifetimeMS )
   {
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) lifetimeVarianceMS >= lifetimeMS", getName());
      lifetimeVarianceMS = lifetimeMS;
   }


   // load the particle datablocks...
   //
   if( particleString != NULL )
   {
      //   particleString is once again a list of particle datablocks so it
      //   must be parsed to extract the particle references.

      // First we parse particleString into a list of particle name tokens 
      Vector<char*> dataBlocks(__FILE__, __LINE__);
      char* tokCopy = new char[dStrlen(particleString) + 1];
      dStrcpy(tokCopy, particleString);

      char* currTok = dStrtok(tokCopy, " \t");
      while (currTok != NULL) 
      {
         dataBlocks.push_back(currTok);
         currTok = dStrtok(NULL, " \t");
      }
      if (dataBlocks.size() == 0) 
      {
         Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) invalid particles string.  No datablocks found", getName());
         delete [] tokCopy;
         return false;
      }    

      // Now we convert the particle name tokens into particle datablocks and IDs 
      particleDataBlocks.clear();
      dataBlockIds.clear();

      for (U32 i = 0; i < dataBlocks.size(); i++) 
      {
         ParticleData* pData = NULL;
         if (Sim::findObject(dataBlocks[i], pData) == false) 
         {
            Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find particle datablock: %s", getName(), dataBlocks[i]);
         }
         else 
         {
            particleDataBlocks.push_back(pData);
            dataBlockIds.push_back(pData->getId());
         }
      }

      // cleanup
      delete [] tokCopy;

      // check that we actually found some particle datablocks
      if (particleDataBlocks.size() == 0) 
      {
         Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find any particle datablocks", getName());
         return false;
      }
   }

   return true;
}

//-----------------------------------------------------------------------------
// preload
//-----------------------------------------------------------------------------
bool ParticleEmitterData::preload(bool server, char errorBuffer[256])
{
   if( Parent::preload(server, errorBuffer) == false )
      return false;

   if (!reload(server))
      return false;

   return true;
}

bool ParticleEmitterData::reload(bool server)
{
   particleDataBlocks.clear();
   for (U32 i = 0; i < dataBlockIds.size(); i++) 
   {
      ParticleData* pData = NULL;
      if (Sim::findObject(dataBlockIds[i], pData) == false)
         Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) unable to find particle datablock: %d", getName(), dataBlockIds[i]);
      else
         particleDataBlocks.push_back(pData);
   }

   if (!server)
   {
      // load emitter texture if specified
      if (textureName && textureName[0])
      {
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
         textureHandle = GFXTexHandle(textureName, &GFXDefaultStaticDiffuseProfile);
#else
         textureHandle = TextureHandle(textureName, MeshTexture);
#endif
         if (!textureHandle)
         {
            Con::warnf(ConsoleLogEntry::General, "Missing particle emitter texture: %s", textureName);
            return false;
         }
      }
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
      // otherwise, check that all particles refer to the same texture
      else if (particleDataBlocks.size() > 1)
      {
         StringTableEntry txr_name = particleDataBlocks[0]->textureName;
         for (S32 i = 1; i < particleDataBlocks.size(); i++)
         {
            // warn if particle textures are inconsistent
            if (particleDataBlocks[i]->textureName != txr_name)
            {
               Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) particles reference different textures.", getName());
               break;
            }
         }
      }
#endif
   }

   // if blend-style is set to User, check for defined blend-factors
   if (blendStyle == BlendUser && (srcBlendFactor == BLEND_UNDEFINED || dstBlendFactor == BLEND_UNDEFINED))
   {
      blendStyle = BlendUndefined;
      Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) incomplete blend factor specification.", getName());
   }

   // silently switch Undefined blend-style to User if blend factors are both defined
   if (blendStyle == BlendUndefined && srcBlendFactor != BLEND_UNDEFINED && dstBlendFactor != BLEND_UNDEFINED)
   {
      blendStyle = BlendUser;
   }

   // if blend-style is undefined check legacy useInvAlpha settings
   if (blendStyle == BlendUndefined && particleDataBlocks.size() > 0)
   {
      bool useInvAlpha = particleDataBlocks[0]->useInvAlpha;
      for (S32 i = 1; i < particleDataBlocks.size(); i++)
      {
         // warn if blend-style legacy useInvAlpha settings are inconsistent
         if (particleDataBlocks[i]->useInvAlpha != useInvAlpha)
         {
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
            Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) particles have inconsistent useInvAlpha settings.", getName());
#else
            mixedInvAlpha = true; // flag that this emitter's particles have mixed useInvAlpha settings 
#endif
            break;
         }
      }
      blendStyle = (useInvAlpha) ? BlendNormal : BlendAdditive;
   }

#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   // check for old animated particles
   for (S32 i = 0; i < particleDataBlocks.size(); i++)
   {
      // warn if using oldstyle multi-texture animation
      if (particleDataBlocks[i]->oldstyleAnimated)
      {
         Con::warnf(ConsoleLogEntry::General, "ParticleEmitterData(%s) particles use oldstyle multi-tetxure animation.", getName());
         oldstyleAnimated = true;
         break;
      }
   }
#endif

   // set pre-defined blend-factors 
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   switch (blendStyle)
   {
   case BlendNormal:
      srcBlendFactor = GFXBlendSrcAlpha;
      dstBlendFactor = GFXBlendInvSrcAlpha;
      break;
   case BlendSubtractive:
      srcBlendFactor = GFXBlendZero;
      dstBlendFactor = GFXBlendInvSrcColor;
      break;
   case BlendPremultAlpha:
      srcBlendFactor = GFXBlendOne;
      dstBlendFactor = GFXBlendInvSrcAlpha;
      break;
   case BlendUser:
      break;
   case BlendAdditive:
      srcBlendFactor = GFXBlendSrcAlpha;
      dstBlendFactor = GFXBlendOne;
      break;
   case BlendUndefined:
   default:
      blendStyle = BlendAdditive;
      srcBlendFactor = GFXBlendSrcAlpha;
      dstBlendFactor = GFXBlendOne;
      break;
   }
#else
   switch (blendStyle)
   {
   case BlendNormal:
      srcBlendFactor = GL_SRC_ALPHA;
      dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
      break;
   case BlendSubtractive:
      srcBlendFactor = GL_ZERO;
      dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
      break;
   case BlendPremultAlpha:
      srcBlendFactor = GL_ONE;
      dstBlendFactor = GL_ONE_MINUS_SRC_ALPHA;
      break;
   case BlendUser:
      break;
   case BlendAdditive:
      srcBlendFactor = GL_SRC_ALPHA;
      dstBlendFactor = GL_ONE;
      break;
   case BlendUndefined:
   default:
      blendStyle = BlendAdditive;
      srcBlendFactor = GL_SRC_ALPHA;
      dstBlendFactor = GL_ONE;
      break;
   }
#endif
   
   if( !server )
   {
      allocPrimBuffer();
   }

   return true;
}

//-----------------------------------------------------------------------------
// alloc PrimitiveBuffer
// The datablock allocates this static index buffer because it's the same
// for all of the emitters - each particle quad uses the same index ordering
//-----------------------------------------------------------------------------
void ParticleEmitterData::allocPrimBuffer( S32 overrideSize )
{
   AssertFatal(particleDataBlocks.size() > 0, "Error, no particles found." );

   // calculate particle list size
   if (overrideSize == -1)
   {
      // find maximum particle lifetime
      U32 maxPartLife = particleDataBlocks[0]->lifetimeMS + particleDataBlocks[0]->lifetimeVarianceMS;
      for (S32 i = 1; i < particleDataBlocks.size(); i++)
      {
        U32 mpl = particleDataBlocks[i]->lifetimeMS + particleDataBlocks[i]->lifetimeVarianceMS;
        if (mpl > maxPartLife)
          maxPartLife = mpl;
      }

      // calculate particle list size
      partListInitSize = maxPartLife / (ejectionPeriodMS - periodVarianceMS);
      // add 18 as "fudge factor" to make sure it doesn't realloc if it goes over by a little
      // the value of 18 was derived empirically using a highly particle rich fireworks scene
      partListInitSize += 18; 
      // AFX CODE BLOCK (enhanced-emitter) <<
      if (parts_per_eject > 1)
        partListInitSize *= parts_per_eject;
      // AFX CODE BLOCK (enhanced-emitter) >>
   }
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   // use given size
   else
   {
      partListInitSize = overrideSize;
   }
#endif

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   // create index buffer based on that size
   U32 indexListSize = partListInitSize * 6; // 6 indices per particle
   U16 *indices = new U16[ indexListSize ];

   for( U32 i=0; i<partListInitSize; i++ )
   {
      // this index ordering should be optimal (hopefully) for the vertex cache
      U16 *idx = &indices[i*6];
      volatile U32 offset = i * 4;  // set to volatile to fix VC6 Release mode compiler bug
      idx[0] = 0 + offset;
      idx[1] = 1 + offset;
      idx[2] = 3 + offset;
      idx[3] = 1 + offset;
      idx[4] = 3 + offset;
      idx[5] = 2 + offset;
   }

   U16 *ibIndices;
   GFXBufferType bufferType = GFXBufferTypeStatic;

#ifdef TORQUE_OS_XENON
   // Because of the way the volatile buffers work on Xenon this is the only
   // way to do this.
   bufferType = GFXBufferTypeVolatile;
#endif
   primBuff.set( GFX, indexListSize, 0, bufferType );
   primBuff.lock( &ibIndices );
   dMemcpy( ibIndices, indices, indexListSize * sizeof(U16) );
   primBuff.unlock();

   delete [] indices;
#endif
}

//-----------------------------------------------------------------------------
// ParticleEmitter
//-----------------------------------------------------------------------------
ParticleEmitter::ParticleEmitter()
{
   mDeleteWhenEmpty  = false;
   mDeleteOnTick     = false;

   mInternalClock    = 0;
   mNextParticleTime = 0;

   mLastPosition.set(0, 0, 0);
   mHasLastPosition = false;

   mLifetimeMS = 0;
   mElapsedTimeMS = 0;

   //part_store = 0;
   part_freelist = NULL;
   part_list_head.next = NULL;
   n_part_capacity = 0;
   n_parts = 0;

   mCurBuffSize = 0;

   mDead = false;

   // AFX CODE BLOCK (enhanced-emitter) <<
   fade_amt = 1.0f;
   forced_bbox = false;
   pos_pe.set(0,0,0);
   sort_priority = 0;
   // AFX CODE BLOCK (enhanced-emitter) >>
}

//-----------------------------------------------------------------------------
// destructor
//-----------------------------------------------------------------------------
ParticleEmitter::~ParticleEmitter()
{
   for( S32 i = 0; i < part_store.size(); i++ )
   {
      delete [] part_store[i];
   }
}

//-----------------------------------------------------------------------------
// onAdd
//-----------------------------------------------------------------------------
bool ParticleEmitter::onAdd()
{
   if( !Parent::onAdd() )
      return false;

   // add to client side mission cleanup
   SimGroup *cleanup = dynamic_cast<SimGroup *>( Sim::findObject( "ClientMissionCleanup") );
   if( cleanup != NULL )
   {
      cleanup->addObject( this );
   }
   else
   {
      AssertFatal( false, "Error, could not find ClientMissionCleanup group" );
      return false;
   }

   removeFromProcessList();

   mLifetimeMS = mDataBlock->lifetimeMS;
   if( mDataBlock->lifetimeVarianceMS )
   {
      mLifetimeMS += S32( gRandGen.randI() % (2 * mDataBlock->lifetimeVarianceMS + 1)) - S32(mDataBlock->lifetimeVarianceMS );
   }

   //   Allocate particle structures and init the freelist. Member part_store
   //   is a Vector so that we can allocate more particles if partListInitSize
   //   turns out to be too small. 
   //
   if (mDataBlock->partListInitSize > 0)
   {
      for( S32 i = 0; i < part_store.size(); i++ )
      {
         delete [] part_store[i];
      }
      part_store.clear();
      n_part_capacity = mDataBlock->partListInitSize;
      Particle* store_block = new Particle[n_part_capacity];
      part_store.push_back(store_block);
      part_freelist = store_block;
      Particle* last_part = part_freelist;
      Particle* part = last_part+1;
      for( S32 i = 1; i < n_part_capacity; i++, part++, last_part++ )
      {
         last_part->next = part;
      }
      store_block[n_part_capacity-1].next = NULL;
      part_list_head.next = NULL;
      n_parts = 0;
   }

   F32 radius = 5.0;
   mObjBox.min = Point3F(-radius, -radius, -radius);
   mObjBox.max = Point3F(radius, radius, radius);
   resetWorldBox();

   return true;
}


//-----------------------------------------------------------------------------
// onRemove
//-----------------------------------------------------------------------------
void ParticleEmitter::onRemove()
{
   removeFromScene();
   Parent::onRemove();
}


//-----------------------------------------------------------------------------
// onNewDataBlock
//-----------------------------------------------------------------------------
bool ParticleEmitter::onNewDataBlock(GameBaseData* dptr)
{
   mDataBlock = dynamic_cast<ParticleEmitterData*>(dptr);
   if( !mDataBlock || !Parent::onNewDataBlock(dptr) )
      return false;

   scriptOnNewDataBlock();
   return true;
}

//-----------------------------------------------------------------------------
// getCollectiveColor
//-----------------------------------------------------------------------------
ColorF ParticleEmitter::getCollectiveColor()
{
	U32 count = 0;
	ColorF color = ColorF(0.0f, 0.0f, 0.0f);

   count = n_parts;
   for( Particle* part = part_list_head.next; part != NULL; part = part->next )
   {
      color += part->color;
   }

	if(count > 0)
   {
      color /= F32(count);
   }

	return color;
}

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
//-----------------------------------------------------------------------------
// prepRenderImage
//-----------------------------------------------------------------------------
bool ParticleEmitter::prepRenderImage(SceneState* state, const U32 stateKey,
                                      const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if( isLastState(state, stateKey) )
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if( state->isObjectRendered(this) )
   {
      prepBatchRender( state->getCameraPosition() );
   }

   return false;
}

//-----------------------------------------------------------------------------
// prepBatchRender
//-----------------------------------------------------------------------------
void ParticleEmitter::prepBatchRender( const Point3F &camPos )
{
   if (n_parts == 0 || part_list_head.next == NULL) return;
   if( mDead ) return;

   copyToVB( camPos );

   RenderInst *ri = gRenderInstManager.allocInst();
   ri->vertBuff = &mVertBuff;
   ri->primBuff = &mDataBlock->primBuff;
   ri->matInst = NULL;
   ri->translucent = true;
   ri->type = RenderInstManager::RIT_Translucent;
   ri->calcSortPoint( this, camPos );
   ri->particles = true;


   ri->worldXform = gRenderInstManager.allocXform();
   MatrixF world = GFX->getWorldMatrix();
   *ri->worldXform = world;
   ri->primBuffIndex = n_parts;
   //   For particles, transFlags now contains both src and dest blend
   //   settings packed together. SrcBlend is in the upper 4 bits and
   //   DestBlend is in the lower 4.
   ri->transFlags = ((mDataBlock->srcBlendFactor << 4) & 0xf0) | (mDataBlock->dstBlendFactor & 0x0f);
   // use first particle's texture unless there is an emitter texture to override it
   if (mDataBlock->textureHandle)
     ri->miscTex = &*(mDataBlock->textureHandle);
   else
     ri->miscTex = &*(part_list_head.next->dataBlock->textureHandle);

   gRenderInstManager.addInst( ri );

}
#else
//-----------------------------------------------------------------------------
// prepRenderImage
//-----------------------------------------------------------------------------
bool ParticleEmitter::prepRenderImage(SceneState* state, const U32 stateKey,
                                      const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
   if( isLastState(state, stateKey) )
      return false;
   setLastState(state, stateKey);

   // This should be sufficient for most objects that don't manage zones, and
   //  don't need to return a specialized RenderImage...
   if (state->isObjectRendered(this))
   {
      SceneRenderImage* image = new SceneRenderImage;
      image->obj = this;
      image->isTranslucent = true;
      image->sortType = SceneRenderImage::Point;
      // AFX CODE BLOCK (enhanced-emitter) <<
      image->tieBreaker = sort_priority;
      // AFX CODE BLOCK (enhanced-emitter) >>
      state->setImageRefPoint(this, image);

      state->insertRenderImage(image);
   }

   return false;
}

// structure used for particle sorting.
struct SortParticle
{
   Particle* p;
   F32       k;
};

// qsort callback function for particle sorting
int QSORT_CALLBACK cmpSortParticles(const void* p1, const void* p2)
{
   const SortParticle* sp1 = (const SortParticle*)p1;
   const SortParticle* sp2 = (const SortParticle*)p2;

   if (sp2->k > sp1->k)
      return 1;
   else if (sp2->k == sp1->k)
      return 0;
   else
      return -1;
}

inline void ParticleEmitter::prepParticleRender(const Particle* particle, bool& prevInvAlpha, U32 src_blend, U32 dst_blend)
{
   if (particle->dataBlock->useInvAlpha != prevInvAlpha)
   {
      if (particle->dataBlock->useInvAlpha)
         glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      else
         glBlendFunc(src_blend, dst_blend);
      prevInvAlpha = particle->dataBlock->useInvAlpha;
   }

   if (particle->dataBlock->animateTexture && particle->dataBlock->oldstyleAnimated)
   {
      U32 texNum = (U32)(particle->currentAge * (1.0f/1000.0f) * particle->dataBlock->framesPerSec);
      texNum %= particle->dataBlock->numFrames;
      glBindTexture(GL_TEXTURE_2D, particle->dataBlock->textureList[texNum].getGLName());
   }
   else
   {
      glBindTexture(GL_TEXTURE_2D, particle->dataBlock->textureList[0].getGLName());
   }
}

void ParticleEmitter::renderObject(SceneState* state, SceneRenderImage*)
{
   PROFILE_START(ParticleEmitter_render);

   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

   RectI viewport;
   dglGetViewport(&viewport);

   // Uncomment this if this is a "simple" (non-zone managing) object
   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   state->setupObjectProjection(this);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   dglMultMatrix(&mObjToWorld);

   MatrixF modelview;
   dglGetModelview(&modelview);

   Point3F x, y, viewvec;
   modelview.getRow(0, &x);
   modelview.getRow(2, &y);
   modelview.getRow(1, &viewvec);

   MatrixF camView;
   modelview.transposeTo( (F32*) &camView );

   // DMMFIX: slow!
   //
   static Vector<SortParticle> orderedVector(__FILE__, __LINE__);
   orderedVector.clear();

   // add each particle and a distance based sort key to orderedVector
   for (Particle* pp = part_list_head.next; pp != NULL; pp = pp->next)
   {
      orderedVector.increment();
      orderedVector.last().p = pp;
      orderedVector.last().k = mDot(pp->pos, viewvec);
   }

   if (mDataBlock->sortParticles)
   {
     // qsort the list into far to near ordering
     dQsort(orderedVector.address(), orderedVector.size(), sizeof(SortParticle), cmpSortParticles);
   }

   glEnable(GL_BLEND);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glDepthMask(GL_FALSE);

   // setup blending
   U32 src_blend = mDataBlock->srcBlendFactor;
   U32 dst_blend = mDataBlock->dstBlendFactor;
   glBlendFunc(src_blend, dst_blend);
   if( src_blend != GL_SRC_ALPHA || (dst_blend != GL_ONE_MINUS_SRC_ALPHA && dst_blend != GL_ONE) )
   {
      //GFX->setAlphaTestEnable( false );
   }

   const U32   orderedVecSize = orderedVector.size();
   bool        allowlighting = false;
   
   for(U32 i=0; i<orderedVecSize; i++)
   {
      if(!orderedVector[i].p->dataBlock->allowLighting)
	     continue;
      allowlighting = true;
	  break;
   }

   setupParticleLighting(allowlighting);

   Point3F basePoints[4];
   basePoints[0].set(-1.0f, 0.0f, -1.0f);
   basePoints[1].set( 1.0f, 0.0f, -1.0f);
   basePoints[2].set( 1.0f, 0.0f,  1.0f);
   basePoints[3].set(-1.0f, 0.0f,  1.0f);

   const F32 spinFactor = (1.0f/1000.0f) * (1.0f/360.0f) * M_PI_F * 2.0f;

   if (mDataBlock->mixedInvAlpha || mDataBlock->oldstyleAnimated)
   {
      bool prevInvAlpha = false;
      if (mDataBlock->orientParticles)
      {
         if (mDataBlock->reverseOrder)
         {
            for (U32 i = orderedVecSize-1; i >= 0; i--)
            {
               const Particle* particle = orderedVector[i].p;
               prepParticleRender(particle, prevInvAlpha, src_blend, dst_blend);
               renderOrientedParticle(*particle, state->getCameraPosition());
            }
         }
         else
         {
            for (U32 i = 0; i < orderedVecSize; i++)
            {
               const Particle* particle = orderedVector[i].p;
               prepParticleRender(particle, prevInvAlpha, src_blend, dst_blend);
               renderOrientedParticle(*particle, state->getCameraPosition());
            }
         }
      }
      else
      {
         if (mDataBlock->reverseOrder)
         {
            for (U32 i = orderedVecSize-1; i >= 0; i--)
            {
               const Particle* particle = orderedVector[i].p;
               prepParticleRender(particle, prevInvAlpha, src_blend, dst_blend);
               renderBillboardParticle( *particle, basePoints, camView, spinFactor );
            }
         }
         else
         {
            for (U32 i = 0; i < orderedVecSize; i++)
            {
               const Particle* particle = orderedVector[i].p;
               prepParticleRender(particle, prevInvAlpha, src_blend, dst_blend);
               renderBillboardParticle( *particle, basePoints, camView, spinFactor );
            }
         }
      }
   }
   else // do it the faster way
   {
      if (mDataBlock->orientParticles)
      {
         if (mDataBlock->reverseOrder)
         {
            for (U32 i = orderedVecSize-1; i >= 0; i--)
            {
               const Particle* particle = orderedVector[i].p;
               glBindTexture(GL_TEXTURE_2D, particle->dataBlock->textureList[0].getGLName());
               renderOrientedParticle(*particle, state->getCameraPosition());
            }
         }
         else
         {
            for (U32 i = 0; i < orderedVecSize; i++)
            {
               const Particle* particle = orderedVector[i].p;
               glBindTexture(GL_TEXTURE_2D, particle->dataBlock->textureList[0].getGLName());
               renderOrientedParticle(*particle, state->getCameraPosition());
            }
         }
      }
      else
      {
         if (mDataBlock->reverseOrder)
         {
            for (U32 i = orderedVecSize-1; i >= 0; i--)
            {
               const Particle* particle = orderedVector[i].p;
               glBindTexture(GL_TEXTURE_2D, particle->dataBlock->textureList[0].getGLName());
               renderBillboardParticle( *particle, basePoints, camView, spinFactor );
            }
         }
         else
         {
            for (U32 i = 0; i < orderedVecSize; i++)
            {
               const Particle* particle = orderedVector[i].p;
               glBindTexture(GL_TEXTURE_2D, particle->dataBlock->textureList[0].getGLName());
               renderBillboardParticle( *particle, basePoints, camView, spinFactor );
            }
         }
      }
   }

   resetParticleLighting();

   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   glDepthMask(GL_TRUE);

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();
   glMatrixMode(GL_PROJECTION);
   glPopMatrix();
   glMatrixMode(GL_MODELVIEW);
   dglSetViewport(viewport);

   AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");

   PROFILE_END();
}
#endif

//-----------------------------------------------------------------------------
// setSizes
//-----------------------------------------------------------------------------
void ParticleEmitter::setSizes( F32 *sizeList )
{
   for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
   {
      sizes[i] = sizeList[i];
   }
}

//-----------------------------------------------------------------------------
// setColors
//-----------------------------------------------------------------------------
void ParticleEmitter::setColors( ColorF *colorList )
{
   for( int i=0; i<ParticleData::PDC_NUM_KEYS; i++ )
   {
      colors[i] = colorList[i];
   }
}

//-----------------------------------------------------------------------------
// deleteWhenEmpty
//-----------------------------------------------------------------------------
void ParticleEmitter::deleteWhenEmpty()
{
   mDeleteWhenEmpty = true;
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void ParticleEmitter::emitParticles(const Point3F& point,
                                    const bool     useLastPosition,
                                    const Point3F& axis,
                                    const Point3F& velocity,
                                    const U32      numMilliseconds)
{
   if( mDead ) return;

   // lifetime over - no more particles
   if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
   {
      return;
   }

   // AFX CODE BLOCK (enhanced-emitter) <<
   pos_pe = point;
   // AFX CODE BLOCK (enhanced-emitter) >>

   Point3F realStart;
   if( useLastPosition && mHasLastPosition )
      realStart = mLastPosition;
   else
      realStart = point;

   emitParticles(realStart, point,
                 axis,
                 velocity,
                 numMilliseconds);
}

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void ParticleEmitter::emitParticles(const Point3F& start,
                                    const Point3F& end,
                                    const Point3F& axis,
                                    const Point3F& velocity,
                                    const U32      numMilliseconds)
{
   if( mDead ) return;

   // lifetime over - no more particles
   if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
      return;

   U32 currTime = 0;
   bool particlesAdded = false;

   Point3F axisx;
   if( mFabs(axis.z) < 0.9f )
      mCross(axis, Point3F(0, 0, 1), &axisx);
   else
      mCross(axis, Point3F(0, 1, 0), &axisx);
   axisx.normalize();

   if( mNextParticleTime != 0 )
   {
      // Need to handle next particle
      //
      if( mNextParticleTime > numMilliseconds )
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

         // AFX CODE BLOCK (enhanced-emitter) <<
         addParticle(pos, axis, velocity, axisx, numMilliseconds-currTime);
         /* ORIGINAL CODE
         addParticle(pos, axis, velocity, axisx);
         */
         // AFX CODE BLOCK (enhanced-emitter) >>
         particlesAdded = true;
         mNextParticleTime = 0;
      }
   }

   while( currTime < numMilliseconds )
   {
      S32 nextTime = mDataBlock->ejectionPeriodMS;
      if( mDataBlock->periodVarianceMS != 0 )
      {
         nextTime += S32(gRandGen.randI() % (2 * mDataBlock->periodVarianceMS + 1)) -
            S32(mDataBlock->periodVarianceMS);
      }
      AssertFatal(nextTime > 0, "Error, next particle ejection time must always be greater than 0");

      if( currTime + nextTime > numMilliseconds )
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

      // AFX CODE BLOCK (enhanced-emitter) <<
      addParticle(pos, axis, velocity, axisx, numMilliseconds-currTime);
      /* ORIGINAL CODE
      addParticle(pos, axis, velocity, axisx);
      */
      // AFX CODE BLOCK (enhanced-emitter) >>
      particlesAdded = true;

      //   This override-advance code is restored in order to correctly adjust
      //   animated parameters of particles allocated within the same frame
      //   update. Note that ordering is important and this code correctly 
      //   adds particles in the same newest-to-oldest ordering of the link-list.
      //
      // NOTE: We are assuming that the just added particle is at the head of our
      //  list.  If that changes, so must this...
      U32 advanceMS = numMilliseconds - currTime;
      if (mDataBlock->overrideAdvance == false && advanceMS != 0) 
      {
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
            if (advanceMS != 0)
            {
               F32 t = F32(advanceMS) / 1000.0;

               Point3F a = last_part->acc;
               a -= last_part->vel * last_part->dataBlock->dragCoefficient;
               a -= mWindVelocity * last_part->dataBlock->windCoefficient;
               //a += Point3F(0, 0, -9.81) * last_part->dataBlock->gravityCoefficient;
               a.z += -9.81f*last_part->dataBlock->gravityCoefficient; // as long as gravity is a constant, this is faster

               last_part->vel += a * t;
               //last_part->pos += last_part->vel * t;
               last_part->pos_local += last_part->vel * t;

               // AFX -- allow subclasses to adjust the particle params here
               sub_particleUpdate(last_part);

               if (last_part->dataBlock->constrain_pos)
                  last_part->pos = last_part->pos_local + this->pos_pe;
               else
                  last_part->pos = last_part->pos_local;

               updateKeyData( last_part );
            }
         }
      }
   }

   // DMMFIX: Lame and slow...
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

//-----------------------------------------------------------------------------
// emitParticles
//-----------------------------------------------------------------------------
void ParticleEmitter::emitParticles(const Point3F& rCenter,
                                    const Point3F& rNormal,
                                    const F32      radius,
                                    const Point3F& velocity,
                                    S32 count)
{
   if( mDead ) return;

   // lifetime over - no more particles
   if( mLifetimeMS > 0 && mElapsedTimeMS > mLifetimeMS )
   {
      return;
   }


   Point3F axisx, axisy;
   Point3F axisz = rNormal;

   if( axisz.isZero() )
   {
      axisz.set( 0.0, 0.0, 1.0 );
   }

   if( mFabs(axisz.z) < 0.98 )
   {
      mCross(axisz, Point3F(0, 0, 1), &axisy);
      axisy.normalize();
   }
   else
   {
      mCross(axisz, Point3F(0, 1, 0), &axisy);
      axisy.normalize();
   }
   mCross(axisz, axisy, &axisx);
   axisx.normalize();

   // Should think of a better way to distribute the
   // particles within the hemisphere.
   for( S32 i = 0; i < count; i++ )
   {
      Point3F pos = axisx * (radius * (1 - (2 * gRandGen.randF())));
      pos        += axisy * (radius * (1 - (2 * gRandGen.randF())));
      pos        += axisz * (radius * gRandGen.randF());

      Point3F axis = pos;
      axis.normalize();
      pos += rCenter;

      // AFX CODE BLOCK (enhanced-emitter) <<
      addParticle(pos, axis, velocity, axisz, 0);
      /* ORIGINAL CODE
      addParticle(pos, axis, velocity, axisz);
      */
      // AFX CODE BLOCK (enhanced-emitter) >>
   }

   // Set world bounding box
   mObjBox.min = rCenter - Point3F(radius, radius, radius);
   mObjBox.max = rCenter + Point3F(radius, radius, radius);
   resetWorldBox();

   // Make sure we're part of the world
   if( n_parts > 0 && mSceneManager == NULL )
   {
      gClientSceneGraph->addObjectToScene(this);
      gClientContainer.addObject(this);
      gClientProcessList.addObject(this);
   }

   mHasLastPosition = false;
}

//-----------------------------------------------------------------------------
// updateBBox - SLOW, bad news
//-----------------------------------------------------------------------------
void ParticleEmitter::updateBBox()
{
   // AFX CODE BLOCK (enhanced-emitter) <<
   if (forced_bbox)
     return;
   // AFX CODE BLOCK (enhanced-emitter) >>

   Point3F min(1e10,   1e10,  1e10);
   Point3F max(-1e10, -1e10, -1e10);

   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      min.setMin( part->pos );
      max.setMax( part->pos );
   }
   
   mObjBox = Box3F(min, max);
   MatrixF temp = getTransform();
   setTransform(temp);
}

//-----------------------------------------------------------------------------
// addParticle
//-----------------------------------------------------------------------------
// AFX CODE BLOCK (enhanced-emitter) <<
void ParticleEmitter::addParticle(const Point3F& pos,
                                  const Point3F& axis,
                                  const Point3F& vel,
                                  const Point3F& axisx,
                                  const U32 age_offset)
/* ORIGINAL CODE
void ParticleEmitter::addParticle(const Point3F& pos,
                                  const Point3F& axis,
                                  const Point3F& vel,
                                  const Point3F& axisx)
*/
// AFX CODE BLOCK (enhanced-emitter) >>
{
   n_parts++;
   if (n_parts > n_part_capacity)
   {
      // In an emergency we allocate additional particles in blocks of 16.
      // This should happen rarely.
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

   // AFX CODE BLOCK (enhanced-emitter) <<
   // for earlier access to constrain_pos, the ParticleData datablock is chosen here instead
   // of later in the method.
   U32 dBlockIndex = (U32)(mCeil(gRandGen.randF() * F32(mDataBlock->particleDataBlocks.size())) - 1);
   ParticleData* part_db = mDataBlock->particleDataBlocks[dBlockIndex];
   // set start position to world or local space
   Point3F pos_start; 
   if (part_db->constrain_pos)
     pos_start.set(0,0,0);
   else
     pos_start = pos;
   // AFX CODE BLOCK (enhanced-emitter) >>

   Point3F ejectionAxis = axis;
   F32 theta = (mDataBlock->thetaMax - mDataBlock->thetaMin) * gRandGen.randF() +
               mDataBlock->thetaMin;

   F32 ref  = (F32(mInternalClock) / 1000.0) * mDataBlock->phiReferenceVel;
   F32 phi  = ref + gRandGen.randF() * mDataBlock->phiVariance;

   // Both phi and theta are in degs.  Create axis angles out of them, and create the
   //  appropriate rotation matrix...
   AngAxisF thetaRot(axisx, theta * (M_PI / 180.0));
   AngAxisF phiRot(axis,    phi   * (M_PI / 180.0));

   MatrixF temp(true);
   thetaRot.setMatrix(&temp);
   temp.mulP(ejectionAxis);
   phiRot.setMatrix(&temp);
   temp.mulP(ejectionAxis);

   F32 initialVel = mDataBlock->ejectionVelocity;
   initialVel    += (mDataBlock->velocityVariance * 2.0f * gRandGen.randF()) - mDataBlock->velocityVariance;
   // AFX CODE BLOCK (enhanced-emitter) <<
   pNew->pos = pos_start + (ejectionAxis * mDataBlock->ejectionOffset);
   pNew->pos_local = pNew->pos;
   pNew->vel = mDataBlock->ejectionInvert ? ejectionAxis * -initialVel : ejectionAxis * initialVel;
   /* ORIGINAL CODE
   pNew->pos = pos + (ejectionAxis * mDataBlock->ejectionOffset);
   pNew->vel = ejectionAxis * initialVel;
   */
   // AFX CODE BLOCK (enhanced-emitter) >>
   pNew->orientDir = ejectionAxis;
   pNew->acc.set(0, 0, 0);
   // AFX CODE BLOCK (enhanced-emitter) <<
   pNew->currentAge = age_offset;
   pNew->t_last = 0.0f;
   /* ORIGINAL CODE
   pNew->currentAge = 0;
   */
   // AFX CODE BLOCK (enhanced-emitter) >>

   // AFX CODE BLOCK (enhanced-emitter) <<
   /* ORIGINAL CODE
   // Choose a new particle datablack randomly from the list
   U32 dBlockIndex = (U32)(mCeil(gRandGen.randF() * F32(mDataBlock->particleDataBlocks.size())) - 1);
   */
   // AFX CODE BLOCK (enhanced-emitter) >>
   mDataBlock->particleDataBlocks[dBlockIndex]->initializeParticle(pNew, vel);
   updateKeyData( pNew );

}


//-----------------------------------------------------------------------------
// processTick
//-----------------------------------------------------------------------------
void ParticleEmitter::processTick(const Move*)
{
   if( mDeleteOnTick == true )
   {
      mDead = true;
      deleteObject();
   }
}


//-----------------------------------------------------------------------------
// advanceTime
//-----------------------------------------------------------------------------
void ParticleEmitter::advanceTime(F32 dt)
{
   if( dt < 0.00001 ) return;

   Parent::advanceTime(dt);

   if( dt > 0.5 ) dt = 0.5;

   if( mDead ) return;

   mElapsedTimeMS += (S32)(dt * 1000.0f);

   U32 numMSToUpdate = (U32)(dt * 1000.0f);
   if( numMSToUpdate == 0 ) return;

   // remove dead particles
   Particle* last_part = &part_list_head;
   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
     part->currentAge += numMSToUpdate;
     if (part->currentAge > part->totalLifetime)
     {
       n_parts--;
       last_part->next = part->next;
       part->next = part_freelist;
       part_freelist = part;
       part = last_part;
     }
     else
     {
       last_part = part;
     }
   }

   AssertFatal( n_parts >= 0, "ParticleEmitter: negative part count!" );

   if (n_parts < 1 && mDeleteWhenEmpty)
   {
      mDeleteOnTick = true;
      return;
   }

   if( numMSToUpdate != 0 && n_parts > 0 )
   {
      update( numMSToUpdate );
   }
}

//-----------------------------------------------------------------------------
// Update key related particle data
//-----------------------------------------------------------------------------
void ParticleEmitter::updateKeyData( Particle *part )
{
   // TGEA vs TGE: this code logic is found in the 2nd part of
   // PEngine::updateSingleParticle() in TGE.

   // AFX CODE BLOCK (enhanced-emitter) <<
   if (part->currentAge > part->totalLifetime)
      part->currentAge = part->totalLifetime;
   F32 t = F32(part->currentAge) / F32(part->totalLifetime);
   /* ORIGINAL CODE
   F32 t = F32(part->currentAge) / F32(part->totalLifetime);
   AssertFatal(t <= 1.0f, "Out out bounds filter function for particle.");
   */
   // AFX CODE BLOCK (enhanced-emitter) >>

   for( U32 i = 1; i < ParticleData::PDC_NUM_KEYS; i++ )
   {
      if( part->dataBlock->times[i] >= t )
      {
         F32 firstPart = t - part->dataBlock->times[i-1];
         F32 total     = part->dataBlock->times[i] -
                         part->dataBlock->times[i-1];

         firstPart /= total;

         if( mDataBlock->useEmitterColors )
         {
            part->color.interpolate(colors[i-1], colors[i], firstPart);
         }
         else
         {
            part->color.interpolate(part->dataBlock->colors[i-1],
                                    part->dataBlock->colors[i],
                                    firstPart);
         }

         if( mDataBlock->useEmitterSizes )
         {
            part->size = (sizes[i-1] * (1.0 - firstPart)) +
                         (sizes[i]   * firstPart);
         }
         else
         {
            part->size = (part->dataBlock->sizes[i-1] * (1.0 - firstPart)) +
                         (part->dataBlock->sizes[i]   * firstPart);
         }

         // AFX CODE BLOCK (particle-fade) <<
         if (mDataBlock->fade_color) 
         {
           if (mDataBlock->fade_alpha) 
             part->color *= fade_amt; 
           else 
           {
             part->color.red *= fade_amt; 
             part->color.green *= fade_amt; 
             part->color.blue *= fade_amt; 
           }
         }
         else if (mDataBlock->fade_alpha) 
           part->color.alpha *= fade_amt;

         if (mDataBlock->fade_size)
           part->size *= fade_amt;
         // AFX CODE BLOCK (particle-fade) >>
         break;
      }
   }
}

//-----------------------------------------------------------------------------
// Update particles
//-----------------------------------------------------------------------------
void ParticleEmitter::update( U32 ms )
{
   F32 t = F32(ms)/1000.0f;

   for (Particle* part = part_list_head.next; part != NULL; part = part->next)
   {
      // TGEA vs TGE: this code logic is found in the 1st part of
      // PEngine::updateSingleParticle() in TGE.

      Point3F a = part->acc;
      a -= part->vel * part->dataBlock->dragCoefficient;
      a -= mWindVelocity * part->dataBlock->windCoefficient;
      //a += Point3F(0, 0, -9.81) * part->dataBlock->gravityCoefficient;
      a.z += -9.81f*part->dataBlock->gravityCoefficient; // as long as gravity is a constant, this is faster

      part->vel += a * t;
      // AFX CODE BLOCK (enhanced-emitter) <<
      part->pos_local += part->vel * t; 

      // AFX -- allow subclasses to adjust the particle params here
      sub_particleUpdate(part);

      if (part->dataBlock->constrain_pos)
        part->pos = part->pos_local + this->pos_pe;
      else
        part->pos = part->pos_local;
      /* ORIGINAL CODE
      part->pos += part->vel * t;
      */
      // AFX CODE BLOCK (enhanced-emitter) >>

      updateKeyData( part );
   }
}

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
//-----------------------------------------------------------------------------
// Copy particles to vertex buffer
//-----------------------------------------------------------------------------

// structure used for particle sorting.
struct SortParticle
{
   Particle* p;
   F32       k;
};

// qsort callback function for particle sorting
int QSORT_CALLBACK cmpSortParticles(const void* p1, const void* p2)
{
   const SortParticle* sp1 = (const SortParticle*)p1;
   const SortParticle* sp2 = (const SortParticle*)p2;

   if (sp2->k > sp1->k)
      return 1;
   else if (sp2->k == sp1->k)
      return 0;
   else
      return -1;
}

void ParticleEmitter::copyToVB( const Point3F &camPos )
{
   static Vector<SortParticle> orderedVector(__FILE__, __LINE__);

   // build sorted list of particles (far to near)
   if (mDataBlock->sortParticles)
   {
     orderedVector.clear();

     MatrixF modelview = GFX->getWorldMatrix();
     Point3F viewvec; modelview.getRow(1, &viewvec);

     // add each particle and a distance based sort key to orderedVector
     for (Particle* pp = part_list_head.next; pp != NULL; pp = pp->next)
     {
       orderedVector.increment();
       orderedVector.last().p = pp;
       orderedVector.last().k = mDot(pp->pos, viewvec);
     }

     // qsort the list into far to near ordering
     dQsort(orderedVector.address(), orderedVector.size(), sizeof(SortParticle), cmpSortParticles);
   }

   static Vector<GFXVertexPCT> tempBuff(2048);
   tempBuff.reserve( n_parts*4 + 64); // make sure tempBuff is big enough
   GFXVertexPCT *buffPtr = tempBuff.address(); // use direct pointer (faster)

   if (mDataBlock->orientParticles)
   {
      if (mDataBlock->reverseOrder)
      {
        buffPtr += 4*(n_parts-1);
        // do sorted-oriented particles
        if (mDataBlock->sortParticles)
        {
          SortParticle* partPtr = orderedVector.address();
          for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr-=4 )
             setupOriented(partPtr->p, camPos, buffPtr);
        }
        // do unsorted-oriented particles
        else
        {
          for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
             setupOriented(partPtr, camPos, buffPtr);
        }
      }
      else
      {
        // do sorted-oriented particles
        if (mDataBlock->sortParticles)
        {
          SortParticle* partPtr = orderedVector.address();
          for (U32 i = 0; i < n_parts; i++, partPtr++, buffPtr+=4 )
             setupOriented(partPtr->p, camPos, buffPtr);
        }
        // do unsorted-oriented particles
        else
        {
          for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
             setupOriented(partPtr, camPos, buffPtr);
        }
      }
   }
   else
   {
      // somewhat odd ordering so that texture coordinates match the oriented
      // particles
      Point3F basePoints[4];
      basePoints[0] = Point3F(-1.0, 0.0,  1.0);
      basePoints[1] = Point3F(-1.0, 0.0, -1.0);
      basePoints[2] = Point3F( 1.0, 0.0, -1.0);
      basePoints[3] = Point3F( 1.0, 0.0,  1.0);

      MatrixF camView = GFX->getWorldMatrix();
      camView.transpose();  // inverse - this gets the particles facing camera

      if (mDataBlock->reverseOrder)
      {
        buffPtr += 4*(n_parts-1);
        // do sorted-billboard particles
        if (mDataBlock->sortParticles)
        {
          SortParticle *partPtr = orderedVector.address();
          for( U32 i=0; i<n_parts; i++, partPtr++, buffPtr-=4 )
             setupBillboard( partPtr->p, basePoints, camView, buffPtr );
        }
        // do unsorted-billboard particles
        else
        {
          for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr-=4)
             setupBillboard( partPtr, basePoints, camView, buffPtr );
        }
      }
      else
      {
        // do sorted-billboard particles
        if (mDataBlock->sortParticles)
        {
          SortParticle *partPtr = orderedVector.address();
          for( U32 i=0; i<n_parts; i++, partPtr++, buffPtr+=4 )
             setupBillboard( partPtr->p, basePoints, camView, buffPtr );
        }
        // do unsorted-billboard particles
        else
        {
          for (Particle* partPtr = part_list_head.next; partPtr != NULL; partPtr = partPtr->next, buffPtr+=4)
             setupBillboard( partPtr, basePoints, camView, buffPtr );
        }
      }
   }

   // create new VB if emitter size grows
   if( !mVertBuff || n_parts > mCurBuffSize )
   {
      mCurBuffSize = n_parts;
      mVertBuff.set( GFX, n_parts * 4, GFXBufferTypeDynamic );
   }
   // lock and copy tempBuff to video RAM
   GFXVertexPCT *verts = mVertBuff.lock();
   dMemcpy( verts, tempBuff.address(), n_parts * 4 * sizeof(GFXVertexPCT) );
   mVertBuff.unlock();
}

//-----------------------------------------------------------------------------
// Set up particle for billboard style render
//-----------------------------------------------------------------------------
void ParticleEmitter::setupBillboard( Particle *part,
                                      Point3F *basePts,
                                      const MatrixF &camView,
                                      GFXVertexPCT *lVerts )
{
   const F32 spinFactor = (1.0f/1000.0f) * (1.0f/360.0f) * M_PI_F * 2.0f;

   F32 width     = part->size * 0.5f;
   F32 spinAngle = part->spinSpeed * part->currentAge * spinFactor;

   F32 sy, cy;
   mSinCos(spinAngle, sy, cy);

   // fill four verts, use macro and unroll loop
   #define fillVert(){ \
      lVerts->point.x = cy * basePts->x - sy * basePts->z;  \
      lVerts->point.y = 0.0f;                                \
      lVerts->point.z = sy * basePts->x + cy * basePts->z;  \
      camView.mulV( lVerts->point );                        \
      lVerts->point *= width;                               \
      lVerts->point += part->pos;                           \
      lVerts->color = part->color; }                        \

   // Here we deal with UVs for animated particle (billboard)
   if (part->dataBlock->animateTexture)
   { 
     // Let particle compute the UV indices for current frame
     S32 fm = (S32)(part->currentAge*(1.0/1000.0)*part->dataBlock->framesPerSec);
     U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
     S32 uv[4];
     uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
     uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
     uv[2] = uv[1] + 1;
     uv[3] = uv[0] + 1;

     fillVert();
     // Here and below, we copy UVs from particle datablock's current frame's UVs (billboard)
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
     ++lVerts;
     ++basePts;

     fillVert();
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
     ++lVerts;
     ++basePts;

     fillVert();
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
     ++lVerts;
     ++basePts;

     fillVert();
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
     ++lVerts;
     ++basePts;

     return;
   }

   fillVert();
   // Here and below, we copy UVs from particle datablock's texCoords (billboard)
   lVerts->texCoord = part->dataBlock->texCoords[0];
   ++lVerts;
   ++basePts;

   fillVert();
   lVerts->texCoord = part->dataBlock->texCoords[1];
   ++lVerts;
   ++basePts;

   fillVert();
   lVerts->texCoord = part->dataBlock->texCoords[2];
   ++lVerts;
   ++basePts;

   fillVert();
   lVerts->texCoord = part->dataBlock->texCoords[3];
   ++lVerts;
   ++basePts;
}

//-----------------------------------------------------------------------------
// Set up oriented particle
//-----------------------------------------------------------------------------
void ParticleEmitter::setupOriented( Particle *part,
                                     const Point3F &camPos,
                                     GFXVertexPCT *lVerts )
{
   Point3F dir;

   if( mDataBlock->orientOnVelocity )
   {
      // don't render oriented particle if it has no velocity
      if( part->vel.magnitudeSafe() == 0.0 ) return;
      dir = part->vel;
   }
   else
   {
      dir = part->orientDir;
   }

   Point3F dirFromCam = part->pos - camPos;
   Point3F crossDir;
   mCross( dirFromCam, dir, &crossDir );
   crossDir.normalize();
   dir.normalize();


   F32 width = part->size * 0.5;
   dir *= width;
   crossDir *= width;
   Point3F start = part->pos - dir;
   Point3F end = part->pos + dir;

   // Here we deal with UVs for animated particle (oriented)
   if (part->dataBlock->animateTexture)
   { 
     // Let particle compute the UV indices for current frame
     S32 fm = (S32)(part->currentAge*(1.0/1000.0)*part->dataBlock->framesPerSec);
     U8 fm_tile = part->dataBlock->animTexFrames[fm % part->dataBlock->numFrames];
     S32 uv[4];
     uv[0] = fm_tile + fm_tile/part->dataBlock->animTexTiling.x;
     uv[1] = uv[0] + (part->dataBlock->animTexTiling.x + 1);
     uv[2] = uv[1] + 1;
     uv[3] = uv[0] + 1;

     lVerts->point = start + crossDir;
     lVerts->color = part->color;
     // Here and below, we copy UVs from particle datablock's current frame's UVs (oriented)
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[0]];
     ++lVerts;

     lVerts->point = start - crossDir;
     lVerts->color = part->color;
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[1]];
     ++lVerts;

     lVerts->point = end - crossDir;
     lVerts->color = part->color;
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[2]];
     ++lVerts;

     lVerts->point = end + crossDir;
     lVerts->color = part->color;
     lVerts->texCoord = part->dataBlock->animTexUVs[uv[3]];
     ++lVerts;

     return;
   }

   lVerts->point = start + crossDir;
   lVerts->color = part->color;
   // Here and below, we copy UVs from particle datablock's texCoords (oriented)
   lVerts->texCoord = part->dataBlock->texCoords[0];
   ++lVerts;

   lVerts->point = start - crossDir;
   lVerts->color = part->color;
   lVerts->texCoord = part->dataBlock->texCoords[1];
   ++lVerts;

   lVerts->point = end - crossDir;
   lVerts->color = part->color;
   lVerts->texCoord = part->dataBlock->texCoords[2];
   ++lVerts;

   lVerts->point = end + crossDir;
   lVerts->color = part->color;
   lVerts->texCoord = part->dataBlock->texCoords[3];
   ++lVerts;
}
#else
void ParticleEmitter::setupParticleLighting(bool allowlighting)
{
	// ya, not yet...
	allowLighting = (!gGLState.isDirect3D) &&
		LightManager::sgAllowDynamicParticleSystemLighting() &&
		allowlighting;
	lastLightingValue = allowLighting;

	if(!allowLighting)
		return;

	if(!((TextureObject *)whiteTexture))
		whiteTexture = TextureHandle("common/lighting/whiteNoAlpha", MeshTexture);
	
	LightManager *lightManager = gClientSceneGraph->getLightManager();
	AssertFatal(lightManager!=NULL, "SceneObject::installLights: LightManager not found");

	lightManager->sgSetupLights(this);

	glNormal3f( 0.0f, 0.0f, 1.0f );
	
	const U32	whiteTextureName = whiteTexture.getGLName();
	
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, whiteTextureName);

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_TEXTURE);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA,GL_CONSTANT);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA,GL_SRC_ALPHA);
	
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, whiteTextureName);
	LightManager::sgSetupExposureRendering();

	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_COMBINE);
	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,GL_MODULATE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,GL_SRC_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_RGB,GL_PRIMARY_COLOR);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_RGB,GL_SRC_COLOR);

	glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA,GL_REPLACE);
	glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA,GL_PREVIOUS);
	glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

	glActiveTextureARB(GL_TEXTURE0_ARB);
}

void ParticleEmitter::resetParticleLighting()
{
	if(!allowLighting)
		return;
	
	LightManager *lightManager = gClientSceneGraph->getLightManager();
	lightManager->sgResetLights();

	glActiveTextureARB(GL_TEXTURE1_ARB);
	LightManager::sgResetExposureRendering();
	glDisable(GL_TEXTURE_2D);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

void ParticleEmitter::lightParticle(const Particle &part)
{
	if(allowLighting)
	{
		glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, part.color);

		if(lastLightingValue != part.dataBlock->allowLighting)
		{
			if(part.dataBlock->allowLighting)
			{
				glEnable(GL_LIGHTING);
			}
			else
			{
				glDisable(GL_LIGHTING);
				glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
			}

			lastLightingValue = part.dataBlock->allowLighting;
		}
	}
	else
		glColor4fv(part.color);
}

inline void ParticleEmitter::renderBillboardParticle( const Particle &part, const Point3F *basePnts,
                                                      const MatrixF &camView, const F32 spinFactor )
{
   lightParticle(part);

   glBegin(GL_QUADS);

      F32 width     = part.size * 0.5f;
      F32 spinAngle = part.spinSpeed * part.currentAge * spinFactor;

      F32 sy, cy;
      mSinCos(spinAngle, sy, cy);
      Point3F points[4];

      for( int i=0; i<4; i++ )
      {
         points[i].x = cy * basePnts[i].x - sy * basePnts[i].z;
         points[i].y = basePnts[i].y;
         points[i].z = sy * basePnts[i].x + cy * basePnts[i].z;
         camView.mulP( points[i] );
         points[i] *= width;
         points[i] += part.pos;
      }

      // Here we deal with UVs for animated particle (billboard)
      if (part.dataBlock->animateTexture && !part.dataBlock->oldstyleAnimated)
      { 
         S32 fm = (S32)(part.currentAge*(1.0/1000.0)*part.dataBlock->framesPerSec);
         U8 fm_tile = part.dataBlock->animTexFrames[fm % part.dataBlock->numFrames];
         S32 uv[4];
         uv[0] = fm_tile + fm_tile/part.dataBlock->animTexTiling.x;
         uv[1] = uv[0] + (part.dataBlock->animTexTiling.x + 1);
         uv[2] = uv[1] + 1;
         uv[3] = uv[0] + 1;

         glTexCoord2fv(part.dataBlock->animTexUVs[uv[0]]);
         glVertex3fv(points[0]);
         glTexCoord2fv(part.dataBlock->animTexUVs[uv[1]]);
         glVertex3fv(points[1]);
         glTexCoord2fv(part.dataBlock->animTexUVs[uv[2]]);
         glVertex3fv(points[2]);
         glTexCoord2fv(part.dataBlock->animTexUVs[uv[3]]);
         glVertex3fv(points[3]);

         return;
      }

      glTexCoord2fv(part.dataBlock->texCoords[1]);
      glVertex3fv(points[0]);
      glTexCoord2fv(part.dataBlock->texCoords[2]);
      glVertex3fv(points[1]);
      glTexCoord2fv(part.dataBlock->texCoords[3]);
      glVertex3fv(points[2]);
      glTexCoord2fv(part.dataBlock->texCoords[0]);
      glVertex3fv(points[3]);

   glEnd();
}

inline void ParticleEmitter::renderOrientedParticle( const Particle &part, const Point3F &camPos )
{
   Point3F dir;

   if( mDataBlock->orientOnVelocity )
   {
      // don't render oriented particle if it has no velocity
      if( part.vel.magnitudeSafe() == 0.0f )
	     return;
      dir = part.vel;
   }
   else
   {
      dir = part.orientDir;
   }

   Point3F dirFromCam = part.pos - camPos;
   Point3F crossDir;
   mCross( dirFromCam, dir, &crossDir );
   crossDir.normalize();
   dir.normalize();

   lightParticle(part);

   glBegin(GL_QUADS);

      F32 width = part.size * 0.5f;
      dir *= width;
      crossDir *= width;
      Point3F start = part.pos - dir;
      Point3F end = part.pos + dir;

      // Here we deal with UVs for animated particle (oriented)
      if (part.dataBlock->animateTexture && !part.dataBlock->oldstyleAnimated)
      { 
         // Let particle compute the UV indices for current frame
         S32 fm = (S32)(part.currentAge*(1.0/1000.0)*part.dataBlock->framesPerSec);
         U8 fm_tile = part.dataBlock->animTexFrames[fm % part.dataBlock->numFrames];
         S32 uv[4];
         uv[0] = fm_tile + fm_tile/part.dataBlock->animTexTiling.x;
         uv[1] = uv[0] + (part.dataBlock->animTexTiling.x + 1);
         uv[2] = uv[1] + 1;
         uv[3] = uv[0] + 1;

         glTexCoord2fv(part.dataBlock->animTexUVs[uv[0]]);
         glVertex3fv( start + crossDir );
         glTexCoord2fv(part.dataBlock->animTexUVs[uv[1]]);
         glVertex3fv( start - crossDir );
         glTexCoord2fv(part.dataBlock->animTexUVs[uv[2]]);
         glVertex3fv( end - crossDir );
         glTexCoord2fv(part.dataBlock->animTexUVs[uv[3]]);
         glVertex3fv( end + crossDir );

         return;
      }

      glTexCoord2fv(part.dataBlock->texCoords[1]);
      glVertex3fv( start + crossDir );
      glTexCoord2fv(part.dataBlock->texCoords[2]);
      glVertex3fv( start - crossDir );
      glTexCoord2fv(part.dataBlock->texCoords[3]);
      glVertex3fv( end - crossDir );
      glTexCoord2fv(part.dataBlock->texCoords[0]);
      glVertex3fv( end + crossDir );

   glEnd();
}
#endif

ConsoleMethod(ParticleEmitterData, reload, void, 2, 2, "(void)"
              "Reloads this emitter")
{
   object->loadParameters();
   object->reload(false);
}

// AFX CODE BLOCK (enhanced-emitter) <<
void ParticleEmitter::emitParticlesExt(const MatrixF& xfm, const Point3F& point, 
                                       const Point3F& velocity, const U32 numMilliseconds)
{
   if (mDataBlock->use_emitter_xfm)
   {
      Point3F zero_point(0.0f, 0.0f, 0.0f);
      this->pos_pe = zero_point;
      this->setTransform(xfm);
      Point3F axis(0.0,0.0,1.0);    
      xfm.mulV(axis);
      emitParticles(zero_point, true, axis, velocity, numMilliseconds);
   }
   else
   {
      this->pos_pe = point;
      Point3F axis(0.0,0.0,1.0);    
      xfm.mulV(axis);
      emitParticles(point, true, axis, velocity, numMilliseconds);
   }
}  

void ParticleEmitter::setForcedObjBox(Box3F& box) 
{
  mObjBox = box;
  forced_bbox = true;
}

void ParticleEmitter::setSortPriority(S8 priority) 
{
  sort_priority = priority;
}
// AFX CODE BLOCK (enhanced-emitter) >>
