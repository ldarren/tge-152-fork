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

#ifndef _H_PARTICLE_EMITTER
#define _H_PARTICLE_EMITTER

#ifndef _GAMEBASE_H_
#include "game/gameBase.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif

#include "particle.h"
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
#include "gfx/gfxDevice.h"
#endif

class  ParticleData;

//*****************************************************************************
// Particle Emitter Data
//*****************************************************************************
class ParticleEmitterData : public GameBaseData
{
   typedef GameBaseData Parent;

  public:
   ParticleEmitterData();
   DECLARE_CONOBJECT(ParticleEmitterData);
   static void initPersistFields();
   void packData(BitStream* stream);
   void unpackData(BitStream* stream);
   bool preload(bool server, char errorBuffer[256]);
   bool onAdd();
   // AFX CODE BLOCK (to-tgea-from-tge) <<
   bool loadParameters();
   bool reload(bool server);
   // AFX CODE BLOCK (to-tgea-from-tge) >>

   void allocPrimBuffer( S32 overrideSize = -1 );

  public:
   S32   ejectionPeriodMS;                   ///< Time, in Miliseconds, between particle ejection
   S32   periodVarianceMS;                   ///< Varience in ejection peroid between 0 and n

   F32   ejectionVelocity;                   ///< Ejection velocity
   F32   velocityVariance;                   ///< Variance for velocity between 0 and n
   F32   ejectionOffset;                     ///< Z offset from emitter point to eject from

   F32   thetaMin;                           ///< Minimum angle, from the horizontal plane, to eject from
   F32   thetaMax;                           ///< Maximum angle, from the horizontal plane, to eject from

   F32   phiReferenceVel;                    ///< Reference angle, from the verticle plane, to eject from
   F32   phiVariance;                        ///< Varience from the reference angle, from 0 to n

   U32   lifetimeMS;                         ///< Lifetime of particles
   U32   lifetimeVarianceMS;                 ///< Varience in lifetime from 0 to n

   bool  overrideAdvance;                    ///<
   bool  orientParticles;                    ///< Particles always face the screen
   bool  orientOnVelocity;                   ///< Particles face the screen at the start
   bool  useEmitterSizes;                    ///< Use emitter specified sizes instead of datablock sizes
   bool  useEmitterColors;                   ///< Use emitter specified colors instead of datablock colors

   StringTableEntry      particleString;     ///< Used to load particle data directly from a string

   Vector<ParticleData*> particleDataBlocks; ///< Particle Datablocks 
   Vector<U32>           dataBlockIds;       ///< Datablock IDs (parellel array to particleDataBlocks)

   U32                   partListInitSize;   /// initial size of particle list calc'd from datablock info

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   GFXPrimitiveBufferHandle   primBuff;
#else
   bool                  mixedInvAlpha;
   bool                  oldstyleAnimated;
#endif

   // This enum specifies common blend settings with predefined values
   // for src/dst blend factors. 
   enum BlendStyle {
     BlendUndefined,
     BlendNormal,
     BlendAdditive,
     BlendSubtractive,
     BlendPremultAlpha,
     BlendUser,
   };
   enum {
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
     BLEND_UNDEFINED = GFXBlend_COUNT      // An undefined blend factor value
#else
     BLEND_UNDEFINED = 0xffff      // An undefined blend factor value
#endif
   };
   S32                   blendStyle;         ///< Pre-define blend factor setting
   S32                   srcBlendFactor;     ///< Source blend factor setting
   S32                   dstBlendFactor;     ///< Destination blend factor setting
   bool                  sortParticles;      ///< Particles are sorted back-to-front
   bool                  reverseOrder;       ///< reverses draw order
   StringTableEntry      textureName;        ///< Emitter texture file to override particle textures
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   GFXTexHandle          textureHandle;      ///< Emitter texture handle from txrName
#else
   TextureHandle         textureHandle;
#endif

   // AFX CODE BLOCK (enhanced-emitter) <<
   bool          fade_color;
   bool          fade_size;
   bool          fade_alpha;
   bool          ejectionInvert;
   U8            parts_per_eject; 

   bool          use_emitter_xfm;
   // AFX CODE BLOCK (enhanced-emitter) >>
};

DECLARE_CONSOLETYPE(ParticleEmitterData)

//*****************************************************************************
// Particle Emitter
//*****************************************************************************
class ParticleEmitter : public GameBase
{
   typedef GameBase Parent;

  public:
   ParticleEmitter();
   ~ParticleEmitter();

   static Point3F mWindVelocity;
   static void setWindVelocity( const Point3F &vel ){ mWindVelocity = vel; }
   
#if !defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   bool allowLighting;
   bool lastLightingValue;
   TextureHandle whiteTexture;
   void setupParticleLighting(bool allowlighting);
   void resetParticleLighting();
   void lightParticle(const Particle &part);
#endif

   ColorF getCollectiveColor();

   /// Sets sizes of particles based on sizelist provided
   /// @param   sizeList   List of sizes
   void setSizes( F32 *sizeList );

   /// Sets colors for particles based on color list provided
   /// @param   colorList   List of colors
   void setColors( ColorF *colorList );

   ParticleEmitterData *getDataBlock(){ return mDataBlock; }
   bool onNewDataBlock(GameBaseData* dptr);

   /// By default, a particle renderer will wait for it's owner to delete it.  When this
   /// is turned on, it will delete itself as soon as it's particle count drops to zero.
   void deleteWhenEmpty();

   /// @name Particle Emission
   /// Main interface for creating particles.  The emitter does _not_ track changes
   ///  in axis or velocity over the course of a single update, so this should be called
   ///  at a fairly fine grain.  The emitter will potentially track the last particle
   ///  to be created into the next call to this function in order to create a uniformly
   ///  random time distribution of the particles.  If the object to which the emitter is
   ///  attached is in motion, it should try to ensure that for call (n+1) to this
   ///  function, start is equal to the end from call (n).  This will ensure a uniform
   ///  spatial distribution.
   /// @{

