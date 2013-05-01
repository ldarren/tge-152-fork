#include "sceneGraph/sceneGraph.h"
#include "game/gameConnection.h"
#include "console/consoleTypes.h"
#include "terrain/sky.h"
#include "terrain/terrData.h"
#include "terrain/terrRender.h"
#include "terrain/waterBlock.h"

#include "gui/core/guiTSControl.h"

extern bool gRenderingMap;

class rpgMapViewport : public GuiTSCtrl
{
public:
   typedef GuiTSCtrl Parent;

   Point3F           mRotateView;              // View Rotation.
   Point3F           mOffsetView;              // Offset Distance.
   F32               mFov;                     // Field of View.
   Point3F           mSweepAmplitude;          // Sweep Amplitude.
   Point3F           mSweepTime;               // Sweep Time.

   bool              mUseOverlayBitmap;        // Use Overlay Bitmap Flag.
   bool              mUseOverlayColour;        // Use Overlay Colour Flag.
   bool              mOverlayTile;             // Overlay Tile Flag.
   ColorF            mOverlayColor;            // Filter Colour Vector.
   bool              mOverlayRedMask;          // Overlay Red Mask Flag.
   bool              mOverlayGreenMask;        // Overlay Green Mask Flag.
   bool              mOverlayBlueMask;         // Overlay Blue Mask Flag.

   StringTableEntry  mObjectName;              // Attached Object Name.
   SceneObject*      mAttachedObject;          // Attached Object.

   U32               mLastTimeStamp;           // Last Time Stamp.
   Point3F           mCurrentSweepMagnitude;   // Current Sweep Phase.
   StringTableEntry  mOverlayBitmapName;       // Overlay Bitmap Name.
   TextureHandle     mOverlayTextureHandle;    // Overlay Texture Handle.
   TextureHandle     mArrowTextureHandle;

   U32               mZoomLevel;

   F32               mCameraAngle;

   void renderWorld(const RectI & updateRect);
   void onRender(Point2I offset, const RectI &updateRect);

public:
   rpgMapViewport();

   static void initPersistFields();
   static void consoleInit();

   bool processCameraQuery(CameraQuery * query);

   void setViewObject(const char* ObjectName);
   void setViewRotation(Point3F Rotation);
   void setOverlayBitmap(const char *name);
   void setOverlayColor(ColorF OverlayColor);
   void setOverlayMask(bool RedMask, bool GreenMask, bool BlueMask);
   bool onMouseWheelUp(const GuiEvent &event);
   bool onMouseWheelDown(const GuiEvent &event);

   bool onWake();
   void onSleep();

   struct contact
   {
      StringTableEntry name;
      Point3F          pos;
      U32              type;
   };

   Vector<contact> mContacts;

   DECLARE_CONOBJECT(rpgMapViewport);
};

//------------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(rpgMapViewport);

//------------------------------------------------------------------------------


rpgMapViewport::rpgMapViewport() :
   mRotateView(0,0,0),
   mOffsetView(0,0,0),
   mFov(60.0),
   mSweepAmplitude(0,0,60),
   mSweepTime(5000,5000,5000),
   mCurrentSweepMagnitude(0,0,0),
   mAttachedObject(NULL),
   mOverlayRedMask(GL_TRUE),
   mOverlayGreenMask(GL_TRUE),
   mOverlayBlueMask(GL_TRUE),
   mOverlayTile(false),
   mUseOverlayBitmap(false),
   mUseOverlayColour(false),
   mLastTimeStamp(Platform::getRealMilliseconds())
{
   // Create Empty Attached Object Name.
   mObjectName = StringTable->insert("");

   // Create Empty Overlay Bitmap Name.
   mOverlayBitmapName = StringTable->insert("");

   // Set Default Overlay Colour.
   mOverlayColor.set(1, 1, 1, 0.5f);

   mZoomLevel = 4;
};

//------------------------------------------------------------------------------

