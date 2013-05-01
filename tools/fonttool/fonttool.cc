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
#include "dgl/gFont.h"
#include "core/frameAllocator.h"
#include "core/unicode.h"

#include "core/fileStream.h"

#include "fonttool.h"
#include "Getopt.h"

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////

FontToolGame GameObject;

// FOR SILLY LINK DEPENDANCY. REMOVE THIS AT YOUR PERIL.
bool gEditingMission = false;

//////////////////////////////////////////////////////////////////////////

static bool initLibraries()
{
	// asserts should be created FIRST
	PlatformAssert::create();	
	_StringTable::create();	
	Con::init();	
	Math::init();
	Platform::init();
   FrameAllocator::init(3 * 1024 * 1024);
   TextureManager::create();
	return(true);
}

static void shutdownLibraries()
{
	Platform::shutdown();
	Con::shutdown();
	_StringTable::destroy();
	PlatformAssert::destroy();
}

//////////////////////////////////////////////////////////////////////////

static void usage()
{
	dPrintf("Usage: fonttool [options] <font> <size> <cachefile>\n\n");
	
	dPrintf("Where options is one or more of:\n\n");
	
	dPrintf(" -r    Populate cache with range. Followup parameters are:\n");
	dPrintf("          fonttool [options] <font> <size> <cachefile> <beginChar> <endChar>\n");
	dPrintf("        Note that characters are provided in decimal.\n\n");
	
	dPrintf(" -s    Populate cache with strings. You provide a file; the\n");
	dPrintf("       cache is populated with all the characters that the\n");
	dPrintf("       file contains. Followup parameters are:\n");
	dPrintf("          fonttool [options] <font> <size> <cachefile> <stringFile>\n\n");

	dPrintf(" -i    Get info about a cache file. Only parameter is a cache file.\n\n");
	
   dPrintf("fonttool ONLY supports UTF8 encoded files. Extended ASCII characters\n");
   dPrintf("and other encodings will not work and may result in strange results!\n\n");

	dPrintf("\nMore information can be found in the documentation at:\n   %s\n", FONTTOOL_DOC_URL);
}

static void printfConsumer(ConsoleLogEntry::Level level, const char *consoleLine)
{
   char *pref = "";
   switch(level)
   {
   case ConsoleLogEntry::Error:
      pref="[error] ";
      break;

   case ConsoleLogEntry::Warning:
      pref = "[warn] ";
      break;
   }
   dPrintf("%s%s\n", pref, consoleLine);
}