   void emitParticles(const Point3F& start,
                      const Point3F& end,
                      const Point3F& axis,
                      const Point3F& velocity,
                      const U32      numMilliseconds);
   void emitParticles(const Point3F& point,
                      const bool     useLastPosition,
                      const Point3F& axis,
                      const Point3F& velocity,
                      const U32      numMilliseconds);
   void emitParticles(const Point3F& rCenter,
                      const Point3F& rNormal,
                      const F32      radius,
                      const Point3F& velocity,
                      S32 count);
   /// @}

   bool mDead;

  protected:
   /// @name Internal interface
   /// @{

   /// Adds a particle
   /// @param   pos   Initial position of particle
   /// @param   axis
   /// @param   vel   Initial velocity
   /// @param   axisx
   // AFX CODE BLOCK (enhanced-emitter) <<
   void addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx, const U32 age_offset);
   /* ORIGINAL CODE
   void addParticle(const Point3F &pos, const Point3F &axis, const Point3F &vel, const Point3F &axisx);
   */
   // AFX CODE BLOCK (enhanced-emitter) >>

#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   inline void setupBillboard( Particle *part,
                               Point3F *basePts,
                               const MatrixF &camView,
                               GFXVertexPCT *lVerts );

   inline void setupOriented( Particle *part,
                              const Point3F &camPos,
                              GFXVertexPCT *lVerts );
#else
   void renderBillboardParticle( const Particle &part, const Point3F *basePnts, const MatrixF &camView, const F32 spinFactor );
   void renderOrientedParticle( const Particle &part, const Point3F &camPos );
#endif

   /// Updates the bounding box for the particle system
   void updateBBox();

   /// @}
  protected:
   bool onAdd();
   void onRemove();

   void processTick(const Move *move);
   void advanceTime(F32 dt);

   // Rendering
  protected:
   bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   void prepBatchRender( const Point3F& camPos );
   void copyToVB( const Point3F &camPos );
#else
   inline void ParticleEmitter::prepParticleRender(const Particle*, bool& prevInvAlpha, U32 src_blend, U32 dst_blend);
   void renderObject(SceneState *state, SceneRenderImage *image);
#endif

   // PEngine interface

   // AFX CODE BLOCK (enhanced-emitter) <<
   //   Some methods/members moved from private to protected to make them visible
   //   to afxParticleEmitter and other subclasses.
  private:
   ParticleEmitterData* mDataBlock;
   bool      mDeleteWhenEmpty;
   bool      mDeleteOnTick;
   F32       sizes[ ParticleData::PDC_NUM_KEYS ];
   ColorF    colors[ ParticleData::PDC_NUM_KEYS ];
   S32       mCurBuffSize;
#if defined(TORQUE_SHADER_ENGINE) // AFX PREPROC BLOCK
   GFXVertexBufferHandle<GFXVertexPCT> mVertBuff;
#endif
   void      update( U32 ms );

  protected:
   U32       mInternalClock;
   U32       mNextParticleTime;
   Point3F   mLastPosition;
   bool      mHasLastPosition;
   S32       mLifetimeMS;
   S32       mElapsedTimeMS;
   void      updateKeyData( Particle *part );

   //   These members are for implementing a link-list of the active emitter 
   //   particles. Member part_store contains blocks of particles that can be
   //   chained in a link-list. Usually the first part_store block is large
   //   enough to contain all the particles but it can be expanded in emergency
   //   circumstances.
   Vector <Particle*> part_store;
   Particle*  part_freelist;
   Particle   part_list_head;
   S32        n_part_capacity;
   S32        n_parts;
   /* ORIGINAL CODE
  private:

   void update( U32 ms );
   inline void updateKeyData( Particle *part );


  private:
   ParticleEmitterData* mDataBlock;

   U32       mInternalClock;

   U32       mNextParticleTime;

   Point3F   mLastPosition;
   bool      mHasLastPosition;

   bool      mDeleteWhenEmpty;
   bool      mDeleteOnTick;

   S32       mLifetimeMS;
   S32       mElapsedTimeMS;

   F32       sizes[ ParticleData::PDC_NUM_KEYS ];
   ColorF    colors[ ParticleData::PDC_NUM_KEYS ];

   GFXVertexBufferHandle<GFXVertexPCT> mVertBuff;

   //   These members are for implementing a link-list of the active emitter 
   //   particles. Member part_store contains blocks of particles that can be
   //   chained in a link-list. Usually the first part_store block is large
   //   enough to contain all the particles but it can be expanded in emergency
   //   circumstances.
   Vector <Particle*> part_store;
   Particle*  part_freelist;
   Particle   part_list_head;
   S32        n_part_capacity;
   S32        n_parts;
   S32       mCurBuffSize;
   */
   // AFX CODE BLOCK (enhanced-emitter) >>

   // AFX CODE BLOCK (enhanced-emitter) <<
  protected:
   F32         fade_amt;
   bool        forced_bbox;
   Point3F     pos_pe;
   S8          sort_priority;

   virtual void   sub_particleUpdate(Particle*) { }

  public:
   virtual void   emitParticlesExt(const MatrixF& xfm, const Point3F& point, const Point3F& velocity, const U32 numMilliseconds);
   void           setFadeAmount(F32 amt) { fade_amt = amt; }  
   void           setForcedObjBox(Box3F& box);
   void           setSortPriority(S8 priority);
   // AFX CODE BLOCK (enhanced-emitter) >>
};

#endif // _H_PARTICLE_EMITTER

