//-----------------------------------------------------------------------------
// Forest Renderer Pack
// Copyright (C) PushButton Labs
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mRandom.h"
#include "math/mathIO.h"
#include "terrain/terrData.h"
#include "game/gameConnection.h"
#include "console/simBase.h"
#include "sceneGraph/sceneGraph.h"
#include "forest/forestClearing.h"

//------------------------------------------------------------------------------
//
//  Put this in /example/common/editor/EditorGui.cs in [function Creator::init( %this )]
//  (around line 1149)
//
//   %Environment_Item[12] = "ForestClearing";  <-- ADD THIS.
//
//------------------------------------------------------------------------------
//
//  Put the function in /example/common/editor/ObjectBuilderGui.gui [around line 458] ...
//
//  function ObjectBuilderGui::buildForestClearing(%this)
//  {
//      %this.className = "ForestClearing";
//      %this.process();
//  }
//
//------------------------------------------------------------------------------

extern bool gEditingMission;
ForestClearing * ForestClearing::smClearingList = NULL;

//------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(ForestClearing);

ForestClearing::ForestClearing()
{
    // Setup NetObject.
    mTypeMask |= StaticObjectType | StaticTSObjectType | StaticRenderedObjectType;
    mAddedToScene = false;
    mNetFlags.set(Ghostable | ScopeAlways);

    // Reset Creation Area Angle Animation.
    mCreationAreaAngle = 0;
}

//------------------------------------------------------------------------------

ForestClearing::~ForestClearing()
{
   // Remove ourselves from the clearing list... Just to be sure.
   ForestClearing **walk = &smClearingList;
   while(*walk)
   {
      if(*walk==this)
      {
         *walk = this->mNext;
         break;
      }

      walk = &((*walk)->mNext);
   }
}

//------------------------------------------------------------------------------

void ForestClearing::initPersistFields()
{
    // Initialise parents' persistent fields.
    Parent::initPersistFields();

    // Add out own persistent fields.
    addGroup( "Debugging" );
    addField( "ShowPlacementArea",  TypeBool,       Offset( mFieldData.mShowPlacementArea,      ForestClearing ) );
    addField( "PlacementAreaHeight",TypeS32,        Offset( mFieldData.mPlacementBandHeight,    ForestClearing ) );
    addField( "PlacementColour",    TypeColorF,     Offset( mFieldData.mPlaceAreaColour,        ForestClearing ) );
    endGroup( "Debugging" );

    addGroup( "Placement Radius" );
    addField( "InnerRadiusX",       TypeS32,        Offset( mFieldData.mInnerRadiusX,           ForestClearing ) );
    addField( "InnerRadiusY",       TypeS32,        Offset( mFieldData.mInnerRadiusY,           ForestClearing ) );
    addField( "OuterRadiusX",       TypeS32,        Offset( mFieldData.mOuterRadiusX,           ForestClearing ) );
    addField( "OuterRadiusY",       TypeS32,        Offset( mFieldData.mOuterRadiusY,           ForestClearing ) );
    endGroup( "Placement Radius" );

}

//------------------------------------------------------------------------------

bool ForestClearing::onAdd()
{
    if(!Parent::onAdd())
       return(false);

    // Set Default Object Box.
    mObjBox.min.set( -0.5, -0.5, -0.5 );
    mObjBox.max.set(  0.5,  0.5,  0.5 );

    // Reset the World Box.
    resetWorldBox();

    // Are we editing the Mission?
    if(gEditingMission)
    {
        // Yes, so set the Render Transform.
        setRenderTransform(mObjToWorld);
        setScale(mObjScale);

        // Add to Scene.
        addToScene();
        mAddedToScene = true;
    }

    // Add ourselves to the list of forest clearings.
    mNext = smClearingList;
    smClearingList = this;

    // Return OK.
    return(true);
}

//------------------------------------------------------------------------------

