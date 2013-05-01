//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "console/console.h"
#include "core/stringTable.h"
#include <sys/sysctl.h>
#include <mach/machine.h>
#include <math.h>

// The Gestalt() API will no longer be updated, so this code should migrate to
// the unix sysctl() and sysctlbyname() API. This is the current doctrine from Apple, dec 2005.

// Thanks to Gary "ChunkyKs" Briggs for contributing the sysctl() code patch.

Platform::SystemInfo_struct Platform::SystemInfo;
static char* _MacCarbGetMachineName();
static void  _MacCarbGetCpuTypeAndFeatures(U32 *type, char **typeString, U32 *features);
static void  _MacCarbGetBasicHWInfo(U32 *osVersion, U64 *cpuMhz, U32 *numCpus, U64 *memMbSize);

void Processor::init()
{
   U32 osVersion;
   U64 cpuMhz; 
   U32 numCpus; 
   U64 memMbSize;

   U32 cpuType;
   char* cpuTypeString;
   U32 cpuFeatures;

   Con::printf("System & Processor Information:");

   // Get the mac model identifier, just for fun...
   char *machineName = _MacCarbGetMachineName();
   
   // get data about the machine out of sysctl and Gestalt ...
   _MacCarbGetBasicHWInfo(&osVersion, &cpuMhz, &numCpus, &memMbSize);
   _MacCarbGetCpuTypeAndFeatures(&cpuType, &cpuTypeString, &cpuFeatures);
   
   // Set Torque's processor mhz var. This is reported to master servers.
   Platform::SystemInfo.processor.mhz = cpuMhz;

   // Set Torque's processor type var.
   // Don't use special code paths based on this var, instead you should use the
   // processor properties bits: Platform::SystemInfo.processor.properties
   Platform::SystemInfo.processor.type = cpuType;
  
   // Set Torque's processor name var.
   Platform::SystemInfo.processor.name = StringTable->insert(cpuTypeString);
   
   // Set Torques processor properties. 
   // These should determine what special code paths we use.
   Platform::SystemInfo.processor.properties = cpuFeatures;

   // Make pretty strings...
   char freqString[32];
   if(cpuMhz >= 1000)
      dSprintf(freqString, 32, "%2.3g GHz", cpuMhz / 1000.0f);
   else
      dSprintf(freqString, 32, "%i MHz", cpuMhz); 

   char coresString[32] = "single core";
   if(numCpus > 1)
      dSprintf(coresString, 32, "%i cores", numCpus);
   
   char memString[32];
   if(memMbSize >= 1024)
      dSprintf(memString, 32, "%2.1f GB", memMbSize / 1024.0f);
   else
      dSprintf(memString, 32, "%i MB", memMbSize);
   
   // Dump info to console
   Con::printf("  Mac OS X %x.%x.%x", (osVersion>>8), (osVersion&0xFF)>>4, (osVersion&0x0F));
   Con::printf("  %s RAM", memString);
   Con::printf("  %s %s (%s)", freqString, machineName, coresString);
   Con::printf("    %s Architecture", cpuTypeString);
   if(Platform::SystemInfo.processor.properties & CPU_PROP_MMX)
      Con::printf("    MMX detected");
   if(Platform::SystemInfo.processor.properties & CPU_PROP_SSE)
      Con::printf("    SSE detected");
   if(Platform::SystemInfo.processor.properties & CPU_PROP_ALTIVEC)
      Con::printf("    Altivec detected");

   Con::printf(" ");
}

