//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "math/mPoint.h"
#include "dgl/gTexManager.h"
#include "dgl/dgl.h"
#include "core/color.h"
#include "math/mPoint.h"
#include "math/mRect.h"
#include "dgl/gFont.h"
#include "console/console.h"
#include "math/mMatrix.h"
#include "core/frameAllocator.h"
#include "platform/profiler.h"
#include "core/unicode.h"

namespace {

ColorI sg_bitmapModulation(255, 255, 255, 255);
ColorI sg_textAnchorColor(255, 255, 255, 255);
ColorI sg_stackColor(255, 255, 255, 255);
RectI sgCurrentClipRect;

} // namespace {}


//--------------------------------------------------------------------------
void dglSetBitmapModulation(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_bitmapModulation = c;
   sg_textAnchorColor = sg_bitmapModulation;
}

void dglGetBitmapModulation(ColorF* color)
{
   *color = sg_bitmapModulation;
}

void dglGetBitmapModulation(ColorI* color)
{
   *color = sg_bitmapModulation;
}

void dglClearBitmapModulation()
{
   sg_bitmapModulation.set(255, 255, 255, 255);
}

void dglSetTextAnchorColor(const ColorF& in_rColor)
{
   ColorF c = in_rColor;
   c.clamp();
   sg_textAnchorColor = c;
}


//--------------------------------------------------------------------------
void dglDrawBitmapStretchSR(TextureObject* texture,
                           const RectI&    dstRect,
                           const RectI&    srcRect,
                           const U32       in_flip,
                           F32             fSpin,
                           bool				 bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapStretchSR: NULL Handle");
   if(!dstRect.isValidRect())
      return;
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapStretchSR: routines assume normal rects");

   glDisable(GL_LIGHTING);

   glEnable(GL_TEXTURE_2D);
   glBindTexture(GL_TEXTURE_2D, texture->texGLName);
   //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   if (bSilhouette)
   {
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
   
      ColorF kModulationColor;
      dglGetBitmapModulation(&kModulationColor);
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, kModulationColor);
   }
   else
   {
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   }
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   
   Point2F scrPoints[4];
   if(fSpin == 0.0f)
   {
			scrPoints[0].x = dstRect.point.x;
			scrPoints[0].y = dstRect.point.y;
			scrPoints[1].x = dstRect.point.x + dstRect.extent.x;
			scrPoints[1].y = dstRect.point.y;
			scrPoints[2].x = dstRect.point.x;
			scrPoints[2].y = dstRect.point.y + dstRect.extent.y;
			scrPoints[3].x = dstRect.point.x + dstRect.extent.x;
			scrPoints[3].y = dstRect.point.y + dstRect.extent.y;
			//screenLeft   = dstRect.point.x;
			//screenRight  = dstRect.point.x + dstRect.extent.x;
			//screenTop    = dstRect.point.y;
			//screenBottom = dstRect.point.y + dstRect.extent.y;
   }
   else
   {
      //WE NEED TO IMPLEMENT A FAST 2D ROTATION -- NOT THIS SLOWER 3D ROTATION
      MatrixF rotMatrix( EulerF( 0.0f, 0.0f, mDegToRad(fSpin) ) );

      const F32   halfExtentX = dstRect.extent.x * 0.5f;
      const F32   halfExtentY = dstRect.extent.y * 0.5f;
      
      Point3F offset( dstRect.point.x + halfExtentX, dstRect.point.y + halfExtentY, 0.0f );

      Point3F points[4];
      points[0].set(-halfExtentX,  halfExtentY, 0.0f);
      points[1].set( halfExtentX,  halfExtentY, 0.0f);
      points[2].set(-halfExtentX, -halfExtentY, 0.0f);
      points[3].set( halfExtentX, -halfExtentY, 0.0f);

      for( int i=0; i<4; i++ )
      {
         rotMatrix.mulP( points[i] );
         points[i] += offset;
         scrPoints[i].x = points[i].x;
         scrPoints[i].y = points[i].y;
      }
   }
  
   F32 invTexWidth = 1.0f / texture->texWidth;
   F32 invTexHeight = 1.0f / texture->texHeight;

   F32 texLeft   = (srcRect.point.x)                    * invTexWidth;
   F32 texRight  = (srcRect.point.x + srcRect.extent.x) * invTexWidth;
   F32 texTop    = (srcRect.point.y)                    * invTexHeight;
   F32 texBottom = (srcRect.point.y + srcRect.extent.y) * invTexHeight;

   if(in_flip & GFlip_X)
   {
      F32 temp = texLeft;
      texLeft = texRight;
      texRight = temp;
   }
   if(in_flip & GFlip_Y)
   {
      F32 temp = texTop;
      texTop = texBottom;
      texBottom = temp;
   }

   glColor4ub(sg_bitmapModulation.red,
             sg_bitmapModulation.green,
             sg_bitmapModulation.blue,
             sg_bitmapModulation.alpha);

   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(texLeft, texBottom);
      glVertex2f(scrPoints[2].x, scrPoints[2].y);

      glTexCoord2f(texRight, texBottom);
      glVertex2f(scrPoints[3].x, scrPoints[3].y);

      glTexCoord2f(texRight, texTop);
      glVertex2f(scrPoints[1].x, scrPoints[1].y);

      glTexCoord2f(texLeft, texTop);
      glVertex2f(scrPoints[0].x, scrPoints[0].y);
   glEnd();

   if (bSilhouette)
   {
      glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, ColorF(0.0f, 0.0f, 0.0f, 0.0f));
   }

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);
}