void rpgMapViewport::initPersistFields()
{
   // Initialise parents' persistent fields.
   Parent::initPersistFields();

   // Add out own persistent fields.
   addField( "ViewRotation",   TypePoint3F,  Offset( mRotateView,        rpgMapViewport ) );
   addField( "ViewOffset",     TypePoint3F,  Offset( mOffsetView,        rpgMapViewport ) );
   addField( "FOV",            TypeF32,      Offset( mFov,               rpgMapViewport ) );
   addField( "SweepAmplitude", TypePoint3F,  Offset( mSweepAmplitude,    rpgMapViewport ) );
   addField( "SweepTime",      TypePoint3F,  Offset( mSweepTime,         rpgMapViewport ) );
   addField( "AttachedObject", TypeString,   Offset( mObjectName,        rpgMapViewport ) );
   addField( "OverlayBitmap",  TypeBool,     Offset( mUseOverlayBitmap,  rpgMapViewport ) );
   addField( "OverlayTile",    TypeBool,     Offset( mOverlayTile,       rpgMapViewport ) );
   addField( "OverlayColour",  TypeBool,     Offset( mUseOverlayColour,  rpgMapViewport ) );
   addField( "BitmapOverlay",  TypeFilename, Offset( mOverlayBitmapName, rpgMapViewport ) );
   addField( "ColorOverlay",   TypeColorF,   Offset( mOverlayColor,      rpgMapViewport ) );
   addField( "RedMask",        TypeBool,     Offset( mOverlayRedMask,    rpgMapViewport ) );
   addField( "GreenMask",      TypeBool,     Offset( mOverlayGreenMask,  rpgMapViewport ) );
   addField( "BlueMask",       TypeBool,     Offset( mOverlayBlueMask,   rpgMapViewport ) );
}

//------------------------------------------------------------------------------

bool rpgMapViewport::onWake()
{
   // Wake-up Parent.
   if (!Parent::onWake()) return false;

   // Set Active.
   setActive(true);

   // Get Game Connection.
   GameConnection* pConnection = dynamic_cast<GameConnection *>(NetConnection::getConnectionToServer());
   if (pConnection)
      mAttachedObject = pConnection->getControlObject();
   else
      mAttachedObject = NULL;

   // Set Overlay Bitmap.
   setOverlayBitmap(mOverlayBitmapName);

   //fix me
   char s[512];
   dSprintf(s,512,"%s/data/ui/spellicons/mercury",Con::getVariable("$defaultGame"));
   mArrowTextureHandle = TextureHandle(s, BitmapTexture, true);

   // Return OK.
   return true;
}

//------------------------------------------------------------------------------

void rpgMapViewport::onSleep()
{
   // Reset Overlay Texture Handle.
   mOverlayTextureHandle = NULL;
   mArrowTextureHandle = NULL;
   // Call Parent.
   Parent::onSleep();
}

//------------------------------------------------------------------------------

void rpgMapViewport::setViewRotation(Point3F Rotation)
{
   // Set the Rotation internally.
   mRotateView = Rotation;
}

//------------------------------------------------------------------------------

void rpgMapViewport::setViewObject(const char* ObjectName)
{
}

//------------------------------------------------------------------------------

void rpgMapViewport::setOverlayBitmap(const char *name)
{
   // Set Overlay Bitmap Name.
   mOverlayBitmapName = StringTable->insert(name);

   // Have we got an Overlay Name?
   if (*mOverlayBitmapName)
      // Yes, so get Texture Handle.
      mOverlayTextureHandle = TextureHandle(mOverlayBitmapName, BitmapTexture, true);
   else
      // No, so reset Texture Handle.
      mOverlayTextureHandle = NULL;

   // Update.
   setUpdate();
}   

//------------------------------------------------------------------------------

void rpgMapViewport::setOverlayColor(ColorF OverlayColor)
{
   // Set the Overlay Colour internally.
   mOverlayColor = OverlayColor;
}

//------------------------------------------------------------------------------

void rpgMapViewport::setOverlayMask(bool RedMask, bool GreenMask, bool BlueMask)
{
   // Set the Overlay Masks internally.
   mOverlayRedMask      = RedMask;
   mOverlayGreenMask   = GreenMask;
   mOverlayBlueMask   = BlueMask;
}

//------------------------------------------------------------------------------

ConsoleMethod(rpgMapViewport,setViewRotation,void,5,5,"Sets View Rotation.")
{
   Point3F      Rotation;

   // Fetch the rpgMapViewport object.
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   // Fetch Rotation.
   Rotation.set(mDegToRad(dAtof(argv[2])),
                mDegToRad(dAtof(argv[3])),
                mDegToRad(dAtof(argv[4])));

   // Set Rotation.
   Viewport->setViewRotation(Rotation);
}

