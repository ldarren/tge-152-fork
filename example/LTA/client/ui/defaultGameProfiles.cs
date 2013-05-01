//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Override base controls

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

new GuiControlProfile ("ChatHudBorderProfile")
{
   bitmap = "common/ui/darkBorder.png";
   hasBitmapArray = true;
   opaque = false;
};


//-----------------------------------------------------------------------------
// Center and bottom print

new GuiControlProfile ("CenterPrintProfile")
{
   opaque = false;
   fillColor = "128 128 128";
   fontColor = "0 255 0";
   border = true;
   borderColor = "0 255 0";
};

new GuiControlProfile ("CenterPrintTextProfile")
{
   opaque = false;
   fontType = "Arial";
   fontSize = 12;
   fontColor = "0 255 0";
};

new GuiControlProfile (GuiUnicodeTextProfile:GuiTextProfile)
{
   // font
   fontType = "Tahoma";
   fontSize = 14;
};

new GuiControlProfile (GuiUnicodeWindowProfile:GuiWindowProfile)
{
   // font
   fontType = "Tahoma";
   fontSize = 14;
};

new GuiControlProfile (GuiUnicodeButtonProfile:GuiButtonProfile)
{
   // font
   fontType = "Tahoma";
   fontSize = 14;
   opaque = true;
   border = true;
   fontColor = "0 0 0";
   fontColorHL = "32 100 100";
   fixedExtent = true;
   justify = "center";
	canKeyFocus = false;
};

new GuiControlProfile (LTAGuiButtonProfile : GuiButtonProfile)
{
	soundButtonOver = "LTAButtonHoverAudio";
	soundButtonDown = "LTAButtonDownAudio";
};

if(!isObject(LTAGuiCheckboxProfile)) new GuiControlProfile (LTAGuiCheckboxProfile : GuiCheckBoxProfile)
{
	bitmap = "./pictures/buttons/RadioButton";
	hasBitmapArray = true;
	opaque=false;
	border=false;
	soundButtonDown = "LTACheckDownAudio";
};

if(!isObject(LTAGuiRadioProfile)) new GuiControlProfile (LTAGuiRadioProfile : GuiRadioProfile)
{
	bitmap = "./pictures/buttons/RadioButton";
	hasBitmapArray = true;
	opaque=false;
	border=false;
	soundButtonDown = "LTACheckDownAudio";
};

if(!isObject(LTAGuiProgressProfile)) new GuiControlProfile (LTAGuiProgressProfile : GuiProgressProfile)
{
   opaque = true;
   fillColor = "44 250 52 255";
   border = false;
   borderColor   = "78 88 120";
};

if(!isObject(LTAGuiScrollProfile))  new GuiControlProfile (LTAGuiScrollProfile)
{
   opaque = true;
   fillColor = "210 210 255";
   border = 3;
   borderThickness = 2;
   borderColor = "0 0 0";
   bitmap = "common/ui/osxScroll";
   hasBitmapArray = true;
};

if(!isObject(LTAGuiTextListProfile)) new GuiControlProfile (LTAGuiTextListProfile)
{
   opaque = true;
   fillColor = "128 255 255";
   fillColorHL = "255 210 50";
   border = true;
   borderColor = "0 0 0";
   fontColor = "0 0 0";
   fontColorHL = "128 130 255";
   fontColorNA = "128 128 128";
   textOffset = "0 2";
   autoSizeWidth = false;
   autoSizeHeight = true;
   tab = true;
   canKeyFocus = true;
};

