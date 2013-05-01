
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#include "afx/arcaneFX.h"

#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mathIO.h"

#include "afx/util/afxPath.h"
#include "afx/util/afxPath3D.h"

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// afxPathData

IMPLEMENT_CONSOLETYPE(afxPathData)
IMPLEMENT_GETDATATYPE(afxPathData)
IMPLEMENT_SETDATATYPE(afxPathData)
IMPLEMENT_CO_DATABLOCK_V1(afxPathData);

static EnumTable::Enums PathLoopEnums[] =
{
   { afxPath3D::LOOP_CONSTANT,  "constant"  },
   { afxPath3D::LOOP_CYCLE,     "cycle"     },   
   { afxPath3D::LOOP_OSCILLATE, "oscillate" }, 
};
static EnumTable PathLoopTable(3, PathLoopEnums);

afxPathData::afxPathData()
{
  points_string = ST_NULLSTRING;
  roll_string   = ST_NULLSTRING;
  loop_string   = ST_NULLSTRING;
  delay = 0;
  lifetime = 0;
  num_points = 0;
  points = 0;
  rolls = 0;
  loop_type = 0;
  mult = 1.0f;
  time_offset = 0.0f;
  resolved = false;
  reverse = false;
  offset.zero();
  echo = false;
  concentric = false;
}

afxPathData::~afxPathData()
{
  if (points)
    delete [] points;
  if (rolls)
    delete [] rolls;
}

void afxPathData::initPersistFields()
{
  Parent::initPersistFields();

  addField("points",     TypeString,  Offset(points_string, afxPathData));
  addField("roll",       TypeString,  Offset(roll_string,   afxPathData));
  addField("loop",       TypeString,  Offset(loop_string,   afxPathData));
  addField("mult",       TypeF32,     Offset(mult,          afxPathData));
  addField("delay",      TypeF32,     Offset(delay,         afxPathData));
  addField("lifetime",   TypeF32,     Offset(lifetime,      afxPathData));
  addField("timeOffset", TypeF32,     Offset(time_offset,   afxPathData));
  addField("reverse",    TypeBool,    Offset(reverse,       afxPathData));
  addField("offset",     TypePoint3F, Offset(offset,        afxPathData));
  addField("echo",       TypeBool,    Offset(echo,         afxPathData));
  addField("concentric", TypeBool,    Offset(concentric,   afxPathData));
}