void dglDrawBitmap(TextureObject* texture, const Point2I& in_rAt, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmap: NULL Handle");

   // All non-StretchSR bitmaps are transformed into StretchSR calls...
   //
   RectI subRegion(0, 0,
                   texture->bitmapWidth,
                   texture->bitmapHeight);
   RectI stretch(in_rAt.x, in_rAt.y,
                   texture->bitmapWidth,
                   texture->bitmapHeight);
   dglDrawBitmapStretchSR(texture,
                          stretch,
                          subRegion,
                          in_flip);
}

void dglDrawBitmapStretch(TextureObject* texture, const RectI& dstRect, const U32 in_flip, F32 fSpin, bool bSilhouette)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapStretch: NULL Handle");
   AssertFatal(dstRect.isValidRect() == true,
               "GSurface::drawBitmapStretch: routines assume normal rects");

   RectI subRegion(0, 0,
                   texture->bitmapWidth,
                   texture->bitmapHeight);
   dglDrawBitmapStretchSR(texture,
                          dstRect,
                          subRegion,
                          in_flip,
						  fSpin,
						  bSilhouette);
}

void dglDrawBitmapSR(TextureObject *texture, const Point2I& in_rAt, const RectI& srcRect, const U32 in_flip)
{
   AssertFatal(texture != NULL, "GSurface::drawBitmapSR: NULL Handle");
   AssertFatal(srcRect.isValidRect() == true,
               "GSurface::drawBitmapSR: routines assume normal rects");

   RectI stretch(in_rAt.x, in_rAt.y,
                 srcRect.len_x(),
                 srcRect.len_y());
   dglDrawBitmapStretchSR(texture,
                          stretch,
                          srcRect,
                          in_flip);
}

U32 dglDrawText(const GFont*   font,
                const Point2I& ptDraw,
                const UTF16*   in_string,
                const ColorI*  colorTable,
                const U32      maxColorIndex,
                F32            rot)
{
   return dglDrawTextN(font, ptDraw, in_string, dStrlen(in_string), colorTable, maxColorIndex, rot);
}

U32 dglDrawText(const GFont*   font,
                const Point2I& ptDraw,
                const UTF8*    in_string,
                const ColorI*  colorTable,
                const U32      maxColorIndex,
                F32            rot)
{
   // Just a note - dStrlen(utf8) isn't strictly correct but it's guaranteed to be
   // as long or longer than the real length. dglDrawTextN fails gracefully
   // if you specify overlong, so this is ok.
   return dglDrawTextN(font, ptDraw, in_string, dStrlen((const UTF8 *) in_string), colorTable, maxColorIndex, rot);
}

struct TextVertex
{
   Point2F p;
   Point2F t;
   ColorI c;
   inline void set(F32 x, F32 y, F32 tx, F32 ty, const ColorI &color)
   {
      p.x = x;
      p.y = y;
      t.x = tx;
      t.y = ty;
      c = color;
   }
};

//------------------------------------------------------------------------------

U32 dglDrawTextN(const GFont*    font,
                 const Point2I&  ptDraw,
                 const UTF8*     in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   PROFILE_START(DrawText_UTF8);
   
   U32 len = dStrlen(in_string) + 1;
   FrameTemp<UTF16> ubuf(len);
   convertUTF8toUTF16(in_string, ubuf, len);
   U32 tmp = dglDrawTextN(font, ptDraw, ubuf, n, colorTable, maxColorIndex, rot);

   PROFILE_END();

   return tmp;
}

