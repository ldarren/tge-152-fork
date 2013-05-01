 
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//  afxPerformSpellbookCast()
//
//    Used to cast spells from the player's spellbook.
//    Used by the spellbank gui to cast spells
//
function afxPerformSpellbookCast(%caster, %book_slot, %target, %client)
{
  %spell_data = spellbook.getSpellData(%book_slot);
  if (!isObject(%spell_data))
  {
    DisplayScreenMessage(%client, "Failed to find spell definition in spellbook.");
    return;
  }

  afxPerformSpellCast(%caster, %spell_data, %target, %client);
}

// Note - Players and NPCs can cast spells using afxPerformSpellCast(). 
// When used to cast an NPC spell, leave off the %client argument
// or pass a value of "". This prevents message display.
function afxPerformSpellCast(%caster, %spell_data, %target, %client)
{
  if (!isObject(%spell_data))
  {
    DisplayScreenMessage(%client, "Invalid spell definition.");
    return;
  }

  // test if caster exists
  if (!isObject(%caster))
  {
    DisplayScreenMessage(%client, "This client has no spellcaster.");
    return;
  }
 
  // test if caster is alive (enabled)
  if (!%caster.isEnabled())
  {
    DisplayScreenMessage(%client, "You're dead.");
    return;
  }

  // test is caster is already casting a spell
  if (%caster.spellBeingCast != 0)
  {
    DisplayScreenMessage(%client, "Already casting another spell.");
    return;
  }

  // test is caster is anim-locked
  if (%caster.isAnimationLocked())
  {
    DisplayScreenMessage(%client, "You're unable to concentrate.");
    return;
  }

  %rpg_data = %spell_data.rpgSpellData;
  if (!isObject(%rpg_data))
  {
    DisplayScreenMessage(%client, "Failed to find RPG spell definition.");
    return;
  }

  %mana_cost = %rpg_data.manaCost;
  %mana_pool = %caster.getEnergyLevel();

  // test if caster has enough mana
  if (%mana_pool < %mana_cost)
  {
    DisplayScreenMessage(%client, "Not enough mana.");
    return;
  }

  %tgt = %rpg_data.target;

  // clear superfluous target 
  if (%tgt $= "nothing")
    %target = 0;

  // test if target is required
  if (!isObject(%target))
  {
    if (%tgt $= "enemy" || %tgt $= "corpse" || %tgt $= "friend")
    {
      DisplayScreenMessage(%client, "Spell requires a target.");
      return;
    }
  }

  // validate target
  if (isObject(%target))
  {
    // make sure corpse targets are really dead 
    if (%tgt $= "corpse" && %target.isEnabled())
    {
      DisplayScreenMessage(%client, "Try targeting something that's dead.");
      return;
    }

    // make sure targeting self is allowed 
    if (%target $= %caster && %tgt !$= "self" && !%rpg_data.canTargetSelf)
    {
      DisplayScreenMessage(%client, "Casting this spell on yourself is not good idea.");
      return;
    }

    // check range
    if (%rpg_data.range > 0)
    {
      %target_dist = VectorDist(%caster.getWorldBoxCenter(), %target.getWorldBoxCenter());
      if (%target_dist > %rpg_data.range)
      {
        DisplayScreenMessage(%client, "Target is out of range.");
        return;
      }
    }
  }

  // self-targeting
  if (%tgt $= "self")
    %target = %caster;

  // spell datablock gets last chance to find reasons to fizzle
  if (!%spell_data.readyToCast(%caster, %target))
    return;

  if (isObject(%client)) // for cooldown display 
    %caster.activeSpellbook = %client.spellbook;
   
  castSpell(%spell_data, %caster, %target, %rpg_data);
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//  Default script methods called from afxMagicSpell


function afxMagicSpellData::onActivate(%this, %spell, %caster, %target)
{
  //echo("Default afxMagicSpellData::onActivate()");

  // attach active spell to caster. this prevent overlapping casts.
  %caster.spellBeingCast = %spell;

  // save mana recharge-rate, then set to zero while casting
  //%caster.rechargeRate_saved = %caster.getRechargeRate();
  %caster.setRechargeRate(0.0);

  // initiate global cooldown on spellbook
  if (isObject(%caster.activeSpellbook))
    %caster.activeSpellbook.startAllSpellCooldown();
}

function afxMagicSpellData::onLaunch(%this, %spell, %caster, %target, %missile)
{
  //if (isObject(%caster.client))
  //{
  //  %new_target = %caster.client.getSelectedObj();
  //  if (%new_target != %target)
  //    %spell.setTarget(%new_target);
  //}

  if (!isObject(%caster))
    return false;

  %caster.spellBeingCast = 0;
  //%caster.setRechargeRate(%caster.rechargeRate_saved);
  %caster.setRechargeRate(%caster.getDataBlock().rechargeRate);

  %rpg_data = %this.rpgSpellData;
  if (isObject(%rpg_data))
  {
    %mana_cost = %rpg_data.manaCost;
    %mana_pool = %caster.getEnergyLevel();

    // test if caster has enough mana
    //echo("ManaCost " @ %mana_cost @ "/" @ %mana_pool);
    //if (%mana_pool < %mana_cost)
    //  return false;

    %caster.setEnergyLevel(%mana_pool - %mana_cost);
  }

  return true;
}

function afxMagicSpellData::onImpact(%this, %spell, %caster, %impObj, %impPos, %impNorm)
{
  //echo("Default afxMagicSpellData::onImpact()");
  if (%this.rpgSpellData.directDamage != 0 || %this.rpgSpellData.areaDamageRadius > 0)
  {
    %dd_amt = %this.rpgSpellData.directDamage;
    %ad_amt = %this.rpgSpellData.areaDamage;
    %ad_rad = %this.rpgSpellData.areaDamageRadius;
    %ad_imp = %this.rpgSpellData.areaDamageImpulse;

    %this.onDamage(%spell, "directDamage", "spell", %impObj, %dd_amt, 0,
                   %impPos, %ad_amt, %ad_rad, %ad_imp);               
  }
}

function afxMagicSpellData::onInterrupt(%this, %spell, %caster)
{
  //echo("Default afxMagicSpellData::onInterrupt()");
  if (isObject(%caster))
  {
    %caster.spellBeingCast = 0;
    %caster.setRechargeRate(%caster.getDataBlock().rechargeRate);
  }
}

function afxMagicSpellData::onDeactivate(%this, %spell)
{
  //echo("Default afxMagicSpellData::onDeactivate()");
}

function afxMagicSpellData::readyToCast(%this, %caster, %target)
{
  //echo("Default afxMagicSpellData::readyToCast()");
  return true;
}

function afxMagicSpellData::onDamage(%this, %spell, %label, %flavor, %damaged_obj, 
                                     %amount, %count, %pos, %ad_amount, %radius, %impulse)
{
  // deal the direct damage
  if (isObject(%damaged_obj) && (%damaged_obj.getType() & $TypeMasks::PlayerObjectType))
    %damaged_obj.damage(%spell, %pos, %amount, %flavor);

  // deal area damage
  if (%radius > 0)
  {
    radiusDamage(%spell, %pos, %radius, %ad_amount, %flavor, %impulse);
  }
}

function afxTestSpellcastingDamageInterruption(%caster, %damage, %damageType)
{
  if (%caster.spellBeingCast == 0)
    return;

  %spell = %caster.spellBeingCast;
  %rpg_data = %spell.getDataBlock().rpgSpellData;
  if (%rpg_data.allowDamageInterrupts && %rpg_data.minDamageToInterrupt <= %damage)
  {
    %spell.interrupt();
    if (isObject(%caster.client))
      DisplayScreenMessage(%caster.client, "Spellcasting interrupted by damage.");
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
//  performSpellCastingInterrupt()
//
//    Used to interrupt a spell being cast by the player
//    associated with the given client.
//
function performSpellCastingInterrupt(%client)
{
  %caster = %client.player;
  if (%client.player.spellBeingCast != 0)
  {
    %client.player.spellBeingCast.interrupt();
    DisplayScreenMessage(%client, "Spellcasting interrupted.");
  }
}

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
