
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// Define spell placeholders for the demo spellbook. These
// placeholders reserve a spot in the demo spellbook for
// a spell which may or may not get auto-loaded later. If
// the actual spell is not loaded, the placeholder will
// display an inactive icon in the spellbank.

$CORE_TECH = "Core Tech";
$SPELLPACK_ONE = "Spell Pack 1";

// PRIMARY PLACEHOLDERS

datablock afxRPGMagicSpellData(FB_Placeholder_RPG)
{
  name = "Flame Broil";
  desc = "This fiery combat spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/fb";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(SOR_Placeholder_RPG)
{
  name = "Spirit of Roach";
  desc = "This creepy crawly buff spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/sor";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(RM_Placeholder_RPG)
{
  name = "Reaper Madness";
  desc = "This shimmering resurrection spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/rm";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(MLF_Placeholder_RPG)
{
  name = "Mapleleaf Frag";
  desc = "This mighty nature spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/mlf";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(GBOF_Placeholder_RPG)
{
  name = "Great Ball of Fire";
  desc = "This enormous fireball spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/gbof";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(CTAF_Placeholder_RPG)
{
  name = "Cantrip and Fall";
  desc = "This childish knockdown spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/ctaf";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(SFM_Placeholder_RPG)
{
  name = "Summon Feckless Moth";
  desc = "This giant insect summoning spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/sfm";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(TH_Placeholder_RPG)
{
  name = "Thor's Hammer";
  desc = "This anachronistic area effect spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/th";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(AP_Placeholder_RPG)
{
  name = "Astral Passport";
  desc = "This essential transportation spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/ap";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(AITM_Placeholder_RPG)
{
  name = "Arcane in the Membrane";
  desc = "This psycho debuff spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/aitm";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(IOP_Placeholder_RPG)
{
  name = "Insectoplasm";
  desc = "This monster insect combat spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/iop";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(LMF_Placeholder_RPG)
{
  name = "Light My Fire";
  desc = "This hypnotic bonfire effect is currently not installed.";
  iconBitmap = "~/data/effects/icons/lmf";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

// LAB PLACEHOLDERS

datablock afxRPGMagicSpellData(MGG_Placeholder_RPG)
{
  name = "Mark of GG";
  desc = "This novelty spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/mgg";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(RoHC_Placeholder_RPG)
{
  name = "Rid of Habeas Corpus";
  desc = "This waste management spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/rohc";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(SMKN_Placeholder_RPG)
{
  name = "Smokin'";
  desc = "This smoldering experimental spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/smkn";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(TFL_Placeholder_RPG)
{
  name = "Teleport to FaustLogic.com";
  desc = "This novelty spell is currently not installed.";
  iconBitmap = "~/data/effects/icons/tfl";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(TMG_Placeholder_RPG)
{
  name = "Try MachineGun";
  desc = "This experimental spell is currently not installed.";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(TDB_Placeholder_RPG)
{
  name = "Try Debris Burst";
  desc = "This experimental spell is currently not installed.";
  sourcePack = $SPELLPACK_ONE;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(PHT_Placeholder_RPG)
{
  name = "Phrase Tester";
  desc = "This testing spell is currently not installed.";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

// SCI-FI PLACEHOLDERS

datablock afxRPGMagicSpellData(SF_OL_Placeholder_RPG)
{
  name = "Occam's Laser (sci-fi)";
  desc = "This sci-fi effect is currently not installed.";
  iconBitmap = "~/data/effects/icons/sf";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(SF_OL2_Placeholder_RPG)
{
  name = "Occam's Laser Reloaded (sci-fi)";
  desc = "This sci-fi effect is currently not installed.";
  iconBitmap = "~/data/effects/icons/sf";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

datablock afxRPGMagicSpellData(SF_OL3_Placeholder_RPG)
{
  name = "Occam's Laser Redux (sci-fi)";
  desc = "This sci-fi effect is currently not installed.";
  iconBitmap = "~/data/effects/icons/sf";
  sourcePack = $CORE_TECH;
  isPlaceholder = true;
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//

// Add placeholders to slots in the demo spellbook.

// PRIMARY SPELLBANK
addDemoSpellbookPlaceholder(GBOF_Placeholder_RPG,  0, 0);
addDemoSpellbookPlaceholder(RM_Placeholder_RPG,    0, 1);
addDemoSpellbookPlaceholder(AP_Placeholder_RPG,    0, 2);
addDemoSpellbookPlaceholder(LMF_Placeholder_RPG,   0, 3);
addDemoSpellbookPlaceholder(FB_Placeholder_RPG,    0, 4);
addDemoSpellbookPlaceholder(SOR_Placeholder_RPG,   0, 5);
addDemoSpellbookPlaceholder(MLF_Placeholder_RPG,   0, 6);
addDemoSpellbookPlaceholder(CTAF_Placeholder_RPG,  0, 7);
addDemoSpellbookPlaceholder(SFM_Placeholder_RPG,   0, 8);
addDemoSpellbookPlaceholder(TH_Placeholder_RPG,    0, 9);
addDemoSpellbookPlaceholder(AITM_Placeholder_RPG,  0, 10);
addDemoSpellbookPlaceholder(IOP_Placeholder_RPG,   0, 11);

// LAB SPELLBANK
addDemoSpellbookPlaceholder(MGG_Placeholder_RPG,   1, 0);
addDemoSpellbookPlaceholder(RoHC_Placeholder_RPG,  1, 1);
addDemoSpellbookPlaceholder(SMKN_Placeholder_RPG,  1, 2);
addDemoSpellbookPlaceholder(TMG_Placeholder_RPG,   1, 3);
addDemoSpellbookPlaceholder(TFL_Placeholder_RPG,   1, 4);
addDemoSpellbookPlaceholder(TDB_Placeholder_RPG,   1, 5);
addDemoSpellbookPlaceholder(PHT_Placeholder_RPG,   1, 11);

// SCI-FI SPELLBANK
addDemoSpellbookPlaceholder(SF_OL_Placeholder_RPG,   2, 0);
addDemoSpellbookPlaceholder(SF_OL2_Placeholder_RPG,  2, 1);
addDemoSpellbookPlaceholder(SF_OL3_Placeholder_RPG,  2, 2);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