bool afxPathData::onAdd()
{
  if (Parent::onAdd() == false)
    return false;

  // Points ////////////////////////////////////////////////////////////

  // bail if points_string is NULL
  if (points_string == ST_NULLSTRING && num_points == 0) 
  {
    Con::warnf(ConsoleLogEntry::General, "afxPathData(%s) no points_string, invalid datablock", getName());
    return false;
  }

  bool do_rolls = false;
  if (num_points == 0)
  {
    // Tokenize input string and convert to Point3F array
    //
    Vector<char*> dataBlocks(__FILE__, __LINE__);

    // make a copy of points_string
    char* tokCopy = new char[dStrlen(points_string) + 1];
    dStrcpy(tokCopy, points_string);

    // extract tokens one by one, adding them to dataBlocks
    char* currTok = dStrtok(tokCopy, " \t");
    while (currTok != NULL) 
    {
      dataBlocks.push_back(currTok);
      currTok = dStrtok(NULL, " \t");
    }

    // bail if there were no tokens in the string
    if (dataBlocks.size() == 0) 
    {
      Con::warnf(ConsoleLogEntry::General, "afxPathData(%s) invalid points string. No tokens found", getName());
      delete [] tokCopy;
      return false;
    }

    // Find num_points (round up to multiple of 3) // WARNING here if not multiple of 3?
    for (U32 i = 0; i < dataBlocks.size()%3; i++) 
    {
      dataBlocks.push_back("0.0");
    }

    num_points = dataBlocks.size()/3;
    points = new Point3F[num_points];
    do_rolls = true;

    if (reverse)
    {    
      U32 p_i = 0;
      for (S32 i = dataBlocks.size()-1; i > 0; i-=3, p_i++)
      {        
        F32 x,y,z;        
        x = dAtof(dataBlocks[i-2]);  // What about overflow?
        y = dAtof(dataBlocks[i-1]);
        z = dAtof(dataBlocks[i]);
        points[p_i].set(x,y,z);        
      }
    }
    else
    {
      U32 p_i = 0;
      for (U32 i = 0; i < dataBlocks.size(); i+=3, p_i++)
      {
        F32 x,y,z;
        x = dAtof(dataBlocks[i]);  // What about overflow?
        y = dAtof(dataBlocks[i+1]);
        z = dAtof(dataBlocks[i+2]);
        points[p_i].set(x,y,z);
      }
    }

    // Add offset, here for efficiency (saves an addition from afxXM_PathConform)
    for (U32 i = 0; i < num_points; i++)
      points[i] += offset;
      
    delete [] tokCopy; 
  }

  // Roll ////////////////////////////////////////////////////////////

  if (do_rolls)
  {
    if (roll_string != ST_NULLSTRING) 
    {   
      // Tokenize input string and convert to F32 array
      //
      Vector<char*> dataBlocks(__FILE__, __LINE__);

      // make a copy of roll_string
      char* tokCopy = new char[dStrlen(roll_string) + 1];
      dStrcpy(tokCopy, roll_string);

      // extract tokens one by one, adding them to dataBlocks
      char* currTok = dStrtok(tokCopy, " \t");
      while (currTok != NULL) 
      {
        dataBlocks.push_back(currTok);
        currTok = dStrtok(NULL, " \t");
      }

      // bail if there were no tokens in the string
      if (dataBlocks.size() == 0) 
      {
        Con::warnf(ConsoleLogEntry::General, "afxPathData(%s) invalid roll string.  No tokens found", getName());
        delete [] tokCopy;
        return false;
      }

      // Append 0.0 if not enough values
      if (dataBlocks.size() < num_points ) 
      {
        for (U32 i=0; i < num_points; i++) 
          dataBlocks.push_back("0.0");
      }     

      rolls = new F32[num_points];
 
      if (reverse)
      {
        U32 r_i = 0;
        for (S32 i = num_points-1; i >= 0; i--, r_i++ )
        {          
          rolls[r_i] = dAtof(dataBlocks[i]);  // What about overflow?    
        }
      }
      else
      {
        for (U32 i = 0; i < num_points; i++ )
        {
          rolls[i] = dAtof(dataBlocks[i]);  // What about overflow?    
        }
      }
      
      delete [] tokCopy; 
    }
  }

  // Loop string ////////////////////////////////////////////////////////////
  //
  if (loop_string != ST_NULLSTRING) {
    // look for matching name in PathLoopTable      
    for (U32 i = 0; i < PathLoopTable.size; i++)
    {
      if (dStricmp(PathLoopTable.table[i].label, loop_string) == 0)
      {
        loop_type = PathLoopTable.table[i].index;
        break;
      }
    }
  }
  else {
    loop_string = PathLoopTable.table[0].label;
    loop_type   = PathLoopTable.table[0].index;
  }
  //Con::warnf(ConsoleLogEntry::General, "afxPathData(%s) loop type: %s (%i)", getName(), loop_string, loop_type);        
   

  return true;
}

void afxPathData::onRemove()
{
  if (points)
    delete [] points;
  points = 0;
  num_points = 0;

  if (rolls)
    delete [] rolls;
  rolls = 0;

  loop_type = 0;

  Parent::onRemove();
}

void afxPathData::packData(BitStream* stream)
{
  Parent::packData(stream);

  //stream->writeString(points_string);
  //stream->writeString(roll_string);

  stream->write(num_points);
  for (U32 i = 0; i < num_points; i++)
    mathWrite(*stream, points[i]);
  if (!rolls)
    stream->write((U32)0);
  else
  {
    stream->write(num_points);
    for (U32 i = 0; i < num_points; i++)
      stream->write(rolls[i]);
  }
  stream->writeString(loop_string);
  stream->write(delay);
  stream->write(lifetime);
  stream->write(time_offset);
  stream->write(mult);
  stream->writeFlag(reverse);
  mathWrite(*stream, offset);
  stream->writeFlag(echo);
  stream->writeFlag(concentric);
}

void afxPathData::unpackData(BitStream* stream)
{
  Parent::unpackData(stream);

  if (points)
    delete [] points;
  points = 0;
  num_points = 0;

  if (rolls)
    delete [] rolls;
  rolls = 0;

  //points_string = stream->readSTString();
  //roll_string = stream->readSTString();

  // read the points and rolls
  stream->read(&num_points);
  if (num_points > 0)
  {
    points = new Point3F[num_points];
    for (U32 i = 0; i < num_points; i++)
      mathRead(*stream, &points[i]);
    U32 n_rolls;
    stream->read(&n_rolls);
    if (n_rolls == num_points)
    { 
      rolls = new F32[num_points]; 
      for (U32 i = 0; i < num_points; i++)
        stream->read(&rolls[i]);
    }
  }

  loop_string = stream->readSTString();
  stream->read(&delay);
  stream->read(&lifetime);
  stream->read(&time_offset);
  stream->read(&mult);
  reverse = stream->readFlag();
  mathRead(*stream, &offset);
  echo = stream->readFlag();
  concentric = stream->readFlag();
}

bool afxPathData::preload(bool server, char errorBuffer[256])
{
  if (!Parent::preload(server, errorBuffer))
    return false;

  //if (!server)
  //{    
  //}

  return true;
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
