//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "interior/interiorMap.h"
#include "map2difPlus/csgBrush.h"
#include "map2difPlus/morianBasics.h"
#include "map2difPlus/editGeometry.h"
#include "map2difPlus/entityTypes.h"
#include "console/console.h"

void loadTextures(InteriorMap* map);
void convertInteriorMap(InteriorMap* map);
void getBrushes(InteriorMap* map);
void getWorldSpawn(InteriorMap* map);
void getEntities(InteriorMap* map);

#endif //_CONVERT_H_