ConsoleMethod(rpgMapViewport,zoomIn,void,2,2,"Sets View Rotation.")
{
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   if (!Viewport->mZoomLevel)
      return;

   Viewport->mZoomLevel--;

}
ConsoleMethod(rpgMapViewport,zoomOut,void,2,2,"Sets View Rotation.")
{
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   if (Viewport->mZoomLevel < 6)
      Viewport->mZoomLevel++;

}

ConsoleMethod(rpgMapViewport,clearContacts,void,2,2,"Clears contact list.")
{
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   Viewport->mContacts.clear();

}

ConsoleMethod(rpgMapViewport,addContact,void,7,7,"name,type,x,y,z")
{
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   rpgMapViewport::contact c;

   c.name = StringTable->insert(argv[2],true);
   c.type = U32(dAtoi(argv[3]));
   c.pos.x = dAtof(argv[4]);
   c.pos.y = dAtof(argv[5]);
   c.pos.z = dAtof(argv[6]);

   //dPrintf("%s\n",c.name);

   Viewport->mContacts.push_back(c);
  
}

//------------------------------------------------------------------------------

ConsoleMethod(rpgMapViewport,setViewObject,void,3,3,"Sets View to Object.")
{
   F32      Rotation;

   // Fetch the rpgMapViewport object.
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   // Set the GuiFilter Filter Colour.
   Viewport->setViewObject(argv[2]);
}

//------------------------------------------------------------------------------

ConsoleMethod(rpgMapViewport,setOverlayBitmap,void,3,3,"Sets Overlay Bitmap.")
{
   // Fetch the rpgMapViewport object.
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   // Set Overlay Bitmap.
   Viewport->setOverlayBitmap(argv[2]);
}

//------------------------------------------------------------------------------

ConsoleMethod(rpgMapViewport,setOverlayColor,void,5,6,"Sets Overlay Color.")
{
   F32      r,g,b,a;
   ColorF   TempColor;

   // Fetch the rpgMapViewport object.
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   // Convert RGB Ascii parms to float.
   r = dAtof(argv[2]);
   g = dAtof(argv[3]);
   b = dAtof(argv[4]);

   // Did we get an alpha param?
   if (argc == 6)
      // Yep, so convert it.
      a = dAtof(argv[5]);
   else
      // Nope, so default to 1.
      a = 1;

   // Setup our temporary colour vector.
   TempColor.set(r,g,b,a);

   // Set the rpgMapViewport Overlay Colour.
   Viewport->setOverlayColor(TempColor);
}

//------------------------------------------------------------------------------

ConsoleMethod(rpgMapViewport,setOverlayMask,void,5,5,"Sets Overlay Masks.")
{
   F32      r,g,b;

   // Fetch the rpgMapViewport object.
   rpgMapViewport *Viewport = static_cast<rpgMapViewport*>(object);

   // Convert RGB Ascii parms to float.
   r = dAtof(argv[2]);
   g = dAtof(argv[3]);
   b = dAtof(argv[4]);

   // Set the rpgMapViewport Masks.
   Viewport->setOverlayMask(r,g,b);
}

//------------------------------------------------------------------------------

static void cfxGuiBitmapSetOverlayBitmap(SimObject *obj, S32, const char **argv)
{
   // Fetch HUD Control.
   rpgMapViewport *ctrl = static_cast<rpgMapViewport*>(obj);

   // Set Overlay Bitmap.
   ctrl->setOverlayBitmap(argv[2]);
}

//------------------------------------------------------------------------------

void rpgMapViewport::consoleInit()
{
   Con::addCommand("rpgMapViewport", "setOverlayBitmap",  cfxGuiBitmapSetOverlayBitmap, "rpgMapViewport.setOverlayBitmap(Bitmap)", 3, 3);
}

//------------------------------------------------------------------------------

