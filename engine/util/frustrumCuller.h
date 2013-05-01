//-----------------------------------------------------------------------------
// Torque Shader Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _FRUSTRUMCULLER_H_
#define _FRUSTRUMCULLER_H_

#include "math/mBox.h"
#include "sceneGraph/sceneState.h"

namespace FrustrumCuller
{
   enum
   {
      /// We need to store at least 4 clip planes (sides of view frustrum).
      MaxClipPlanes = 5,

      // Clipping related flags...
      ClipPlaneMask    = BIT(MaxClipPlanes) - 1,
      FarSphereMask    = BIT(MaxClipPlanes + 1),
      FogPlaneBoxMask  = BIT(MaxClipPlanes + 2),

      AllClipPlanes    = ClipPlaneMask | FarSphereMask,

   };

   extern SceneState *smSceneState;
   extern Point3F     smCamPos;
   extern F32         smFarDistance;
   extern U32         smNumClipPlanes;
   extern PlaneF      smClipPlane[MaxClipPlanes];

   void init(SceneState *state);
   S32  testBoxVisibility(const Box3F &bounds, const S32 mask, const F32 expand);
   F32  getBoxDistance(const Box3F &box);
};

#endif