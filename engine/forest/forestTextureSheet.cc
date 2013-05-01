//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "dgl/dgl.h"
#include "core/dnet.h"
#include "core/bitstream.h"
#include "game/game.h"
#include "math/mMath.h"
#include "math/MathIO.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/ConsoleTypes.h"
#include "game/moveManager.h"
#include "game/gameConnection.h"
#include "ts/tsShapeInstance.h"
#include "core/resManager.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sceneState.h"

#include "forest/forestItem.h"
#include "forest/forest.h"
#include "forest/forestTextureSheet.h"

ForestTextureSheet::ForestTextureSheet( TSShapeInstance *shape, U32 numEquatorSteps, U32 numPolarSteps, F32 polarAngle, bool includePoles, S32 detailLevel, U32 textureSize )
{
   createTextureSheet( shape, numEquatorSteps, numPolarSteps, polarAngle, includePoles, detailLevel, textureSize );
}

void ForestTextureSheet::createTextureSheet( TSShapeInstance *shape, U32 numEquatorSteps, U32 numPolarSteps, F32 polarAngle, bool includePoles, S32 detailLevel, U32 textureSize )
{
   // Preallocate our pointers.
   Vector<GBitmap *> bitmaps(numEquatorSteps * (numPolarSteps * 2 + 1));

   mNumEquatorSteps = numEquatorSteps;
   mNumPolarSteps   = numPolarSteps;
   mPolarAngle      = polarAngle;
   mIncludePoles    = includePoles;

   const F32 equatorStepSize = M_2PI_F / (F32) numEquatorSteps;
   const F32 polarStepSize   = numPolarSteps>0 ? (0.5f * M_PI_F - polarAngle) / (F32)numPolarSteps : 0.0f;

   F32 rotZ = 0;
   for (U32 i=0; i<numEquatorSteps; i++)
   {
      F32 rotX = (numPolarSteps > 0 ? -(0.5 * M_PI_F - polarAngle) : 0.0f);
      for (U32 j=0; j<2*numPolarSteps+1; j++)
      {
         MatrixF angMat;
         angMat.mul(MatrixF(EulerF(0,0,-M_PI_F+rotZ)),MatrixF(EulerF(-M_PI_F + rotX,0,0)));
         bitmaps.push_back(shape->snapshot(textureSize,textureSize,true,angMat,detailLevel,1.0f,true));
         rotX += polarStepSize;
      }
      rotZ += equatorStepSize;
   }

   if (includePoles)
   {
      MatrixF m1( EulerF( M_PI_F / 2.0f, 0, 0 ) );
      MatrixF m2( EulerF( -M_PI_F / 2.0f, 0, 0 ) );
      bitmaps.push_back(shape->snapshot(textureSize,textureSize,true,m1,detailLevel,1.0f,true));
      bitmaps.push_back(shape->snapshot(textureSize,textureSize,true,m2,detailLevel,1.0f,true));
   }

   buildTextureSheet( bitmaps );
}

const S32 ForestTextureSheet::getIndexFromMatrix( const F32 *m, F32 *outRotY ) const
{
   S32 bitmapId;
   F32 dotX = m[3] * m[0];
   F32 dotY = m[3] * m[1];
   F32 dotZ = m[3] * m[2];
   dotX    += m[7] * m[4];
   dotY    += m[7] * m[5];
   dotZ    += m[7] * m[6];
   dotX    += m[11] * m[8];
   dotY    += m[11] * m[9];
   dotZ    += m[11] * m[10];

   // Get the amount of X rotation and init the Z rotation
   F32 rotX = mIncludePoles || mNumPolarSteps ? mAcos(dotZ/mSqrt(dotX*dotX+dotY*dotY+dotZ*dotZ)) : 0.0f;
   F32 rotZ = 0.0f;

   // min=  0.432170    max=  2.305885

   const F32 polarStepSize = mNumPolarSteps>0 ? (0.5f * M_PI_F - mPolarAngle) / (F32)mNumPolarSteps : 0.0f;
   F32 poleLowerThresh = mPolarAngle;
   F32 poleUpperThresh = M_PI - mPolarAngle;

   // Are we looking at the poles?
   if (mIncludePoles && (rotX<poleLowerThresh || rotX>poleUpperThresh))
   {
      // The polar bitmaps are the last few stored, so go to the end of the
      // list...
      bitmapId = mNumEquatorSteps * (2*mNumPolarSteps+1);

      // And if we're dealing with the upper polar view, advance past
      // the lower.
      if (rotX<poleLowerThresh)
         bitmapId++;

      // And calculate our Y rotation...
      *outRotY = mAtan(m[5],m[4]);
   }
   else
   {
      // Figure our equator and polar step size.
      const F32 equatorStepSize = M_2PI_F / (F32) mNumEquatorSteps;

      // Now, ascertain what our Z rotation is...
      rotZ = 0.999f * (mAtan(dotX,dotY) + M_PI_F); // the 0.99f makes sure we are in range

      // Figure what bitmap we're looking along the equator
      bitmapId = ((S32)(rotZ/equatorStepSize)) * (S32)(2*mNumPolarSteps+1);

      // If we have multiple polar steps, adjust based on the polar step.
      if (mNumPolarSteps>0)
      {
         F32 preRound = ((-rotX + M_PI_F) - mPolarAngle) / polarStepSize;
         if(preRound > mNumPolarSteps *  polarStepSize)
            bitmapId += mFloor(preRound);
         else
            bitmapId += mFloor(preRound);
      }

      // Temporarily disable rotation.
      //bitmapId += mCeil(F32(0.5f * M_PI_F - mPolarAngle + rotX) / F32(polarStepSize)); // (S32)((F32)(M_PI_F - (rotX-mPolarAngle)) / (F32)polarStepSize); // +mPolarAngle here? -pw

      // And figure our Y rotation.
      *outRotY = mAtan(m[2],m[10]);
   }

   // make sure we don´t get an invalid bitmap index!
   bitmapId = mClamp(bitmapId, 0, getNumTextures() - 1);

   return bitmapId;
}