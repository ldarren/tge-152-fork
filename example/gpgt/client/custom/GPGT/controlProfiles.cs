//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Hall Of Worlds Profiles (for GPGT Kit)  ---------");
//--------------------------------------------------------------------------
// HOWProfiles.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


if(!isObject(gpgtButtonProfile1)) new GuiControlProfile (gpgtButtonProfile1)
{
   opaque = true;
   border = true;
   fontColor = "0 0 0";
   fontColorHL = $fontColorHL;
   fixedExtent = true;
   justify = "center";
	canKeyFocus = false;
};

if(!isObject(gpgtMainPanel)) new GuiControlProfile (gpgtMainPanel)
{
	opaque = true;
	border = 0;
};

if(!isObject(gpgtWindow)) new GuiControlProfile (gpgtWindow)
{
	opaque = true;
	border = 2;

	fillColor = "102 102 255";
	fillColorHL = "204 102 255";
	fillColorNA = "192 192 192";

	fontColor = "255 255 255";
	fontColorHL = "255 0 0";
	hasBitmapArray = true;
	bitmap = "~/client/custom/howwindowframe";
	textOffset = "6 6";
	justify = "left";
};

if(!isObject(gpgtBitmapBorder)) new GuiControlProfile (gpgtBitmapBorder)
{
	opaque = false;
	border = 2;
	fillColor = "255 153 255";
	fillColorHL = "204 255 255";
	fillColorNA = "255 255 51";
	fontColor = "255 255 255";
	fontColorHL = "255 0 0";
	hasBitmapArray = true;
	bitmap = "~/client/custom/nicerbitmapborder";
	textOffset = "6 6";
	justify = "left";
};

if(!isObject(GuiModelessMLTextProfile)) new GuiControlProfile ("GuiModelessMLTextProfile")
{
   modal = false; // Inputs will pass right through the GUIMLTextCtrl
   canKeyFocus = false;
	fontColor = "0 0 0 0"; // Hides numeric feedback
	fillColor = "0 0 0 0"; // Hides numeric feedback
//   fontColorLink = "255 96 96";
//   fontColorLinkHL = "0 0 255";
};
