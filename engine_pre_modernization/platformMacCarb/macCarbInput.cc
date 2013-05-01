//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platform/platformInput.h"
#include "platform/event.h"
#include "console/console.h"
#include "platform/gameInterface.h"
#include "core/unicode.h"

// headers for clipboard access
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>


// Static class variables:
InputManager* Input::smManager;
bool           Input::smActive;

bool gInputEnabled = false;
bool gMouseEnabled = false;
bool gKBEnabled = false;
bool gMouseActive = false;
bool gKBActive = false;


//------------------------------------------------------------------------------
// Helper functions.  Should migrate to an InputManager object at some point.
bool enableKeyboard(void);
void disableKeyboard(void);
bool activateKeyboard(void);
void deactivateKeyboard(void);
bool enableMouse(void);
void disableMouse(void);
bool activateMouse(void);
void deactivateMouse(void);



static void fillAsciiTable();

//------------------------------------------------------------------------------
//
// This function gets the standard ASCII code corresponding to our key code
// and the existing modifier key state.
//
//------------------------------------------------------------------------------
struct AsciiData
{
   struct KeyData
   {
      U16   ascii;
      bool  isDeadChar;
   };

   KeyData upper;
   KeyData lower;
   KeyData goofy;
};


#define NUM_KEYS ( KEY_OEM_102 + 1 )
#define KEY_FIRST KEY_ESCAPE
static AsciiData AsciiTable[NUM_KEYS];


//--------------------------------------------------------------------------
void Input::init()
{
   OSStatus status = noErr;

   Con::printf( "Input Init:" );
   destroy();

   smManager = NULL;
   smActive = false;

   // set up the object that tells the system we want unicode input
   InterfaceTypeList tsmDocTypeList = { kUnicodeDocument };
   NewTSMDocument( 1, tsmDocTypeList, &platState.tsmDoc, NULL);
   UseInputWindow( platState.tsmDoc, true);
   
   // stop the double-cursor thing
   Con::setBoolVariable("$pref::Gui::noClampTorqueCursorToWindow", true);
   
   // enable main input
   Input::enable();

   
   
   Con::printf( "" );
}

//------------------------------------------------------------------------------
ConsoleFunction( isJoystickDetected, bool, 1, 1, "Always false on the MAC." )
{
/*
   argc; argv;
   return( DInputDevice::joystickDetected() );
*/
   return(false);
}

//------------------------------------------------------------------------------
ConsoleFunction( getJoystickAxes, const char*, 2, 2, "(handle instance)" )
{

   return( "" );
}

//------------------------------------------------------------------------------
static void fillAsciiTable()
{

}

//------------------------------------------------------------------------------
U16 Input::getKeyCode( U16 asciiCode )
{
   U16 keyCode = 0;
   U16 i;
   
   // This is done three times so the lowerkey will always
   // be found first. Some foreign keyboards have duplicate
   // chars on some keys.
   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].lower.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].upper.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   for ( i = KEY_FIRST; i < NUM_KEYS && !keyCode; i++ )
   {
      if ( AsciiTable[i].goofy.ascii == asciiCode )
      {
         keyCode = i;
         break;
      };
   }

   return( keyCode );
}

//------------------------------------------------------------------------------
U16 Input::getAscii( U16 keyCode, KEY_STATE keyState )
{
   if ( keyCode >= NUM_KEYS )
      return 0;

   switch ( keyState )
   {
      case STATE_LOWER:
         return AsciiTable[keyCode].lower.ascii;
      case STATE_UPPER:
         return AsciiTable[keyCode].upper.ascii;
      case STATE_GOOFY:
         return AsciiTable[keyCode].goofy.ascii;
      default:
         return(0);
            
   }
}

//------------------------------------------------------------------------------
void Input::destroy()
{
#ifdef LOG_INPUT
   if ( gInputLog )
   {
      log( "*** CLOSING LOG ***\n" );
      CloseHandle( gInputLog );
      gInputLog = NULL;
   }
#endif

   // turn us off.
   if (gInputEnabled)
      disable();
   
/*
   if ( smManager && smManager->isEnabled() )
   {
      smManager->disable();
      delete smManager;
      smManager = NULL;
   }
*/
}

//------------------------------------------------------------------------------
bool Input::enable()
{
//Con::printf( "[]Input::enable." );

   gInputEnabled = true;

//   if ( smManager && !smManager->isEnabled() )
//      return( smManager->enable() );

   enableMouse();
   //enableKeyboard();

   return( gInputEnabled );
}

