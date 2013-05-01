//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX 
//    Changes:
//        cursor -- improved handling of cursor when windowed and fullscreen.
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "platformMacCarb/platformMacCarb.h"
#include "platform/event.h"
#include "platform/platformInput.h"
#include "platform/gameInterface.h"
#include "platform/platformThread.h"
#include "platform/platformVideo.h"

#include "platformMacCarb/macCarbUtil.h"
#include "platformMacCarb/macCarbEvents.h"
#include "platformMacCarb/macCarbAlerts.h"
#include "gui/core/guiCanvas.h"
#include <pthread.h>

EventHandlerRef gWinEventHandlerRef       = NULL;
EventHandlerRef gAppEventHandlerRef       = NULL;
EventHandlerRef gTextEventHandlerRef      = NULL;
EventHandlerRef gWinMouseEventHandlerRef  = NULL;
EventHandlerRef gAppMouseEventHandlerRef  = NULL;
EventHandlerRef gAppAEEventHandlerRef     = NULL;
EventHandlerRef gWinCloseEventHandlerRef  = NULL;
EventHandlerRef gTorqueEventHandlerRef     = NULL;

#pragma mark ---- Mouse Management ----
//-----------------------------------------------------------------------------
// Hides or shows the system mouse cursor, and tracks the hidden state.
void MacCarbSetHideCursor(bool shouldHide)
{
   if(CGCursorIsVisible() == !shouldHide)
      return;
      
   if(shouldHide)
      CGDisplayHideCursor(platState.cgDisplay);
   else
      CGDisplayShowCursor(platState.cgDisplay);
}

//-----------------------------------------------------------------------------
// Hides the mouse cursor when the mouse is locked, when we are in fullscreen
// mode, and when the cursor is inside the window bounds.
// Optionally takes the cursor location, in window local coords.
void MacCarbCheckHideCursor(S32 x=-1, S32 y=-1)
{
   bool shouldHide = false;
   if(platState.mouseLocked || Video::isFullScreen())
   {
      shouldHide = true;
   }
   else
   {
      Point2I win = Platform::getWindowSize();

      if(x >=0 && y>=0 && x<= win.x && y<= win.y)
         shouldHide = true;
   }

   MacCarbSetHideCursor(shouldHide && !platState.minimized);
}

//-----------------------------------------------------------------------------
// Utility func that sets the mouse position to the center of the appWindow
static void _MacCarbRecenterCapturedMouse(void)
{
   if (!platState.appWindow || !platState.mouseLocked)
      return;
      
   Rect r;
   Point wndCenter;
   GrafPtr savePort;
   
   // get the center of the window
   GetWindowBounds(platState.appWindow,kWindowContentRgn,&r);
   wndCenter.h = r.left + (r.right-r.left) / 2;
   wndCenter.v = r.top + (r.bottom-r.top) / 2;
      
   CGRect bounds = CGDisplayBounds(platState.cgDisplay);
   CGPoint cgWndCenter;
   cgWndCenter.x =  wndCenter.h + bounds.origin.x;
   cgWndCenter.y =  wndCenter.v + bounds.origin.y;
   CGDisplayMoveCursorToPoint(platState.cgDisplay, cgWndCenter);
}

//-----------------------------------------------------------------------------
// Utility func that gets appWindow local coords from a Carbon mouse event.
static Point _MacCarbGetMouseEventLocalCoords(EventRef theEvent)
{
   // get the global coords
   Point ret;
   GetEventParameter (theEvent, kEventParamMouseLocation, typeQDPoint,
                      NULL, sizeof(Point), NULL, &ret);
   
   //  return global coords if we have no window.
   if(!platState.appWindow)
      return ret;
   
   // get the window's content rect
   Rect bounds;
   GetWindowBounds(platState.appWindow, kWindowContentRgn, &bounds);
   // convert to local coords
   ret.h -= bounds.left;
   ret.v -= bounds.top; 
   
   return ret;
}


//-----------------------------------------------------------------------------
// Sets whether the mouse is locked to the appWindow.
// Function name is a little deceptive, but this directly affects nary a window.
void Platform::setWindowLocked(bool locked)
{
   if(platState.mouseLocked == locked)
      return;

   platState.mouseLocked = locked;

   // center the mouse when we lock.
   // AFX CODE BLOCK (cursor) <<
   static Point stash_mouse_loc;
   if (platState.mouseLocked)
   {
      GetMouse(&stash_mouse_loc);
      _MacCarbRecenterCapturedMouse();
   }
   else
   {
     CGPoint cg_pos; cg_pos.x = stash_mouse_loc.h; cg_pos.y = stash_mouse_loc.v;
     CGSetLocalEventsSuppressionInterval(0);
		 CGDisplayMoveCursorToPoint(platState.cgDisplay, cg_pos);
		 CGSetLocalEventsSuppressionInterval(0.25);     
   }
   /* ORIGINAL CODE
   if (platState.mouseLocked)
      _MacCarbRecenterCapturedMouse();
   */
   // AFX CODE BLOCK (cursor) >>
}

