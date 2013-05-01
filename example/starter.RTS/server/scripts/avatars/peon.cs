datablock RTSUnitData(villagerBlock : UnitBaseBlock)
{
   shapeFile = "~/data/shapes/peasant/player.dts";
   RTSUnitTypeName = "villager";
   baseDamage = 10;
   attackDelay = 42;
   damagePlus = 1;
   
   armor = 1;
   
   moveSpeed = 3;
   
   range = 12;
   
   maxDamage = 150; //maxDamage = health
   vision = 100;
   
   boundingBox = "2.0 2.0 2.0";
};

function peasantBlock::onAttack(%this, %attacker, %target)
{
   %damage = %attacker.getDataBlock().baseDamage;
   if(%attacker.getNetModifier().baseDamage)
      %damage *= %attacker.getNetModifier().baseDamage;

   %armor  = %target.getDataBlock().armor;
   if(%target.getNetModifier().armor)
      %armor *= %target.getNetModifier().armor;
      
   if(%damage > %armor)
      %damage -= %armor;
   else
      %damage = 0;
// it's not nice to apply damage directly
   %target.applyDamage(%damage);
 }
