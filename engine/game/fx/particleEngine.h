
#ifndef _PARTICLE_ENGINE_H_
#define _PARTICLE_ENGINE_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//
// With TGEA style particles, the particle system source is reorganized into
// a more logical arrangement of classes and files. 
//   -- former contents of particleEmitter.[h,cc] are moved to more
//      appropriatly named particleEmitterNode.[h,cc].
//   -- particle and particleEmitter implementations are moved to
//      particle.[h,cc] and particleEmitter.[h,cc].
//   -- particleEngine functionality is mostly folded into particleEmitter.
//
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// This stub is here to allow some engine code to still get these values without
// requiring modification.

#include "game/fx/particleEmitter.h"

namespace ParticleEngine {

   enum ParticleConsts
   {
      PC_COLOR_KEYS = 8,
      PC_SIZE_KEYS = 8,
   };

   /// Initalize the particle engine
   inline void init() { }

   /// Destroy the particle engine
   inline void destroy() { }

   
   inline void setWindVelocity(const Point3F& vel){ ParticleEmitter::setWindVelocity(vel); }
}

#endif // _PARTICLE_ENGINE_H_