void ForestClearing::onRemove()
{
    // Are we editing the Mission?
    if(gEditingMission)
    {
        // Yes, so remove from Scene.
        removeFromScene();
        mAddedToScene = false;
    }

    // Remove ourselves from the clearing list...
    ForestClearing **walk = &smClearingList;
    while(*walk)
    {
       if(*walk==this)
       {
          *walk = this->mNext;
          break;
       }

       walk = &((*walk)->mNext);
    }

    // Do Parent.
    Parent::onRemove();
}

//------------------------------------------------------------------------------

void ForestClearing::inspectPostApply()
{
    // Set Parent.
    Parent::inspectPostApply();
}

//------------------------------------------------------------------------------

void ForestClearing::onEditorEnable()
{
    // Are we in the Scene?
    if(!mAddedToScene)
    {
        // No, so add to scene.
        addToScene();
        mAddedToScene = true;
    }
}

//------------------------------------------------------------------------------

void ForestClearing::onEditorDisable()
{
    // Are we in the Scene?
    if(mAddedToScene)
    {
        // Yes, so remove from scene.
        removeFromScene();
        mAddedToScene = false;
    }
}

//------------------------------------------------------------------------------

bool ForestClearing::prepRenderImage(SceneState* state, const U32 stateKey, const U32 startZone,
                                const bool modifyBaseZoneState)
{
    // Return if last state.
    if (isLastState(state, stateKey))
       return false;

    // Set Last State.
    setLastState(state, stateKey);

   // Is Object Rendered?
   if (state->isObjectRendered(this))
   {
        // Yes, so get a SceneRenderImage.
        SceneRenderImage* image = new SceneRenderImage;
        // Populate it.
        image->obj = this;
        image->sortType = SceneRenderImage::Normal;
        // Insert it into the scene images.
        state->insertRenderImage(image);
   }

   return false;
}

//------------------------------------------------------------------------------

