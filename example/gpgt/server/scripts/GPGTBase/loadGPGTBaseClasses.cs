//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\c3******************** Loading Volume 1 Base Classes");

exec("./Audio/AudioDescriptions.cs"); 
exec("./Audio/AudioProfiles.cs"); 

exec("./Markers/markers.cs"); 

exec("./Trigger/Trigger.cs"); 

exec("./Particles/ParticleData/ParticleData.cs"); 
exec("./Particles/ParticleEmitters/ParticleEmitter.cs"); 
exec("./Particles/ParticleEmitterNodes/ParticleEmitterNode.cs"); 

exec("./Decals/DecalManager.cs"); 
exec("./Decals/DecalData.cs"); 

exec("./ShapeBase/ShapeBase.cs");
exec("./ShapeBase/ShapeBaseMethods.cs");
exec("./ShapeBase/ShapeBaseDataMethods.cs");

exec("./ShapeBaseImage/ShapeBaseImage.cs");

exec("./Item/Item.cs");
exec("./Item/ItemMethods.cs");
exec("./Item/ItemDataMethods.cs");

exec("./StaticShape/StaticShape.cs");
exec("./StaticShape/StaticShapeDataMethods.cs");
	
exec("./Camera/CameraGlobals.cs");
exec("./Camera/Camera.cs");
exec("./Camera/CameraDataMethods.cs");

exec("./Player/PlayerMethods.cs");
exec("./Player/PlayerDataMethods.cs");
exec("./Player/Player.cs");

exec("./Vehicles/VehicleDataMethods.cs");

exec("./Vehicles/WheeledVehicle/WheeledVehicles_BoxCar.cs");
exec("./Vehicles/WheeledVehicle/WheeledVehicleDataMethods.cs");

exec("./Vehicles/HoverVehicle/HoverVehicles_BoxHover.cs");
exec("./Vehicles/HoverVehicle/HoverVehicleDataMethods.cs");

exec("./Precipitation/Precipitation.cs"); 


exec("./Lightning/Lightning.cs"); 