#pragma mark -
#pragma mark ---- Event Handlers ----

//-----------------------------------------------------------------------------
/// Returns the Torque modifier keys of a Carbon event.
static U32 _MacCarbGetEventModifierKeys(EventRef theEvent)
{
   UInt32 keyMods = 0;
   GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32,
                     NULL, sizeof(UInt32), NULL, &keyMods);
   
   U32 modifierKeys = 0;
   if (keyMods & shiftKey)          modifierKeys |= SI_LSHIFT;
   if (keyMods & rightShiftKey)     modifierKeys |= SI_RSHIFT;
   if (keyMods & cmdKey)            modifierKeys |= SI_LALT;
   if (keyMods & optionKey)         modifierKeys |= SI_MAC_LOPT;
   if (keyMods & rightOptionKey)    modifierKeys |= SI_MAC_ROPT;
   if (keyMods & controlKey)        modifierKeys |= SI_LCTRL;
   if (keyMods & rightControlKey)   modifierKeys |= SI_RCTRL;
   
   return modifierKeys;
}

//-----------------------------------------------------------------------------
static void _OnActivate(bool activating)
{
   if(activating)
   {
      Input::activate();
      Game->refreshWindow();
      platState.backgrounded = false;
      pthread_kill(platState.torqueThreadId, SIGALRM);
   }
   else
   {
      Input::deactivate();
      platState.backgrounded = true;
   }
}


//--------------------------------------
static void _OnMouseUpDown(EventRef theEvent, bool down, U32 modifiers )
{
   EventMouseButton whatButton;
   GetEventParameter (theEvent, kEventParamMouseButton, typeMouseButton,
                     NULL, sizeof(EventMouseButton), NULL, &whatButton);

   InputEvent event;
   event.deviceType = MouseDeviceType;
   event.deviceInst = 0;
   event.objType    = SI_BUTTON;
   event.objInst    = KEY_BUTTON0 + whatButton - 1;
   event.modifier   = modifiers;
   event.ascii      = 0;
   event.action     = down ? SI_MAKE : SI_BREAK;
   event.fValue     = down ? 1.0 : 0.0;

   Game->postEvent(event);
}

//--------------------------------------
static void _OnMouseMovedDragged(EventRef theEvent, bool drag, U32 modifiers)
{
   // When the mouse is locked, we deliver events in deltas.
   // When not locked, we deliver in window-local coords.
   // When not locked, a MouseMoveEvent instead of an InputEvent is sent.
   // Discard the first 2 events after locking, to avoid spurious delta values.
   static U32 discardEventCount = 2;
   // AFX CODE BLOCK (cursor) <<
   if (!platState.mouseLocked)
   {
      // convert to local window coords if possible.
      Point where = _MacCarbGetMouseEventLocalCoords(theEvent);
      // deliver the event
      MouseMoveEvent event;
      event.xPos = where.h;
      event.yPos = where.v;
      event.modifier = modifiers;
      Game->postEvent(event);
      MacCarbCheckHideCursor(where.h, where.v);
      discardEventCount = 2;
   }
   else if(discardEventCount > 0)
   {
      discardEventCount--;
      CGMouseDelta mdx, mdy;
      CGGetLastMouseDelta(&mdx, &mdy);
   }
   else
   {
      InputEvent event;
      event.deviceType = MouseDeviceType;
      event.deviceInst = 0;
      event.objInst    = 0;
      event.modifier   = modifiers;
      event.ascii      = 0;
      event.action     = SI_MOVE;

      // get the deltas
      CGMouseDelta mdx, mdy;
      CGGetLastMouseDelta(&mdx, &mdy);
      
      // deliver the x and y events separately. 
      if(mdx != 0)
      {
         event.objType = SI_XAXIS;
         event.fValue  = F32(mdx);
         Game->postEvent(event);
      }
      if(mdy != 0)
      {
         event.objType = SI_YAXIS;
         event.fValue  = F32(mdy);
         Game->postEvent(event);
      }
      
      // recenter the mouse
      _MacCarbRecenterCapturedMouse();
      // hide the mouse if needed
      MacCarbCheckHideCursor();
   }
   /* ORIGINAL CODE
   if(platState.mouseLocked)
   {
      InputEvent event;
      event.deviceType = MouseDeviceType;
      event.deviceInst = 0;
      event.objInst    = 0;
      event.modifier   = modifiers;
      event.ascii      = 0;
      event.action     = SI_MOVE;

      // get the deltas
      CGMouseDelta mdx, mdy;
      CGGetLastMouseDelta(&mdx, &mdy);
      
      // deliver the x and y events separately. 
      if(mdx != 0)
      {
         event.objType = SI_XAXIS;
         event.fValue  = F32(mdx);
         Game->postEvent(event);
      }
      if(mdy != 0)
      {
         event.objType = SI_YAXIS;
         event.fValue  = F32(mdy);
         Game->postEvent(event);
      }
      
      // recenter the mouse
      _MacCarbRecenterCapturedMouse();
      // hide the mouse if needed
      MacCarbCheckHideCursor();
      discardEventCount = 2;
   }
   else if(discardEventCount > 0)
   {
      discardEventCount--;
   }
   else
   {
      // convert to local window coords if possible.
      Point where = _MacCarbGetMouseEventLocalCoords(theEvent);
      // deliver the event
      MouseMoveEvent event;
      event.xPos = where.h;
      event.yPos = where.v;
      event.modifier = modifiers;
      Game->postEvent(event);
      MacCarbCheckHideCursor(where.h, where.v);
   }
   */
   // AFX CODE BLOCK (cursor) >>
}

