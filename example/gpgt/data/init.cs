//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

function autoLoadPropertyMaps() 
{
   echo("\n\c3******************** Automatically finding and loading all propertyMaps.cs files...");

   //// Find every map file and load lesson file specified within
   //
   %propertyMapFile = findFirstFile("*/propertyMap.cs");

   while("" !$= %propertyMapFile) 
   {
      exec(%propertyMapFile);

      echo("\c4...Loaded PropertyMap from directory: ", filePath(%propertyMapFile) );

      %propertyMapFile = findNextFile("*/propertyMap.cs");
   }
}
autoLoadPropertyMaps();