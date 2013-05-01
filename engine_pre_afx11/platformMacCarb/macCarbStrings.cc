//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platformMacCarb/platformMacCarb.h"
#include "platform/platform.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <CoreFoundation/CoreFoundation.h>
#include "console/console.h"
#include "platform/profiler.h"

char *dStrdup_r(const char *src, const char *file, dsize_t line)
{
   char *buffer = (char *) dMalloc_r(dStrlen(src) + 1, file, line);
   dStrcpy(buffer, src);
   return buffer;
}

char *dStrnew(const char *src)
{
   char *buffer = new char[dStrlen(src) + 1];
   dStrcpy(buffer, src);
   return buffer;
}

char* dStrcat(char *dst, const char *src)
{
   return strcat(dst,src);
}   

char* dStrncat(char *dst, const char *src, dsize_t len)
{
   return strncat(dst,src,len);
}

// concatenates a list of src's onto the end of dst
// the list of src's MUST be terminated by a NULL parameter
// dStrcatl(dst, sizeof(dst), src1, src2, NULL);
char* dStrcatl(char *dst, dsize_t dstSize, ...)
{
   const char* src;
   char *p = dst;

   AssertFatal(dstSize > 0, "dStrcatl: destination size is set zero");
   dstSize--;  // leave room for string termination

   // find end of dst
   while (dstSize && *p++)                    
      dstSize--;   
   
   va_list args;
   va_start(args, dstSize);

   // concatenate each src to end of dst
   while ( (src = va_arg(args, const char*)) != NULL )
      while( dstSize && *src )
      {
         *p++ = *src++;
         dstSize--;   
      }

   va_end(args);

   // make sure the string is terminated 
   *p = 0;

   return dst;
}   


// copy a list of src's into dst
// the list of src's MUST be terminated by a NULL parameter
// dStrccpyl(dst, sizeof(dst), src1, src2, NULL);
char* dStrcpyl(char *dst, dsize_t dstSize, ...)
{
   const char* src;
   char *p = dst;

   AssertFatal(dstSize > 0, "dStrcpyl: destination size is set zero");
   dstSize--;  // leave room for string termination

   va_list args;
   va_start(args, dstSize);

   // concatenate each src to end of dst
   while ( (src = va_arg(args, const char*)) != NULL )
      while( dstSize && *src )
      {
         *p++ = *src++;
         dstSize--;   
      }

   va_end(args);

   // make sure the string is terminated 
   *p = 0;

   return dst;
}   


int dStrcmp(const char *str1, const char *str2)
{
   return strcmp(str1, str2);   
}

int dStrcmp( const UTF16 *str1, const UTF16 *str2)
{
   int ret;
   const UTF16 *a, *b;
   a = str1;
   b = str2;

   while( *a && *b && (ret = *a - *b) == 0)
      a++, b++;

   return ret;
}  

 
int dStricmp(const char *str1, const char *str2)
{
   char c1, c2;
   while (1)
   {
      c1 = tolower(*str1++);
      c2 = tolower(*str2++);
      if (c1 < c2) return -1;
      if (c1 > c2) return 1;
      if (c1 == 0) return 0;
   }
}  

int dStrncmp(const char *str1, const char *str2, dsize_t len)
{
   return strncmp(str1, str2, len);   
}  
 
int dStrnicmp(const char *str1, const char *str2, dsize_t len)
{
   return strncasecmp( str1, str2, len );
}   


char* dStrcpy(char *dst, const char *src)
{
   return strcpy(dst,src);
}   

char* dStrncpy(char *dst, const char *src, dsize_t len)
{
   return strncpy(dst,src,len);
}   

dsize_t dStrlen(const char *str)
{
   if(!str)
      return 0;
   return strlen(str);
}   


char* dStrupr(char *str)
{
   char* saveStr = str;
   while (*str)
   {
      *str = toupper(*str);
      str++;
   }
   return saveStr;
}   


char* dStrlwr(char *str)
{
   char* saveStr = str;
   while (*str)
   {
      *str = tolower(*str);
      str++;
   }
   return saveStr;
}   


char* dStrchr(char *str, int c)
{
   return strchr(str,c);
}   


const char* dStrchr(const char *str, int c)
{
   return strchr(str,c);
}   

const char* dStrrchr(const char *str, int c)
{
   return strrchr(str,c);
}   


char* dStrrchr(char *str, int c)
{
   return strrchr(str,c);
}   

dsize_t dStrspn(const char *str, const char *set)
{
   return(strspn(str, set));
}

dsize_t dStrcspn(const char *str, const char *set)
{
   return strcspn(str, set);
}   


char* dStrstr(char *str1, char *str2)
{
   return strstr(str1,str2);
}

char* dStrstr(const char *str1, const char *str2)
{
   return strstr(str1,str2);
}   

char* dStrtok(char *str, const char *sep)
{
   return strtok(str, sep);
}


int dAtoi(const char *str)
{
   if(!str)
      return 0;
   return atoi(str);
}  

 
float dAtof(const char *str)
{
   if(!str)
      return 0;
   return atof(str);   
}   

bool dAtob(const char *str)
{
   return !dStricmp(str, "true") || !dStricmp(str, "1") || (0!=dAtoi(str));
}   

bool dIsalnum(const char c)
{
   return isalnum(c);
}

bool dIsalpha(const char c)
{
   return(isalpha(c));
}

bool dIsspace(const char c)
{
   return(isspace(c));
}

bool dIsdigit(const char c)
{
   return(isdigit(c));
}

void dPrintf(const char *format, ...)
{
   va_list args;
   va_start(args, format);
   vprintf(format, args);
}   

int dVprintf(const char *format, void *arglist)
{
   S32 len = vprintf(format, (char*)arglist);

   return (len);
}   

int dSprintf(char *buffer, dsize_t bufferSize, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   S32 len = vsnprintf(buffer, bufferSize, format, args);

   AssertFatal( len < bufferSize, "dSprintf wrote to more memory than the specified buffer size" );

   return (len);
}   

int dVsprintf(char *buffer, dsize_t bufferSize, const char *format, void *arglist)
{
   S32 len = vsnprintf(buffer, bufferSize, format, (char*)arglist);

   AssertFatal( len < bufferSize, "dVsprintf wrote to more memory than the specified buffer size" );

   return (len);
}

int dSscanf(const char *buffer, const char *format, ...)
{
   va_list args;
   va_start(args, format);
   return vsscanf(buffer, format, args);   
}   

int dFflushStdout()
{
   return fflush(stdout);
}

int dFflushStderr()
{
   return fflush(stderr);
}

void dQsort(void *base, U32 nelem, U32 width, int (QSORT_CALLBACK *fcmp)(const void *, const void *))
{
   qsort(base, nelem, width, fcmp);
}   