static void _OnMouseWheelMoved(EventRef theEvent, U32 modifiers)
{
   InputEvent event;

   S32 dWheel = 0;
   GetEventParameter (theEvent, kEventParamMouseWheelDelta, typeLongInteger,
                      NULL, sizeof(long), NULL, &dWheel);
   
   event.deviceType = MouseDeviceType;
   event.deviceInst = 0;
   event.objType    = SI_ZAXIS;
   event.objInst    = 0;
   event.modifier   = modifiers; 
   event.ascii      = 0; 
   event.action     = SI_MOVE;
   event.fValue     = dWheel * kTMouseWheelMagnificationFactor;
   Game->postEvent(event);
}

//--------------------------------------
static OSStatus _OnWindowEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
   OSStatus result = noErr;
   
   // event class will always be kEventClassWindow in this handler.
   UInt32 eventKind = GetEventKind(theEvent);
   
   // the window the event is directed to
   WindowRef theWind;
   GetEventParameter (theEvent, kEventParamDirectObject, typeWindowRef,
                    NULL, sizeof(WindowRef), NULL, &theWind);

   // try to snuff the event if the window is no longer valid.
   if(!IsValidWindowPtr(theWind))
      return noErr;
   
   // whether to propagate the event up the chain. only some events should be
   // proagated up the responder chain.
   bool propagateEvent = false;

   switch(eventKind)
   {
      case kEventWindowDrawContent:
         // we must redraw the entire window
         Game->refreshWindow();
         break;

      case kEventWindowActivated:
      case kEventWindowDeactivated:
         // window was switched into or out of the foreground
         _OnActivate(eventKind==kEventWindowActivated);
         // claim we did not handle, so that the event may propagate.
         result = eventNotHandledErr;
         propagateEvent = true;
         break;

      case kEventWindowGetClickActivation:
         // this is the activating click, just update the cursor position,
         // and then bump the event to the standard handler
         result = eventNotHandledErr;

         // if the mouse is locked, we don't need the new position.
         // just skip the event & let the system feed us an activate event.
         if(platState.mouseLocked)
              break;

         // get and update the mouse position
         Point where = _MacCarbGetMouseEventLocalCoords(theEvent);
         Canvas->setCursorPos(Point2I(where.h,where.v));
         break;

      case kEventWindowTransitionCompleted:
         // a window transition has completed.
         U32 transition;
         GetEventParameter(theEvent, kEventParamWindowTransitionAction, typeWindowTransitionAction,
                        NULL, sizeof(bool), NULL, &transition);

         if(transition == kWindowHideTransitionAction)
         {
            MacCarbSendTorqueEventToMain(kEventTorqueReleaseWindow, theWind);
         }            
         break;
         
      case kEventWindowBoundsChanged:
      case kEventWindowZoomed:
      {
         Rect rectPort;
         GetWindowPortBounds(theWind, &rectPort);
         Platform::setWindowSize( rectPort.right-rectPort.left , rectPort.bottom-rectPort.top );

         // tell the torque thread to update the AGL context
         platState.ctxNeedsUpdate=true;
         break;
      }
         
      case kEventWindowExpanded:
         platState.minimized = false;
         break;
      case kEventWindowCollapsed:
         platState.minimized = true;
         break;

      default:
         AssertFatal(false,"Unknown window event");
         result = eventNotHandledErr;
         break;
   }
   
   // Now propagate the event to the next handler (in this case the standard window handler)
   if (nextHandler && propagateEvent)
      result = CallNextEventHandler (nextHandler, theEvent);

   return result;
}

