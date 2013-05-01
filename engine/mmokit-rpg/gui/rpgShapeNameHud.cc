//-----------------------------------------------------------------------------
// MMO Kit variant of the original GuiShapeNameHud defined in
// game/fps/guiShapeNameHud.cc.
//
// The rpgShapeNameHud takes advantage of the rpgPlayer class to show additional
// information for player objects like realm, alliance, relative encounter setting,
// if a mob has a dialog or a special modifier, the master of a pet or if the
// target is hostile. Various preferences get queried so the player can choose
// whose player (or AI) objects names should be displayed.
//-----------------------------------------------------------------------------

#include "dgl/dgl.h"
#include "dgl/gFont.h"
#include "gui/core/guiControl.h"
#include "gui/core/guiTSControl.h"
#include "console/consoleTypes.h"
#include "sceneGraph/sceneGraph.h"
#include "game/shapeBase.h"
#include "game/gameConnection.h"
#include "rpg/game/rpgPlayer.h"


// Color definitions to color code different player objects.
static const ColorF   rpgShapeNameHud_playerFoLColor(.35f,1.f,.35f);
static const ColorF   rpgShapeNameHud_playerMoDColor(1.f,.5f,1.f);
static const ColorF   rpgShapeNameHud_playerMonsterColor(.85f,.5f,.33f);
static const ColorF   rpgShapeNameHud_monsterColor(.83f,.70f,.33f);
static const ColorF   rpgShapeNameHud_uniqueMonsterColor(.4f,.85f,1.f);
static const ColorF   rpgShapeNameHud_dialogColor(.88f,.79f,.42f);
static const ColorF   rpgShapeNameHud_diffColor(.97f,.51f,0.f);


//----------------------------------------------------------------------------
/// Displays name and additional information above shape objects.
///
/// This control displays the name and additional information (see top comment)
/// of all named ShapeBase objects on the client. The information on objects
/// within the control's display area are overlayed above the object.
///
/// This GUI control must be a child of a TSControl, and a server connection
/// and control object must be present.
///
/// This is a stand-alone control and relies only on the standard base GuiControl.
class rpgShapeNameHud : public GuiControl
{
   typedef GuiControl Parent;

   // field data
   ColorF   mFillColor;
   ColorF   mFrameColor;
   ColorF   mTextColor;
   ColorF   mSelectedTextColor;

   F32      mVerticalOffset;
   F32      mDistanceFade;
   bool     mShowFrame;
   bool     mShowFill;
   
   Resource<GFont> mAllianceFont;
   
   bool onInputEvent(const InputEvent &evt){return getParent()->onInputEvent(evt);}
   
   bool pointInControl(const Point2I& parentCoordPoint){return false;}
   bool cursorInControl(){return false;}

protected:
   StringTableEntry     mHostilePlayerBitmap;
   TextureHandle        mHostilePlayerTex;
   StringTableEntry     mFriendlyPlayerBitmap;
   TextureHandle        mFriendlyPlayerTex;
   
   void drawName(Point2I offset, const char *buf, F32 opacity, bool selected, bool player,
                    bool myself, bool hostile, S8 encounterSuperior, S32 realm, bool grave);

public:
   rpgShapeNameHud();

   // GuiControl
   virtual void onRender(Point2I offset, const RectI &updateRect);

   static void initPersistFields();
   DECLARE_CONOBJECT( rpgShapeNameHud );
};


//-----------------------------------------------------------------------------

IMPLEMENT_CONOBJECT(rpgShapeNameHud);

/// Default distance for object's information to be displayed.
static const F32 cDefaultVisibleDistance = 500.0f;

rpgShapeNameHud::rpgShapeNameHud()
{
   mFillColor.set( 0.25, 0.25, 0.25, 0.25 );
   mFrameColor.set( 0, 1, 0, 1 );
   mTextColor.set( 0, 1, 0, 1 );
   mSelectedTextColor.set( 1, 0, 0, 1 );
   mShowFrame = mShowFill = true;
   
   mHostilePlayerBitmap = StringTable->insert("");
   mFriendlyPlayerBitmap = StringTable->insert("");
   mHostilePlayerTex = mFriendlyPlayerTex = NULL;
   
   mVerticalOffset = 0.5;
   mDistanceFade = 0.1;
   
   mAllianceFont = GFont::create(StringTable->insert("Arial"), 16, Con::getVariable("$GUI::fontCacheDirectory"));
   if (mAllianceFont.isNull())
      Con::errorf("Failed to load/create alliance hud font\n");
}