static char* _MacCarbGetMachineName()
{
   // data from: http://www.cocoadev.com/index.pl?MacintoshModels
   // and http://www.theapplemuseum.com/index.php?id=36#
   // and 10.4.8's /System/Library/SystemProfiler/SPPlatformReporter.spreporter/Contents/Resources/SPMachineTypes.plist
   char* _unknownMac = "unknown Mac model";
   char* _macNames[][2] = {
      { "AAPL,Gossamer", "PowerMacG3Series" },
      { "AAPL,PowerBook1998", "PowerBookG3Series" },
      { "AAPL,PowerMac G3", "PowerMacG3" },
      { "ADP2,1" , "Developer Transition Kit" },
      { "iMac,1" , "iMac" },
      { "iMac4,1" , "iMac Core Duo" },
      { "iMac4,2" , "iMac (Core Duo, mid 2006)" },
      { "iMac5,1" , "iMac (Core 2 Duo)" },
      { "iMac6,1" , "iMac (24-inch Core 2 Duo)" },
      { "MacBook1,1" , "MacBook (Core Duo)" },
      { "MacBook2,1" , "MacBook (Core 2 Duo)" },
      { "MacBookPro1,1" , "MacBook Pro (15-inch Core Duo)" },
      { "MacBookPro1,2" , "MacBook Pro (17-inch Core Duo)" },
      { "MacBookPro2,1" , "MacBook Pro (17-inch Core 2 Duo)" },
      { "MacBookPro2,2" , "MacBook Pro (15-inch Core 2 Duo)" },
      { "Macmini1,1" , "Mac mini (Core Duo/Solo)" },
      { "MacPro1,1" , "Mac Pro (Quad Xeon)" },
      { "MacPro1,1,Quad" , "Mac Pro (Quad Xeon)" },
      { "PowerBook1,1" , "PowerBook G3" },
      { "PowerBook2,1" , "iBook" },
      { "PowerBook2,2" , "iBook (FireWire)" },
      { "PowerBook3,1" , "PowerBook G3 (FireWire)" },
      { "PowerBook3,2" , "PowerBook G4" },
      { "PowerBook3,3" , "PowerBook G4 (Gigabit Ethernet)" },
      { "PowerBook3,4" , "PowerBook G4 (DVI)" },
      { "PowerBook3,5" , "PowerBook G4 (867, 1 GHz)" },
      { "PowerBook4,1" , "iBook G3" },
      { "PowerBook4,2" , "iBook G3" },
      { "PowerBook4,3" , "iBook G3" },
      { "PowerBook4,4" , "iBook" },
      { "PowerBook5,1" , "PowerBook G4 (17-inch)" },
      { "PowerBook5,2" , "PowerBook G4 (15-inch FW800)" },
      { "PowerBook5,3" , "PowerBook G4 (17-inch 1.33 GHz)" },
      { "PowerBook5,4" , "PowerBook G4 (15-inch 1.5/1.33 GHz)" },
      { "PowerBook5,5" , "PowerBook G4 (17-inch 1.5 GHz)" },
      { "PowerBook5,6" , "PowerBook G4 (15-inch 1.67/1.5 GHz)" },
      { "PowerBook5,7" , "PowerBook G4 (17-inch 1.67 GHz)" },
      { "PowerBook5,8" , "PowerBook G4 (Double-Layer SD, 15-inch)" },
      { "PowerBook5,9" , "PowerBook G4 (Double-Layer SD, 17-inch)" },
      { "PowerBook6,1" , "PowerBook G4 (12-inch)" },
      { "PowerBook6,2" , "PowerBook G4 (12-inch DVI)" },
      { "PowerBook6,3" , "iBook G4" },
      { "PowerBook6,4" , "PowerBook G4 (12-inch 1.33 GHz)" },
      { "PowerBook6,5" , "iBook G4" },
      { "PowerBook6,7" , "iBook G4" },
      { "PowerBook6,8" , "PowerBook G4 (12-inch 1.5 GHz)" },
      { "PowerMac1,1" , "Power Macintosh G3 (B&W)" },
      { "PowerMac1,2" , "Power Macintosh G4 (PCI-Graphics)" },
      { "PowerMac2,1" , "iMac (Slot-Loading)" },
      { "PowerMac2,2" , "iMac (2000)" },
      { "PowerMac3,1" , "Power Macintosh G4 (AGP-Graphics)" },
      { "PowerMac3,2" , "Power Macintosh G4 (AGP-Graphics)" },
      { "PowerMac3,3" , "Power Macintosh G4 (Gigabit Ethernet)" },
      { "PowerMac3,4" , "Power Macintosh G4 (Digital Audio)" },
      { "PowerMac3,5" , "Power Macintosh G4 (Quick Silver)" },
      { "PowerMac3,6" , "Power Macintosh G4 (Mirrored Drive Doors)" },
      { "PowerMac4,1" , "iMac (2001)" },
      { "PowerMac4,2" , "iMac (Flat Panel)" },
      { "PowerMac4,4" , "eMac G3" },
      { "PowerMac4,5" , "iMac (17-inch Flat Panel)" },
      { "PowerMac5,1" , "Power Macintosh G4 Cube" },
      { "PowerMac5,2" , "Power Macintosh G4 Cube" },
      { "PowerMac6,1" , "iMac (USB 2.0)" },
      { "PowerMac6,3" , "iMac (20-inch Flat Panel)" },
      { "PowerMac6,4" , "eMac (USB 2.0)" },
      { "PowerMac7,2" , "Power Macintosh G5" },
      { "PowerMac7,3" , "Power Macintosh G5" },
      { "PowerMac8,1" , "iMac G5" },
      { "PowerMac8,2" , "iMac G5 (Ambient Light Sensor)" },
      { "PowerMac9,1" , "Power Macintosh G5 (Late 2004)" },
      { "PowerMac10,1" , "Mac mini" },
      { "PowerMac10,2" , "Mac mini (Late 2005)" },
      { "PowerMac11,2" , "Power Macintosh G5 (PCIe)" },
      { "PowerMac11,2,Quad", "Power Macintosh G5 Quad" },
      { "PowerMac12,1" , "iMac G5 (iSight)" },
      { "RackMac1,1" , "Xserve G4" },
      { "RackMac1,2" , "Xserve G4 (Slot-Loading)" },
      { "RackMac3,1" , "Xserve G5" },
      { NULL , NULL }
   };
   
   char macModel[MAXPATHLEN];
   U32  macModelLen = MAXPATHLEN;
   
   macModel[macModelLen-1] = '\0';
   sysctlbyname("hw.model", &macModel, &macModelLen, NULL, 0);
   
   for(int i=0; _macNames[i][0] != NULL; i++)
   {
      if(!strcmp(_macNames[i][0], macModel))
         return _macNames[i][1];
   }
   #if defined(TORQUE_DEBUG)
   Con::warnf("Unknown mac model. Add a definition to _MacCarbGetMachineName() for %s.", macModel);
   #endif
   return _unknownMac;
}

