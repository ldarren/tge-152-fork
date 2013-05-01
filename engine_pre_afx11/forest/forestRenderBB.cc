//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "dgl/dgl.h"
#include "core/dnet.h"
#include "game/game.h"
#include "math/mMath.h"
#include "console/simBase.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "console/consoleInternal.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "sim/sceneObject.h"
#include "sceneGraph/sceneState.h"
#include "sceneGraph/sgUtil.h"
#include "game/gameBase.h"
#include "ts/TSShapeInstance.h"
#include "ts/TSShapeConstruct.h"
#include "ts/tsLastDetail.h"
#include "math/mathIO.h"
#include "platform/profiler.h"
#include "core/frameAllocator.h"
#include "dgl/gTextureSheet.h"

#include "forest/forestItem.h"
#include "forest/forest.h"
#include "forest/forestTextureSheet.h"

// OMG WE ARE GONNA BATCH
// I LOVE BATCHING 
// IT MAKES ME SO HOT
// I FEEL SO PRETTY

class ForestBatcher
{
public:

   Vector<U16> mIndices;

   ///  Our vertex format.
   struct pcntf
   {
      Point3F p; // 12
      ColorI  c; // 4
      Point3F n; // 12
      Point2F t; // 8
      F32     f; // 4
                 //    = 36 bytes... couldn't it be 4 less? :(
   };

   Vector<pcntf> mVertices;
   S32 mTexturePage;

   ForestBatcher()
   {
      mTexturePage = -1;
      mVertices.setSize(4*256);
      prepIndices();
      mVertices.clear();
   }

   /// Make sure we have enough indices for our rendering.
   void prepIndices()
   {
      U32 indexGoal = (mVertices.size() / 4) * 6;

      AssertFatal((indexGoal / 6) * 4  == mVertices.size(), "ForestBatcher::prepIndices - got non-multiple-of-4 vertex count!");

      if(indexGoal > mIndices.size())
      {
         U32 preSize = mIndices.size();

         mIndices.increment( indexGoal - mIndices.size());
         
         // Generate some more indices; we've not enough.
         for(S32 i=preSize/6; i<indexGoal/6; i++)
         {
            // Ok, we make indices in groups of six.
            const U32 vertOffset = i*4;
            const U32 idxOffset = i*6;

            mIndices[idxOffset + 0] = vertOffset + 0;
            mIndices[idxOffset + 1] = vertOffset + 1;
            mIndices[idxOffset + 2] = vertOffset + 2;

            mIndices[idxOffset + 3] = vertOffset + 0;
            mIndices[idxOffset + 4] = vertOffset + 2;
            mIndices[idxOffset + 5] = vertOffset + 3;
         }
      }

      // Ok, we have enough indices now!
   }

   void add(const Point3F pts[4], Point3F norm, const F32 alpha, const Point2F texMin, const Point2F texMax, const F32 fog)
   {
      // Ok, if you ever have the problem with the texture being sideways
      // load a test texture, see why it is messed up, throw a breakpoint 
      // in here, but only hit at it after the game is rendering, then 
      // graph the points in excel and figure out how to adjust the texture
      // coords to make them right. This should work now, but if it ever gets
      // hosed again, that's how I fixed it.

      // Stuff into the vertex list.
      mVertices.increment(4);
      pcntf *geom = &(mVertices.last()) - 3;

      geom[0].p = pts[0];
      geom[0].c.set(0xFF,0xFF,0xFF, F32(0xFF)*alpha);
      geom[0].n.set(norm);
      geom[0].t = texMin;
      geom[0].f = fog;

      geom[1].p = pts[1];
      geom[1].c.set(0xFF,0xFF,0xFF, F32(0xFF)*alpha);
      geom[1].n.set(norm);
      geom[1].t.set(texMax.x,texMin.y);
      geom[1].f = fog;

      geom[2].p = pts[2];
      geom[2].c.set(0xFF, 0xFF, 0xFF, F32(0xFF)*alpha);
      geom[2].n.set(norm);
      geom[2].t = texMax;
      geom[2].f = fog;

      geom[3].p = pts[3];
      geom[3].c.set(0xFF,0xFF,0xFF, F32(0xFF)*alpha);
      geom[3].n.set(norm);
      geom[3].t.set(texMin.x,texMax.y);
      geom[3].f = fog;
   }

