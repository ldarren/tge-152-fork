//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "platform/event.h"
#include "platform/platformAssert.h"
#include "console/console.h"
#include "console/consoleTypes.h"
#include "math/mathTypes.h"
#include "langc/langc.h"

#include "core/fileStream.h"

#include "i18n/lang.h"
#include "langcomp.h"
#include "Getopt.h"

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////

LangCGame GameObject;

// FOR SILLY LINK DEPENDANCY. REMOVE THIS AT YOUR PERIL.
bool gEditingMission = false;

//////////////////////////////////////////////////////////////////////////

static bool initLibraries()
{
	// asserts should be created FIRST
	PlatformAssert::create();
	
	_StringTable::create();
//	ResManager::create();
	
	Con::init();
	
	Math::init();
	Platform::init();    // platform specific initialization
	return(true);
}

static void shutdownLibraries()
{
	// Purge any resources on the timeout list...
//	if (ResourceManager)
//      ResourceManager->purge();
	
	// shut down
	Platform::shutdown();
	Con::shutdown();
	
//	ResManager::destroy();
	_StringTable::destroy();
	
	// asserts should be destroyed LAST
	PlatformAssert::destroy();
}

//////////////////////////////////////////////////////////////////////////

static void usage(void)
{
	dPrintf("Usage: langc [options] <filename> <outbasename>\n\n");
	dPrintf("Where options is one or more of:\n\n");
	dPrintf(" -l    Write Language File              -h    Write C++ Header\n");
	dPrintf(" -s    Write Script                     -d    Write C++ Defaults\n");
	dPrintf(" -t    Compile a translation            -r    Write translation file\n");
	dPrintf(" -e <filename>   Specify english file when compiling translations\n");
	dPrintf("\n");
	dPrintf(" -S    Don't strip leading spaces       -T    Strip trailing spaces\n");
	dPrintf(" -I    Don't warn for invalid chars     -W    Don't warn for empty identifiers\n");
	dPrintf(" -q    Quiet mode, no warnings at all\n");
	dPrintf("\nMore information can be found in the documentation at:\n   %s\n", I18N_DOC_URL);
}

S32 LangCGame::main(S32 argc, const char **argv)
{
	S32 i, ch;
	U32 flags = LCO_WARNNOSTRING;
	Getopt opts("STIWqhsdlrte:");
	char *englishFile = NULL;
	
	if(! initLibraries())
		return 0;

	while((ch = opts.getopt(argc, (char **)argv)) != -1)
	{
		switch(ch)
		{
			case 't':
				// Compile a translation
				flags |= LCO_COMPILETRANSLATION;
				break;

			case 'e':
				// Specify english file
				englishFile = opts.m_sOptArg;
				break;

			case 'S':
				// Don't strip spaces
				flags |= LCO_DONTSTRIPSPACES;
				break;

			case 'T':
				// Strip trailing space
				flags |= LCO_STRIPTRAILINGSPACE;
				break;

			case 'I':
				// Don't warn for invalid chars
				flags |= LCO_DONTWARNINVALIDCHAR;
				break;

			case 'W':
				// Don't warn for empty identifiers
				flags &= ~LCO_WARNNOSTRING;
				break;

			case 'q':
				// Quiet mode, no warnings at all
				flags |= LCO_NOWARNINGS;
				break;

			case 'h':
				// Write Header
				flags |= LCO_WRITEHEADER;
				break;

			case 's':
				// Write Script
				flags |= LCO_WRITESCRIPT;
				break;

			case 'd':
				// Write C++ Defaults
				flags |= LCO_WRITECDEFAULTS;
				break;

			case 'l':
				// Write Lang Table
				flags |= LCO_WRITELANGTABLE;
				break;

			case 'r':
				// Write translation
				flags |= LCO_WRITETRANSLATION;
				break;

			case GO_BAD_ARG:
				dPrintf("option %c requires an argument\n", opts.m_nOptOpt);
				break;

			case GO_INVALID_CHAR:
				dPrintf("%c is an invalid option\n", opts.m_nOptOpt);
				break;
				
			case GO_BAD_CHAR:
				usage();
				shutdownLibraries();
				return 0;
		}
	}
	argc -= opts.m_nOptInd;
	argv += opts.m_nOptInd;

	if(argc < 2)
	{
		usage();
		return 0;
	}

	LangComp c(flags);

	dPrintf("Compiling ... \n");
	c.Compile(argv[0], argv[1], englishFile);

	shutdownLibraries();
	return 0;
}

//////////////////////////////////////////////////////////////////////////

void GameReactivate()
{
}

void GameDeactivate( bool )
{
}
