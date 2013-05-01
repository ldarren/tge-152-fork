#ifndef _OpenMayaMac
#define _OpenMayaMac
//
//	Description:
//		Macintosh Maya Common definition settings
//		For use with prefix headers to set CodeWarrior
//		definitions properly
//
//	Usage:
//		// Define DEBUG or NDEBUG as appropriate
//		//
//		#define DEBUG	1
//		#define _DEBUG	1
//
//		#define NDEBUG	1
//		#define _NDEBUG	1
//
//		#include <OpenMayaMac.h>
//
//		// Add this to generate a pre-compiled header
//		//
//		#pragma precompile_target "{plug-in}.pch"
//
//-
// ==========================================================================
// Copyright  (C)  Alias|Wavefront,  a division of Silicon Graphics  Limited.
// All rights  reserved.  These coded  instructions,  statements and computer
// programs contain unpublished information  proprietary to  Alias|Wavefront,
// a  division  of  Silicon  Graphics  Limited,  which  is  protected by  the
// Canadian  and  US federal copyright law and  may not be disclosed to third
// parties or  copied  or  duplicated,  in  whole  or in part,  without prior
// written consent of Alias|Wavefront, a division of Silicon Graphics Limited
// ==========================================================================
//+

#define NOMINMAX 
#define national
#define STYLEAPP
#ifdef __cplusplus
#	define __ANSI_CPP__
#endif
#define _BOOL

#define AL_STYLE

#define OSMac_					1
#define MAC_PLUGIN 				1

#define TARGET_API_MAC_CARBON 	1

#ifndef BIG_ENDIAN
#	define BIG_ENDIAN 4321
#endif
#ifndef LITTLE_ENDIAN
#	define LITTLE_ENDIAN 1234
#endif
#ifndef BYTE_ORDER
#	define BYTE_ORDER BIG_ENDIAN
#endif

#define Window			WindowPtr
#define	MAXPATHLEN		_MAX_PATH

// This is a Metrowerks file that is required to use the MW_MSL.Carbon.Shlib properly
// if this file is NOT included you will get strange link errors about std:: functionality
#include <UseDLLPrefix.h>
#include <Files.h>
#include <CFURL.h>
#include <size_t.h>
#ifdef __cplusplus
using namespace std;
#endif

// Temporary Utilities provided by Foundation.lib and may be removed when Maya becomes a Mach-O
// based application . Refer to Readme.txt for more details
// Convert file system representations
// Possible styles: kCFURLHFSPathStyle, kCFURLPOSIXPathStyle
// kCFURLHFSPathStyle = Emerald:aw:Maya:projects:default:scenes:eagle.ma
// kCFURLPOSIXPathStyle = /Volumes/Emerald/aw/Maya/projects/default/scenes/eagle.ma
// The conversion will be done in place, so make sure fileName is big enough
// to hold the result
//
#ifdef __cplusplus
extern "C" {
#endif
Boolean convertFileRepresentation (char *fileName, short inStyle, short outStyle);
Boolean createMacFile (const char *fileName, FSRef *fsRef, long creator, long type);
int strcasecmp (const char *, const char *);
char *strdup (const char *rhs);
#ifdef __cplusplus
}
#endif

#endif
