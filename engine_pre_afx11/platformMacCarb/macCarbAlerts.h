//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Puts the current thread in a modal state & shows the alert.
/// This should only be called from the main thread. 
/// If you call it from any other thread, bad things will happen. So don't.
/// The 'main thread' here means the program's thread zero. The first thread.
/// In multithreaded Torque, this is NOT the game loop thread.
/// On the Mac, events and alerts have to happen in thread zero.
//-----------------------------------------------------------------------------
void MacCarbRunAlertMain();