//-----------------------------------------------------------------------------

U32 dglDrawTextN(const GFont*    font,
                 const Point2I&  ptDraw,
                 const UTF16*    in_string,
                 U32             n,
                 const ColorI*   colorTable,
                 const U32       maxColorIndex,
                 F32             rot)
{
   // return on zero length strings
   if( n < 1 )
      return ptDraw.x;
      
   PROFILE_START(DrawText);

   FrameTemp<TextVertex> vert(4*n);

   glDisable(GL_LIGHTING);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glEnable(GL_BLEND);

   glEnableClientState ( GL_VERTEX_ARRAY );
   glVertexPointer     ( 2, GL_FLOAT, sizeof(TextVertex), &(vert[0].p) );

   glEnableClientState ( GL_COLOR_ARRAY );
   glColorPointer      ( 4, GL_UNSIGNED_BYTE, sizeof(TextVertex), &(vert[0].c) );

   glEnableClientState ( GL_TEXTURE_COORD_ARRAY );
   glTexCoordPointer   ( 2, GL_FLOAT, sizeof(TextVertex), &(vert[0].t) );

   MatrixF rotMatrix;
   if ( rot != 0.0f )
      rotMatrix.set( EulerF( 0.0f, 0.0f, mDegToRad( rot ) ) );

   const Point3F offset( ptDraw.x, ptDraw.y, 0.0f );

   const ColorI  *currentColor = &sg_bitmapModulation;
   S32         currentPt = 0;

   F32 invTexWidth;
   F32 invTexHeight;

   TextureObject *lastTexture = NULL;

   const U32	fontBaseline = font->getBaseline();
   const PlatformFont::CharInfo &tabci = font->getCharInfo( dT(' ') );
   const U32	fontTabIncrement = tabci.xIncrement * GFont::TabWidthInSpaces;

   // first build the point, color, and coord arrays

   U32         nCharCount = 0;
   Point2I     pt( 0, 0 );
   UTF16       c;
   U32         i;

   for(i = 0,c = in_string[i];in_string[i] && i < n;i++,c = in_string[i])
   {
      nCharCount++;
      if(nCharCount > n)
          break;

      switch( c )
      {
         // We have to do a little dance here since \t = 0x9, \n = 0xa, and \r = 0xd
         case 1: case 2: case 3: case 4: case 5: case 6: case 7:
         case 11: case 12:
         case 14:
         {
            // Color code
            if (colorTable)
            {
               static U8 remap[15] =
               {
               0x0, // 0 special null terminator
               0x0, // 1 ascii start-of-heading??
               0x1, 
               0x2, 
               0x3, 
               0x4, 
               0x5, 
               0x6, 
               0x0, // 8 special backspace
               0x0, // 9 special tab
               0x0, // a special \n
               0x7, 
               0x8,
               0x0, // a special \r
               0x9 
               };

               U8 remapped = remap[c];
               // Ignore if the color is greater than the specified max index:
               if ( remapped <= maxColorIndex )
               {
                  const ColorI &clr = colorTable[remapped];
                  sg_bitmapModulation = clr;
                  currentColor = &clr;
               }
            }
            continue;
         }

         // reset color?
         case 15:
         {
            currentColor = &sg_textAnchorColor;
            sg_bitmapModulation = sg_textAnchorColor;
            continue;
         }

         // push color:
         case 16:
         {
            sg_stackColor = sg_bitmapModulation;
            continue;
         }

         // pop color:
         case 17:
         {
            currentColor = &sg_stackColor;
            sg_bitmapModulation = sg_stackColor;
            continue;
         }

         // Tab character
         case dT('\t'):
         {
            pt.x += fontTabIncrement;
            continue;
         }

         default:
         {
            if( !font->isValidChar( c ) )  
               continue;
         }
	  }

      const PlatformFont::CharInfo &ci = font->getCharInfo(c);

      if(ci.bitmapIndex == -1)
      {
         pt.x += ci.xOrigin + ci.xIncrement;
         continue;
      }

      TextureObject *newObj = font->getTextureHandle(ci.bitmapIndex);
      if(newObj != lastTexture)
      {
         if(currentPt)
         {
            glBindTexture(GL_TEXTURE_2D, lastTexture->texGLName);
            glDrawArrays( GL_QUADS, 0, currentPt );
            currentPt = 0;
         }
         lastTexture = newObj;
         invTexWidth = 1.0f / lastTexture->texWidth;
         invTexHeight = 1.0f / lastTexture->texHeight;
      }
      if(ci.width != 0 && ci.height != 0)
      {
         pt.y = fontBaseline - ci.yOrigin;
         pt.x += ci.xOrigin;

         F32 texLeft   = (ci.xOffset)             * invTexWidth;
         F32 texRight  = (ci.xOffset + ci.width)  * invTexWidth;
         F32 texTop    = (ci.yOffset)             * invTexHeight;
         F32 texBottom = (ci.yOffset + ci.height) * invTexHeight;

         F32 screenLeft   = pt.x;
         F32 screenRight  = screenLeft + ci.width;
         F32 screenTop    = pt.y;
         F32 screenBottom = screenTop + ci.height;

         if ( rot == 0.0f )
         {			 
            vert[currentPt++].set( screenLeft + offset.x, screenBottom + offset.y, texLeft, texBottom, *currentColor );
            vert[currentPt++].set( screenRight + offset.x, screenBottom + offset.y, texRight, texBottom, *currentColor );
            vert[currentPt++].set( screenRight + offset.x, screenTop + offset.y, texRight, texTop, *currentColor );
            vert[currentPt++].set( screenLeft + offset.x, screenTop + offset.y, texLeft, texTop, *currentColor );
         }
         else
         {
            Point3F point( screenLeft, screenBottom, 0.0f );
            rotMatrix.mulP( point );
            point += offset;
            vert[currentPt++].set( point.x, point.y, texLeft, texBottom, *currentColor );

            point.set( screenRight,  screenBottom, 0.0f );
            rotMatrix.mulP( point );
            point += offset;
            vert[currentPt++].set( point.x, point.y, texRight, texBottom, *currentColor );

            point.set( screenRight,  screenTop, 0.0f );
            rotMatrix.mulP( point );
            point += offset;
            vert[currentPt++].set( point.x, point.y, texRight, texTop, *currentColor );

            point.set( screenLeft, screenTop, 0.0f );
            rotMatrix.mulP( point );
            point += offset;
            vert[currentPt++].set( point.x, point.y, texLeft, texTop, *currentColor );
         }

         pt.x += ci.xIncrement - ci.xOrigin;
      }
      else
         pt.x += ci.xIncrement;
   }
   
   if(currentPt)
   {
      glBindTexture(GL_TEXTURE_2D, lastTexture->texGLName);
      glDrawArrays( GL_QUADS, 0, currentPt );
   }

   glDisableClientState ( GL_VERTEX_ARRAY );
   glDisableClientState ( GL_COLOR_ARRAY );
   glDisableClientState ( GL_TEXTURE_COORD_ARRAY );

   glDisable(GL_BLEND);
   glDisable(GL_TEXTURE_2D);

   pt.x += ptDraw.x; // DAW: Account for the fact that we removed the drawing point from the text start at the beginning.

   AssertFatal(pt.x >= ptDraw.x, "How did this happen?");
   PROFILE_END();

   return pt.x - ptDraw.x;
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// Drawing primitives

void dglDrawLine(S32 x1, S32 y1, S32 x2, S32 y2, const ColorI &color)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glColor4ub(color.red, color.green, color.blue, color.alpha);
   glBegin(GL_LINES);
   glVertex2f((F32)x1 + 0.5f,  (F32)y1 + 0.5f);
   glVertex2f((F32)x2 + 0.5f,    (F32)y2 + 0.5f);
   glEnd();
	//glBegin(GL_POINTS);
	//glVertex2f((F32)x2 + 0.5, (F32)y2 + 0.5);
	//glEnd();
}