void rpgMapViewport::renderWorld(const RectI & updateRect)
{
#ifdef DARREN_MMO
   SceneObject* terrain = gClientSceneGraph->getCurrentTerrain();


   if (!terrain)
   {
      gRenderingMap = false;
      return;
   }

   gRenderingMap = true;
   // Set-up OpenGL for a scene render.
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glClear(GL_DEPTH_BUFFER_BIT);
   glDisable(GL_CULL_FACE);
   glMatrixMode(GL_MODELVIEW);
   dglSetCanonicalState();

   Sky* sky = gClientSceneGraph->getCurrentSky();

   
   U32 mask = TerrainObjectType|InteriorObjectType|WaterObjectType|StaticShapeObjectType;
   if (mZoomLevel <= 1)
      mask |= StaticTSObjectType;
   if (sky)
      gClientSceneGraph->setFogDistance(10000.0f);
   gClientSceneGraph->renderScene(mask);
   if (sky)
      gClientSceneGraph->setFogDistance(sky->getFogDistance());


   // Disable Depth Test.
   glDisable(GL_DEPTH_TEST);

   dglSetClipRect(updateRect);

   S32 i;
   Point3F projPnt;
   Point2I offset;
   ColorF tcolor(255,255,255,255);

   dglSetBitmapModulation(tcolor);
   RectI srcRegion;
   S32 size = 24;
   Point2I half( ( mBounds.extent.x >> 1)  - 1, ( mBounds.extent.y >> 1 ) - 1 );
   Point2I texSize(24, 24 );
   Point2I center(half.x-size/2, half.y-size/2); 
   srcRegion.set(0,0,size,size); 
   center = localToGlobalCoord(center);
   RectI rect(center, texSize);
   dglDrawBitmapRotated(mArrowTextureHandle, rect, srcRegion, false, mCameraAngle+180);

   gRenderingMap = false;

/*
#normal mob
type = 0
if m.spawn.isMonster:
type = 1
if m.vendor:
type = 2
if m.dialog and m.dialog.greeting and not len(m.dialog.greeting.choices):
type = 3
if m.player:
type = 4
*/

   ColorF colors[] =
   {
      ColorF(1,1,1,255), //normal
      ColorF(.95f,.36f,.36f,255), //monster
      ColorF(.36f,.76f,.83f,255), //vendor
      ColorF(.95f,.79f,.36f,255), //quest
      ColorF(.36f,.75f,.95f,255), //player
      ColorF(.52f,.76f,.36f,255), //inn
      ColorF(1.0f,.60f,1.0f,255), //point of interest
   };

   bool splayers = Con::getBoolVariable("$Py::RPG::ShowPlayers");
   bool snpcs = Con::getBoolVariable("$Py::RPG::ShowNPCS");
   bool senemies = Con::getBoolVariable("$Py::RPG::ShowEnemies");
   bool spoi = Con::getBoolVariable("$Py::RPG::ShowPoints");

   for (i = 0; i < mContacts.size(); i++)
   {
      contact* c = &mContacts[i];

      if (!snpcs)
         if (c->type == 0 || c->type == 2 || c->type == 3 || c->type == 5)
            continue;

      if (!splayers)
         if (c->type == 4)
            continue;

      if (!senemies)
         if (c->type == 1)
            continue;

      if (!spoi)
         if (c->type == 6)
            continue;


      if (!project(c->pos, &projPnt))
         continue;

      
      offset.x = (S32)projPnt.x;
      offset.y = (S32)projPnt.y;

      // Center the name
      offset.x -= mProfile->mFont->getStrWidth(c->name) / 2;
      offset.y -= mProfile->mFont->getHeight();

      dglSetBitmapModulation(ColorF(0.f,0.f,0.f));

      offset.x += 1;
      offset.y += 1;

      dglDrawText(mProfile->mFont, offset, c->name);

      offset.x -= 1;
      offset.y -= 1;


      dglSetBitmapModulation(colors[c->type]);
      //dglDraw2DSquare(Point2F(offset.x,offset.y),16,0);


      dglDrawText(mProfile->mFont, offset, c->name);
   }

   dglClearBitmapModulation();
#endif //DARREN_MMO
}

//------------------------------------------------------------------------------

