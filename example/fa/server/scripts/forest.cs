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
   shapeFile     = "fa/data/shapes/trees/shrub.dts";
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
   shapeFile     = "fa/data/shapes/trees/oak1.dts";
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
   shapeFile     = "fa/data/shapes/trees/oak2.dts";
   centered      = false;
   disableBillboardLighting = false;
};

function newForest::onAdd(%this) 
{ 
   %this.addForestEntry("BurningBush", "fa/data/terrains/highplains/rock", 100, 5);
   %this.addForestEntry("OakTree01", "fa/data/terrains/highplains/scorched", 100, 5);
   %this.addForestEntry("OakTree02", "fa/data/terrains/highplains/stone", 100, 5);
}
