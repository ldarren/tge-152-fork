//------------------------------------------------------
// Copyright 2000-2005, GarageGames.com, Inc.
// Written, modfied, or otherwise interpreted by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading Coin Datablock Definition and Scripts ---------");

function rldcoin() {
exec("./coins.cs");
}

datablock ItemData( Coin : BaseItem )
{
   shapeFile = "~/data/coin/coin.dts";
   category  = "GameItems";
   sticky    = true; 
   lightType = NoLight; 
   mass      = 1.0;
   respawn   = false;
};

// ******************************************************************
//					onAdd() 
// ******************************************************************
//
// 1. Force coin to be static and to use the item class' rotation animation.
//
function Coin::onAdd( %DB , %Obj )
{
   Parent::onAdd( %DB , %Obj );
   
   // 1
   %Obj.static			= true; 
   %Obj.rotate			= true;
   
   CoinsGroup.add( %Obj );
}


// ******************************************************************
//					 Coin::onPickup() 
// ******************************************************************
//
// 1. Call parent version of onPickup() first!
// 2. Check for no-remaining coins.
// 3. Return status from parent call
//
function Coin::onPickup( %pickupDB , %pickupObj , %ownerObj )
{
   // 1
   %status = Parent::onPickup( %pickupDB , %pickupObj , %ownerObj );
   scorecounter.setCounterValue( scorecounter.getCounterValue() + 1 );
   coincounter.setCounterValue( CoinsGroup.getCount() );

   // 2
   if (CoinsGroup.getCount() == 0 )
   {
      $Game::Player.setTransform ( "0 0 10000" );
      //serverConnection.setBlackout(1 , 500 );
      
      
      // 2a
      BuildLevel($Game::NextLevelMap);

      // 2c
      $Game::Player.lives++;
      
      // 2c
      livescounter.setCounterValue($Game::Player.lives);
   }
   
   // 3
   return %status;
}