//--------------------------------------
OSStatus _OnWindowCloseEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
   // the window the event is directed to
   WindowRef theWind;
   GetEventParameter (theEvent, kEventParamDirectObject, typeWindowRef,
                    NULL, sizeof(WindowRef), NULL, &theWind);

   // try to snuff the event if the window is no longer valid.
   if(!IsValidWindowPtr(theWind))
      return noErr;

   // be sure we have the right event type for this handler.
   U32 eventKind = GetEventKind(theEvent);

   switch(eventKind)
   {
      case kEventWindowClose:
      {
         Platform::postQuitMessage(0);
         return noErr;
      }
      default:
         return eventNotHandledErr;
   }
}

//--------------------------------------
//static OSStatus _OnWindowMouseEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
//{
//   Con::errorf("_WinMouseCarbonEventHandle");
//   OSStatus result = noErr;
//   
//   // event class will always be kEventClassMouse in this handler.
//   UInt32 eventKind = GetEventKind(theEvent);
//   
//   U32 modifierKeys = _MacCarbGetEventModifierKeys(theEvent);
//   
//   // whether to propagate the event up the chain. only some events should be
//   // proagated up the responder chain.
//   bool propagateEvent = false;
//   bool down = false;
//   
//   switch(eventKind)
//   {
//      case kEventMouseDown:
//         down = true;
//         // fall through 
//      case kEventMouseUp:
//         // mouse down must propagate so that the standard handler can 
//         // handle window activation.
//         propagateEvent = down;
//         _handleMouseUpDown(theEvent, down, modifierKeys);
//         break;
//         
//      case kEventMouseDragged:
//         down = true;
//         // fall through
//      case kEventMouseMoved:
//         _handleMouseMovedDragged(theEvent, down, modifierKeys);
//         break;
//         
//      case kEventMouseWheelMoved:
//         _handleMouseWheelMoved(theEvent, modifierKeys);
//         break;
//         
//      default:
//         result = eventNotHandledErr;
//         break;
//   }
//   
//   // Now propagate the event to the next handler (in this case the standard window handler)
//   if (nextHandler && propagateEvent)
//      result = CallNextEventHandler (nextHandler, theEvent);
//   
//   return result;
//}

//--------------------------------------
static OSStatus _OnAppEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
   OSStatus result = noErr; // Function result
   
   // event class will always be kEventClassApplication in this handler.
   UInt32 eventKind = GetEventKind(theEvent);
   
   // only a few events should be propagated up the responder chain.
   bool propagateEvent = false;
   
   switch(eventKind)
   {
      case kEventAppQuit:
         // quit command from menu
         Platform::postQuitMessage(0);
         propagateEvent = true;
         break;

      case kEventAppActivated:
      case kEventAppDeactivated:
         // app was switched into or out of the foreground
         _OnActivate(eventKind==kEventAppActivated);
         // claim we did not handle, so that the event may propagate.
         result = eventNotHandledErr;
         break;
         
      default:
         result = eventNotHandledErr;
         break;
   }

   if( nextHandler && propagateEvent )
         result = CallNextEventHandler (nextHandler, theEvent);

   return result;
}
   
//--------------------------------------
static OSStatus _OnAppMouseEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
   OSStatus result = noErr;
   
   // event class will always be kEventClassMouse in this handler.
   UInt32 eventKind = GetEventKind(theEvent);
   
   U32 modifierKeys = _MacCarbGetEventModifierKeys(theEvent);
   
   // whether to propagate the event up the chain. only some events should be
   // proagated up the responder chain.
   bool propagateEvent = false;
   bool down = false;
   
   switch(eventKind)
   {
      case kEventMouseDown:
         down = true;
         // fall through 
      case kEventMouseUp:
         // mouse down must propagate so that the standard handler can 
         // handle window activation.
         propagateEvent = down;
         _OnMouseUpDown(theEvent, down, modifierKeys);
         break;
         
      case kEventMouseDragged:
         down = true;
         // fall through
      case kEventMouseMoved:
         _OnMouseMovedDragged(theEvent, down, modifierKeys);
         break;
         
      case kEventMouseWheelMoved:
         _OnMouseWheelMoved(theEvent, modifierKeys);
         break;
         
      default:
         result = eventNotHandledErr;
         break;
   }
   
   // Now propagate the event to the next handler (in this case the standard window handler)
   if (nextHandler && propagateEvent)
      result = CallNextEventHandler (nextHandler, theEvent);
   
   return result;
}
//--------------------------------------
static OSStatus _OnAppleEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
   OSStatus result = noErr; // Function result
   
   // event class will always be kEventClassAppleEvent in this handler.
   UInt32 eventKind = GetEventKind(theEvent);
   
   // only a few events should be propagated up the responder chain.
   bool propagateEvent = false;

   AEEventID aeType;
   GetEventParameter (theEvent, kEventParamAEEventID, typeType,
                       NULL, sizeof(OSType), NULL, &aeType);

   switch(aeType)
   {
      case kAEQuitApplication:
      {
         Platform::postQuitMessage(0);
         propagateEvent = true;
         break;
      }

      default:
         result = eventNotHandledErr;
         break;
   }

   if( nextHandler && propagateEvent )
   {
         result = CallNextEventHandler (nextHandler, theEvent);
   }


   return result;
}