void  _MacCarbGetBasicHWInfo(U32 *osVersion, U64 *cpuMhz, U32 *numCpus, U64 *memMbSize)
{
   U32 u64size = sizeof(U64);
   U32 u32size = sizeof(U32);
   
   // Get MacOSX version #
   // Gestalt is still the only simple way to get the system version number, as of Jan 2007
   Gestalt(gestaltSystemVersion, osVersion);

   // Get system mem
   sysctlbyname("hw.memsize", memMbSize, &u64size, NULL, 0);
   *memMbSize /= 1024 * 1024;
   
   // Get cpu clock speed
   sysctlbyname("hw.cpufrequency", cpuMhz, &u64size, NULL, 0);
   // sometimes the frequency is not a pretty number, so round it.
   *cpuMhz = mCeil((float)(*cpuMhz)/(1000*1000));
      
   // Get the # of processors or cores we have available
   sysctlbyname("hw.ncpu", numCpus, &u32size, NULL, 0);
}


void  _MacCarbGetCpuTypeAndFeatures(U32 *type, char **typeString, U32 *features)
{
   int err;
   U32 cputype;
   U32 cpusubtype;
   U32 cpufeature;

   U32 u64size = sizeof(U64);
   U32 u32size = sizeof(U32);

   char *torqueCpuString;
   U32 torqueCpuType;
   U32 torqueCpuFeatures;

   // Identify the cpu
   err = sysctlbyname("hw.cputype", &cputype, &u32size, NULL, 0);
   err |= sysctlbyname("hw.cpusubtype", &cpusubtype, &u32size, NULL, 0);
   AssertWarn(err == 0, "Couldn't detect CPU type. Assuming generic x86.");

   // determine the processor type & name
   switch(cputype) {
      case CPU_TYPE_X86:
      case CPU_TYPE_X86_64:
         torqueCpuString = "x86";
         torqueCpuType = CPU_X86Compatible;
         break;
      case CPU_TYPE_POWERPC64:
      case CPU_TYPE_POWERPC:
         switch(cpusubtype)
         {
            case CPU_SUBTYPE_POWERPC_970:
               torqueCpuString = "G5";
               torqueCpuType = CPU_PowerPC_G5;
               break;
            case CPU_SUBTYPE_POWERPC_7400:
            case CPU_SUBTYPE_POWERPC_7450:
               torqueCpuString = "G4";
               torqueCpuType = CPU_PowerPC_G4;
               break;
            case CPU_SUBTYPE_POWERPC_750:
               torqueCpuString = "G3";
               torqueCpuType = CPU_PowerPC_G3;
               break;
            default:
               torqueCpuString = "PowerPC";
               torqueCpuType = CPU_PowerPC_Unknown;
         }
         break;
      default:
         torqueCpuString = "unknown";
         torqueCpuType = CPU_X86Compatible;
   }
   
   // Get CPU features. These should determine what special code paths we use.
   torqueCpuFeatures = 0;
   switch(torqueCpuType) {
	   case CPU_X86Compatible:
         err = sysctlbyname("hw.optional.mmx", &cpufeature, &u32size, NULL, 0);
         if(!err && cpufeature)
            torqueCpuFeatures |= CPU_PROP_MMX;
         
         err = sysctlbyname("hw.optional.sse", &cpufeature, &u32size, NULL, 0);
         if(!err && cpufeature)
            torqueCpuFeatures |= CPU_PROP_SSE;
            
         break;
	   case CPU_PowerPC_G5:
      case CPU_PowerPC_G4:
         err = sysctlbyname("hw.optional.altivec", &cpufeature, &u32size, NULL, 0);
         if(!err && cpufeature)
            torqueCpuFeatures |= CPU_PROP_ALTIVEC;

         break;
   }

   // return data
   *type = torqueCpuType;
   *typeString = torqueCpuString;
   *features = torqueCpuFeatures;
}
