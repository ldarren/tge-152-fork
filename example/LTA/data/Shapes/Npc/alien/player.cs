//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

datablock TSShapeConstructor(PlayerDts)
{
   baseShape = "./player.dts";
   sequence0 = "./player_root.dsq root";
   sequence1 = "./player_forward.dsq run";
};

datablock TSShapeConstructor(PlayerBicycleDts)
{
   baseShape = "./player_b.dts";
   sequence0 = "./player_b_root.dsq root";
   sequence1 = "./player_b_forward.dsq run";
};

datablock TSShapeConstructor(PlayerCarDts)
{
   baseShape = "./player_c.dts";
   sequence0 = "./player_c_root.dsq root";
   sequence1 = "./player_c_forward.dsq run";
};