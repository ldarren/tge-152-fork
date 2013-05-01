// ============================================================
// Project            :  test.mmo
// File               :  .\test.mmo\server\scripts\forest.cs
// Copyright          :  
// Author             :  ldarren
// Created on         :  Sunday, October 28, 2007 4:09 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================
new ForestItemData(BurningBush)
{
   collidable    = true;
   groundAligned = false;
   maxItems      = 3;
   radius        = 2.00;
   minScaleFactor= 0.8;
   maxScaleFactor= 1.5;
   shapeFile     = "test.mmo/data/shapes/forest/shrub.dts";
   centered      = false;
   disableBillboardLighting = false;
};

new ForestItemData(OakTree01)
{
   collidable    = true;
   groundAligned = false;
   maxItems      = 6;
   radius        = 4.00;
   minScaleFactor= 0.8;
   maxScaleFactor= 1.5;
   shapeFile     = "test.mmo/data/shapes/forest/oak1.dts";
   centered      = false;
   disableBillboardLighting = false;
};

new ForestItemData(OakTree02)
{
   collidable    = true;
   groundAligned = false;
   maxItems      = 6;
   radius        = 4.00;
   minScaleFactor= 0.8;
   maxScaleFactor= 1.5;
   shapeFile     = "test.mmo/data/shapes/forest/oak2.dts";
   centered      = false;
   disableBillboardLighting = false;
};

function newForest::onAdd(%this) 
{ 
   %this.addForestEntry("BurningBush", "test.mmo/data/terrains/forest/ForestDirt", 15, 5);
   %this.addForestEntry("OakTree01", "test.mmo/data/terrains/forest/ForestGrass", 15, 5);
   %this.addForestEntry("OakTree02", "test.mmo/data/terrains/forest/ForestRock", 15, 5);
}