void ForestClearing::renderObject(SceneState* state, SceneRenderImage*)
{
    // Return if placement area not needed.
    if (!mFieldData.mShowPlacementArea)
       return;

    // Check we are in Canonical State.
    AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

    // Setup out the Projection Matrix/Viewport.
    RectI viewport;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    dglGetViewport(&viewport);
    state->setupBaseProjection();

    // Setup our rendering state.
    glPushMatrix();
    dglMultMatrix(&getTransform());
    glScalef(mObjScale.x, mObjScale.y, mObjScale.z);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Do we need to draw the Inner Radius?
    if (mFieldData.mInnerRadiusX || mFieldData.mInnerRadiusY)
    {
        // Yes, so draw Inner Radius.
        glBegin(GL_TRIANGLE_STRIP);
        for (U32 Angle = mCreationAreaAngle; Angle < (mCreationAreaAngle+360); Angle++)
        {
            F32     XPos, YPos;

            // Calculate Position.
            XPos = mFieldData.mInnerRadiusX * mCos(mDegToRad(-(F32)Angle));
            YPos = mFieldData.mInnerRadiusY * mSin(mDegToRad(-(F32)Angle));

            // Set Colour.
            glColor4f(  mFieldData.mPlaceAreaColour.red,
                        mFieldData.mPlaceAreaColour.green,
                        mFieldData.mPlaceAreaColour.blue,
                        AREA_ANIMATION_ARC * (Angle-mCreationAreaAngle));

            // Draw Arc Line.
            glVertex3f( XPos, YPos, -(F32)mFieldData.mPlacementBandHeight/2.0f);
            glVertex3f( XPos, YPos, +(F32)mFieldData.mPlacementBandHeight/2.0f);

        }
        glEnd();
    }

    // Do we need to draw the Outer Radius?
    if (mFieldData.mOuterRadiusX || mFieldData.mOuterRadiusY)
    {
        // Yes, so draw Outer Radius.
        glBegin(GL_TRIANGLE_STRIP);
        for (U32 Angle = mCreationAreaAngle; Angle < (mCreationAreaAngle+360); Angle++)
        {
            F32     XPos, YPos;

            // Calculate Position.
            XPos = mFieldData.mOuterRadiusX * mCos(mDegToRad(-(F32)Angle));
            YPos = mFieldData.mOuterRadiusY * mSin(mDegToRad(-(F32)Angle));

            // Set Colour.
            glColor4f(  mFieldData.mPlaceAreaColour.red,
                        mFieldData.mPlaceAreaColour.green,
                        mFieldData.mPlaceAreaColour.blue,
                        AREA_ANIMATION_ARC * (Angle-mCreationAreaAngle));

            // Draw Arc Line.
            glVertex3f( XPos, YPos, -(F32)mFieldData.mPlacementBandHeight/2.0f);
            glVertex3f( XPos, YPos, +(F32)mFieldData.mPlacementBandHeight/2.0f);

        }
        glEnd();
    }

    // Restore rendering state.
    glDisable(GL_BLEND);
    glPopMatrix();

    // Animate Area Selection.
    mCreationAreaAngle += 10;
    mCreationAreaAngle = mCreationAreaAngle % 360;

    // Restore out nice and friendly canonical state.
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    dglSetViewport(viewport);

    // Check we have restored Canonical State.
    AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//------------------------------------------------------------------------------

U32 ForestClearing::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
{
   // Pack Parent.
   U32 retMask = Parent::packUpdate(con, mask, stream);

   stream->writeAffineTransform(mObjToWorld);                     // Clearing Position.
   stream->write(mObjScale.x);
   stream->write(mObjScale.y);
   stream->write(mObjScale.z);
   stream->writeInt(mFieldData.mInnerRadiusX, 32);                // Shapes Inner Radius X.
   stream->writeInt(mFieldData.mInnerRadiusY, 32);                // Shapes Inner Radius Y.
   stream->writeInt(mFieldData.mOuterRadiusX, 32);                // Shapes Outer Radius X.
   stream->writeInt(mFieldData.mOuterRadiusY, 32);                // Shapes Outer Radius Y.
   stream->writeSignedInt(mFieldData.mOffsetZ, 32);               // Shapes Offset Z.
   stream->writeFlag(mFieldData.mShowPlacementArea);              // Show Placement Area Flag.
   stream->writeInt(mFieldData.mPlacementBandHeight, 32);         // Placement Area Height.
   stream->write(mFieldData.mPlaceAreaColour);                    // Placement area color.

   // We're done ...
   return(retMask);
}

//------------------------------------------------------------------------------

void ForestClearing::unpackUpdate(NetConnection * con, BitStream * stream)
{
   // Unpack Parent.
   Parent::unpackUpdate(con, stream);

   MatrixF     clearingObjectMatrix;
   Point3F     scale;

   stream->readAffineTransform(&clearingObjectMatrix);                 // Clearing Position.
   stream->read(&scale.x);
   stream->read(&scale.y);
   stream->read(&scale.z);
   mFieldData.mInnerRadiusX            = stream->readInt(32);          // Shapes Inner Radius X.
   mFieldData.mInnerRadiusY            = stream->readInt(32);          // Shapes Inner Radius Y.
   mFieldData.mOuterRadiusX            = stream->readInt(32);          // Shapes Outer Radius X.
   mFieldData.mOuterRadiusY            = stream->readInt(32);          // Shapes Outer Radius Y.
   mFieldData.mOffsetZ                 = stream->readSignedInt(32);    // Shapes Offset Z.
   mFieldData.mShowPlacementArea       = stream->readFlag();           // Show Placement Area Flag.
   mFieldData.mPlacementBandHeight     = stream->readInt(32);          // Placement Area Height.
   stream->read(&mFieldData.mPlaceAreaColour);                         // Placement area color.

   // Set Transform.
   setTransform(clearingObjectMatrix);
   setScale(scale);
}

const bool ForestClearing::contains(const Point3F pos)
{
   // crazy ellipse equation:
   // [(x - xo) / a]2 + [(y - yo) / b]2 + [(z - zo) / c]2 = r2

   // Transform Point3F into our local object space
   Point3F localPos;
   
   mWorldToObj.mulP(pos, &localPos);
   localPos.convolveInverse(mObjScale);

   // Then check to see if it's within the radius (ignore Z for now)
   if( mPow((localPos.x / mFieldData.mOuterRadiusX), 2) + mPow((localPos.y / mFieldData.mOuterRadiusY), 2) < 1.f)
      return true;

   return false;
}