//--------------------------------------
// here we manually poll for events, and send them to the dispatcher.
// we only use this in single-threaded mode.
static void _MacCarbPollEvents()
{
   EventRef carbonEvent;
   EventTargetRef theTarget;
   OSStatus theErr;
   
   theTarget = GetEventDispatcherTarget();
   do
   {
      theErr = ReceiveNextEvent(0, NULL, kEventDurationNoWait, true, &carbonEvent);
      if (theErr == noErr && carbonEvent != NULL)
      {
         theErr = SendEventToEventTarget(carbonEvent, theTarget);
         ReleaseEvent(carbonEvent);
      }
   }
   while (theErr == noErr);
}


//--------------------------------------
void Platform::process()
{
   // TODO: HID input

   // ProcessMessages() manually polls for events when we are single threaded
   if(Thread::getCurrentThreadId() == platState.firstThreadId)
      _MacCarbPollEvents();
      
   // Some things do not get carbon events, we must always poll for them.
   // HID ( usb gamepad et al ) input, for instance.
   Input::process();
   
   if(platState.ctxNeedsUpdate)
   {
      aglUpdateContext(platState.ctx);
      platState.ctxNeedsUpdate=false;
   }
}

#pragma mark -
#pragma mark ---- Mac Torque Events ----

//--------------------------------------
void MacCarbSendTorqueEventToMain( U32 eventKind, void* userData )
{
   EventRef theEvent;
   CreateEvent(NULL, kEventClassTorque, eventKind, 0, kEventAttributeNone, &theEvent);
   SetEventParameter(theEvent, kEventParamTorqueData, typeVoidPtr, sizeof(void*),&userData);
   
   OSStatus err = PostEventToQueue(platState.mainEventQueue, theEvent, kEventPriorityStandard);
   if(err != noErr)
      Platform::forceShutdown(-1);
}

//--------------------------------------
static OSStatus _OnTorqueEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
   UInt32 eventKind = GetEventKind(theEvent);
   void* torqueData;
   GetEventParameter(theEvent, kEventParamTorqueData, typeVoidPtr,
                     NULL, sizeof(void*), NULL, &torqueData);
   
   switch( eventKind )
   {
      case kEventTorqueAlert:
         MacCarbRunAlertMain();
         break;

      case kEventTorqueFadeInWindow:
         MacCarbFadeInWindow((WindowPtr)torqueData);
         break;
      
      case kEventTorqueFadeOutWindow:
         MacCarbFadeAndReleaseWindow((WindowPtr)torqueData);
         break;
         
      case kEventTorqueReleaseWindow:
         ReleaseWindow((WindowPtr)torqueData);
         break;
         
      case kEventTorqueShowMenuBar:
         MacCarbShowMenuBar(torqueData);
         break;
   }
   return noErr;
}

#pragma mark -
#pragma mark ---- Keyboard input ----

//--------------------------------------
void Platform::enableKeyboardTranslation(void)
{
   ActivateTSMDocument(platState.tsmDoc);
   platState.tsmActive=true;
}

//--------------------------------------
void Platform::disableKeyboardTranslation(void)
{
   DeactivateTSMDocument(platState.tsmDoc);
   platState.tsmActive=false;
}

//--------------------------------------
static void _OnRawKey(EventRef theEvent, InputEvent &torqueEvent)
{
   // --------- modifiers ---------
   UInt32 keyMods = 0;
   GetEventParameter (theEvent, kEventParamKeyModifiers, typeUInt32,
                     NULL, sizeof(UInt32), NULL, &keyMods);
   
   U8 modifierKeys = 0;
   if (keyMods & shiftKey)          modifierKeys |= SI_LSHIFT;
   if (keyMods & rightShiftKey)     modifierKeys |= SI_RSHIFT;
   if (keyMods & cmdKey)            modifierKeys |= SI_LALT;
   if (keyMods & optionKey)         modifierKeys |= SI_MAC_LOPT;
   if (keyMods & rightOptionKey)    modifierKeys |= SI_MAC_ROPT;
   if (keyMods & controlKey)        modifierKeys |= SI_LCTRL;
   if (keyMods & rightControlKey)   modifierKeys |= SI_RCTRL;
   // --
   
   // ------- keycode & keychar -----
   UInt32   keyCode = 0;
   char     keyChar = 0;
   
   GetEventParameter (theEvent, kEventParamKeyCode, typeUInt32, NULL, sizeof(UInt32), NULL, &keyCode);
   GetEventParameter (theEvent, kEventParamKeyMacCharCodes, typeChar,NULL, sizeof(char), NULL, &keyChar);
   // -- 

   // ------- action & value ( make, break, repeat ) -----
   U8 action;
   float fvalue;
   U32 eventKind = GetEventKind(theEvent);
   switch(eventKind)
   {
      case kEventRawKeyDown:
         action = SI_MAKE;
         fvalue = 1.0f;
         break;
      case kEventRawKeyUp:
         action = SI_BREAK;
         fvalue = 0.0f;
         break;
      case kEventRawKeyRepeat:
         action = SI_REPEAT;
         fvalue = 1.0f;
         break;
      default:
         AssertISV(false, "Unhandled keyboard event kind!");
   }
   // --
   
   //---- fill the event ----
   torqueEvent.deviceType  = KeyboardDeviceType;
   torqueEvent.deviceInst  = 0;
   torqueEvent.objType     = SI_KEY;
   torqueEvent.objInst     = TranslateOSKeyCode(keyCode);
   torqueEvent.modifier    = modifierKeys;
   torqueEvent.ascii       = 0;
   torqueEvent.action      = action;
   torqueEvent.fValue      = fvalue;
   
}

