//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//    Changes:
//        enhanced-emitter -- enhancements to particle emitters.
//       enhanced-particle -- enhancements to particles.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#ifndef _PARTICLEEMITTER_H_
#define _PARTICLEEMITTER_H_

#ifndef _GAMEBASE_H_
#include "game/gameBase.h"
#endif
#ifndef _COLOR_H_
#include "core/color.h"
#endif

//-------------------------------------- Engine initialization...
//
namespace ParticleEngine {

   enum ParticleConsts
   {
      PC_COLOR_KEYS = 4,
      PC_SIZE_KEYS = 4,
   };

   /// Initalize the particle engine
   void init();

   /// Destroy the particle engine
   void destroy();

   extern Point3F windVelocity; ///< Global wind velocity for all particles

   /// Sets the wind velocity for all particles
   /// @param   vel   Velocity
   inline void setWindVelocity(const Point3F & vel) { windVelocity = vel; }

   /// Returns the wind velocity
   inline Point3F getWindVelocity() { return windVelocity; }
}


//--------------------------------------------------------------------------
//-------------------------------------- The data and the Emitter class
//                                        are all that the game should deal
//                                        with (other than initializing the
//                                        global engine pointer of course)
//
struct Particle;
class  ParticleData;


class  ParticleEmitter;

class ParticleData : public SimDataBlock
{
   typedef SimDataBlock Parent;

   enum PDConst
   {
      PDC_MAX_TEX = 50,
   };

  public:
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


   bool  allowLighting;


   bool  animateTexture;
   U32   numFrames;
   U32   framesPerSec;

   ColorF colors[ParticleEngine::PC_COLOR_KEYS];
   F32    sizes[ParticleEngine::PC_SIZE_KEYS];
   F32    times[4];

   StringTableEntry  textureNameList[PDC_MAX_TEX];
   TextureHandle     textureList[PDC_MAX_TEX];

  public:
   ParticleData();
   ~ParticleData();

   void initializeParticle(Particle*, const Point3F&);

   void packData(BitStream* stream);
   void unpackData(BitStream* stream);
   bool onAdd();
   bool preload(bool server, char errorBuffer[256]);
   bool loadParameters();
   bool reload(char errorBuffer[256]);
   DECLARE_CONOBJECT(ParticleData);
   static void  initPersistFields();

  // AFX CODE BLOCK (enhanced-particle) <<
  private:
   static void   afx_initPersistFields();
   void          afx_init();
   void          afx_packData(BitStream* stream);
   void          afx_unpackData(BitStream* stream);
   bool          afx_loadParameters();
   bool          afx_reload();
  public:
   bool          constrain_pos;
  // AFX CODE BLOCK (enhanced-particle) >>
};

struct Particle
{
   Point3F  pos;     // current instantaneous position
   Point3F  vel;     //   "         "         velocity
   Point3F  acc;     // Constant acceleration
   Point3F  orientDir;  // direction particle should go if using oriented particles

   U32           totalLifetime;   // Total ms that this instance should be "live"
   ParticleData* dataBlock;       // datablock that contains global parameters for
                                  //  this instance

   Particle* nextInList;   // Managed by the current owning emitter
   U32       currentAge;

   Particle*        nextInEngine; // Managed by the global engine object
   ParticleEmitter* currentOwner;
   ColorF           color;
   F32              size;
   F32              spinSpeed;

   // AFX CODE BLOCK (enhanced-particle) <<
   Point3F pos_local;
   // AFX CODE BLOCK (enhanced-particle) >>
};

// AFX CODE BLOCK (enhanced-emitter) <<
//   forward declarations
class afxPathData;
class afxPath3D;
// AFX CODE BLOCK (enhanced-emitter) >>

//--------------------------------------
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

   bool loadParameters();
   bool reload();

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
   Vector<ParticleData*> particleDataBlocks; ///< Datablocks for particle emissions
   Vector<U32>           dataBlockIds;       ///< Datablock IDs which corospond to the particleDataBlocks

  // AFX CODE BLOCK (enhanced-emitter) <<
  private:
    static void   afx_initPersistFields();
    void          afx_init();
    void          afx_packData(BitStream* stream);
    void          afx_unpackData(BitStream* stream);
    bool          afx_loadParameters();
    bool          afx_reload(); 
    
  public: 
    // The afx enhanced particle emitter adds
    // these different emitter types:
    //
    enum EmitterTypes 
    {
      EMITTER_SPRINKLER,    // classic torque particle emitter
      EMITTER_VECTOR,       // emits particles along a vector
      EMITTER_CONE,         // emits particles in a cone shell
      EMITTER_PATH,         // emits particles along a path
      EMITTER_DISC          // emits particles in a disk shape
    };
    U32           emitter_type; 
    bool          ejectionInvert;    // cones and paths
    
    // The afx enhanced particle emitter allows fading
    // of particle color, size, velocity, and/or offset.
    // Fading is controlled by a common value which is
    // set externally using setFadeAmount().
    //
    bool          fade_color;
    bool          fade_size;
    bool          fade_velocity;
    bool          fade_offset;
    Point3F       pe_vector;
    
    // these define the cone for EMITTER_CONE emitters.                     
    F32           spread_min;
    F32           spread_max;
    
    enum PathOriginTypes 
    {
      PATHEMIT_ORIGIN,
      PATHEMIT_POINT,
      PATHEMIT_VECTOR,
      PATHEMIT_TANGENT
    };   
    StringTableEntry      paths_string;       // 
    Vector<afxPathData*>  pathDataBlocks;     // datablocks for paths
    Vector<U32>           pathDataBlockIds;   // datablock IDs which correspond to the pathDataBlocks
    U32                   path_origin_type;
    F32                   pe_radius_min;
    F32                   pe_radius_max;
  // AFX CODE BLOCK (enhanced-emitter) >>
};
DECLARE_CONSOLETYPE(ParticleEmitterData)