void rpgShapeNameHud::initPersistFields()
{
   Parent::initPersistFields();
   addGroup("Colors");
   addField( "fillColor",  TypeColorF, Offset( mFillColor,  rpgShapeNameHud ) );
   addField( "frameColor", TypeColorF, Offset( mFrameColor, rpgShapeNameHud ) );
   addField( "textColor",  TypeColorF, Offset( mTextColor,  rpgShapeNameHud ) );
   endGroup("Colors");

   addGroup("Misc");
   addField( "showFill",             TypeBool,     Offset( mShowFill,             rpgShapeNameHud ) );
   addField( "showFrame",            TypeBool,     Offset( mShowFrame,            rpgShapeNameHud ) );
   addField( "verticalOffset",       TypeF32,      Offset( mVerticalOffset,       rpgShapeNameHud ) );
   addField( "distanceFade",         TypeF32,      Offset( mDistanceFade,         rpgShapeNameHud ) );
   addField( "hostilePlayerBitmap",  TypeFilename, Offset( mHostilePlayerBitmap,  rpgShapeNameHud ) );
   addField( "friendlyPlayerBitmap", TypeFilename, Offset( mFriendlyPlayerBitmap, rpgShapeNameHud ) );
   endGroup("Misc");
}


//----------------------------------------------------------------------------
/// Core rendering method for this control.
///
/// This method scans through all the current client ShapeBase objects.
/// If one is named, it displays the name information for it.
///
/// Information is offset from the center of the object's bounding box,
/// unless the object is a PlayerObjectType, in which case the eye point
/// is used.
///
/// @param   updateRect   Extents of control.
void rpgShapeNameHud::onRender( Point2I, const RectI &updateRect)
{
#ifdef DARREN_MMO
   if(*mHostilePlayerBitmap && !mHostilePlayerTex)
      mHostilePlayerTex = TextureHandle(mHostilePlayerBitmap, BitmapTexture, true);
   if(*mFriendlyPlayerBitmap && !mFriendlyPlayerTex)
      mFriendlyPlayerTex = TextureHandle(mFriendlyPlayerBitmap, BitmapTexture, true);
   
   bool selected = false, myself = false;
   SimObjectId selectedId = 0;
   
   // Background fill first
   if (mShowFill)
      dglDrawRectFill(updateRect, mFillColor);

   // Must be in a TS Control
   GuiTSCtrl *parent = dynamic_cast<GuiTSCtrl*>(getParent());
   if (!parent) return;

   // Must have a connection and control object
   GameConnection* conn = GameConnection::getConnectionToServer();
   if (!conn)
      return;

   ShapeBase* control = conn->getControlObject();
   if (!control)
      return;
   
   // Get player information for realm/hostility checks
   rpgPlayer *selfPlayer = dynamic_cast<rpgPlayer*>(control), *otherPlayer;
   
   U8 charIndex = 0;
   StringTableEntry PyCharacterIndex = Con::getVariable("$Py::SelectionCharacterIndex");
   if (!PyCharacterIndex[0])
      selectedId = 0;
   else
   {
      charIndex = (U8) dAtoi(PyCharacterIndex);
      selectedId = control->getSelectedObjectId(charIndex);
   }
   
   // Get control camera info
   MatrixF cam;
   Point3F camPos;
   VectorF camDir;
   conn->getControlCameraTransform(0,&cam);
   cam.getColumn(3, &camPos);
   cam.getColumn(1, &camDir);

   F32 camFov;
   conn->getControlCameraFov(&camFov);
   camFov = mDegToRad(camFov) / 2;

   // Visible distance info & name fading
   F32 visDistance = 90.0f;//gClientSceneGraph->getVisibleDistance();
   F32 visDistanceSqr = visDistance * visDistance;
   F32 fadeDistance = visDistance * mDistanceFade;

   // Collision info. We're going to be running LOS tests and we
   // don't want to collide with the control object.
   static U32 losMask = TerrainObjectType | InteriorObjectType | StaticShapeObjectType | StaticObjectType;
   control->disableCollision();

   // All ghosted objects are added to the server connection group,
   // so we can find all the shape base objects by iterating through
   // our current connection.
   for (SimSetIterator itr(conn); *itr; ++itr) 
   {
      selected = false;
      S32 realm = 0;
      
      if ((*itr)->getType() & ShapeBaseObjectType)
      {
         ShapeBase* shape = static_cast<ShapeBase*>(*itr);
         
         if (shape->getShapeName())
         {
            if (shape == control)
            {
               if(!Con::getBoolVariable("$pref::game::displayAvatarTitle") || Con::getBoolVariable("$Py::ISSINGLEPLAYER"))
                  continue;
               myself = true;
            }
            else
               myself = false;

            // Target pos to test, if it's a player run the LOS to his eye
            // point, otherwise we'll grab the generic box center.
            Point3F shapePos;
            bool grave = false;
            bool isPlayer = false;
            
            if (shape->getType() & PlayerObjectType) 
            {
               if (shape->getId() == selectedId)
                  selected = true;
               
               isPlayer = !(shape->getTypeMask()&AIObjectType || shape->getTypeMask()&CorpseObjectType);
               otherPlayer = dynamic_cast<rpgPlayer*>(shape);
               
               // Client can choose whether or not to always render targets name.
               if ( !myself && !(selected))// && Con::getBoolVariable("$pref::game::displayTargetName")))
               {
                  // If it is a player, check settings.
                  if (isPlayer)
                  {
                     // Check if client wants other player names rendered.
                     //if (!Con::getBoolVariable("$pref::game::displayPlayerNames"))
                     //   continue;
                  }
                  // It is an NPC, check settings.
                  else
                  {
                     // Check if client wants NPC names rendered.
                     //if (!Con::getBoolVariable("$pref::game::displayNpcNames"))
                     //   continue;
                  }
               }
               
               realm = otherPlayer->mRealm;
               
               MatrixF eye;

               // Use the render eye transform, otherwise we'll see jittering
               eye = shape->getRenderTransform();
               eye.getColumn(3, &shapePos);
               
               Point3F scale = shape->getScale();
               //shapePos = shape->getPosition();
               shapePos.z += 2.f * scale[2];
            }
            else 
            {
                // Use the render transform instead of the box center
                // otherwise it'll jitter.
                grave = true;
                MatrixF srtMat = shape->getRenderTransform();
                srtMat.getColumn(3, &shapePos);
                shapePos.z += .6f;
                otherPlayer = NULL;
            }

            VectorF shapeDir = shapePos - camPos;

            // Test to see if it's in range
            F32 shapeDist = shapeDir.lenSquared();
            if (shapeDist == 0 || shapeDist > visDistanceSqr)
               continue;
            shapeDist = mSqrt(shapeDist);

            // Test to see if it's within our viewcone, this test doesn't
            // actually match the viewport very well, should consider
            // projection and box test.
            shapeDir.normalize();
            F32 dot = mDot(shapeDir, camDir);
            if (dot < camFov)
               continue;

            // Test to see if it's behind something, and we want to
            // ignore anything it's mounted on when we run the LOS.
            RayInfo info;
            
            if (shape->getType()&AIObjectType)
               shape->disableCollision();
            //ShapeBase *mount = shape->getObjectMount();
            //if (mount)
            //   mount->disableCollision();
            
            bool los = !gClientContainer.castRay(camPos, shapePos,losMask, &info);
            
            if (shape->getType()&AIObjectType)
               shape->enableCollision();
            //shape->enableCollision();
            //if (mount)
            //   mount->enableCollision();

            if (!los)
               continue;

            // Project the shape pos into screen space and calculate
            // the distance opacity used to fade the labels into the
            // distance.
            Point3F projPnt;
            //shapePos.z += mVerticalOffset;
            if (!parent->project(shapePos, &projPnt))
               continue;
            F32 opacity = (shapeDist < fadeDistance)? 1.0:
               1.0 - (shapeDist - fadeDistance) / (visDistance - fadeDistance);

            char name[256];
            
            if (shape->getTypeMask()&CorpseObjectType)
               sprintf(name,"%s's Corpse",shape->getShapeName());
            else
               sprintf(name,"%s",shape->getShapeName());
            
            if(shape->getCloakLevel() != 0.0)
               opacity *= 1.0 - shape->getCloakLevel();
            
            bool isHostile = false;
            S32 relativeEncounterSetting = -1;
            
            if(!grave && selfPlayer)
            {
               isHostile = isPlayer ? selfPlayer->allowHarmful(otherPlayer) : true;
               relativeEncounterSetting = isPlayer ? otherPlayer->mEncounterSetting - selfPlayer->mEncounterSetting : -1;
            }
            
            // Render the shape's name
            drawName(Point2I((S32)projPnt.x, (S32)projPnt.y), name, opacity, selected, isPlayer,
                     myself, isHostile, relativeEncounterSetting, realm, grave);
         }
      }
   }

   // Restore control object collision
   control->enableCollision();

   // Border last
   if (mShowFrame)
      dglDrawRect(updateRect, mFrameColor);
#endif //DARREN_MMO
}