static OSStatus _OnTextInput(EventRef theEvent)
{
   UniChar  *text = NULL;
   UInt32   textLen;
   U32      charCount = 0;

   EventRef rawEvent;
   InputEvent rawTorqueEvent;
   rawTorqueEvent.objInst  = 0;
   rawTorqueEvent.modifier = 0;
   rawTorqueEvent.action   = SI_MAKE;
   rawTorqueEvent.fValue   = 1.0f;
   
   U32 eventKind = GetEventKind(theEvent);
    switch(eventKind)
   {
      case kEventTextInputUnicodeForKeyEvent:
      {
         // get the input string from the text input event
         // first get the number of bytes required
         GetEventParameter( theEvent, kEventParamTextInputSendText, typeUnicodeText, NULL, 0, &textLen, NULL);
         charCount = textLen / sizeof(UniChar);
         text = new UniChar[charCount];

         // now that we've allocated space, get the buffer of text from the input method or keyboard.
         GetEventParameter( theEvent, kEventParamTextInputSendText, typeUnicodeText, NULL, textLen, NULL, text);
         
         // now trap the raw event.
         OSStatus err = 
         GetEventParameter( theEvent, kEventParamTextInputSendKeyboardEvent, typeEventRef, NULL, sizeof(EventRef), NULL, &rawEvent);
         if( err==noErr) 
         {
             _OnRawKey(rawEvent,rawTorqueEvent);
         }
         
         break;
      }
      default:  // if we somehow get the wrong kind of event, let someone else handle it.
      {
         return eventNotHandledErr;
      }
   }
   
   
   // make torque events to enter that string.
   InputEvent torqueEvent;
   torqueEvent.deviceType  = KeyboardDeviceType;
   torqueEvent.deviceInst  = 0;
   torqueEvent.objType     = SI_KEY;
   torqueEvent.objInst     = rawTorqueEvent.objInst;
   torqueEvent.modifier    = rawTorqueEvent.modifier;
   torqueEvent.action      = rawTorqueEvent.action;
   torqueEvent.fValue      = rawTorqueEvent.fValue;

   for( int i=0; i < charCount; i++)
   {
      torqueEvent.ascii = text[i];
      Game->postEvent(torqueEvent);
   }

   return noErr;
}

//-----------------------------------------------------------------------------
static OSStatus _OnRawModifiers(EventRef theEvent)
{
   static U32 oldModState = 0L;
   U32 mods;
   GetEventParameter(theEvent, kEventParamKeyModifiers, 
                     typeUInt32, NULL, sizeof(UInt32), NULL, &mods);

   oldModState = mods;

   return noErr;  
}


//--------------------------------------
static OSStatus _OnKeyboardEvent(EventHandlerCallRef nextHandler, EventRef theEvent, void* userData)
{
   // be sure we have the right event type for this handler.
   U32 eventClass = GetEventClass(theEvent);
   U32 eventKind  = GetEventKind(theEvent);

   switch(eventClass)
   {
      case kEventClassTextInput:
      {
         return _OnTextInput(theEvent);
         break;
      }
      case kEventClassKeyboard:
      {
         if(eventKind == kEventRawKeyModifiersChanged)
         {
            return _OnRawModifiers(theEvent);
            break;
         }
         
         InputEvent keyEvent;
         _OnRawKey( theEvent, keyEvent);

         // This is hackish and requires explanation: 
         // We don't get key-up events as text events, only key-down and key-repeat events. 
         // So we explicitly post key-up events, even if text translation is active.
         if(platState.tsmActive && keyEvent.action != SI_BREAK)
            return eventNotHandledErr;
         
         Game->postEvent(keyEvent);
         return noErr;
         break;
     }
      default:
         return eventNotHandledErr;
   }
   
}