void dglDrawLine(const Point2I &startPt, const Point2I &endPt, const ColorI &color)
{
	dglDrawLine(startPt.x, startPt.y, endPt.x, endPt.y, color);
}

void dglDrawRect(const Point2I &upperL, const Point2I &lowerR, const ColorI &color)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glColor4ub(color.red, color.green, color.blue, color.alpha);
   glBegin(GL_LINE_LOOP);
      glVertex2f((F32)upperL.x + 0.5f, (F32)upperL.y + 0.5f);
      glVertex2f((F32)lowerR.x + 0.5f, (F32)upperL.y + 0.5f);
      glVertex2f((F32)lowerR.x + 0.5f, (F32)lowerR.y + 0.5f);
      glVertex2f((F32)upperL.x + 0.5f, (F32)lowerR.y + 0.5f);
   glEnd();
}

// the fill convention for lined rects is that they outline the rectangle border of the
// filled region specified.

void dglDrawRect(const RectI &rect, const ColorI &color)
{
   Point2I lowerR(rect.point.x + rect.extent.x - 1, rect.point.y + rect.extent.y - 1);
   dglDrawRect(rect.point, lowerR, color);
}

// the fill convention says that pixel at upperL will be filled and
// that pixel at lowerR will NOT be filled.

void dglDrawRectFill(const Point2I &upperL, const Point2I &lowerR, const ColorI &color)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glDisable(GL_TEXTURE_2D);

   glColor4ub(color.red, color.green, color.blue, color.alpha);
   glRecti((S32)upperL.x, (S32)upperL.y, (S32)lowerR.x, (S32)lowerR.y);
}
void dglDrawRectFill(const RectI &rect, const ColorI &color)
{
   Point2I lowerR(rect.point.x + rect.extent.x, rect.point.y + rect.extent.y);
   dglDrawRectFill(rect.point, lowerR, color);
}