//--------------------------------------
class ParticleEmitter : public GameBase
{
   typedef GameBase Parent;
   friend class PEngine;

  public:
   ParticleEmitter();
   ~ParticleEmitter();

   bool allowLighting;
   bool lastLightingValue;
   TextureHandle whiteTexture;
   void setupParticleLighting(bool allowlighting);
   void resetParticleLighting();
   void lightParticle(const Particle &part);
   ColorF getCollectiveColor()
   {
	   U32 count = 0;
	   ColorF color(0.0f, 0.0f, 0.0f);
	   Particle *particle = mParticleListHead;

	   while(particle)
	   {
		   color += particle->color;
		   particle = particle->nextInList;
		   count++;
	   }

	   if(count > 0)
		   color /= count;
	   return color;
   }

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

   virtual void setTransform(const MatrixF & mat);

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

   /// Renders a particle facing the camera with a spin factor
   /// @param   part   Particle
   /// @param   basePnts   Base points for the quad the particle is rendered on
   /// @param   camView   Camera view matrix
   /// @param   spinFactor   0.0-1.0 modifyer for
   void renderBillboardParticle( const Particle &part, const Point3F *basePnts, const MatrixF &camView, const F32 spinFactor );

   /// Renders a particle which will face the camera but spin itself to look
   /// like it is facing a particular velocity.
   /// @param   part   Particle
   /// @param   camPos   Camera position
   void renderOrientedParticle( const Particle &part, const Point3F &camPos );

   /// Updates the bounding box for the particle system
   bool updateBBox(const Point3F &position);

   /// @}
  protected:
   bool onAdd();
   void onRemove();

   void processTick(const Move *move);
   void advanceTime(F32 dt);

   // Rendering
  protected:
   bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState);
   void renderObject(SceneState *state, SceneRenderImage *image);

   // PEngine interface
  private:
   /// Removes the provided particle from the system and lets the caller of the
   /// function assume control of it
   /// @param   part   Particle
   void stealParticle(Particle *part);

  private:
   ParticleEmitterData* mDataBlock;

   /// This is used so we only update our transform/bounding box
   /// on ticks, to minimize calls to setTransform.
   bool      mNeedTransformUpdate;

   Particle* mParticleListHead;

   U32       mInternalClock;

   U32       mNextParticleTime;

   Point3F   mLastPosition;
   bool      mHasLastPosition;

   bool      mDeleteWhenEmpty;
   bool      mDeleteOnTick;

   S32       mLifetimeMS;
   S32       mElapsedTimeMS;

   F32       sizes[ParticleEngine::PC_SIZE_KEYS];
   ColorF    colors[ParticleEngine::PC_COLOR_KEYS];

  // AFX CODE BLOCK (enhanced-emitter) <<
  private:
    F32         fade_amt;
    Point3F     pe_vector, pe_vector_norm;
    Point3F     cone_v, cone_s0, cone_s1;
    Point3F     disc_v, disc_r;
    Vector<afxPath3D*> paths;
    Vector<F32> path_mults;
    U32         n_path_points;
    Point3F**   path_points;
    bool        forced_bbox;
    Point3F     pos;
    
    void        afx_init();
    void        afx_initPaths();
    void        afx_cleanupPaths();
    void        afx_preCompute(const MatrixF& mat);
    void        afx_emitParticles(const Point3F& point, const bool useLastPosition, const Point3F& velocity, const U32 numMilliseconds);
    void        afx_emitParticles(const Point3F& start, const Point3F& end, const Point3F& velocity, const U32 numMilliseconds);

    void        addParticleVector(const Point3F& pos, const Point3F& vec, const Point3F& vel, const U32 age_offset);
    void        addParticleCone(const Point3F& pos, const Point3F& vel, const U32 age_offset);
    void        addParticlePath(const Point3F& pos, const Point3F& vel, const U32 age_offset);
    void        addParticleDisc(const Point3F& pos, const Point3F& vel, const U32 age_offset);
    
  public:
    void        emitParticlesExt(const MatrixF& xfm, const Point3F& point, const Point3F& velocity, const U32 numMilliseconds);
    void        setFadeAmount(F32 amt) { fade_amt = amt; }  
    void        setForcedObjBox(Box3F& box) { mObjBox = box; forced_bbox = true; }
  // AFX CODE BLOCK (enhanced-emitter) >>
};

#endif // _H_PARTICLEEMITTER