#pragma mark -
#pragma mark ---- Event Install/Remove ----
//--------------------------------------
// Install event handlers for communication between Torque and the first thread.
// Since the first thread must handle certain events and system calls, we have
// to have a way to send commands to that very first thread.
void MacCarbInstallTorqueCarbonEventHandlers()
{
   // list the events we want to recieve in each event handler
   int c = 0;
   const int MAX_EVENT_TYPES = 16; // arbitrary value.

   static EventTypeSpec    torqueEventTypes[MAX_EVENT_TYPES];
   static EventHandlerUPP  torqueHandlerUPP = NULL;
   static int              torqueEventCount = 0;

   if (torqueHandlerUPP==NULL)
   {
      c = 0;
      torqueEventTypes[c].eventClass = kEventClassTorque;
      torqueEventTypes[c++].eventKind = kEventTorqueAlert;
      torqueEventTypes[c].eventClass = kEventClassTorque;
      torqueEventTypes[c++].eventKind = kEventTorqueFadeInWindow;
      torqueEventTypes[c].eventClass = kEventClassTorque;
      torqueEventTypes[c++].eventKind = kEventTorqueFadeOutWindow;
      torqueEventTypes[c].eventClass = kEventClassTorque;
      torqueEventTypes[c++].eventKind = kEventTorqueReleaseWindow;
      torqueEventTypes[c].eventClass = kEventClassTorque;
      torqueEventTypes[c++].eventKind = kEventTorqueShowMenuBar;

      torqueHandlerUPP = NewEventHandlerUPP(_OnTorqueEvent);
      torqueEventCount = c;
   }

   // we only install this one once, because we never remove it.
   if(!gTorqueEventHandlerRef)
      InstallEventHandler(GetApplicationEventTarget(), torqueHandlerUPP, torqueEventCount, torqueEventTypes, NULL, &gTorqueEventHandlerRef);

}

