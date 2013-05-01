
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// Shared Effects Elements
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// Shared Spell Audio Descriptions

datablock AudioDescription(SpellAudioDefault_AD)
{
  volume             = 1.0;
  isLooping          = false;
  is3D               = true;
  ReferenceDistance  = 20.0;
  MaxDistance        = 100.0;
  type               = $SimAudioType;
};

datablock AudioDescription(SpellAudioLoop_AD : SpellAudioDefault_AD)
{
  isLooping= true;
};

// good for casting sounds near spellecaster //

datablock AudioDescription(SpellAudioCasting_soft_AD : SpellAudioDefault_AD)
{
  ReferenceDistance= 10.0;
  MaxDistance = 30;
};

datablock AudioDescription(SpellAudioCasting_AD : SpellAudioDefault_AD)
{
  ReferenceDistance= 20.0;
  MaxDistance = 55;
};

datablock AudioDescription(SpellAudioCasting_loud_AD : SpellAudioDefault_AD)
{
  ReferenceDistance= 30.0;
  MaxDistance = 80;
};

// good for impacts //

datablock AudioDescription(SpellAudioImpact_AD : SpellAudioDefault_AD)
{
  ReferenceDistance= 25.0;
  MaxDistance= 120.0;
};

// good for projectiles //

datablock AudioDescription(SpellAudioMissileLoop_AD : SpellAudioDefault_AD)
{
  isLooping= true;
  ReferenceDistance= 10.0;
};

datablock AudioDescription(SpellAudioMissileLoop_loud_AD : SpellAudioDefault_AD)
{
  isLooping= true;
  ReferenceDistance= 25.0;
};

// good for shockwaves //

datablock AudioDescription(SpellAudioShockwaveLoop_AD : SpellAudioDefault_AD)
{
  isLooping= true;
  ReferenceDistance= 35.0;
  MaxDistance= 70.0;
};

datablock AudioDescription(SpellAudioShockwaveLoop_soft_AD : SpellAudioDefault_AD)
{
  isLooping= true;
  ReferenceDistance= 8.0;
  MaxDistance= 25.0;
};

// Shared Freeze X-Mod. Usually used to stick zodiacs to the
// ground.

datablock afxXM_FreezeData(SHARED_freeze_XM)
{
  mask = $afxXfmMod::POS;
};

datablock afxXM_SpinData(SHARED_MainZodeRevealLight_spin1_XM)
{
  spinAxis = "0 0 1";
  spinAngle = 0;
  spinRate = -30;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