//------------------------------------------------------------------------------
void Input::disable()
{
//Con::printf( "[]Input::disable." );

   gInputEnabled = false;

//  if ( smManager && smManager->isEnabled() )
//      smManager->disable();

   disableMouse();
   //disableKeyboard();
}

//------------------------------------------------------------------------------
void Input::activate()
{
   smActive = true;

   enableMouse();
// enableKeyboard();
}

//------------------------------------------------------------------------------
void Input::deactivate()
{
//Con::printf( "[]Input::deactivate." );

   deactivateMouse();
   //deactivateKeyboard();

   smActive = false;

}

//------------------------------------------------------------------------------
void Input::reactivate()
{
   // don't think mac needs to do anything right now!!!!!! TBD
   
   // This is soo hacky...
//   SetForegroundWindow( winState.appWindow );
//   PostMessage( winState.appWindow, WM_ACTIVATE, WA_ACTIVE, NULL );
}

//------------------------------------------------------------------------------
bool Input::isEnabled()
{
//   if ( smManager )
//      return smManager->isEnabled();

   return(gInputEnabled);
}

//------------------------------------------------------------------------------
bool Input::isActive()
{
   return smActive;
}

//------------------------------------------------------------------------------
void Input::process()
{
   if (!smActive || !gInputEnabled)
      return;

   if (!gMouseEnabled || !gMouseActive)
      return;
      
 
//   if ( smManager && smManager->isEnabled() && smActive )
//      smManager->process();
}

//------------------------------------------------------------------------------
InputManager* Input::getManager()
{
   return( smManager );
}


