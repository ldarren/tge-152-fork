//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

//new GuiControlProfile (GuiDefaultProfile)
//{
//   tab = false;
//   canKeyFocus = false;
//   hasBitmapArray = false;
//   mouseOverSelected = false;
//
//   // fill color
//   opaque = false;
//   fillColor = "201 182 153";
//   fillColorHL = "221 202 173";
//   fillColorNA = "221 202 173";
//
//   // border color
//   border = false;
//   borderColor   = "0 0 0"; 
//   borderColorHL = "179 134 94";
//   borderColorNA = "126 79 37";
//   
//   // bevel color
//   bevelColorHL = "255 255 255";
//   bevelColorLL = "0 0 0";
//
//   // font
//   fontType = "Arial";
//   fontSize = 14;
//
//   fontColor = "0 0 0";
//   fontColorHL = "32 100 100";
//   fontColorNA = "0 0 0";
//   fontColorSEL= "200 200 200";
//
//   // bitmap information
//   bitmap = "./demoWindow";
//   bitmapBase = "";
//   textOffset = "0 0";
//
//   // used by guiTextControl
//   modal = true;
//   justify = "left";
//   autoSizeWidth = false;
//   autoSizeHeight = false;
//   returnTab = false;
//   numbersOnly = false;
//   cursorColor = "0 0 0 255";
//
//   // sounds
//   soundButtonDown = "";
//   soundButtonOver = "";
//};
//
//new GuiControlProfile (GuiWindowProfile)
//{
//   opaque = true;
//   border = 2;
//   fillColor = "201 182 153";
//   fillColorHL = "221 202 173";
//   fillColorNA = "221 202 173";
//   fontColor = "255 255 255";
//   fontColorHL = "255 255 255";
//   text = "GuiWindowCtrl test";
//   bitmap = "./demoWindow";
//   textOffset = "6 6";
//   hasBitmapArray = true;
//   justify = "center";
//};
//
//new GuiControlProfile (GuiScrollProfile)
//{
//   opaque = true;
//   fillColor = "255 255 255";
//   border = 3;
//   borderThickness = 2;
//   borderColor = "0 0 0";
//   bitmap = "./demoScroll";
//   hasBitmapArray = true;
//};
//
//new GuiControlProfile (GuiCheckBoxProfile)
//{
//   opaque = false;
//   fillColor = "232 232 232";
//   border = false;
//   borderColor = "0 0 0";
//   fontSize = 14;
//   fontColor = "0 0 0";
//   fontColorHL = "32 100 100";
//   fixedExtent = true;
//   justify = "left";
//   bitmap = "./demoCheck";
//   hasBitmapArray = true;
//};
//
//new GuiControlProfile (GuiRadioProfile)
//{
//   fontSize = 14;
//   fillColor = "232 232 232";
//   fontColorHL = "32 100 100";
//   fixedExtent = true;
//   bitmap = "./demoRadio";
//   hasBitmapArray = true;
//};


//GPGT*********************************************************************
//GPGT*********************************************************************
//GPGT** Note: I don't personally like the formatting used by the console, so
//GPGT** I have modified some of the colors:
//GPGT*********************************************************************
//GPGT*********************************************************************
if(!isObject(GuiConsoleProfile)) {
   new GuiControlProfile (GuiConsoleProfile)
   {
      fontType      = ($platform $= "macos") ? "Courier New" : "Lucida Console";
      fontSize      = ($platform $= "macos") ? 14 : 12;
      fontColor     = "0 0 0";
      fontColorHL   = "130 130 130";
      fontColorNA   = "0 0 0";
      fontColors[0] = "0 0 0";	    //GPGT - Black
      fontColors[1] = "255 204 0";   //GPGT - Orange
      fontColors[2] = "255 0 0";	    //GPGT - Pure Red
      fontColors[3] = "51 102 255";  //GPGT - Blue
      fontColors[4] = "53 204 53";   //GPGT - Green
      fontColors[5] = "255 0 255";	 //GPGT - Purple
      fontColors[6] = "50 50 50";
      fontColors[7] = "50 50 0";  
      fontColors[8] = "0 0 50"; 
      fontColors[9] = "0 50 0";   
   };
} else {
   GuiConsoleProfile.fontColors[0] = "0 0 0";	    //GPGT - Black
   GuiConsoleProfile.fontColors[1] = "255 204 0";   //GPGT - Orange
   GuiConsoleProfile.fontColors[2] = "255 0 0";	    //GPGT - Pure Red
   GuiConsoleProfile.fontColors[3] = "51 102 255";  //GPGT - Blue
   GuiConsoleProfile.fontColors[4] = "102 204 102"; //GPGT - Green
   GuiConsoleProfile.fontColors[5] = "255 0 255";	 //GPGT - Purple
   GuiConsoleProfile.fontColors[6] = "50 50 50";
   GuiConsoleProfile.fontColors[7] = "50 50 0";  
   GuiConsoleProfile.fontColors[8] = "0 0 50"; 
   GuiConsoleProfile.fontColors[9] = "0 50 0";   
}