void dglDraw2DSquare( const Point2F &screenPoint, F32 width, F32 spinAngle )
{
   width *= 0.5f;

   Point3F points[4];

   if ( spinAngle == 0.0f )
   {
	   points[0].set(-width + screenPoint.x, -width + screenPoint.y, 0.0f);
	   points[1].set(-width + screenPoint.x,  width + screenPoint.y, 0.0f);
	   points[2].set( width + screenPoint.x,  width + screenPoint.y, 0.0f);
	   points[3].set( width + screenPoint.x, -width + screenPoint.y, 0.0f);
   }
   else
   {
	   MatrixF rotMatrix( EulerF( 0.0f, 0.0f, spinAngle ) );
       Point3F offset( screenPoint.x, screenPoint.y, 0.0f );

	   points[0].set(-width, -width, 0.0f);
	   points[1].set(-width,  width, 0.0f);
	   points[2].set( width,  width, 0.0f);
	   points[3].set( width, -width, 0.0f);

	   for( int i=0; i<4; ++i )
	   {
		  rotMatrix.mulP( points[i] );
		  points[i] += offset;
	   }
   }

   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.0f, 0.0f);
      glVertex2fv(points[0]);

      glTexCoord2f(0.0f, 1.0f);
      glVertex2fv(points[1]);

      glTexCoord2f(1.0f, 1.0f);
      glVertex2fv(points[2]);

      glTexCoord2f(1.0f, 0.0f);
      glVertex2fv(points[3]);
   glEnd();
}

void dglDrawBillboard( const Point3F &position, F32 width, F32 spinAngle )
{
   MatrixF modelview;
   dglGetModelview( &modelview );
   modelview.transpose();

   width *= 0.5f;
   
   Point3F points[4];
   points[0].set(-width, 0.0f, -width);
   points[1].set(-width, 0.0f,  width);
   points[2].set( width, 0.0f,  width);
   points[3].set( width, 0.0f, -width);

   if ( spinAngle == 0.0f )
   {
      for( int i=0; i<4; i++ )
      {
         modelview.mulP( points[i] );
         points[i] += position;
      }
   }
   else
   {
      MatrixF rotMatrix( EulerF( 0.0f, spinAngle, 0.0f ) );

      for( int i=0; i<4; i++ )
      {
         rotMatrix.mulP( points[i] );
         modelview.mulP( points[i] );
         points[i] += position;
      }
   }
   
   glBegin(GL_TRIANGLE_FAN);
      glTexCoord2f(0.0f, 1.0f);
      glVertex3fv(points[0]);

      glTexCoord2f(0.0f, 0.0f);
      glVertex3fv(points[1]);

      glTexCoord2f(1.0f, 0.0f);
      glVertex3fv(points[2]);

      glTexCoord2f(1.0f, 1.0f);
      glVertex3fv(points[3]);
   glEnd();
}

static Point3F cubePoints[8] =
{
	Point3F(-1.0f, -1.0f, -1.0f),
	Point3F(-1.0f, -1.0f,  1.0f),
	Point3F(-1.0f,  1.0f, -1.0f),
	Point3F(-1.0f,  1.0f,  1.0f),
	Point3F( 1.0f, -1.0f, -1.0f),
	Point3F( 1.0f, -1.0f,  1.0f),
	Point3F( 1.0f,  1.0f, -1.0f),
	Point3F( 1.0f,  1.0f,  1.0f)
};

