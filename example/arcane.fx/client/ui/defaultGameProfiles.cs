//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Override base controls
GuiButtonProfile.soundButtonOver = "AudioButtonOver";

//-----------------------------------------------------------------------------
// Chat Hud profiles


new GuiControlProfile (ChatHudEditProfile)
{
   opaque = false;
   fillColor = "255 255 255";
   fillColorHL = "128 128 128";
   border = false;
   borderThickness = 0;
   borderColor = "40 231 240";
   fontColor = "40 231 240";
   fontColorHL = "40 231 240";
   fontColorNA = "128 128 128";
   textOffset = "0 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = true;
   canKeyFocus = true;
};

new GuiControlProfile (ChatHudTextProfile)
{
   opaque = false;
   fillColor = "255 255 255";
   fillColorHL = "128 128 128";
   border = false;
   borderThickness = 0;
   borderColor = "40 231 240";
   fontColor = "40 231 240";
   fontColorHL = "40 231 240";
   fontColorNA = "128 128 128";
   textOffset = "0 0";
   autoSizeWidth = true;
   autoSizeHeight = true;
   tab = true;
   canKeyFocus = true;
};

new GuiControlProfile ("ChatHudMessageProfile")
{
   fontType = "Arial";
   fontSize = 16;
   fontColor = "44 172 181";      // default color (death msgs, scoring, inventory)
   fontColors[1] = "4 235 105";   // client join/drop, tournament mode
   fontColors[2] = "219 200 128"; // gameplay, admin/voting, pack/deployable
   fontColors[3] = "77 253 95";   // team chat, spam protection message, client tasks
   fontColors[4] = "40 231 240";  // global chat
   fontColors[5] = "200 200 50 200";  // used in single player game
   // WARNING! Colors 6-9 are reserved for name coloring 
   autoSizeWidth = true;
   autoSizeHeight = true;
};

new GuiControlProfile ("ChatHudScrollProfile")
{
   opaque = false;
   border = false;
   borderColor = "0 255 0";
   bitmap = "common/ui/darkScroll";
   hasBitmapArray = true;
};


//-----------------------------------------------------------------------------
// Common Hud profiles

new GuiControlProfile ("HudScrollProfile")
{
   opaque = false;
   border = true;
   borderColor = "0 255 0";
   bitmap = "common/ui/darkScroll";
   hasBitmapArray = true;
};

new GuiControlProfile ("HudTextProfile")
{
   opaque = false;
   fillColor = "128 128 128";
   fontColor = "0 255 0";
   border = true;
   borderColor = "0 255 0";
};

new GuiControlProfile ("ChatHudBorderProfile")
{
   bitmap = "./chatHudBorderArray";
   hasBitmapArray = true;
   opaque = false;
};


//-----------------------------------------------------------------------------
// Center and bottom print

new GuiControlProfile ("CenterPrintProfile")
{
   border = false;
   opaque = false;
   fillColor = "0 0 0 200";
};

new GuiControlProfile ("CenterPrintTextProfile")
{
   border = false;
   opaque = false;
   fontType = "Arial";
   fontSize = 22;
};

new GuiControlProfile ("InfoBoxProfile")
{
   fontColorLink = "255 96 96";
   fontColorLinkHL = "0 0 255";
};

new GuiControlProfile ("InfoBoxBackdropProfile")
{
   fontColorLink = "255 96 96";
   fontColorLinkHL = "0 0 255";
   opaque = true;
   fillColor = "0 0 0 200";
};

new GuiControlProfile (AFXButtonProfile)
{
   fillColor = "0 0 0 100";
   fillColorHL = "254 14 25 32";
   fillColorNA = "20 20 20";

   borderColor = "120 120 120";
   borderColorHL = "157 157 157";
   borderColorNA = "64 64 64";

   opaque = true;
   border = 1;
   fontColor = "60 119 108";
   fontColorHL = "254 14 25";
   fixedExtent = true;
   justify = "center";
	 canKeyFocus = false;
   fontSize = 18;
};

new GuiControlProfile (AFXTextProfile)
{
   fontSize = 18;
   fontColor = "60 119 108";
   fontColorLink = "255 96 96";
   fontColorLinkHL = "0 0 255";
   autoSizeWidth = true;
   autoSizeHeight = true;
};

new GuiControlProfile (AFXCheckBoxProfile)
{
   opaque = false;
   fillColor = "232 232 232";
   border = false;
   borderColor = "0 0 0";
   fontSize = 18;
   fontColor = "60 119 108";
   fontColorHL = "255 18 0";
   fixedExtent = true;
   justify = "left";
   bitmap = "./demoCheck";
   hasBitmapArray = true;
};


new GuiControlProfile (AFXButtonStackProfile)
{
   tab = false;
   canKeyFocus = false;
   hasBitmapArray = false;
   mouseOverSelected = false;

   // fill color
   opaque = false;
   fillColor = "255 0 0 100";
   fillColorHL = "221 202 173";
   fillColorNA = "221 202 173";

   // border color
   border = false;
   borderColor   = "0 0 0"; 
   borderColorHL = "179 134 94";
   borderColorNA = "126 79 37";

   // font
   fontType = "Arial";
   fontSize = 14;

   fontColor = "0 0 0";
   fontColorHL = "32 100 100";
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