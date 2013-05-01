//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#ifndef _FORESTCLEARING_H_
#define _FORESTCLEARING_H_

#include "sim/sceneObject.h"

#define AREA_ANIMATION_ARC         (1.0f / 360.0f)

/// Represents an area where we don't want any trees. Works in conjunction with
/// Forest.
///
/// @see Forest
class ForestClearing : public SceneObject
{
private:
   typedef SceneObject Parent;

protected:

   U32                  mCreationAreaAngle;
   bool                 mAddedToScene;

   // Field Data.
   class tagFieldData
   {
      public:
      U32               mInnerRadiusX;
      U32               mInnerRadiusY;
      U32               mOuterRadiusX;
      U32               mOuterRadiusY;
      S32               mOffsetZ;
      S32               mAllowedTerrainSlope;
      bool              mAlignToTerrain;
      bool              mAllowWaterSurface;
      bool              mInteractions;
      bool              mShowPlacementArea;
      U32               mPlacementBandHeight;
      ColorF            mPlaceAreaColour;

      tagFieldData()
      {
         // Set Defaults.
         mInnerRadiusX       = 0;
         mInnerRadiusY       = 0;
         mOuterRadiusX       = 100;
         mOuterRadiusY       = 100;
         mOffsetZ            = 0;

         mShowPlacementArea    = true;
         mPlacementBandHeight  = 25;
         mPlaceAreaColour.set(0, 0.8f, 0);
      }

   } mFieldData;

public:
   ForestClearing();
   ~ForestClearing();

   // SceneObject
   void renderObject(SceneState *state, SceneRenderImage *image);
   virtual bool prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone,
                                const bool modifyBaseZoneState = false);

   // SimObject
   bool onAdd();
   void onRemove();
   void onEditorEnable();
   void onEditorDisable();
   void inspectPostApply();

   // NetObject
   U32 packUpdate(NetConnection *conn, U32 mask, BitStream *stream);
   void unpackUpdate(NetConnection *conn, BitStream *stream);

   // ConObject.
   static void initPersistFields();
   DECLARE_CONOBJECT(ForestClearing);

   // Forest interface code...
   virtual const bool contains(const Point3F pos);
   ForestClearing *mNext;

   static ForestClearing *smClearingList;
};

#endif // _forestClearing_H_