//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------


#include "dgl/gFont.h"
#include "dgl/gBitmap.h"
#include "math/mRect.h"
#include "console/console.h"
#include "core/unicode.h"
#include "platformX86UNIX/platformX86UNIX.h"
#include "platformX86UNIX/x86UNIXFont.h"

// Needed by createFont
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xrender.h>      // For XRenderColor

// Needed for getenv in createFont
#include <stdlib.h>
XftFont *loadFont(const char *name, S32 size, Display *display)
{
  XftFont *fontInfo = NULL;
  char* fontname = const_cast<char*>(name);
  if (dStrlen(fontname)==0)
    fontname = "arial";
  else if (stristr(const_cast<char*>(name), "arial") != NULL)
    fontname = "arial";
  else if (stristr(const_cast<char*>(name), "lucida console") != NULL)
    fontname = "lucida console";

  char* weight = "medium";
  char* slant = "roman"; // no slant

  if (stristr(const_cast<char*>(name), "bold") != NULL)
    weight = "bold";
  if (stristr(const_cast<char*>(name), "italic") != NULL)
    slant = "italic";

  int mSize = size - 2 - (int)((float)size * 0.1);
  char xfontName[512];
  // We specify a lower DPI to get 'correct' looking fonts, if we go with the
  // native DPI the fonts are to big and don't fit the widgets.
  dSprintf(xfontName, 512, "%s-%d:%s:slant=%s:dpi=76", fontname, mSize, weight, slant);

  // Lets see if Xft can get a font for us.
  char xftname[1024];
  fontInfo = XftFontOpenName(display, DefaultScreen(display), xfontName);
  // Cant find a suitabke font, default to a known font (6x10)
  if ( !fontInfo )
    {
  	dSprintf(xfontName, 512, "6x10-%d:%s:slant=%s:dpi=76", mSize, weight, slant);
      fontInfo = XftFontOpenName(display, DefaultScreen(display), xfontName);
    }
      XftNameUnparse(fontInfo->pattern, xftname, 1024);

#ifdef DEBUG
      Con::printf("Font '%s %d' mapped to '%s'\n", name, size, xftname);
#endif

  return fontInfo;
}

// XA: New class for the unix unicode font
PlatformFont *createPlatformFont(const char *name, U32 size, U32 charset /* = TGE_ANSI_CHARSET */)
{
  PlatformFont *retFont = new x86UNIXFont;

  if(retFont->create(name, size, charset))
    return retFont;

  delete retFont;
  return NULL;
}

x86UNIXFont::x86UNIXFont()
{}

x86UNIXFont::~x86UNIXFont()
{}


bool x86UNIXFont::create(const char *name, U32 size, U32 charset)
{
  Display *display = XOpenDisplay(getenv("DISPLAY"));
  if (display == NULL)
    AssertFatal(false, "createFont: cannot connect to X server");

  XftFont *font = loadFont(name, size, display);
  
  if (!font)
  {
    Con::errorf("Error: Could not load font -%s-", name);
    return false;
  }
  char xfontname[1024];
  XftNameUnparse(font->pattern, xfontname, 1024);
#ifdef DEBUG
  Con::printf("CreateFont: request for %s %d, using %s", name, size, xfontname);
#endif
  // store some info about the font
  baseline = font->ascent;
  height = font->height;
  mFontName = StringTable->insert(xfontname);
  XftFontClose (display, font);
  // DISPLAY
  XCloseDisplay(display);

  return true;
}

bool x86UNIXFont::isValidChar(const UTF16 str) const
{
  // 0x20  == 32
  // 0x100 == 256
  if( str < 0x20 || str > 0x100 )
    return false;

  return true;
}

bool x86UNIXFont::isValidChar(const UTF8 *str) const
{

  return isValidChar(oneUTF32toUTF16(oneUTF8toUTF32(str,NULL)));
}

