
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
//
// Shared Effects Elements
//
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

// setup the effects data path
$afxSpellDataPath = expandFilename("~/data/effects");

// reset the selectron styles
resetDemoSelectronStyles();

// init shared effects datablocks
exec("./afxSharedFX.cs");

// setup spellbook placeholders
exec("./afxPlaceholders.cs");

// autoload spells and other effects modules
echo("Autoloading AFX Modules...");
autoloadAFXModules();

// create spellbook
datablock afxSpellBookData(SpellBookData)
{
  spellBookName = "AFX Demo Spellbook";
};

// load autoloaded spells and placeholders
// into spellbook.
loadDemoSpellbook(SpellBookData);

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