//--------------------------------------------------------------------------
#pragma message("input remap table might need tweaking - rumors of ibooks having diff virt keycodes, might need intermediate remap...")
static U8 VcodeRemap[256] =
{
KEY_A,                     // 0x00 
KEY_S,                     // 0x01 
KEY_D,                     // 0x02 
KEY_F,                     // 0x03 
KEY_H,                     // 0x04 
KEY_G,                     // 0x05 
KEY_Z,                     // 0x06 
KEY_X,                     // 0x07 
KEY_C,                     // 0x08 
KEY_V,                     // 0x09 
KEY_Y,                     // 0x0A       // this is questionable - not normal Y code
KEY_B,                     // 0x0B 
KEY_Q,                     // 0x0C 
KEY_W,                     // 0x0D 
KEY_E,                     // 0x0E 
KEY_R,                     // 0x0F 
KEY_Y,                     // 0x10 
KEY_T,                     // 0x11 
KEY_1,                     // 0x12 
KEY_2,                     // 0x13 
KEY_3,                     // 0x14 
KEY_4,                     // 0x15 
KEY_6,                     // 0x16 
KEY_5,                     // 0x17 
KEY_EQUALS,                // 0x18 
KEY_9,                     // 0x19 
KEY_7,                     // 0x1A 
KEY_MINUS,                 // 0x1B 
KEY_8,                     // 0x1C 
KEY_0,                     // 0x1D 
KEY_RBRACKET,              // 0x1E 
KEY_O,                     // 0x1F 
KEY_U,                     // 0x20 
KEY_LBRACKET,              // 0x21 
KEY_I,                     // 0x22 
KEY_P,                     // 0x23 
KEY_RETURN,                // 0x24 
KEY_L,                     // 0x25 
KEY_J,                     // 0x26 
KEY_APOSTROPHE,            // 0x27 
KEY_K,                     // 0x28 
KEY_SEMICOLON,             // 0x29 
KEY_BACKSLASH,             // 0x2A 
KEY_COMMA,                 // 0x2B 
KEY_SLASH,                 // 0x2C 
KEY_N,                     // 0x2D 
KEY_M,                     // 0x2E 
KEY_PERIOD,                // 0x2F 
KEY_TAB,                   // 0x30 
KEY_SPACE,                 // 0x31 
KEY_TILDE,                 // 0x32 
KEY_BACKSPACE,             // 0x33 
0,                         // 0x34 //?
KEY_ESCAPE,                // 0x35 
0,                         // 0x36 //?
KEY_ALT,                   // 0x37 // best mapping for mac Cmd key
KEY_LSHIFT,                // 0x38 
KEY_CAPSLOCK,              // 0x39 
KEY_MAC_OPT,               // 0x3A // direct map mac Option key -- better than KEY_WIN_WINDOWS
KEY_CONTROL,               // 0x3B 
KEY_RSHIFT,                // 0x3C 
0,                         // 0x3D 
0,                         // 0x3E 
0,                         // 0x3F 
0,                         // 0x40 
KEY_DECIMAL,               // 0x41 
0,                         // 0x42 
KEY_MULTIPLY,              // 0x43 
0,                         // 0x44 
KEY_ADD,                   // 0x45 
KEY_SUBTRACT,              // 0x46 // secondary code?
KEY_NUMLOCK,               // 0x47 // also known as Clear on mac...
KEY_SEPARATOR,             // 0x48 // secondary code? for KPEqual
0,                         // 0x49 
0,                         // 0x4A 
KEY_DIVIDE,                // 0x4B 
KEY_NUMPADENTER,           // 0x4C 
KEY_DIVIDE,                // 0x4D // secondary code?
KEY_SUBTRACT,              // 0x4E 
0,                         // 0x4F 
0,                         // 0x50 
KEY_SEPARATOR,             // 0x51 // WHAT IS SEP?  This is KPEqual on mac.
KEY_NUMPAD0,               // 0x52 
KEY_NUMPAD1,               // 0x53 
KEY_NUMPAD2,               // 0x54 
KEY_NUMPAD3,               // 0x55 
KEY_NUMPAD4,               // 0x56 
KEY_NUMPAD5,               // 0x57 
KEY_NUMPAD6,               // 0x58 
KEY_NUMPAD7,               // 0x59 
0,                         // 0x5A 
KEY_NUMPAD8,               // 0x5B 
KEY_NUMPAD9,               // 0x5C 
0,                         // 0x5D 
0,                         // 0x5E 
0,                         // 0x5F 
KEY_F5,                    // 0x60 
KEY_F6,                    // 0x61 
KEY_F7,                    // 0x62 
KEY_F3,                    // 0x63 
KEY_F8,                    // 0x64 
KEY_F9,                    // 0x65 
0,                         // 0x66 
KEY_F11,                   // 0x67 
0,                         // 0x68 
KEY_PRINT,                 // 0x69 
0,                         // 0x6A 
KEY_SCROLLLOCK,            // 0x6B 
0,                         // 0x6C 
KEY_F10,                   // 0x6D 
0,                         // 0x6E 
KEY_F12,                   // 0x6F 
0,                         // 0x70 
KEY_PAUSE,                 // 0x71 
KEY_INSERT,                // 0x72 // also known as mac Help
KEY_HOME,                  // 0x73 
KEY_PAGE_UP,               // 0x74 
KEY_DELETE,                // 0x75 // FwdDel
KEY_F4,                    // 0x76 
KEY_END,                   // 0x77 
KEY_F2,                    // 0x78 
KEY_PAGE_DOWN,             // 0x79 
KEY_F1,                    // 0x7A 
KEY_LEFT,                  // 0x7B 
KEY_RIGHT,                 // 0x7C 
KEY_DOWN,                  // 0x7D 
KEY_UP,                    // 0x7E 
0,                         // 0x7F 
0,                         // 0x80 
0,                         // 0x81 
0,                         // 0x82 
0,                         // 0x83 
0,                         // 0x84 
0,                         // 0x85 
0,                         // 0x86 
0,                         // 0x87 
0,                         // 0x88 
0,                         // 0x89 
0,                         // 0x8A 
0,                         // 0x8B 
0,                         // 0x8C 
0,                         // 0x8D 
0,                         // 0x8E 
0,                         // 0x8F 

0,                         // 0x90 
0,                         // 0x91 
0,                         // 0x92 
0,                         // 0x93 
0,                         // 0x94 
0,                         // 0x95 
0,                         // 0x96 
0,                         // 0x97 
0,                         // 0x98 
0,                         // 0x99 
0,                         // 0x9A 
0,                         // 0x9B 
0,                         // 0x9C 
0,                         // 0x9D 
0,                         // 0x9E 
0,                         // 0x9F 

0,                         // 0xA0 
0,                         // 0xA1 
0,                         // 0xA2 
0,                         // 0xA3 
0,                         // 0xA4 
0,                         // 0xA5 
0,                         // 0xA6 
0,                         // 0xA7 
0,                         // 0xA8 
0,                         // 0xA9 
0,                         // 0xAA 
0,                         // 0xAB 
0,                         // 0xAC 
0,                         // 0xAD 
0,                         // 0xAE 
0,                         // 0xAF 
0,                         // 0xB0 
0,                         // 0xB1 
0,                         // 0xB2 
0,                         // 0xB3 
0,                         // 0xB4 
0,                         // 0xB5 
0,                         // 0xB6 
0,                         // 0xB7 
0,                         // 0xB8 
0,                         // 0xB9 
0,                         // 0xBA 
0,                         // 0xBB 
0,                         // 0xBC 
0,                         // 0xBD 
0,                         // 0xBE 
0,                         // 0xBF 
0,                         // 0xC0 
0,                         // 0xC1 
0,                         // 0xC2 
0,                         // 0xC3 
0,                         // 0xC4 
0,                         // 0xC5 
0,                         // 0xC6 
0,                         // 0xC7 
0,                         // 0xC8 
0,                         // 0xC9 
0,                         // 0xCA 
0,                         // 0xCB 
0,                         // 0xCC 
0,                         // 0xCD 
0,                         // 0xCE 
0,                         // 0xCF 
0,                         // 0xD0 
0,                         // 0xD1 
0,                         // 0xD2 
0,                         // 0xD3 
0,                         // 0xD4 
0,                         // 0xD5 
0,                         // 0xD6 
0,                         // 0xD7 
0,                         // 0xD8 
0,                         // 0xD9 
0,                         // 0xDA 
0,                         // 0xDB 
0,                         // 0xDC 
0,                         // 0xDD 
0,                         // 0xDE 
0,                         // 0xDF 
0,                         // 0xE0 
0,                         // 0xE1 
0,                         // 0xE2 
0,                         // 0xE3 
0,                         // 0xE4 

0,                         // 0xE5 

0,                         // 0xE6 
0,                         // 0xE7 
0,                         // 0xE8 
0,                         // 0xE9 
0,                         // 0xEA 
0,                         // 0xEB 
0,                         // 0xEC 
0,                         // 0xED 
0,                         // 0xEE 
0,                         // 0xEF 
   
0,                         // 0xF0 
0,                         // 0xF1 
0,                         // 0xF2 
0,                         // 0xF3 
0,                         // 0xF4 
0,                         // 0xF5 
   
0,                         // 0xF6 
0,                         // 0xF7 
0,                         // 0xF8 
0,                         // 0xF9 
0,                         // 0xFA 
0,                         // 0xFB 
0,                         // 0xFC 
0,                         // 0xFD 
0,                         // 0xFE 
0                          // 0xFF 
};   


