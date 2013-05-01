//------------------------------------------------------------------------------
// Gui Vector Field Render control
//------------------------------------------------------------------------------

#include "gui/utility/guiTransitionCtrl.h"
#include "gui/core/guiCanvas.h"
#include "dgl/dgl.h"
#include "util/safeDelete.h"

IMPLEMENT_CONOBJECT( GuiVectorFieldCtrl );

//------------------------------------------------------------------------------
// A init function for vector field to provide some distortion
void FN_CDECL testInitFn( const int x, const int y, const Point2I &resolution,
                         const F32 maxX, const F32 maxY, Point2F *outVec )
{
   outVec->x *= mCos( ( x + y - resolution.x ) / 50.f * M_PI );
   outVec->y *= mCos( ( x + y ) / 100.f * M_PI );
}

void FN_CDECL spiralInitFn( const int x, const int y, const Point2I &resolution,
                         const F32 maxX, const F32 maxY, Point2F *outVec )
{
   outVec->x -= 0.5f;
   outVec->y -= 0.5f;

   F32 d = mSqrt( ( x - resolution.x /2.f ) * ( x - resolution.x / 2.f ) +
               ( y - resolution.y / 2.f ) * ( y - resolution.y / 2.f ) );
   F32 c = mCos( d * M_PI / 64.f );
   F32 s = mSin( (d) * M_PI / 64.f );
   outVec->x = c * outVec->x - s * outVec->y;
   outVec->y = s * outVec->x + c * outVec->y;

   outVec->x += 0.5f;
   outVec->y += 0.5f;


   /*outVec->x += 1.f / resolution.x;
   outVec->y += 1.f / resolution.y;*/
}
//------------------------------------------------------------------------------

GuiVectorFieldCtrl::GuiVectorFieldCtrl()
{
   // Just clear things out, don't allocate.
   // SimObjects may not do anything significant in their constructor.
   mVectorField = NULL;
   mTestTexture = NULL;
}

//------------------------------------------------------------------------------
GuiVectorFieldCtrl::~GuiVectorFieldCtrl()
{
   // duplicate with onRemove, just in case we're somehow deleted w/out remove
   SAFE_DELETE( mVectorField );
   SAFE_DELETE( mTestTexture );
}

//------------------------------------------------------------------------------
bool GuiVectorFieldCtrl::onAdd()
{
   if(!Parent::onAdd())
      return false;

   mVectorField = new VectorField(Point2I(16,21));
   mTestTexture = new TextureHandle("common/TestMap", BitmapTexture);

   mFeedbackTexture.setUpdateRect(mBounds);
   mClearColor.set(1.0f, 0.0f, 0.0f, 1.0f);

   return true;
}

//------------------------------------------------------------------------------
void GuiVectorFieldCtrl::onRemove()
{
   SAFE_DELETE( mVectorField );
   SAFE_DELETE( mTestTexture );

   Parent::onRemove();
}

//------------------------------------------------------------------------------

void GuiVectorFieldCtrl::onRender( Point2I offset, const RectI &updateRect )
{
   RectI ctrlRect( offset, mBounds.extent );

   glDisable(GL_LIGHTING);

   //glEnable(GL_TEXTURE_2D);
   glEnable(GL_BLEND);

   //glBindTexture(GL_TEXTURE_2D, mTestTexture->getGLName() );
   //glBindTexture(GL_TEXTURE_2D, mFeedbackTexture.getTextureHandle().getGLName() );

   // CTE = 0x812F
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
   glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (F32 *)&mClearColor );
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   // Blend stuff
   glBlendEquationEXT( GL_FUNC_ADD_EXT );
   glBlendColorEXT( 0.1f, 0.1f, 0.1f, 0.f );
   glBlendFunc( GL_CONSTANT_COLOR_EXT, GL_ONE );

   // Render the current field
   //mVectorField->renderField( true );
   //dglDrawRectFill( ctrlRect, mClearColor );

   glColor4ub( 0, 128, 0, 255 );
   glRecti( (S32)ctrlRect.point.x, (S32)ctrlRect.point.y,
            (S32)ctrlRect.point.x + ctrlRect.extent.x,
            (S32)ctrlRect.point.y + ctrlRect.extent.y );

   glDisable(GL_TEXTURE_2D);
   glDisable(GL_BLEND);

   //mVectorField->visualizeField();
}

//------------------------------------------------------------------------------

void GuiVectorFieldCtrl::resize(const Point2I &newPosition, const Point2I &newExtent)
{
   Parent::resize( newPosition, newExtent );

   mFeedbackTexture.setUpdateRect( RectI( newPosition, newExtent ) );

   TextureObject *obj = (TextureObject *)*mTestTexture;
   //TextureObject *obj = (TextureObject *)mFeedbackTexture.getTextureHandle();
   F32 maxX  = F32(obj->bitmapWidth) / F32(obj->texWidth);
   F32 maxY = F32(obj->bitmapHeight) / F32(obj->texHeight);

   mVectorField->initVectorField( maxX, maxY, VectorField::Flip_None, &spiralInitFn );
}

//------------------------------------------------------------------------------

void GuiVectorFieldCtrl::processTick()
{
   mFeedbackTexture.updateAtEndOfFrame();
}