   void setup(SceneState *state)
   {
      // alpha blend...
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

      // Save some fill rate.
      glEnable(GL_ALPHA_TEST);
      glAlphaFunc(GL_GREATER, 0.1f);

      // Enable texturing
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_COMBINE);
      
      glEnable( GL_COLOR_MATERIAL );
      glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

      // color comes from texture color...
      glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_RGB,   GL_REPLACE);
      glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_RGB,   GL_TEXTURE);
      glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_RGB,  GL_SRC_COLOR);
      
      // alpha is product of texture and constant alpha...
      glTexEnvi(GL_TEXTURE_ENV,GL_COMBINE_ALPHA, GL_MODULATE);
      glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE0_ALPHA, GL_PRIMARY_COLOR);
      glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND0_ALPHA,GL_SRC_ALPHA);

      glTexEnvi(GL_TEXTURE_ENV,GL_SOURCE1_ALPHA, GL_TEXTURE);
      glTexEnvi(GL_TEXTURE_ENV,GL_OPERAND1_ALPHA, GL_SRC_ALPHA);

      // No depth writes!
      glDepthMask(GL_FALSE);

      // Set up fogging.
      glEnable(GL_FOG);
      glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FOG_COORDINATE_EXT);

      GLfloat fogColor[4];
      fogColor[0] = state->getFogColor().red;
      fogColor[1] = state->getFogColor().green;
      fogColor[2] = state->getFogColor().blue;
      fogColor[3] = 1.0f;
      glFogfv(GL_FOG_COLOR, fogColor);
      glFogi(GL_FOG_MODE, GL_LINEAR);
      glFogf(GL_FOG_START, 0.0f);
      glFogf(GL_FOG_END, 1.0f);

      // We only need the first TU.
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glActiveTextureARB(GL_TEXTURE0_ARB);

      // Set up client arrays.
      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);
      glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
      glEnableClientState(GL_NORMAL_ARRAY);
   }

   void flushAndDraw()
   {
      // Skip out if there's nothing to draw.
      if(!mVertices.size())
         return;

      PROFILE_START(forest_flushBBRender);

      prepIndices();

      // Set up texture
      if( Forest::smForestTSheets[mTexturePage]->getTextureHandle() != NULL )
         glBindTexture( GL_TEXTURE_2D, Forest::smForestTSheets[mTexturePage]->getTextureHandle()->getGLName() );

      // Have to rebind in case we changed our memory, and, might indicate
      // that something changed.
      glVertexPointer  (3, GL_FLOAT,         sizeof(pcntf), &mVertices[0].p);
      glColorPointer   (4, GL_UNSIGNED_BYTE, sizeof(pcntf), &mVertices[0].c);
      glNormalPointer  (GL_FLOAT,            sizeof(pcntf), &mVertices[0].n);
      glTexCoordPointer(2, GL_FLOAT,         sizeof(pcntf), &mVertices[0].t);
      glFogCoordPointerEXT(GL_FLOAT,         sizeof(pcntf), &mVertices[0].f);

      // Ok, do the actual render now.
      glDrawElements(GL_TRIANGLES, 6*(mVertices.size()/4), GL_UNSIGNED_SHORT, mIndices.address());

      mVertices.clear();

      PROFILE_END();
   }

   void teardown()
   {
      PROFILE_START(forest_cleanBBRender);

      // Make sure there's nothing more to draw.
      flushAndDraw();

      // Clean up states.
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);

      glDisable(GL_TEXTURE_2D);
      glDisable( GL_COLOR_MATERIAL );

      glDisable(GL_ALPHA_TEST);
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);

      glFogi(GL_FOG_COORDINATE_SOURCE_EXT, GL_FRAGMENT_DEPTH_EXT);
      glDisable(GL_FOG);

      PROFILE_END();
   }
};