U8 TranslateOSKeyCode(U8 vcode)
{
   return VcodeRemap[vcode];   
}   

#pragma mark ---- Clipboard functions ----
//-----------------------------------------------------------------------------
const char* Platform::getClipboard()
{
   // mac clipboards can contain multiple items,
   //  and each item can be in several differnt flavors, 
   //  such as unicode or plaintext or pdf, etc.
   // scan through the clipboard, and return the 1st piece of actual text.
   ScrapRef    clip;
   char        *retBuf = "";
   OSStatus    err = noErr;
   char        *dataBuf = "";
   
   // get a local ref to the system clipboard
   GetScrapByName( kScrapClipboardScrap, kScrapGetNamedScrap, &clip ); 
   
   
   // First try to get unicode data, then try to get plain text data.
   Size dataSize = 0;
   bool plaintext = false;
   err = GetScrapFlavorSize(clip, kScrapFlavorTypeUnicode, &dataSize);
   if( err != noErr || dataSize <= 0)
   {
      Con::errorf("some error getting unicode clip");
      plaintext = true;
      err = GetScrapFlavorSize(clip, kScrapFlavorTypeText, &dataSize);
   }

   // kick out if we don't have any data.
   if( err != noErr || dataSize <= 0)
   {
      Con::errorf("no data, kicking out. size = %i",dataSize);
      return "";
   }
   
   if( err == noErr && dataSize > 0 )
   {
      // ok, we've got something! allocate a buffer and copy it in.
      char buf[dataSize+1];
      dMemset(buf, 0, dataSize+1);
      dataBuf = buf;      
      // plain text needs no conversion.
      // unicode data needs to be converted to normalized utf-8 format.
      if(plaintext)
      { 
         GetScrapFlavorData(clip, kScrapFlavorTypeText, &dataSize, &buf);
         retBuf = Con::getReturnBuffer(dataSize + 1);
         dMemcpy(retBuf,buf,dataSize);
      }
      else
      {
         GetScrapFlavorData(clip, kScrapFlavorTypeUnicode, &dataSize, &buf);         

         // normalize
         CFStringRef cfBuf = CFStringCreateWithBytes(NULL, (const UInt8*)buf, dataSize, kCFStringEncodingUnicode, false);
         CFMutableStringRef normBuf = CFStringCreateMutableCopy(NULL, 0, cfBuf);
         CFStringNormalize(normBuf, kCFStringNormalizationFormC);

         // convert to utf-8
         U32 normBufLen = CFStringGetLength(normBuf);
         U32 retBufLen = CFStringGetMaximumSizeForEncoding(normBufLen,kCFStringEncodingUTF8) + 1; // +1 for the null terminator
         retBuf = Con::getReturnBuffer(retBufLen);
         CFStringGetCString( normBuf, retBuf, retBufLen, kCFStringEncodingUTF8);
         CFStringRef newCFstring = CFStringCreateWithBytes(NULL, retBuf, dStrlen(retBuf), kCFStringEncodingUTF8, false);
         dataSize = retBufLen;
      }

      // manually null terminate, just in case.
      retBuf[dataSize] = 0;
   }
         
    // return the data, or the empty string if we did not find any data.
         return retBuf;
}

