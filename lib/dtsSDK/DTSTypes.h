#ifndef __DTSTYPES_H
#define __DTSTYPES_H

#include <vector>
#include <string>
#include <ostream>
#include <cmath>
#include <cassert>

#include "DTSPoint.h"

#ifndef _WIN32
#define __cdecl
#define strnicmp strncasecmp
#define stricmp strcasecmp
inline float min(float a, float b) { return a<b ? a : b; }
inline float max(float a, float b) { return a>b ? a : b; }
#endif

//! The DTS namespace implements all the classes needed to load a DTS model
//! and to create one from the Milkshape data. There are many basic classes
//! here, such as Point or Quaternion.

namespace DTS
{

}

#endif