PlatformFont::CharInfo &x86UNIXFont::getCharInfo(const UTF16 ch) const
{
  Display *display = XOpenDisplay(getenv("DISPLAY"));
  if (!display )
    AssertFatal(false, "createFont: cannot connect to X server");

  static PlatformFont::CharInfo c;
  dMemset(&c, 0, sizeof(c));
  c.bitmapIndex = 0;
  c.xOffset     = 0;
  c.yOffset     = 0;

  XftFont *fontInfo  = XftFontOpenName(display, DefaultScreen(display), mFontName);
  if (!fontInfo)
  {
    //Try a fall back font before crashing..
    fontInfo  = XftFontOpenName(display, DefaultScreen(display), "lucida console-10:dpi=76");
    if (!fontInfo)
    {
    AssertFatal(false, "createFont: cannot load font");
    }
  }

  int screen = DefaultScreen(display);
  // Create the pixmap to draw on.
  Drawable pixmap = XCreatePixmap(display,
				  DefaultRootWindow(display),
				  fontInfo->max_advance_width,
				  fontInfo->height,
				  DefaultDepth(display, screen));
  // And the Xft wrapper around it.
  XftDraw *draw = XftDrawCreate(display,
                                pixmap,
                                DefaultVisual(display, screen),
                                DefaultColormap(display, screen));
  // Allocate some colors, we don't use XftColorAllocValue here as that
  // Don't appear to function correctly (or I'm using it wrong) As we only do
  // this twice per new un cached font it isn't that big of a penalty. (Each
  // call to XftColorAllocName involves a round trip to the X Server)
  XftColor black, white;
  XftColorAllocName(display,
                    DefaultVisual(display, screen),
                    DefaultColormap(display, screen),
                    "black",
                    &black);
  // White
  XftColorAllocName(display,
                    DefaultVisual(display, screen),
                    DefaultColormap(display, screen),
                    "white",
                    &white);
  
  XGlyphInfo charinfo;
  XftTextExtents16(display, fontInfo, &ch, 1, &charinfo);
  c.height     = fontInfo->height;
  c.xOrigin    = 0; 
  c.yOrigin    = fontInfo->ascent;
  c.xIncrement = charinfo.xOff;
  c.width      = charinfo.xOff;
  // kick out early if the character is undrawable
  if( c.width == 0 || c.height == 0)
    return c;

  // allocate a greyscale bitmap and clear it.
  int bitmapDataSize = c.width * c.height;
  c.bitmapData = new U8[bitmapDataSize];
  dMemset(c.bitmapData, 0, bitmapDataSize);

  XftDrawRect (draw, &black, 0, 0, fontInfo->max_advance_width, fontInfo->height);
  XftDrawString16 (draw, &white, fontInfo, 0, fontInfo->ascent, &ch, 1);
  // grab the pixmap image

  XImage *ximage = XGetImage(display, pixmap, 0, 0, 
			     charinfo.xOff, fontInfo->height, 
			     AllPlanes, XYPixmap);
  if (!ximage)
    AssertFatal(false, "cannot get x image");
  int x, y;

  // grab each pixel and store it in the scratchPad
  for(y = 0; y < fontInfo->height; y++)
  {
    for(x = 0; x < charinfo.xOff; x++)
      c.bitmapData[y * charinfo.xOff + x] = static_cast<U8>(XGetPixel(ximage, x, y));
  }
  XDestroyImage(ximage);

  XftColorFree(display, DefaultVisual(display, screen),
               DefaultColormap(display, screen), &black);
  XftColorFree(display, DefaultVisual(display, screen),
               DefaultColormap(display, screen), &white);
  XftDrawDestroy(draw);
  XFreePixmap(display, pixmap);
  XCloseDisplay(display);

  return c;
}


PlatformFont::CharInfo &x86UNIXFont::getCharInfo(const UTF8 *str) const
{
  return getCharInfo(oneUTF32toUTF16(oneUTF8toUTF32(str,NULL)));
}
