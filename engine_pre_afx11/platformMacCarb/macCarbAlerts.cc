//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platform/platformSemaphore.h"
#include "platform/platformVideo.h"
#include "platform/platformThread.h"
#include "console/console.h"
#include "platformMacCarb/macCarbEvents.h"

// Carbon alerts must be displayed from the main thead, because only the main
// thread recieves user events.

//-----------------------------------------------------------------------------
// Puts the main thread in a modal state & shows the alert.
// This should only be called from the main thread.
//-----------------------------------------------------------------------------
void MacCarbRunAlertMain()
{
   // show the alert - this will return when the user hits a button.
   DialogItemIndex hit;

   OSStatus err = RunStandardAlert(platState.alertDlg,NULL,&hit);
   if(err!=noErr)
      Con::printf("RunStandardAlert error = %d, 0x%x", err, err);

   platState.alertHit = hit;
   Semaphore::releaseSemaphore(platState.alertSemaphore);
}

//-----------------------------------------------------------------------------
// Ensures that our alert windows will be shown above the main window.
//-----------------------------------------------------------------------------
static void _MacCarbSetAlertWindowLevel()
{
   SetWindowGroupLevel( GetWindowGroupOfClass(kAlertWindowClass), kTAlertWindowLevel);
}

//-----------------------------------------------------------------------------
static void _MacCarbSendAlertToMainThread(DialogRef alert)
{
   // create & post an event - this will tell the RAEL thread to display an alert.
   platState.alertDlg = alert;
   if(Thread::getCurrentThreadId() == platState.firstThreadId)
      MacCarbRunAlertMain();
   else
   {
      MacCarbSendTorqueEventToMain( kEventTorqueAlert);
   }
   // wait for the alert semaphore
   Semaphore::acquireSemaphore(platState.alertSemaphore);
   platState.alertDlg = NULL;
}

//-----------------------------------------------------------------------------
// Convenience function to encapsulate creating a DialogRef for an alert
//-----------------------------------------------------------------------------
static DialogRef _MacCarbCreateAlert( const char* error, const char* message, CFStringRef okText, CFStringRef cancelText )
{
   if(platState.headless)
      return NULL;

   DialogRef alert;
   AlertStdCFStringAlertParamRec params;
   CFStringRef cfError, cfMessage;

   GetStandardAlertDefaultParams(&params, kStdCFStringAlertVersionOne);
   if(okText)
      params.defaultText = okText;
   if(cancelText)
   {
      params.cancelText = cancelText;
      params.cancelButton = kAlertStdAlertCancelButton;
   }
   
   cfError   = CFStringCreateWithCString(NULL, error, kCFStringEncodingUTF8);
   cfMessage = CFStringCreateWithCString(NULL, message, kCFStringEncodingUTF8);
   
   OSStatus err=CreateStandardAlert(kAlertStopAlert, cfError, cfMessage, &params, &alert);
   if(err!=noErr)
      Con::printf("CreateStandardAlert error = %d, 0x%x", err, err);
   return alert;
}

//-----------------------------------------------------------------------------
// Convenience function for putting the app in a modal state & showing an alert
//-----------------------------------------------------------------------------
static S32 _MacCarbRunAlert( DialogRef alert )
{
   if(platState.headless)
      return kAlertStdAlertCancelButton;
      
   // prep to show the event - be sure we dont obscure it or eat it's events.
   MacCarbRemoveCarbonEventHandlers();
   _MacCarbSetAlertWindowLevel();
   MacCarbSetHideCursor(false);
   if(Video::isFullScreen() && platState.captureDisplay)
   {
      ShowMenuBar();
      aglSetDrawable(platState.ctx, NULL);
   }
   
   _MacCarbSendAlertToMainThread(alert);
   
   // go back to normal Torque operations...
   if(Video::isFullScreen() && platState.captureDisplay)
   {
      HideMenuBar();
      aglSetFullScreen(platState.ctx, Video::getResolution().w, Video::getResolution().h, 0, 0);
   }
   MacCarbInstallCarbonEventHandlers();
   MacCarbCheckHideCursor();
   
   return platState.alertHit;
}

//-----------------------------------------------------------------------------
void Platform::AlertOK(const char *windowTitle, const char *message)
{
   DialogRef alert = _MacCarbCreateAlert(windowTitle, message,NULL,NULL);
   _MacCarbRunAlert(alert);
}

//-----------------------------------------------------------------------------
bool Platform::AlertOKCancel(const char *windowTitle, const char *message)
{
   DialogRef alert = _MacCarbCreateAlert(windowTitle, message,NULL,CFSTR("Cancel"));
   S32 hit = _MacCarbRunAlert(alert);
   return ( hit == kAlertStdAlertOKButton );
}

//-----------------------------------------------------------------------------
bool Platform::AlertRetry(const char *windowTitle, const char *message)
{
   DialogRef alert = _MacCarbCreateAlert(windowTitle, message,CFSTR("Retry"),CFSTR("Cancel"));
   S32 hit = _MacCarbRunAlert(alert);
   return ( hit == kAlertStdAlertOKButton );
}

// paxorr - remove this when no longer needed.
//-----------------------------------------------------------------------------
ConsoleFunction( testAlert, void, 1,3,"[title],[mesage]")
{
   const char *title = "The penguin is missing!";
   const char *message = "Perhaps the salmon of doubt ate him.";
   if(argc>1)
      title = argv[1];
   if(argc>2)
      message = argv[2];
   
   Platform::AlertOK(title,message);
   Con::errorf("Returned from AlertOK");
   bool ok = Platform::AlertOKCancel(title,message);
   Con::errorf("Returned from AlertCancel, hit %s", ok?"OK":"Cancel");
   ok = Platform::AlertRetry(title,message);
   Con::errorf("Returned from AlertRetry, hit %s", ok?"Retry":"Cancel");
   
   DialogRef alert = _MacCarbCreateAlert("Test","testing alternate cancel texts",NULL,CFSTR("cry havok!"));
   _MacCarbRunAlert(alert);
}
