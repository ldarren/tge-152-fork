//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (c) 2002 GarageGames.Com
//-----------------------------------------------------------------------------

new GuiControlProfile (GuiDefaultProfile)
{
   tab = false;
   canKeyFocus = false;
   hasBitmapArray = false;
   mouseOverSelected = false;

   // fill color
   opaque = false;
   fillColor = "109 27 132";
   fillColorHL = "156 56 183";
   fillColorNA = "156 56 183";

   // border color
   border = false;
   borderColor   = "0 0 0"; 
   borderColorHL = "255 210 61";
   borderColorNA = "255 210 61";
   
   // bevel color
   bevelColorHL = "255 255 255";
   bevelColorLL = "0 0 0";

   // font
   fontType = "Smilage";
   fontSize = 25;
   fontCharset = CHINESEBIG5;

   fontColor = "255 255 255";
   fontColorHL = "255 210 61";
   fontColorNA = "255 255 255";
   fontColorSEL= "0 0 0";

   // bitmap information
   bitmap = "./demoWindow";
   bitmapBase = "";
   textOffset = "0 0";

   // used by guiTextControl
   modal = true;
   justify = "left";
   autoSizeWidth = false;
   autoSizeHeight = false;
   returnTab = false;
   numbersOnly = false;
   cursorColor = "255 255 255 255";

   // sounds
   soundButtonDown = "";
   soundButtonOver = "";
};

new GuiControlProfile (GuiDefaultProfile2)
{
   tab = false;
   canKeyFocus = false;
   hasBitmapArray = false;
   mouseOverSelected = false;

   // fill color
   opaque = false;
   fillColor = "109 27 132";
   fillColorHL = "156 56 183";
   fillColorNA = "156 56 183";

   // border color
   border = false;
   borderColor   = "0 0 0"; 
   borderColorHL = "255 210 61";
   borderColorNA = "255 210 61";
   
   // bevel color
   bevelColorHL = "255 255 255";
   bevelColorLL = "0 0 0";

   // font
   fontType = "Arial";
   fontSize = 14;
   fontCharset = CHINESEBIG5;

   fontColor = "255 255 255";
   fontColorHL = "255 210 61";
   fontColorNA = "255 255 255";
   fontColorSEL= "0 0 0";

   // bitmap information
   bitmap = "./demoWindow";
   bitmapBase = "";
   textOffset = "0 0";

   // used by guiTextControl
   modal = true;
   justify = "left";
   autoSizeWidth = false;
   autoSizeHeight = false;
   returnTab = false;
   numbersOnly = false;
   cursorColor = "255 255 255 255";

   // sounds
   soundButtonDown = "";
   soundButtonOver = "";
};


new GuiControlProfile (GuiWindowProfile)
{
   
   opaque = true;
   border = 1;
   fillColor = "109 27 132";
   fillColorHL = "163 34 180";
   fillColorNA = "109 27 132";
   fontColor = "255 255 255";
   fontColorHL = "255 210 61";
   text = "GuiWindowCtrl test";
   bitmap = "./demoWindow";
   textOffset = "0 0";
   hasBitmapArray = true;
   justify = "center";
};

new GuiControlProfile (GuiScrollProfile)
{
   opaque = true;
   fillColor = "109 27 132";
   border = 3;
   borderThickness = 2;
   borderColor = "0 0 0";
   bitmap = "./demoScroll";
   hasBitmapArray = true;
};

new GuiControlProfile (GuiScrollProfile2)
{
   fontType = "Smilage";
   fontColor = "200 200 200";
   opaque = true;
   fillColor = "109 27 132";
   border = 3;
   borderThickness = 2;
   borderColor = "0 0 0";
   bitmap = "./demoScroll";
   hasBitmapArray = true;
};


new GuiControlProfile (GuiCheckBoxProfile)
{
   opaque = false;
   fillColor = "109 27 132";
   border = false;
   borderColor = "0 0 0";
   fontType = "arial";
   fontSize = 14;
   fontColor = "255 255 255";
   fontColorHL = "255 210 61";
   fixedExtent = true;
   justify = "left";
   bitmap = "./demoCheck";
   hasBitmapArray = true;
};

new GuiControlProfile (GuiRadioProfile)
{
   fontType = "arial";
   fontSize = 14;
   fillColor = "255 255 255";
   fontColorHL = "255 210 61";
   fixedExtent = true;
   bitmap = "./demoRadio";
   hasBitmapArray = true;
};

new GuiControlProfile (GuiButtonProfile)
{
   fontSize = 25;
   fontColorHL = "255 210 61";
   justify = "center";
};

new GuiControlProfile (GuiTextProfile)
{
   fontColor = "237 174 235";
   autoSizeWidth = true;
   autoSizeHeight = true;
};

new GuiControlProfile (GuiControlListPopupProfile)
{
	fontType = "arial";
    fontSize = 14;
    fontColor = "255 255 255";
};

new GuiControlProfile (GuiPopUpMenuProfile)
{
    opaque = true;
    mouseOverSelected = true;

   border = 4;
   borderThickness = 2;
   borderColor = "0 0 0";
   fontType = "arial";
   fontColor = "255 255 255";
   fontSize = 14;
   fontColorHL = "255 210 61";
   fontColorSEL = "255 210 61";
   fixedExtent = true;
   justify = "center";
};

new GuiControlProfile (GuiMLTextProfile)
{
   fontType = "arial";
   fontColor = "255 255 255";
   fontSize = 14;
};

new GuiControlProfile (GuiTextListProfile) 
{
   fontType = "arial";
   fontColor = "255 255 255";
   fontSize = 16;
};

new GuiControlProfile (GuiMediumTextProfile)
{
   fontSize = 35;
};