//-----------------------------------------------------------------------------
bool Platform::setClipboard(const char *text)
{
   ScrapRef       clip;
   U32            textSize;
   OSStatus       err = noErr;
   
   // make sure we have something to copy
   textSize = dStrlen(text);
   if(textSize == 0)
      return false;
   
   // get a local ref to the system clipboard
   GetScrapByName( kScrapClipboardScrap, kScrapClearNamedScrap, &clip );

   // put the data on the clipboard as text
   err = PutScrapFlavor( clip, kScrapFlavorTypeText, kScrapFlavorMaskNone, textSize, text);
   
   // put the data on the clipboard as unicode
   const UTF16 *utf16Data = convertUTF8toUTF16(text);
   err |= PutScrapFlavor( clip, kScrapFlavorTypeUnicode, kScrapFlavorMaskNone,
                        dStrlen(utf16Data) * sizeof(UTF16), utf16Data);
   delete [] utf16Data;

   // and see if we were successful.
   if( err == noErr )
      return true;
   else
      return false;
}
#pragma mark -

//------------------------------------------------------------------------------
bool enableKeyboard()
{
   if ( !gInputEnabled )
      return( false );

   if ( gKBEnabled && gKBActive )
      return( true );

   gKBEnabled = true;
   if ( Input::isActive() )
      gKBEnabled = activateKeyboard();

   if ( gKBEnabled )
   {
      Con::printf( "Hardware-direct keyboard enabled." );
#ifdef LOG_INPUT
      Input::log( "Keyboard enabled.\n" );
#endif
   }
   else
   {
      Con::warnf( "Hardware-direct keyboard failed to enable!" );
#ifdef LOG_INPUT
      Input::log( "Keyboard failed to enable!\n" );
#endif
   }

   return( gKBEnabled );
}

