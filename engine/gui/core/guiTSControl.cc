//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/core/guiTSControl.h"
#include "console/consoleTypes.h"
#include "sceneGraph/sceneLighting.h"
#include "sceneGraph/sceneGraph.h"

IMPLEMENT_CONOBJECT(GuiTSCtrl);

U32 GuiTSCtrl::smFrameCount = 0;

GuiTSCtrl::GuiTSCtrl()
{
   mApplyFilterToChildren = true;

   mCameraZRot = 0;
   mForceFOV = 0;

   for(U32 i = 0; i < 16; i++)
   {
      mSaveModelview[i] = 0;
      mSaveProjection[i] = 0;
   }
   mSaveModelview[0] = 1;
   mSaveModelview[5] = 1;
   mSaveModelview[10] = 1;
   mSaveModelview[15] = 1;
   mSaveProjection[0] = 1;
   mSaveProjection[5] = 1;
   mSaveProjection[10] = 1;
   mSaveProjection[15] = 1;

   mSaveViewport[0] = 0;
   mSaveViewport[1] = 0;
   mSaveViewport[2] = 2;
   mSaveViewport[3] = 2;

   mLastCameraQuery.cameraMatrix.identity();
}

void GuiTSCtrl::initPersistFields()
{
   Parent::initPersistFields();

   addField("applyFilterToChildren",   TypeBool,   Offset(mApplyFilterToChildren, GuiTSCtrl));
   addField("cameraZRot",              TypeF32,    Offset(mCameraZRot, GuiTSCtrl));
   addField("forceFOV",                TypeF32,    Offset(mForceFOV,   GuiTSCtrl));
}

void GuiTSCtrl::consoleInit()
{
   Con::addVariable("$TSControl::frameCount", TypeS32, &smFrameCount);
}

void GuiTSCtrl::onPreRender()
{
   setUpdate();
}

bool GuiTSCtrl::processCameraQuery(CameraQuery *)
{
   return false;
}

void GuiTSCtrl::renderWorld(const RectI& /*updateRect*/)
{
}

bool GuiTSCtrl::project(const Point3F &pt, Point3F *dest)
{
   GLdouble winx, winy, winz;
   GLint result = gluProject(pt.x, pt.y, pt.z,
                     mSaveModelview, mSaveProjection, mSaveViewport,
                     &winx, &winy, &winz);
   if(result == GL_FALSE || winz < 0 || winz > 1)
      return false;
   dest->set(winx, winy, winz);
   return true;
}

bool GuiTSCtrl::unproject(const Point3F &pt, Point3F *dest)
{
   GLdouble objx, objy, objz;
   GLint result = gluUnProject(pt.x, pt.y, pt.z,
                     mSaveModelview, mSaveProjection, mSaveViewport,
                     &objx, &objy, &objz);
   if(result == GL_FALSE)
      return false;
   dest->set(objx, objy, objz);
   return true;
}

void GuiTSCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   if(SceneLighting::isLighting())
      return;

   CameraQuery newCam = mLastCameraQuery;

   if(processCameraQuery(&newCam))
      mLastCameraQuery = newCam;

   if(mForceFOV != 0)
      newCam.fov = mDegToRad(mForceFOV);

   if(mCameraZRot)
   {
      MatrixF rotMat(EulerF(0, 0, mDegToRad(mCameraZRot)));
      newCam.cameraMatrix.mul(rotMat);
   }

   // set up the camera and viewport stuff:
   F32 left, right, top, bottom;
   if (newCam.ortho)
   {
      left     = -newCam.leftRight;
      right    = newCam.leftRight;
      top      = newCam.topBottom;
      bottom   = -newCam.topBottom;
   }
   else
   {
      F32 wwidth = newCam.nearPlane * mTan(newCam.fov / 2);
      F32 wheight = F32(mBounds.extent.y) / F32(mBounds.extent.x) * wwidth;

      F32 hscale = wwidth * 2 / F32(mBounds.extent.x);
      F32 vscale = wheight * 2 / F32(mBounds.extent.y);

      left     = (updateRect.point.x - offset.x) * hscale - wwidth;
      right    = (updateRect.point.x + updateRect.extent.x - offset.x) * hscale - wwidth;
      top      = wheight - vscale * (updateRect.point.y - offset.y);
      bottom   = wheight - vscale * (updateRect.point.y + updateRect.extent.y - offset.y);
   }

   dglSetViewport(updateRect);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   dglSetFrustum(left, right, bottom, top, newCam.nearPlane,  (gClientSceneGraph ? (F64)gClientSceneGraph->getVisibleDistanceMod() : newCam.farPlane), newCam.ortho);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   newCam.cameraMatrix.inverse();
   dglMultMatrix(&newCam.cameraMatrix);

   glGetDoublev(GL_PROJECTION_MATRIX, mSaveProjection);
   glGetDoublev(GL_MODELVIEW_MATRIX, mSaveModelview);

   mSaveViewport[0] = updateRect.point.x;
   mSaveViewport[1] = updateRect.point.y + updateRect.extent.y;
   mSaveViewport[2] = updateRect.extent.x;
   mSaveViewport[3] = -updateRect.extent.y;

   renderWorld(updateRect);

   if(mApplyFilterToChildren)
      renderChildControls(offset, updateRect);

   smFrameCount++;
}
