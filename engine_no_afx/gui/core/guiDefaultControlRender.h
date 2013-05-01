//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (c) 2002 GarageGames.Com
//-----------------------------------------------------------------------------

#ifndef _H_GUIDEFAULTCONTROLRENDER_
#define _H_GUIDEFAULTCONTROLRENDER_

class GuiControlProfile;

void renderRaisedBox(RectI &bounds, GuiControlProfile *profile);
void renderSlightlyRaisedBox(RectI &bounds, GuiControlProfile *profile);
void renderLoweredBox(RectI &bounds, GuiControlProfile *profile);
void renderSlightlyLoweredBox(RectI &bounds, GuiControlProfile *profile);
void renderBorder(RectI &bounds, GuiControlProfile *profile);
void renderFilledBorder( RectI &bounds, GuiControlProfile *profile );
void renderFilledBorder( RectI &bounds, ColorI &borderColor, ColorI &fillColor );
void renderSizableBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile); // DAW: Added
void renderSizableBitmapBordersFilledIndex(RectI &bounds, S32 startIndex, GuiControlProfile *profile);
void renderFixedBitmapBordersFilled(RectI &bounds, S32 baseMultiplier, GuiControlProfile *profile); // DAW: Added
void renderFixedBitmapBordersFilled(RectI &bounds, S32 startIndex, GuiControlProfile *profile);

#endif
