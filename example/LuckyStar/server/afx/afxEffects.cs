
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// Shared Effects Elements
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// setup the effects data path
if (afxLegacyLighting())
  $afxSpellDataPath = "~/data/effects_LL";
else
  $afxSpellDataPath = "~/data/effects";

// reset the selectron styles
resetDemoSelectronStyles();

// init shared effects datablocks
exec("./afxSharedFX.cs");

// setup spellbook placeholders
//exec("./afxPlaceholders.cs"); // DARREN MOD: no spellbook needed

// autoload spells and other effects modules
echo("Autoloading AFX Modules...");
autoloadAFXModules();
/* DARREN MOD: no spell needed
// create spellbook
datablock afxSpellBookData(SpellBookData)
{
  spellBookName = "AFX Demo Spellbook";
};

// load autoloaded spells and placeholders
// into spellbook.
loadDemoSpellbook(SpellBookData);
*/
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