void rpgMapViewport::onRender(Point2I offset, const RectI &updateRect)
{
   // Call Parent Render.
   Parent::onRender(offset, updateRect);

   // Set Clipping Rectangle to GUI Bounds.
   dglSetClipRect(mBounds);

   // Do we have an attached Object?
   if (!mAttachedObject)
   {
      // No, so signal to user this problem ...
      ColorF ErrorColor(1,0,0);
      dglDrawRectFill(updateRect, ErrorColor);
      ErrorColor.set(1,1,1);
      char buf[256];
      dSprintf(buf, sizeof(buf), "*** Object not selected ***");
      dglSetBitmapModulation(ErrorColor);
      dglDrawText(mProfile->mFont, offset, buf);
      dglClearBitmapModulation();

      // Return Error.
      return;
   }

   // Are we using the Overlay Bitmap?
   if (mUseOverlayBitmap)
   {
      // Yes, so do we have a texture Handle?
      if (mOverlayTextureHandle)
      {
         // Yes, so clear Bitmap Modulation.
         dglClearBitmapModulation();

         // Are we tiling the Overlay Bitmap?
         if(mOverlayTile)
         {
            RectI SrcRegion;
            RectI DstRegion;

            // Yes, so fetch texture object.
             TextureObject* TextureObj = (TextureObject *)mOverlayTextureHandle;

            // Calculate Tesselation Count.
            float XTess = ((float)mBounds.extent.x / (float)TextureObj->bitmapWidth) + 1;
            float YTess = ((float)mBounds.extent.y / (float)TextureObj->bitmapHeight) + 1;

            for(int y = 0; y < YTess; ++y)
            {
               for(int x = 0; x < XTess; ++x)
               {
                  // Calculate Source Region.
                  SrcRegion.set(0,0,TextureObj->bitmapWidth, TextureObj->bitmapHeight);

                  // Calculate Destination Region.
                  DstRegion.set(((TextureObj->bitmapWidth*x)+offset.x),
                             ((TextureObj->bitmapHeight*y)+offset.y),
                             TextureObj->bitmapWidth,   
                             TextureObj->bitmapHeight);

                  // Draw Tiled Bitmap.
                  dglDrawBitmapStretchSR(TextureObj, DstRegion, SrcRegion, false);
               }
            }
         }
         else
         {
            // No, so draw stretched Bitmap.
            dglDrawBitmapStretch(mOverlayTextureHandle, mBounds);
         }
      }
   }

   // Are we using the Overlay Colour?
   if (mUseOverlayColour)
   {
      // Set Colour Mask.
      glColorMask(mOverlayRedMask, mOverlayGreenMask, mOverlayBlueMask, GL_TRUE);

      // Draw our filled rectangle with the Filter Colour.
      dglDrawRectFill(updateRect, mOverlayColor);

      // Reset the Colour Mask.
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
   }
}

//------------------------------------------------------------------------------

bool rpgMapViewport::processCameraQuery(CameraQuery * query)
{
   Point3F    CameraRotation;                     // Rotated View.
   float      VisibleDistance = 1100.0f;          // Visible Distance.


   // Get Game Connection.
   GameConnection* pConnection = dynamic_cast<GameConnection *>(NetConnection::getConnectionToServer());

   
   // Did we get the connection?
   if (pConnection)
   {
      
      // Have we got an Attached Object?
      if (mAttachedObject && gClientSceneGraph)
      {

         // Create Camera Matrix.
         MatrixF   Camera(true);

         // Craete Rotation Quaternion.
         AngAxisF   axis(Point3F(-1,0,0),-1.57f);
         axis.setMatrix(&Camera);
         // Set Position @ Attached Object.

         S32 zoom[] = {
         100,200,400,500,600,700,800
         };

         F32 z = zoom[mZoomLevel];
         if (z > gClientSceneGraph->getVisibleDistance()-75.f)
            z = gClientSceneGraph->getVisibleDistance()-75.f;


         Camera.setColumn(3, mAttachedObject->getBoxCenter() + Point3F(0,0,z));

         // Set Camera Matrix.
         query->cameraMatrix = Camera;

         // Set Near/Far Planes.
         query->nearPlane = 0.1;
         query->farPlane = getMax(VisibleDistance, 50.f);

         // Set FOV.
         query->fov = mDegToRad(mFov);

         query->ortho = false;

         MatrixF cam;
         Point3F camRot;
         Point3F camCoord;


         pConnection->getControlCameraTransform(0,&cam);   // store camera information
         cam.getColumn(3, &camCoord);            // get camera position
         cam.getColumn(1,&camRot);               // get camera orientation

         // get angle that camera is facing
         camRot.z = 0.0;
         if( camRot.len() == 0.0f )
            mCameraAngle = 0;
         else
         {
            camRot /= camRot.len();
            mCameraAngle = mAtan((-1.0F * camRot.x),(1.0F * camRot.y)) * (180.0F / M_PI);
         }

         // Return OK.
         return(true);
      }
   }

   // Return Error.
   return(false);
}

bool rpgMapViewport::onMouseWheelUp(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   if (mZoomLevel < 6)
      mZoomLevel++;


   return( true );
}

bool rpgMapViewport::onMouseWheelDown(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

   if (mZoomLevel)
      mZoomLevel--;


   return( true );
}