static U32 cubeFaces[6][4] =
{
  { 0, 2, 6, 4 }, { 0, 2, 3, 1 }, { 0, 1, 5, 4 },
  { 3, 2, 6, 7 }, { 7, 6, 4, 5 }, { 3, 7, 5, 1 }
};

void dglWireCube(const Point3F & extent, const Point3F & center)
{
   glDisable(GL_CULL_FACE);

   for (S32 i = 0; i < 6; i++)
   {
      glBegin(GL_LINE_LOOP);
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * extent.x + center.x,
            cubePoints[idx].y * extent.y + center.y,
            cubePoints[idx].z * extent.z + center.z);
      }
      glEnd();
   }
}


void dglSolidCube(const Point3F & extent, const Point3F & center)
{
   for (S32 i = 0; i < 6; i++)
   {
      glBegin(GL_TRIANGLE_FAN);
      for(int vert = 0; vert < 4; vert++)
      {
         int idx = cubeFaces[i][vert];
         glVertex3f(cubePoints[idx].x * extent.x + center.x,
            cubePoints[idx].y * extent.y + center.y,
            cubePoints[idx].z * extent.z + center.z);
      }
      glEnd();
   }
}

void dglSetClipRect(const RectI &clipRect)
{
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   const U32 screenWidth  = Platform::getWindowSize().x;
   const U32 screenHeight = Platform::getWindowSize().y;

   glOrtho(clipRect.point.x, clipRect.point.x + clipRect.extent.x,
           clipRect.extent.y, 0.0,
           0.0, 1.0);
   glTranslatef(0.0f, -clipRect.point.y, 0.0f);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   glViewport(clipRect.point.x, screenHeight - (clipRect.point.y + clipRect.extent.y),
              clipRect.extent.x, clipRect.extent.y);

   sgCurrentClipRect = clipRect;
}

const RectI& dglGetClipRect()
{
   return sgCurrentClipRect;
}

bool dglPointToScreen( const Point3F &point3D, Point3F &screenPoint )
{
   GLdouble       glMV[16];
   GLdouble       glPR[16];
   GLint          glVP[4];


   glGetDoublev(GL_PROJECTION_MATRIX, glPR);
   glGetDoublev(GL_MODELVIEW_MATRIX, glMV);

   RectI viewport;
   dglGetViewport(&viewport);

   glVP[0] = viewport.point.x;
   glVP[1] = viewport.point.y + viewport.extent.y;
   glVP[2] = viewport.extent.x;
   glVP[3] = -viewport.extent.y;

   MatrixF mv;
   dglGetModelview(&mv);
   MatrixF pr;
   dglGetProjection(&pr);

   F64 x, y, z;
   int result = gluProject( (GLdouble)point3D.x, (GLdouble)point3D.y, (GLdouble)point3D.z, (const F64 *)&glMV, (const F64 *)&glPR, (const GLint *)&glVP, &x, &y, &z );
   screenPoint.x = x;
   screenPoint.y = y;
   screenPoint.z = z;

   return (result == GL_TRUE);
}


bool dglIsInCanonicalState()
{
   bool ret = true;

   // Canonical state:
   //  BLEND disabled
   //  TEXTURE_2D disabled on both texture units.
   //  ActiveTexture set to 0
   //  LIGHTING off
   //  winding : clockwise ?
   //  cullface : disabled

   ret &= glIsEnabled(GL_BLEND) == GL_FALSE;
   ret &= glIsEnabled(GL_CULL_FACE) == GL_FALSE;
   GLint temp;

   if (dglDoesSupportARBMultitexture() == true) {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   } else {
      ret &= glIsEnabled(GL_TEXTURE_2D) == GL_FALSE;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &temp);
      ret &= temp == GL_REPLACE;

      ret &= glIsEnabled(GL_TEXTURE_COORD_ARRAY) == GL_FALSE;
   }

   ret &= glIsEnabled(GL_LIGHTING) == GL_FALSE;

   ret &= glIsEnabled(GL_COLOR_ARRAY)         == GL_FALSE;
   ret &= glIsEnabled(GL_VERTEX_ARRAY)        == GL_FALSE;
   ret &= glIsEnabled(GL_NORMAL_ARRAY)        == GL_FALSE;
   if (dglDoesSupportFogCoord())
      ret &= glIsEnabled(GL_FOG_COORDINATE_ARRAY_EXT) == GL_FALSE;

   return ret;
}


