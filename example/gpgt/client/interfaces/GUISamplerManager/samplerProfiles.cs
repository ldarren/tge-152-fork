//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
if(!isObject(gsDefaultProfile)) 
{
   new GuiControlProfile (gsDefaultProfile)
   {
      tab = false;
      canKeyFocus = false;
      hasBitmapArray = false;
      mouseOverSelected = false;

      // fill color
      opaque = true;
      fillColor	= "196 196 196 128";
      fillColorHL	= "204 255 255  128";
      fillColorNA	= "192 192 192 128";

      // border color
      border = 2;
      borderColor   = "33 0 102";
      borderColorHL = "33 0 255";
      borderColorNA = "64 64 64";

      // font
      fontType = "Arial";
      fontSize = 14;

      fontColor	= "153 0 0";
      fontColorHL	= "255 0 0";
      fontColorNA  = "0 0 0";
      fontColorSEL	= "200 200 200";

      // bitmap information
      bitmap = "./testwinframe0"; 
      textOffset = "0 0";

      // used by guiTextControl
      modal = true;
      justify = "left";
      autoSizeWidth = false;
      autoSizeHeight = false;
      returnTab = false;
      numbersOnly = false;
      cursorColor = "0 0 0 255";

      // sounds
      soundButtonDown = "";
      soundButtonOver = "";
   };
}

if (!isObject(gsLabel)) 
{
   new GuiControlProfile (gsLabel) {
      border      = 0;
      fontType    = "Tahoma";
      fontSize    = 14;
      fontColor   = "32 32 255";
      justify		= right;
   };
}

if (!isObject(gsBorder)) 
{
   new GuiControlProfile (gsBorder) {
      border				= 2;
      borderThickeness    = 1;
      borderColor         = "0 0 0";
   };
}

