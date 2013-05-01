//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3--------- Loading GuiWindowCtrl Samples ---------");
//--------------------------------------------------------------------------
// gsWindow.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

if(!isObject(gsGWindow)) new GuiControlProfile (gsGWindow)
{
	opaque = true;
	border = 2;
	//fillColor = "255 153 255";
	//fillColorHL = "204 255 255";
	//fillColorNA = "255 255 51";

	fillColor = "102 102 255";
	fillColorHL = "204 102 255";
	fillColorNA = "192 192 192";

	fontColor = "255 255 255";
	fontColorHL = "255 0 0";
	hasBitmapArray = true;
	bitmap = "./gwindowframe";
	textOffset = "6 6";
	justify = "left";
};

////
// *****************  NO BITMAP ARRAY
////
if(!isObject(gsGuiWinProf_NoBMP)) new GuiControlProfile (gsGuiWinProf_NoBMP)
{
   opaque			= true;
   border			= 2;
   fillColor		= "192 192 192";
   fillColorHL		= "64 150 150";
   fillColorNA		= "150 150 150";
   fontColor		= "255 255 255";
   fontColorHL		= "0 0 0";
   text				= "gsGuiWinProf_NoBMP";
   hasBitmapArray	= false;
   textOffset		= "6 6";
   justify			= "left";
};

////
// *****************  MS Windows Themed Window
////
if(!isObject(gsGuiWinProf_Windows)) new GuiControlProfile (gsGuiWinProf_Windows)
{
   opaque			= true;
   border			= 2;
   fillColor		= "192 192 192";
   fillColorHL		= "64 150 150";
   fillColorNA		= "150 150 150";
   fontColor		= "255 255 255";
   fontColorHL		= "0 0 0";
   text				= "gsGuiWinProf_Windows";
   hasBitmapArray	= true;
   bitmap			= "./darkWindow";
   textOffset		= "6 6";
   justify			= "left";
};

////
// *****************  MS Windows Themed Window (50% Translucent)
////
if(!isObject(gsGuiWinProf_TranWin)) new GuiControlProfile (gsGuiWinProf_TranWin : gsGuiWinProf_Windows)
{
   opaque			= false;
   fillColor		= "192 192 192 128";
   fillColorHL		= "64 150 150 128";
   fillColorNA		= "150 150 150 128";
};

////
// *****************  OSX Themed Window
////
if(!isObject(gsGuiWinProf_OSX)) new GuiControlProfile (gsGuiWinProf_OSX)
{
   opaque			= true;
   border			= 2;
   fillColor		= "211 211 211";
   fillColorHL		= "190 255 255";
   fillColorNA		= "255 255 255";
   fontColor		= "0 0 0";
   fontColorHL		= "200 200 200";
   text				= "gsGuiWinProf_OSX";
   hasBitmapArray	= true;
   bitmap			= "./osxWindow";
   textOffset		= "5 5";
   justify			= "center";
};

////
// *****************  OSX Themed Window (50% Translucent)
////
if(!isObject(gsGuiWinProf_TranOSX)) new GuiControlProfile (gsGuiWinProf_TranOSX : gsGuiWinProf_OSX)
{
   opaque			= false;
   fillColor		= "211 211 211 128";
   fillColorHL		= "190 255 255 128";
   fillColorNA		= "255 255 255 128";
};

////
// *****************  Test Window 0
////
if(!isObject(gsGuiWinTest0)) new GuiControlProfile (gsGuiWinTest0)
{
   opaque			= true;
   border			= 2;
   fillColor		= "192 192 192";
   fillColorHL		= "64 150 150";
   fillColorNA		= "150 150 150";
   fontColor		= "255 255 255";
   fontColorHL		= "0 0 0";
   hasBitmapArray	= true;
   bitmap			= "./testwinframe0";
   textOffset		= "6 6";
   justify			= "left";
};
////
// *****************  Test Window 0
////
if(!isObject(gsGuiWinTest1)) new GuiControlProfile (gsGuiWinTest1 : gsGuiWinTest0)
{
   hasBitmapArray	= true;
   bitmap			= "./testwinframe1";
};


//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
exec("./gsWindow.gui");

