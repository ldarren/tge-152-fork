
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "afx/afxChoreographer.h"
#include "afx/ce/afxMooring.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

bool afxMooring::prepRenderImage(SceneState* state, const U32 stateKey,
                                 const U32 /*startZone*/, const bool /*modifyBaseState*/)
{
  if (!mDataBlock->display_axis_marker || isLastState(state, stateKey))
    return false;
  setLastState(state, stateKey);
  
  // This should be sufficient for most objects that don't manage zones, and
  //  don't need to return a specialized RenderImage...
  if (state->isObjectRendered(this)) {
    SceneRenderImage* image = new SceneRenderImage;
    image->obj = this;
    image->isTranslucent = true;
    image->sortType = SceneRenderImage::Point;
    state->insertRenderImage(image);
  }
  
  return false;
}

void afxMooring::renderObject(SceneState* state, SceneRenderImage*)
{
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");
  
  RectI viewport;
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  dglGetViewport(&viewport);
  
  // Uncomment this if this is a "simple" (non-zone managing) object
  state->setupObjectProjection(this);
  
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  dglMultMatrix(&getRenderTransform());
  glScalef(mObjScale.x, mObjScale.y, mObjScale.z);
  
  // DRAW HERE
  //glDisable(GL_DEPTH_TEST);

  glBegin(GL_LINES);
  glColor3f(1, 0, 0);
  glVertex3f(-0.5, 0, 0); glVertex3f(0.5, 0, 0);
  glColor3f(0, 1, 0);
  glVertex3f(0, -0.5, 0); glVertex3f(0, 0.5, 0);
  glColor3f(0, 0, 1);
  glVertex3f(0, 0, -0.5); glVertex3f(0, 0, 0.5);
  glEnd();

  //glEnable(GL_DEPTH_TEST);
  ////////
  
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  dglSetViewport(viewport);
  
  AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//