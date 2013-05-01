//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "game/fx/splash.h"

#include "afx/afxMagicMissile.h"
#include "afx/afxChoreographer.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

bool afxMagicMissile::prepRenderImage(SceneState* state, const U32 stateKey,
                                      const U32 /*startZone*/, 
                                      const bool /*modifyBaseState*/)
{
  if (!is_active())
    return false;

  if (isLastState(state, stateKey))
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
    state->setImageRefPoint(this, image);

    // For projectiles, the datablock pointer is a good enough sort key, since they aren't
    //  skinned at all...
    image->textureSortKey = (U32)(dsize_t)datablock;

    state->insertRenderImage(image);
  }

  return false;
}

// this is for rendering a missile-shape only
void afxMagicMissile::renderObject(SceneState* state, SceneRenderImage *)
{
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

  if (!is_active() || !missile_shape)
    return;

  RectI viewport;
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  dglGetViewport(&viewport);

  // Uncomment this if this is a "simple" (non-zone managing) object
  state->setupObjectProjection(this);

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();

  dglMultMatrix(&getRenderTransform());

  glScalef(datablock->missile_shape_scale.x, 
           datablock->missile_shape_scale.y, 
           datablock->missile_shape_scale.z);

  if (missile_shape)
  {
    missile_shape->selectCurrentDetail();
    missile_shape->animate();

    Point3F cameraOffset;
    mObjToWorld.getColumn(3,&cameraOffset);
    cameraOffset -= state->getCameraPosition();
    F32 fogAmount = state->getHazeAndFog(cameraOffset.len(),cameraOffset.z);

    missile_shape->setupFog(fogAmount, state->getFogColor());
    missile_shape->render();
  }

  glDisable(GL_BLEND);
  glDisable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  dglSetViewport(viewport);

  dglSetCanonicalState();
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
