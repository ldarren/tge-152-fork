//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------

package EnergyLessonPackage {
   function checkEnergy( %theShape ) {
      if(!isObject(%theShape)) return;

      %energyPercent = %theShape.getEnergyPercent();


      if(%theShape.getDatablock().disabledEnergyLevelPercent >= %energyPercent) {

         %theShape.pauseThread( 0 );

      } else  {

         %theShape.playThread( 0 , "rotate" );
   
      }
   
      schedule( 100 , 0 , checkEnergy , %theShape );
   }

   function swapEnergySkin( %theShape ) {
      if(!isObject(%theShape)) return;

      %energyPercent = %theShape.getEnergyPercent();

      if(0.0 == %energyPercent) {
         %theShape.setSkinName( "no" );
      } else if (0.5 > %energyPercent)  {
         %theShape.setSkinName( "half" );
      } else  {
         %theShape.setSkinName( "full" );
      }

      schedule( 100 , %theShape , swapEnergySkin , %theShape );
   }

   function determineEnergyState( %theShape ) {
      if(!isObject(%theShape)) return;

      %curEnergy      = %theShape.getEnergyLevel();
      %destroyedEnergy = %theShape.getDatablock().destroyedLevel;
      %disabledEnergy = %theShape.getDatablock().disabledLevel;

      if( %curEnergy > %destroyedEnergy ) {
         %theShape.setEnergyState( Destroyed );
         //		return;
      } else if( %curEnergy > %disabledEnergy ) {
         %theShape.setEnergyState( Disabled );
      } else {
         %theShape.setEnergyState( Enabled );
      }

      schedule( 100 , 0 , determineEnergyState , %theShape );
   }

   function SelfHealingBlock::onAdd( %DB , %theShape ) {

      //
      // Note: You must set the repair rate yourself!  
      //  Just placing a value in the datablock does not initialize it.
      //
      %theShape.setRechargeRate( %DB.repairRate ); 

      // 
      // Check to see if we need to change the skin (based on Energy) 
      // in 100 ms.
      //
      schedule( 100 , %theShape , swapHealthSkin , %theShape );

   }

   function DisableEnergyGears::onAdd( %DB , %theShape ) {

      //
      // Note: You must set the recharge rate yourself!  
      //  Just placing a value in the datablock does not initialize it.
      //
      %theShape.setRechargeRate( %DB.rechargeRate ); 

      // 
      // Check to see if we need to update the energy marker
      // in 100 ms.
      //
      schedule( 100 , 0 , checkEnergy , %theShape );
   
   }

   function SelfRechargingBlock::onAdd( %DB , %theShape ) {

      //
      // Note: You must set the recharge rate yourself!  
      //  Just placing a value in the datablock does not initialize it.
      //
      %theShape.setRechargeRate( %DB.rechargeRate ); 

      schedule( 100 , %theShape , swapEnergySkin , %theShape );
   }

   // 
   // When the Energy ball touches an object, it causes a pre-specified ammount of Energy
   //
   function EnergyBall::onCollision( %colliderDB , %colliderObj , %collidedObj, %fade, %position, %normal)
   {
      //echo("EnergyBall::onCollision()::applying Energy... to " @ %collidedObj);
      //echo("EnergyBall::onCollision()::applying Energy... of " @ %colliderDB.EnergyAmmount);
      %newEnergy = %collidedObj.getEnergyLevel() - %colliderDB.damageAmmount;
      
      %newEnergy = (%newEnergy > 0) ? %newEnergy : 0;
      
      %collidedObj.setEnergyLevel( %newEnergy );

      %colliderObj.setTransform( %colliderObj.initialTransform ); 
      %colliderObj.setVelocity("0 0 0");
      
   }
};