//------------------------------------------------------------------------------
void disableKeyboard()
{
   if ( !gInputEnabled || !gKBEnabled )
      return;

   deactivateKeyboard();
   gKBEnabled = false;

   Con::printf( "Hardware-direct keyboard disabled." );
#ifdef LOG_INPUT
   Input::log( "Keyboard disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool activateKeyboard()
{
   if ( !gInputEnabled || !Input::isActive() || !gKBEnabled )
      return( false );

   OSStatus status = noErr;
   if (status==noErr)
      gKBActive = true;

#ifdef LOG_INPUT
   Input::log( gKBActive ? "Keyboard activated.\n" : "Keyboard failed to activate!\n" );
#endif
   return( gKBActive );
}

//------------------------------------------------------------------------------
void deactivateKeyboard()
{
   if ( gInputEnabled && gKBActive )
   {
      OSStatus status = noErr;
      gKBActive = false;

#ifdef LOG_INPUT
      Input::log( "Keyboard deactivated.\n" );
#endif
   }
}

//------------------------------------------------------------------------------
bool enableMouse()
{
   if ( !gInputEnabled )
      return( false );

   if ( gMouseEnabled && gMouseActive )
      return( true );

   gMouseEnabled = activateMouse();

   bool hwMouse = false;
      
//   if (gMouseEnabled)
//      Con::printf( "%s %s", hwMouse?"Hardware-direct mouse":"Basic mouse capture", "enabled.");
//   else
//      Con::warnf( "%s %s", hwMouse?"Hardware-direct mouse":"Basic mouse capture", "failed to enable!");
#ifdef LOG_INPUT
   Input::log( "Mouse %s.\n", gMouseEnabled?"enabled":"failed to enable");
#endif

   return( gMouseEnabled );
}

//------------------------------------------------------------------------------
void disableMouse()
{
   if ( !gInputEnabled || !gMouseEnabled )
      return;

   deactivateMouse();
   gMouseEnabled = false;

   bool hwMouse = false;
   Con::printf( "%s disabled", hwMouse?"Hardware-direct mouse":"Basic mouse capture");
#ifdef LOG_INPUT
   Input::log( "Mouse disabled.\n" );
#endif
}

//------------------------------------------------------------------------------
bool activateMouse()
{
   if ( !gInputEnabled || !Input::isActive() || !gMouseEnabled )
      return( false );
   
   if (gMouseActive)
      return(true);

   gMouseActive = true;

#ifdef LOG_INPUT
   Input::log( gMouseActive ? "Mouse activated.\n" : "Mouse failed to activate!\n" );
#endif
   return( gMouseActive );
}

//------------------------------------------------------------------------------
void deactivateMouse()
{
//   Con::printf( ">> dm one. %s %s %s", gInputEnabled?"true":"false", gMouseActive?"true":"false");
   if ( !gInputEnabled || !gMouseActive ) return;
   
   gMouseActive = false;

//   Con::printf( ">> dm two." );

#ifdef LOG_INPUT
   Input::log( "Mouse deactivated.\n" );
#endif
}


//------------------------------------------------------------------------------
ConsoleFunction( enableMouse, bool, 1, 1, "enableMouse()" )
{
   return( enableMouse() );
}

//------------------------------------------------------------------------------
ConsoleFunction( disableMouse, void, 1, 1, "disableMouse()" )
{
   disableMouse();
}

//------------------------------------------------------------------------------
void printInputState(void)
{
   if ( gInputEnabled )
   {
         Con::printf( "Low-level input system is enabled." );
      
      Con::printf( "- Keyboard is %sabled and %sactive.", 
            gKBEnabled ? "en" : "dis",
            gKBActive ? "" : "in" );
      Con::printf( "- Mouse is %sabled and %sactive.", 
            gMouseEnabled ? "en" : "dis",
            gMouseActive ? "" : "in" );
/*
      Con::printf( "- Joystick is %sabled and %sactive.", 
            gJoystickEnabled() ? "en" : "dis",
            gJoystickActive() ? "" : "in" );
*/
   }
   else
   {
      Con::printf( "Low-level input system is disabled." );
   }
}

//------------------------------------------------------------------------------
ConsoleFunction( echoInputState, void, 1, 1, "echoInputState()" )
{
   printInputState();
}

//------------------------------------------------------------------------------
ConsoleFunction( toggleInputState, void, 1, 1, "toggleInputState()" )
{
   if (gInputEnabled)
      Input::disable();
   else
      Input::enable();

   printInputState();
}

//------------------------------------------------------------------------------
ConsoleFunction( deactivateKeyboard, void, 1, 1, "deactivateKeyboard();")
{
   // these are only useful on the windows side. They deal with some vagaries of win32 DirectInput.
}

ConsoleFunction( activateKeyboard, void, 1, 1, "activateKeyboard();")
{
   // these are only useful on the windows side. They deal with some vagaries of win32 DirectInput.
}

//------------------------------------------------------------------------------
void Input::setCursorPos(S32 x, S32 y)
{
   if (!platState.appWindow )
      return;
      
   Rect r;
   Point targetPoint;
   GrafPtr savePort;
   
   // get the center of the window
   GetWindowBounds(platState.appWindow, kWindowContentRgn, &r);
   targetPoint.h = r.left + x;
   targetPoint.v = r.top + y;
         
   CGDirectDisplayID displayID = platState.cgDisplay;

   CGRect bounds = CGDisplayBounds(displayID);
   CGPoint cgWndCenter;
   cgWndCenter.x =  targetPoint.h + bounds.origin.x;
   cgWndCenter.y =  targetPoint.v + bounds.origin.y;
   CGSetLocalEventsSuppressionInterval(0);
   CGDisplayMoveCursorToPoint(displayID, cgWndCenter);
   CGSetLocalEventsSuppressionInterval(0.25);
}