ForestBatcher gForestBatcher;

void Forest::prepBBRender()
{
   installLights();
   gForestBatcher.setup(smState);
}

void Forest::cleanBBRender()
{
   gForestBatcher.teardown();
   uninstallLights();
}

void Forest::doBBRender(ForestItem *fi, TSShapeInstance *si, S32 dl, F32 alpha)
{   
   // Update the forest item if appropriate.
   if(!fi->mValidView.isContained(smState->mCamPosition))
   {
      PROFILE_START(forest_doBBRender_updateCache);

      MatrixF modelView = smState->mModelview;

      // Figure bitmap and rotation for this billboard.

      // Actual center-point of billboard.
      const Point3F position = fi->getTransform().getPosition() + fi->mCenter;

      // Generate a positional matrix for the BB...
      MatrixF tmpPos;
      tmpPos = fi->getTransform();
      tmpPos.setPosition(position);

      // Generate a eyespace matrix for the BB.
      MatrixF tmp;
      tmp.mul(modelView, tmpPos);

      // Select the bitmap & rotation.
      fi->mBitmap = smForestTSheets[fi->getData()->mShapeIndex]->getIndexFromMatrix( (F32*)&tmp, &fi->mRotY );

      // Generate a rotation matrix using some optimized code...
      MatrixF rotMatrix(1);
      F32 *m = (F32*)&rotMatrix;

      if (fi->mRotY*fi->mRotY>0.0001f)
      {
         m[0] =  m[10] = mCos(fi->mRotY);
         m[2] = mSin(fi->mRotY); m[8] = -m[2];
         m[1] =  m[4] = m[6] = m[9] = 0.0f;
         m[5] =  1.0f;
      }
      else
      {
         m[0] = m[5] = m[10] = 1.0f;
         m[1] = m[2] = m[4] = m[6] = m[8] = m[9] = 0.0f;
      }

      // modelView needs to be transposed, of course...
      modelView.transpose();

      // Reset the verts to the original points so we can transform them
      // for batching
      dMemcpy( &fi->mVerts, &fi->mPoints, sizeof( fi->mVerts ) );

      // Transform the points in-place.
      for( int i = 0; i < 4; i++ )
      {
         rotMatrix.mulP( fi->mVerts[i] );
         modelView.mulP( fi->mVerts[i] );
         fi->mVerts[i] += position;
      }

      // Update normal.
      fi->mNormal.set(0, -1, 0);
      rotMatrix.mulV(fi->mNormal);
      modelView.mulV(fi->mNormal);

      // Update cache.
      fi->mValidView.center = smState->mCamPosition;
      fi->mValidView.radius = mTan(0.01) * (smState->mCamPosition - fi->getTransform().getPosition()).len();  

      PROFILE_END();
   }

   // If we're switching to a different billboard, then we have to switch
   // texture sheets.
   if(fi->getData()->mShapeIndex != gForestBatcher.mTexturePage || gForestBatcher.mVertices.size() > 4*512)
      gForestBatcher.flushAndDraw();

   // Stick the billboard into the list.
   PROFILE_START(forest_doBBRender_insert);

   gForestBatcher.mTexturePage = fi->getData()->mShapeIndex;

   // Grab the min/max texture coords
   Point2F min, max;
   smForestTSheets[gForestBatcher.mTexturePage]->getTextureCoords( fi->mBitmap, &min, &max );

   // Add it to the thingy
   gForestBatcher.add(fi->mVerts, fi->mNormal, mClampF(alpha, 0, 1), min, max, fi->getFogAmount());

   PROFILE_END();
}
