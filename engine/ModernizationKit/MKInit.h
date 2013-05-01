/*  MKInit.h
 *  Copyright (C) 2007 Alex Scarborough
 *  For conditions of distribution and use, see copyright notice in ModernizationKit.h
 */

// This just gives us a nice easy way of initing all the stuff the MK inits.

#ifndef _MKINIT_H_
#define _MKINIT_H_

/// @defgroup mkinit Modernization Kit Initialization
/// These functions handle initializing, destroying, resetting
/// and resurrecting all of the subsystems in the Modernization Kit
/// @{

/// Initializes all MK subsystems
void MKInit();

/// Destroys all MK subsystems
void MKDestroy();

/// Destroys all context sensitive information in the MK
void MKReset();

/// Restores all context sensitive information in the MK
void MKResurrect();

/// @}

#endif
