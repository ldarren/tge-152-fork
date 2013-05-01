//-----------------------------------------------------------------------------
// Torque Game Engine Advanced
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//        enhanced-particle -- increased keys to 8.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _PARTICLE_H_
#define _PARTICLE_H_

#ifndef _GAMEBASE_H_
#include "game/gameBase.h"
#endif

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
#include "gfx/gfxTextureHandle.h"
#endif

#define MaxParticleSize 50.0

struct Particle;

//*****************************************************************************
// Particle Data
//*****************************************************************************
class ParticleData : public SimDataBlock
{
   typedef SimDataBlock Parent;

  public:
   enum PDConst
   {
      // AFX CODE BLOCK (enhanced-particle) <<
      PDC_NUM_KEYS = 8,
      /* ORIGINAL CODE
      PDC_NUM_KEYS = 4,
      */
      // AFX CODE BLOCK (enhanced-particle) >>
#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
      PDC_MAX_TEX = 50
#endif
   };

   F32   dragCoefficient;
   F32   windCoefficient;
   F32   gravityCoefficient;

   F32   inheritedVelFactor;
   F32   constantAcceleration;

   S32   lifetimeMS;
   S32   lifetimeVarianceMS;

   F32   spinSpeed;        // degrees per second
   F32   spinRandomMin;
   F32   spinRandomMax;

   bool  useInvAlpha;

#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   bool  allowLighting;
#endif
   bool  animateTexture;
   U32   numFrames;
   U32   framesPerSec;
#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   bool  oldstyleAnimated;
#endif

   ColorF colors[ PDC_NUM_KEYS ];
   F32    sizes[ PDC_NUM_KEYS ];
   F32    times[ PDC_NUM_KEYS ];

   Point2F*          animTexUVs;
   Point2F           texCoords[4];   // default: {{0.0,0.0}, {0.0,1.0}, {1.0,1.0}, {1.0,0.0}} 
   Point2I           animTexTiling;
   StringTableEntry  animTexFramesString;
   Vector<U8>        animTexFrames;
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   StringTableEntry  textureName;
   GFXTexHandle      textureHandle;
#else
   StringTableEntry  textureNameList[PDC_MAX_TEX];
   TextureHandle     textureList[PDC_MAX_TEX];
#endif

  public:
   ParticleData();
   ~ParticleData();

   // move this procedure to Particle
   void initializeParticle(Particle*, const Point3F&);

   void packData(BitStream* stream);
   void unpackData(BitStream* stream);
   bool onAdd();
   // AFX CODE BLOCK (to-tgea-from-tge) <<
   bool loadParameters();
   bool reload(char errorBuffer[256]);
   // AFX CODE BLOCK (to-tgea-from-tge) >>
   bool preload(bool server, char errorBuffer[256]);
   DECLARE_CONOBJECT(ParticleData);
   static void  initPersistFields();

   // AFX CODE BLOCK (enhanced-particle) <<
   bool constrain_pos;
   // AFX CODE BLOCK (enhanced-particle) >>
};

//*****************************************************************************
// Particle
// 
// This structure should be as small as possible.
//*****************************************************************************
struct Particle
{
   Point3F  pos;     // current instantaneous position
   Point3F  vel;     //   "         "         velocity
   Point3F  acc;     // Constant acceleration
   Point3F  orientDir;  // direction particle should go if using oriented particles

   U32           totalLifetime;   // Total ms that this instance should be "live"
   ParticleData* dataBlock;       // datablock that contains global parameters for
                                  //  this instance
   U32       currentAge;


   // are these necessary to store here? - they are interpolated in real time
   ColorF           color;
   F32              size;

   F32              spinSpeed;
   Particle *       next;

   // AFX CODE BLOCK (enhanced-particle) <<
   Point3F  pos_local;
   F32      t_last;
   Point3F  radial_v;   // radial vector for concentric effects
   // AFX CODE BLOCK (enhanced-particle) >>
};


#endif // _PARTICLE_H_