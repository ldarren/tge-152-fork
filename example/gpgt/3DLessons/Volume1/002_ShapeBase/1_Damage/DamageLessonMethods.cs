//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
package DamageLessonPackage {
   function swapHealthSkin( %theShape ) {
      if(!isObject(%theShape)) return;

      %damagePercent = %theShape.getDamagePercent();

      if(0.0 == %damagePercent) {
         %theShape.setSkinName( "full" );
      } else if (0.5 > %damagePercent)  {
         %theShape.setSkinName( "half" );
      } else  {
         %theShape.setSkinName( "no" );
      }

      schedule( 100 , %theShape , swapHealthSkin , %theShape );

   }

   function determineDamageState( %theShape ) {
      if(!isObject(%theShape)) return;

      %curDamage      = %theShape.getDamageLevel();
      %destroyedDamage = %theShape.getDatablock().destroyedLevel;
      %disabledDamage = %theShape.getDatablock().disabledLevel;

      if( %curDamage > %destroyedDamage ) {
         %theShape.setDamageState( Destroyed );
         //		return;
      } else if( %curDamage > %disabledDamage ) {
         %theShape.setDamageState( Disabled );
      } else {
         %theShape.setDamageState( Enabled );
      }

      schedule( 100 , 0 , determineDamageState , %theShape );
   }

   function SelfHealingBlock::onAdd( %DB , %theShape ) {

      //
      // Note: You must set the repair rate yourself!  
      //  Just placing a value in the datablock does not initialize it.
      //
      %theShape.setRepairRate( %DB.repairRate ); 

      // 
      // Check to see if we need to change the skin (based on damage) 
      // in 100 ms.
      //
      schedule( 100 , %theShape , swapHealthSkin , %theShape );

   }

   function DisableGears::onAdd( %DB , %theShape ) {

      //
      // Note: You must set the repair rate yourself!  
      //  Just placing a value in the datablock does not initialize it.
      //
      %theShape.setRepairRate( %DB.repairRate ); 

      //
      // Start the gear rotation animation
      //
      %theShape.playThread( 0 , "rotate" );

      //
      // Check to see if we need to change the 'damage state' for these
      // gears in 100 ms.
      //
      schedule( 100 , 0 , determineDamageState , %theShape );
   }

   // 
   // When these gears are enabled be sure they are rotating.
   //
   function DisableGears::onEnabled( %DB , %theShape ) {
      %theShape.playThread( 0 , "rotate" );
   }

   // 
   // When these gears are disabled be sure they are paused (not rotating).
   //
   function DisableGears::onDisabled( %DB , %theShape ) {
      %theShape.pauseThread( 0 );
   }

   // 
   // When the damage ball touches an object, it causes a pre-specified ammount of damage
   //
   function DamageBall::onCollision( %colliderDB , %colliderObj , %collidedObj, %fade, %position, %normal)
   {
      //echo("DamageBall::onCollision()::applying damage... to " @ %collidedObj);
      //echo("DamageBall::onCollision()::applying damage... of " @ %colliderDB.damageAmmount);
      %collidedObj.applyDamage( %colliderDB.damageAmmount );

      %colliderObj.setTransform( %colliderObj.initialTransform ); 
      %colliderObj.setVelocity("0 0 0");
      
   }
};