S32 FontToolGame::main(S32 argc, const char **argv)
{
	S32 i, ch;
	Getopt opts("rsi:");
	
	if(! initLibraries())
   {
      dPrintf("Failed to initialize libraries.\n");
      return 0;
   }

   Con::addConsumer(printfConsumer);

   FontToolMode mode = FTM_SHOW_USAGE;
   const char *paramFile = NULL;

   Con::printf("fonttool v1.0.1");

	while((ch = opts.getopt(argc, (char **)argv)) != -1)
	{
		switch(ch)
		{
			case 'r':
            mode = FTM_CACHE_RANGE;
				break;

			case 's':
            mode = FTM_CACHE_STRINGS;
				break;

			case 'i':
            paramFile = opts.m_sOptArg;
            mode = FTM_CACHE_INFO;
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


   if(mode == FTM_SHOW_USAGE)
   {
      usage();
      shutdownLibraries();
      return 0;
   }

   if(mode == FTM_CACHE_RANGE || mode == FTM_CACHE_STRINGS)
   {
      // Get the cache file.
      Con::printf("Loading cache file '%s' for processing...", argv[2]);

      GFont infoFont;

      {
         FileStream fs;

         if(!fs.open(argv[2], FileStream::Read))
         {
            Con::errorf("  - Could not open '%s'!", argv[2]);
            shutdownLibraries();
            return 0;
         }

         if(!infoFont.read(fs))
         {
            Con::errorf("  - Could not parse GFT file!!\n");
            shutdownLibraries();
            return 0;
         }

         fs.close();
      }

      // Ok, now we've got a font file. So let's load the appropriate
      // platform font and do the cache population.
      Con::printf("Creating platform font '%s' %d...", argv[0], dAtoi(argv[1]));
      PlatformFont *pf =  createPlatformFont(argv[0], dAtoi(argv[1]));

      if(!pf)
      {
         Con::errorf("  - Could not create font %s (%dpt)", argv[0], dAtoi(argv[1]));
         shutdownLibraries();
         return 0;
      }

      // Stuff the platform font into the GFont...
      infoFont.forcePlatformFont(pf);

      // Now do the appropriate caching logic.
      if(mode == FTM_CACHE_RANGE)
      {
         if(argc < 5)
         {
            Con::errorf("Too few arguments!");
            shutdownLibraries();
            return 0;
         }

         U32 rangeStart = dAtoi(argv[3]);
         U32 rangeEnd   = dAtoi(argv[4]);

         if(rangeStart > rangeEnd)
         {
            Con::errorf("populateFontCacheRange - range start is after end!");
            shutdownLibraries();
            return 0;
         }

         Con::printf("Populating cache with characters from %d to %d", rangeStart, rangeEnd);

         // This has the side effect of generating character info, including the bitmaps.
         for(U32 i=rangeStart; i<=rangeEnd; i++)
         {
            if(infoFont.isValidChar(i))
               infoFont.getCharWidth(i);
            else
               Con::warnf("populateFontCacheRange - skipping invalid char 0x%x",  i);
         }

         // Ok, all done.
         Con::printf("Done populating cache!");
      }
      else if(mode == FTM_CACHE_STRINGS)
      {
         if(argc < 4)
         {
            Con::errorf("Too few arguments!");
            shutdownLibraries();
            return 0;
         }

         // Ok, we want to open the file, read every line, and run it
         // through the cache.
         FileStream fs;

         if(!fs.open(argv[3], FileStream::Read))
         {
            Con::errorf("  - Could not open string file '%s'!", argv[3]);
            shutdownLibraries();
            return 0;
         }

         // Peek the first 4 chars so we can check the BOM.
         // Bytes         Encoding Form
         // 00 00 FE FF   UTF-32, big-endian
         // FF FE 00 00   UTF-32, little-endian
         // FE FF         UTF-16, big-endian
         // FF FE         UTF-16, little-endian
         // EF BB BF 	  UTF-8

         U8 bom[4];
         fs.read(&bom[0]);
         fs.read(&bom[1]);
         fs.read(&bom[2]);
         fs.read(&bom[3]);

         // And reset the position...
         fs.setPosition(0);

         // Is it a BOM?
         if(bom[0] == 0)
         {
            // Could be UTF32BE
            if(bom[1] == 0 && bom[2] == 0xFE && bom[3] == 0xFF)
               Con::warnf("Encountered a UTF32 BE BOM in this file; fonttool does NOT support this file encoding. Use UTF8!");
         }
         else if(bom[0] == 0xFF)
         {
            // It's little endian, either UTF16 or UTF 32
            if(bom[1] == 0xFE)
            {
               if(bom[2] == 0 && bom[3] == 0)
                  Con::warnf("Encountered a UTF32 LE BOM in this file; fonttool does NOT support this file encoding. Use UTF8!");
               else
                  Con::warnf("Encountered a UTF16 LE BOM in this file; fonttool does NOT support this file encoding. Use UTF8!");
            }
         }
         else if(bom[0] == 0xFE && bom[1] == 0xFF)
            Con::warnf("Encountered a UTF16 BE BOM in this file; fonttool does NOT support this file encoding. Use UTF8!");
         else if(bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
            Con::printf("Encountered a UTF8 BOM. Fonttool supports this.");

         // Ok, read each line and run it through.
         const U32 buffSize = 64 * 1024;
         UTF8 lineBuff[buffSize];
         UTF16 convBuffer[buffSize];

         Con::printf("Populating cache with strings from %s", argv[3]);

         U32 lineCount = 0;
         while(fs.getStatus() != FileStream::EOS)
         {
            fs.readLine((U8*)lineBuff, buffSize);
            convertUTF8toUTF16(lineBuff, convBuffer, buffSize);
            infoFont.getStrNWidth(convBuffer, dStrlen(convBuffer));
            lineCount++;
         }

         fs.close();

         Con::printf("Done populating cache! Processed %d line(s).", lineCount);
      }
      else
      {
         AssertISV(false, "FontToolGame::main - unknown mode!");
      }

      // Ok, now write it back out.
      {
         FileStream fs;

         if(!fs.open(argv[2], FileStream::Write))
         {
            Con::errorf("  - Could not open '%s' for writing!", argv[2]);
            shutdownLibraries();
            return 0;
         }

         if(!infoFont.write(fs))
         {
            Con::errorf("  - Could not write GFT file!");
            shutdownLibraries();
            return 0;
         }

         fs.close();
      }

   }

   if(mode == FTM_CACHE_INFO)
   {
      Con::printf("Getting info on cache file '%s'...", paramFile);

      // Get a stream to the file.
      GFont infoFont;
      FileStream fs;

      if(!fs.open(paramFile, FileStream::Read))
      {
         Con::errorf("  - Could not open file!\n");
         shutdownLibraries();
         return 0;
      }

      if(!infoFont.read(fs))
      {
         Con::errorf("  - Could not load GFT!\n");
         shutdownLibraries();
         return 0;
      }

      fs.close();

      infoFont.dumpInfo();
      //  All done.
   }


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