//--------------------------------------
void MacCarbInstallCarbonEventHandlers()
{
   // we do not have an appWindow if we are in fullscreen mode - be aware.
   EventTargetRef winTarg = NULL;
   if(platState.appWindow)
      winTarg = GetWindowEventTarget(platState.appWindow);

   // list the events we want to recieve in each event handler
   int c = 0;
   const int MAX_EVENT_TYPES = 16; // arbitrary value.

   static EventTypeSpec    winEventTypes[MAX_EVENT_TYPES];
   static EventHandlerUPP  winHandlerUPP = NULL;
   static int              winEventCount = 0;
   
   static EventTypeSpec    appEventTypes[MAX_EVENT_TYPES];
   static EventHandlerUPP  appHandlerUPP = NULL;
   static int              appEventCount = 0;

   static EventTypeSpec    appMouseEventTypes[MAX_EVENT_TYPES];
   static EventHandlerUPP  appMouseHandlerUPP = NULL;
   static int              appMouseEventCount = 0;
   
   static EventTypeSpec    appAEEventTypes[MAX_EVENT_TYPES];
   static EventHandlerUPP  appAEHandlerUPP = NULL;
   static int              appAEEventCount = 0;

   static EventTypeSpec    textEventTypes[MAX_EVENT_TYPES];
   static EventHandlerUPP  textHandlerUPP = NULL;
   static int              textEventCount = 0;

   static EventTypeSpec    closeEventTypes[MAX_EVENT_TYPES];
   static EventHandlerUPP  closeEventHandlerUPP = NULL;
   static int              closeEventCount = 0;
   
   if(closeEventHandlerUPP == NULL)
   {
      c = 0;
      closeEventTypes[c].eventClass = kEventClassWindow;
      closeEventTypes[c++].eventKind = kEventWindowClose;
      closeEventTypes[c].eventClass = kEventClassWindow;
      closeEventTypes[c++].eventKind = kEventWindowClosed;
      
      closeEventHandlerUPP = NewEventHandlerUPP(_OnWindowCloseEvent);
      closeEventCount = c;
   }

   if(textHandlerUPP==NULL)
   {
      c = 0;
      textEventTypes[c].eventClass = kEventClassTextInput;
      textEventTypes[c++].eventKind = kEventTextInputUnicodeForKeyEvent;
      // we would split these off to another Do... handler, but I want all keyboard interaction
      // to go through a single control point for the moment.  -paxorr
      textEventTypes[c].eventClass = kEventClassKeyboard;
      textEventTypes[c++].eventKind = kEventRawKeyDown;
      textEventTypes[c].eventClass = kEventClassKeyboard;
      textEventTypes[c++].eventKind = kEventRawKeyUp;
      textEventTypes[c].eventClass = kEventClassKeyboard;
      textEventTypes[c++].eventKind = kEventRawKeyRepeat;
      // these should perhaps have their own Do... handler as well.
      textEventTypes[c].eventClass = kEventClassKeyboard;
      textEventTypes[c++].eventKind = kEventRawKeyModifiersChanged;
            
      textHandlerUPP = NewEventHandlerUPP(_OnKeyboardEvent);
      textEventCount = c;
   }

   if (winHandlerUPP==NULL)
   {
      c = 0;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowDrawContent;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowBoundsChanged;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowZoomed;
//      winEventTypes[c].eventClass = kEventClassWindow;
//      winEventTypes[c++].eventKind = kEventWindowActivated;
//      winEventTypes[c].eventClass = kEventClassWindow;
//      winEventTypes[c++].eventKind = kEventWindowDeactivated;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowClose;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowGetClickActivation;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowTransitionCompleted;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowCollapsed;
      winEventTypes[c].eventClass = kEventClassWindow;
      winEventTypes[c++].eventKind = kEventWindowExpanded;

      winHandlerUPP = NewEventHandlerUPP(_OnWindowEvent);
      winEventCount = c;
   }
   
   if (appHandlerUPP==NULL)
   {
      c = 0;
      appEventTypes[c].eventClass = kEventClassApplication;
      appEventTypes[c++].eventKind = kEventAppQuit;
      appEventTypes[c].eventClass = kEventClassApplication;
      appEventTypes[c++].eventKind = kEventAppActivated;
      appEventTypes[c].eventClass = kEventClassApplication;
      appEventTypes[c++].eventKind = kEventAppDeactivated;

      appHandlerUPP = NewEventHandlerUPP(_OnAppEvent);
      appEventCount = c;
   }
   
   if (appMouseHandlerUPP==NULL)
   {
      c = 0;
      appMouseEventTypes[c].eventClass = kEventClassMouse;
      appMouseEventTypes[c++].eventKind = kEventMouseDown;
      appMouseEventTypes[c].eventClass = kEventClassMouse;
      appMouseEventTypes[c++].eventKind = kEventMouseUp;
      appMouseEventTypes[c].eventClass = kEventClassMouse;
      appMouseEventTypes[c++].eventKind = kEventMouseMoved;
      appMouseEventTypes[c].eventClass = kEventClassMouse;
      appMouseEventTypes[c++].eventKind = kEventMouseDragged;
      appMouseEventTypes[c].eventClass = kEventClassMouse;
      appMouseEventTypes[c++].eventKind = kEventMouseWheelMoved;

      appMouseHandlerUPP = NewEventHandlerUPP(_OnAppMouseEvent);
      appMouseEventCount = c;
   }
   
   if (appAEHandlerUPP==NULL)
   {
      c = 0;
      appAEEventTypes[c].eventClass = kEventClassAppleEvent;
      appAEEventTypes[c++].eventKind = kEventAppleEvent; // it's always an appleEvent
      
      appAEHandlerUPP = NewEventHandlerUPP(_OnAppleEvent);
      appAEEventCount = c;
   }
   
   // this installs at the window level
   if(platState.appWindow)
   {
      // this installs into the window -- window doesn't always get all events, such as App events
      InstallEventHandler(winTarg, winHandlerUPP,        winEventCount,    winEventTypes,    NULL, &gWinEventHandlerRef);
      InstallEventHandler(winTarg, closeEventHandlerUPP, closeEventCount,  closeEventTypes,  NULL, &gWinCloseEventHandlerRef);
   }
   // this installs at the app level.
   InstallEventHandler(GetApplicationEventTarget(), appHandlerUPP,      appEventCount,      appEventTypes,      NULL, &gAppEventHandlerRef);
   InstallEventHandler(GetApplicationEventTarget(), appMouseHandlerUPP, appMouseEventCount, appMouseEventTypes, NULL, &gAppMouseEventHandlerRef);
   InstallEventHandler(GetApplicationEventTarget(), appAEHandlerUPP,    appAEEventCount,    appAEEventTypes,    NULL, &gAppAEEventHandlerRef);
   // this installs a handler for unicode text input
   // we install on the app only because we have just 1 window.
   InstallEventHandler(GetApplicationEventTarget(), textHandlerUPP, textEventCount, textEventTypes, NULL, &gTextEventHandlerRef);
}

void MacCarbRemoveCarbonEventHandlers()
{
   RemoveEventHandler(gWinEventHandlerRef);
   RemoveEventHandler(gAppEventHandlerRef);
   RemoveEventHandler(gTextEventHandlerRef);
   RemoveEventHandler(gWinMouseEventHandlerRef);
   RemoveEventHandler(gAppMouseEventHandlerRef);
   RemoveEventHandler(gAppAEEventHandlerRef);
   RemoveEventHandler(gWinCloseEventHandlerRef);
}

