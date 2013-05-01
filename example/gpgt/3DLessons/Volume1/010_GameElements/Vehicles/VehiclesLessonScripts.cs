//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
function serverCmdbumpVehicle( %client ) 
{
   if( ! isObject($currentVehicle)) return;
   
   if( "FlyingVehicle" $= $currentVehicle.getClassName())
   {
      //%zImpulse = $currentVehicle.getDatablock().mass * 50;
      // Don't try to bounce (apply impulses to )the flying vehicle.
      // It will resist this too much to make it useful.
      // Instead, adjust the elevation by 10 world units
      %transform = $currentVehicle.getTransform();
      %elevation = getWord( %transform , 2 );
      %elevation += 10;
      %transform = setWord(%transform , 2 , %elevation );
      //%transform = vectorAdd( $transform , "0 0 10 0 0 0 0");
      $currentVehicle.setTransform( %transform );
   } 
   else
   {
      %zImpulse = $currentVehicle.getDatablock().mass * 5;
   }
   
   $currentVehicle.applyImpulse( $currentVehicle.getWorldBoxCenter() , "0 0 " @ %zImpulse );
}
function serverCmdselectBoxCar( %client ) 
{
   DropVehicle(1);
}
function serverCmdselectPsionicJeep( %client ) 
{
   DropVehicle(2);
}
function serverCmdselectHoverBox( %client ) 
{
   DropVehicle(3);
}

function serverCmdselectFlyingBox( %client ) 
{
   DropVehicle(4);
}



function DropVehicle(%vehicleNum) {
	if(%vehicleNum < 1) return;
	if(%vehicleNum > 4) return;

	// 
	// Deleting the group deletes the contents
	//
	if(isObject($currentVehicle)) $currentVehicle.delete();

	switch(%vehicleNum) {
	case 1:
		%class	= WheeledVehicle;
		%DB		= BoxCar;
//		$Game::ClientHandle.player.enableMountVehicle = 1; // Mount vehicle?
//		$Game::ClientHandle.player.enableSwapVehicle  = 0; // Swap vehicle in as player?

	case 2:
		%class	= WheeledVehicle;
		%DB		= PsionicJeep0;
//		$Game::ClientHandle.player.enableMountVehicle = 1; // Mount vehicle?
//		$Game::ClientHandle.player.enableSwapVehicle  = 0; // Swap vehicle in as player?

	case 3:
		%class	= HoverVehicle;
		%DB		= BoxHover;
//		$Game::ClientHandle.player.enableMountVehicle = 1; // Mount vehicle?
//		$Game::ClientHandle.player.enableSwapVehicle  = 0; // Swap vehicle in as player?

 case 4:
		%class	= FlyingVehicle;
		%DB		= BoxFlyer;
//		$Game::ClientHandle.player.enableMountVehicle = 1; // Mount vehicle?
//		$Game::ClientHandle.player.enableSwapVehicle  = 0; // Swap vehicle in as player?
	}
	
	//
	// Build up a command and evaluate it (be sure not to use tab 
	// characters, as they will NOT eval correctly...)
	//
	%buildIt = 	"$currentVehicle = new " @ %class @ "() {";
	%buildIt = %buildIt @ "    datablock = " @ %DB @ ";";
	%buildIt = %buildIt @ "    position = North10.getPosition();";
	%buildIt = %buildIt @ "    disableMove = \"0\";";
	%buildIt = %buildIt @ "};";
	%buildIt = %buildIt @ "DropObject($currentVehicle, \"0 0 2\");";
	%buildIt = %buildIt @ "VehiclesLesson.add( $currentVehicle );";

	echo(%buildIt);
	eval(%buildIt);
}
	