//----------------------------------------------------------------------------
/// Render object names.
///
/// Helper function for rpgShapeNameHud::onRender
///
/// @param   offset  Screen coordinates to render name label. (Text is centered
///                  horizontally about this location, with bottom of text at
///                  specified y position.)
/// @param   name    String name to display.
/// @param   opacity Opacity of name (a fraction).
/// @param   ... bleh, should be self-descriptive
void rpgShapeNameHud::drawName(Point2I offset, const char *name, F32 opacity, bool selected, bool player,
                               bool myself, bool hostile, S8 encounterSuperior, S32 realm, bool grave)
{
    // Center the name
    
    bool monster = false;
    bool unique = false;
    bool hasdialog = false;
    bool diff = false;
    
    const char *usename = name;
    char newname[256];
    char alliance[256];
    alliance[0] = 0;
    
    S32 x;
    
    switch(name[0])
    {
        case '0':
            dStrcpy(newname,&name[1]);
            hasdialog = true;
            break;
        case '1':
            dStrcpy(newname,&name[1]);
            monster = true;
            unique = true;
            break;
        case '2':
            dStrcpy(newname,&name[1]);
            monster = true;
            diff = true;
            break;
        default:
            dStrcpy(newname,name);
            break;
    }
    usename = newname;
    
    for (x = 0; x < dStrlen(newname); x++)
    {
        if (newname[x] == '<')
        {
            dSprintf(alliance,256,"<%s",&newname[x+1]);
            newname[x] = 0;
            break;
        }
    }
    
    if (dStrstr(newname,(char*)"(Monster)"))
    {
        newname[dStrlen(name)-10] = 0;
        usename = newname;
        monster = true;
    }
    
    x = offset.x;
    offset.x -= mProfile->mFont->getStrWidth((const UTF8 *)usename) / 2;
    offset.y -= mProfile->mFont->getHeight();
    
    // Deal with opacity and draw.
    ColorF shadow(0,0,0,opacity);
    
    dglSetBitmapModulation(shadow);
    dglDrawText(mProfile->mFont, offset+Point2I(1,1), usename);
    
    ColorF color, conColor;
    
    mTextColor.alpha = opacity;
    if (grave)
        color = ColorF(120,120,120);
    else if (selected)
        color = mSelectedTextColor;
    else if (player)
    {
        switch(realm)
        {
            case 1:  // FoL
                color = rpgShapeNameHud_playerFoLColor;
                break;
            case 2:  // MoD
                color = rpgShapeNameHud_playerMoDColor;
                break;
            default: // Monster
                color = rpgShapeNameHud_playerMonsterColor;
                break;
        }
    }
    else
    {
        if (hasdialog)
            color = rpgShapeNameHud_dialogColor;
        else if (monster)
        {
            if (diff)
                color = rpgShapeNameHud_diffColor;
            else if (!unique)
                color = rpgShapeNameHud_monsterColor;
            else
                color = rpgShapeNameHud_uniqueMonsterColor;
        }
        else
            color = mTextColor;
    }
    
    if(player && !grave && !myself)
    {
        if(encounterSuperior < 0)
            conColor = ColorF(0.0f,1.0f,0.0f);
        else if(encounterSuperior == 0)
            conColor = ColorF(0.0f,0.5f,1.0f);
        else
            conColor = ColorF(1.0f,0.0f,0.0f);
        conColor.alpha = opacity;
        dglSetBitmapModulation(conColor);
        if(hostile)
        {
            if(mHostilePlayerTex)
                dglDrawBitmapStretch(mHostilePlayerTex, RectI(2*x-offset.x+1,offset.y,16,16));
        }
        else if(mFriendlyPlayerTex)
            dglDrawBitmapStretch(mFriendlyPlayerTex, RectI(2*x-offset.x+1,offset.y,8,8));
    }
    
    color.alpha = opacity;
    
    dglSetBitmapModulation(color);
    dglDrawText(mProfile->mFont, offset, usename);
    
    
    if (alliance[0] && !mAllianceFont.isNull())
    {
        offset.x = x - mAllianceFont->getStrWidth((const UTF8 *)alliance) / 2;
        offset.y += mProfile->mFont->getHeight();
        
        dglSetBitmapModulation(shadow);
        dglDrawText(mAllianceFont, offset+Point2I(1,1), alliance);
        
        color.red   -= .2;
        color.green -= .2;
        color.blue  -= .2;
        
        dglSetBitmapModulation(color);
        dglDrawText(mAllianceFont, offset,alliance);
    }
    
    
    dglClearBitmapModulation();
}
