
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "sceneGraph/sceneState.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/detailManager.h"

#include "afx/ce/afxModel.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

void afxModel::renderObject(SceneState* state, SceneRenderImage* image)
{
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

  if (!shape_inst || !DetailManager::selectCurrentDetail(shape_inst))
    return;

  RectI viewport;
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  dglGetViewport(&viewport);

  gClientSceneGraph->getLightManager()->sgSetupLights(this);

  state->setupObjectProjection(this);

  if (mDataBlock->texture_filtering)
  {
    F32 axis = (getObjBox().len_x() + getObjBox().len_y() + getObjBox().len_z()) / 3.0;
    F32 dist = (getRenderWorldBox().getClosestPoint(state->getCameraPosition()) - state->getCameraPosition()).len();
    if (dist != 0)
    {
      F32 projected = dglProjectRadius(dist, axis) / 350;
      if (projected < (1.0 / 16.0))
      {
        TextureManager::setSmallTexturesActive(true);
      }
    }
  }

  F32 opacity = fade_amt*mDataBlock->alpha_mult;

  shape_inst->animate();

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  dglMultMatrix(&mObjToWorld);
  glScalef(mObjScale.x, mObjScale.y, mObjScale.z);
  
  shape_inst->setEnvironmentMap(state->getEnvironmentMap());
  shape_inst->setEnvironmentMapOn(true,1);
  ///shape_inst->setAlphaAlways(1.0);
  
  Point3F cameraOffset;
  mObjToWorld.getColumn(3,&cameraOffset);
  cameraOffset -= state->getCameraPosition();
  F32 fogAmount = state->getHazeAndFog(cameraOffset.len(),cameraOffset.z);
  fogAmount *= mDataBlock->fog_mult;
  
  // render all transparent and opaque meshes
  if (opacity < 1.0f)
  {
    TSShapeInstance::smNoRenderNonTranslucent = false;
    TSShapeInstance::smNoRenderTranslucent    = false;
  }
  // render transparent meshes only 
  else if (image->isTranslucent == true)
  {
    TSShapeInstance::smNoRenderNonTranslucent = true;
    TSShapeInstance::smNoRenderTranslucent    = false;
  }
  // render opaque meshes only
  else
  {
    TSShapeInstance::smNoRenderNonTranslucent = false;
    TSShapeInstance::smNoRenderTranslucent    = true;
  }
  
  if (seq_animates_vis || mDataBlock->use_vertex_alpha)
    shape_inst->setAlphaAlways(opacity);
  else
    TSMesh::setOverrideFade(opacity);

  shape_inst->setupFog(fogAmount, state->getFogColor());
  shape_inst->render();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

#if defined(AFX_MODEL_SHADOWS)
  // shadow rendering code -- this works well for independent models
  // but for mounted models, the shadowing must be a merged operation
  // to appear correct
  if (mDataBlock->shadowEnable)
  {
    shadows.sgRender(this, shape_inst, cameraOffset.len(), fogAmount, 0.0f, 0.0f, -1, true, true);
  }
#endif

  if (seq_animates_vis || mDataBlock->use_vertex_alpha)
    shape_inst->setAlphaAlways(1.0f);
  else
    TSMesh::setOverrideFade(1.0f);

  TSShapeInstance::smNoRenderNonTranslucent = false;
  TSShapeInstance::smNoRenderTranslucent    = false;
  TextureManager::setSmallTexturesActive(false);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();

  glMatrixMode(GL_MODELVIEW);
  dglSetViewport(viewport);

  gClientSceneGraph->getLightManager()->sgResetLights();

  dglSetCanonicalState();
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