void dglSetCanonicalState()
{
   glDisable(GL_BLEND);
   glDisable(GL_CULL_FACE);
   glBlendFunc(GL_ONE, GL_ZERO);
   glDisable(GL_LIGHTING);
   if (dglDoesSupportARBMultitexture() == true) {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   } else {
      glDisable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
   }

   glDisableClientState(GL_COLOR_ARRAY);
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   if (dglDoesSupportFogCoord())
      glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
}

void dglGetTransformState(S32* mvDepth,
                          S32* pDepth,
                          S32* t0Depth,
                          F32* t0Matrix,
                          S32* t1Depth,
                          F32* t1Matrix,
                          S32* vp)
{
   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH, (GLint*)mvDepth);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, (GLint*)pDepth);

   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t0Depth);
   glGetFloatv(GL_TEXTURE_MATRIX, t0Matrix);
   if (dglDoesSupportARBMultitexture())
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, (GLint*)t1Depth);
      glGetFloatv(GL_TEXTURE_MATRIX, t1Matrix);
      glActiveTextureARB(GL_TEXTURE0_ARB);
   }
   else
   {
      *t1Depth = 0;
      for (U32 i = 0; i < 16; i++)
         t1Matrix[i] = 0.0f;
   }

   RectI v;
   dglGetViewport(&v);
   vp[0] = v.point.x;
   vp[1] = v.point.y;
   vp[2] = v.extent.x;
   vp[3] = v.extent.y;
}


bool dglCheckState(const S32 mvDepth, const S32 pDepth,
                   const S32 t0Depth, const F32* t0Matrix,
                   const S32 t1Depth, const F32* t1Matrix,
                   const S32* vp)
{
   GLint md, pd;
   RectI v;

   glGetIntegerv(GL_MODELVIEW_STACK_DEPTH,  &md);
   glGetIntegerv(GL_PROJECTION_STACK_DEPTH, &pd);

   GLint t0d, t1d;
   GLfloat t0m[16], t1m[16];
   glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t0d);
   glGetFloatv(GL_TEXTURE_MATRIX, t0m);
   if (dglDoesSupportARBMultitexture())
   {
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glGetIntegerv(GL_TEXTURE_STACK_DEPTH, &t1d);
      glGetFloatv(GL_TEXTURE_MATRIX, t1m);
      glActiveTextureARB(GL_TEXTURE0_ARB);
   }
   else
   {
      t1d = 0;
      for (U32 i = 0; i < 16; i++)
         t1m[i] = 0.0f;
   }

   dglGetViewport(&v);

   return ((md == mvDepth) &&
           (pd == pDepth) &&
           (t0d == t0Depth) &&
           (dMemcmp(t0m, t0Matrix, sizeof(F32) * 16) == 0) &&
           (t1d == t1Depth) &&
           (dMemcmp(t1m, t1Matrix, sizeof(F32) * 16) == 0) &&
           ((v.point.x  == vp[0]) &&
            (v.point.y  == vp[1]) &&
            (v.extent.x == vp[2]) &&
            (v.extent.y == vp[3])));
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
// Advanced hardware functionality.

// now owns the global stuff for FSAA, rather than sceneGraph.

// define the globals first.  don't bracket them, as it doesn't hurt to have
// them defined always...

S32 gFSAASamples = 1; ///< 1==no FSAA.

#if defined(TORQUE_OS_MAC)
// new FSAA simple-method handling

ConsoleFunctionGroupBegin( MacFSAA, "Mac-specific FSAA control functions.");

//------------------------------------------------------------------------------
ConsoleFunction( setFSAA, void, 2, 2, "setFSAA(int);")
{
   argc;
   gFSAASamples = dAtoi(argv[1]);
   if (gFSAASamples<1)
      gFSAASamples = 1;
   else if (gFSAASamples>gGLState.maxFSAASamples)
      gFSAASamples = gGLState.maxFSAASamples;
   dglSetFSAASamples(gFSAASamples);
}

//------------------------------------------------------------------------------
ConsoleFunction( increaseFSAA, void, 1, 1, "increaseFSAA()" )
{
   if (gFSAASamples<gGLState.maxFSAASamples)
   {
      gFSAASamples<<=1;
      dglSetFSAASamples(gFSAASamples);
   }
}

//------------------------------------------------------------------------------
ConsoleFunction( decreaseFSAA, void, 1, 1, "decreaseFSAA()" )
{
   if (gFSAASamples>1)
   {
      gFSAASamples>>=1;
      dglSetFSAASamples(gFSAASamples);
   }
}

ConsoleFunctionGroupEnd( MacFSAA );

#endif

