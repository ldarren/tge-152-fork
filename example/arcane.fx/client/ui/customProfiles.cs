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
  fillColor = "177 177 177";
  fillColorHL = "197 197 197";
  fillColorNA = "197 197 197";
  
  // border color
  border = false;
  borderColor   = "0 0 0"; 
  borderColorHL = "136 136 136";
  borderColorNA = "81 81 81";
  
  // font
  fontType = "Arial";
  fontSize = 14;
  
  fontColor = "0 0 0";
  fontColorHL = "255 18 0";
  fontColorNA = "0 0 0";
  fontColorSEL= "200 200 200";
  
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
  cursorColor = "0 0 0 255";
  
  // sounds
  soundButtonDown = "";
  soundButtonOver = "";
};

new GuiControlProfile (GuiWindowProfile)
{
  opaque = true;
  border = 2;
  fillColor = "177 177 177";
  fillColorHL = "197 197 197";
  fillColorNA = "197 197 197";
  fontColor = "255 255 255";
  fontColorHL = "255 255 255";
  text = "GuiWindowCtrl test";
  bitmap = "./demoWindow";
  textOffset = "6 6";
  hasBitmapArray = true;
  justify = "center";
};

new GuiControlProfile(AFXToolTipProfile)
{
   tab = false;
   canKeyFocus = false;
   hasBitmapArray = false;
   mouseOverSelected = false;

   // fill color
   opaque = true;
   fillColor = "200 200 200 220";

   // border color
   border = true;
   borderColor   = "0 0 0";
   
   fontColor = "0 0 0";
   fontColorHL = "0 0 0";
   fontColorNA = "0 0 0";
   

   // used by guiTextControl
   modal = true;
   justify = "left";
   autoSizeWidth = false;
   autoSizeHeight = false;
   returnTab = false;
   numbersOnly = false;
   cursorColor = "0 0 0 255";

};

new GuiControlProfile (GuiScrollProfile)
{
  opaque = true;
  fillColor = "255 255 255";
  border = 3;
  borderThickness = 2;
  borderColor = "0 0 0";
  bitmap = "./demoScroll";
  hasBitmapArray = true;
};

new GuiControlProfile (GuiCheckBoxProfile)
{
  opaque = false;
  fillColor = "232 232 232";
  border = false;
  borderColor = "0 0 0";
  fontSize = 14;
  fontColor = "0 0 0";
  fontColorHL = "255 18 0";
  fixedExtent = true;
  justify = "left";
  bitmap = "./demoCheck";
  hasBitmapArray = true;
};

new GuiControlProfile (GuiRadioProfile)
{
  fontSize = 14;
  fillColor = "232 232 232";
  fontColorHL = "255 18 0";
  fixedExtent = true;
  bitmap = "./demoRadio";
  hasBitmapArray = true;
};

new GuiControlProfile (GuiTextProfile)
{
   fontColor = "0 0 0";
   fontColorLink = "255 96 96";
   fontColorLinkHL = "0 0 255";
   autoSizeWidth = true;
   autoSizeHeight = true;
};

new GuiControlProfile (GuiTextArrayProfile : GuiTextProfile)
{
  fontColorHL = "255 18 0";
  fillColorHL = "200 200 200";
};

new GuiControlProfile (GuiButtonProfile)
{
  opaque = true;
  border = true;
  fontColor = "0 0 0";
  fontColorHL = "255 18 0";
  fixedExtent = true;
  justify = "center";
  canKeyFocus = false;
};

new GuiControlProfile (GuiProgressProfile)
{
   opaque = false;
   fillColor = "178 13 0 200";
   border = true;
   borderColor   = "0 0 0";
};

new GuiControlProfile (GuiTextEditProfile)
{
   opaque = true;
   border = 3;
   borderThickness = 2;
   textOffset = "0 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = true;
   canKeyFocus = true;

   fontColor = "0 0 0";
   fontColorHL = "255 255 255";
   fontColorNA = "128 128 128";
   fillColor = "225 225 225";
   fillColorHL = "128 128 128";
   borderColor = "0 0 0";
   borderColorNA = "56 56 56";
   borderColorHL = "159 159 159";

   bevelColorHL = "241 241 241";
   bevelColorLL = "159 159 159";
};

new GuiControlProfile (HUDProfile : GuiDefaultProfile)
{
  fontSize = 16;
};