//------------------------------------------------------------------------------
ConsoleFunction(png2jpg, S32, 2, 3, "png2jpg(pngName,[quality=0-100])")
{
   extern U32 gJpegQuality;
   const char * rgbname = NULL;
   const char * alphaname = NULL;
   const char * basname = NULL;
   const char * bmpname = argv[1];
   if(argc == 3)
      gJpegQuality = dAtoi(argv[2]);
   else
      gJpegQuality = 90;
   bool basOpt = false;

   Con::printf("Converting file: %s", argv[1]);

   if (!rgbname)
   {
      char * buf = new char[dStrlen(bmpname)+32];
      dStrcpy(buf,bmpname);
      char * pos = dStrstr((const char*)buf,".png");
      if (!pos)
         pos = buf + dStrlen(buf);
      dStrcpy(pos,".jpg");
      rgbname = buf;
   }
   if (!alphaname)
   {
      char * buf = new char[dStrlen(bmpname)+32];
      dStrcpy(buf,bmpname);
      char * pos = dStrstr((const char*)buf,".png");
      if (!pos)
         pos = buf + dStrlen(buf);
      dStrcpy(pos,".alpha.jpg");
      alphaname = buf;
   }
   GBitmap bmp;
   FileStream fs;
   if (fs.open(bmpname, FileStream::Read) == false) {
      Con::printf("Error: unable to open file: %s for reading\n", bmpname);
      return -1;
   }
   if (bmp.readPNG(fs) == false) {
      Con::printf("Error: unable to read %s as a .PNG\n", bmpname);
      return -1;
   }
   fs.close();

   if (bmp.getFormat() != GBitmap::RGB &&
       bmp.getFormat() != GBitmap::RGBA) {
      Con::printf("Error: %s is not a 24 or 32-bit .PNG\n", bmpname);
      return false;
   }

   GBitmap * outRGB = NULL;
   GBitmap * outAlpha = NULL;
   GBitmap workRGB, workAlpha;
   if (bmp.getFormat() == GBitmap::RGB)
      outRGB = &bmp;
   else
   {
      S32 w = bmp.getWidth();
      S32 h = bmp.getHeight();
      workRGB.allocateBitmap(w,h,false,GBitmap::RGB);
      workAlpha.allocateBitmap(w,h,false,GBitmap::Alpha);

      U8 * rgbBits = workRGB.getWritableBits();
      U8 * alphaBits = workAlpha.getWritableBits();
      U8 * bmpBits = bmp.getWritableBits();
      for (S32 i=0; i<w; i++)
      {
         for (S32 j=0; j<h; j++)
         {
            rgbBits[i*3 + j*3*w + 0] = bmpBits[i*4 + j*4*w + 0];
            rgbBits[i*3 + j*3*w + 1] = bmpBits[i*4 + j*4*w + 1];
            rgbBits[i*3 + j*3*w + 2] = bmpBits[i*4 + j*4*w + 2];
            alphaBits[i + j*w]       = bmpBits[i*4 + j*4*w + 3];
         }
      }
      Con::printf("texture: width=%i, height=%i\n",w,h);
      outRGB = &workRGB;
      outAlpha = &workAlpha;
   }

   if (outRGB)
   {
      FileStream fws;
      if (fws.open(rgbname, FileStream::Write) == false)
      {
         Con::printf("Error: unable to open file: %s for writing\n", rgbname);
         return -1;
      }

      if (dStrstr(rgbname,".png"))
      {
         if (outRGB->writePNG(fws) == false)
         {
            fws.close();
            Con::printf("Error: couldn't write RGB as a png\n");
            return -1;
         }
      }
      else if (outRGB->writeJPEG(fws) == false)
      {
         Con::printf("Error: couldn't write RGB as a jpg\n");
         return -1;
      }
      fws.close();
   }
   if (outAlpha)
   {
      gJpegQuality = 60;
      FileStream fws;
      if (fws.open(alphaname, FileStream::Write) == false)
      {
         Con::printf("Error: unable to open file: %s for writing\n", alphaname);
         return -1;
      }

      if (dStrstr(alphaname,".png"))
      {
         if (outAlpha->writePNG(fws) == false)
         {
            fws.close();
            Con::printf("Error: couldn't write alpha as a png\n");
            return -1;
         }
      }
      else if (outAlpha->writeJPEG(fws) == false)
      {
         Con::printf("Error: couldn't write alpha as a jpg\n");
         return -1;
      }
      fws.close();
   }

   return(0);
}
