//------------------------------------------------------
// Copyright (c) 2000-2005, GarageGames.com, Inc.
//------------------------------------------------------

// Caution: You may find that placing this file too deeply in your hiearchy will cause
// the game to crash/fail when transmitting datablocks.  The reason for this is
// that the total size of this datablock will exceed 1024 bytes which is the default
// limit for a single packet.  
//
// Solutions:
// 1. Move lower in hierarchy
// 2. Increase packet size.
// 3. Modify packet code to split this into two or more packets.
//


datablock TSShapeConstructor(PlayerDts)
{
   baseShape = "./player.dts";
   sequence0 = "./player_root.dsq root";
   sequence1 = "./player_forward.dsq run";
   sequence2 = "./player_back.dsq back";
   sequence3 = "./player_side.dsq side";
   sequence4 = "./player_lookde.dsq look";
   sequence5 = "./player_head.dsq head";
   sequence6 = "./player_fall.dsq fall";
   sequence7 = "./player_land.dsq land";
   sequence8 = "./player_jump.dsq jump";
   sequence9  = "./player_diehead.dsq death1";
   sequence10 = "./player_diechest.dsq death2";
   sequence11 = "./player_dieback.dsq death3";
   sequence12 = "./player_diesidelf.dsq death4";
   sequence13 = "./player_diesidert.dsq death5";
   sequence14 = "./player_dieleglf.dsq death6";
   sequence15 = "./player_dielegrt.dsq death7";
   sequence16 = "./player_dieslump.dsq death8";
   sequence17 = "./player_dieknees.dsq death9";
   sequence18 = "./player_dieforward.dsq death10";  
   sequence19 = "./player_diespin.dsq death11";
   sequence20 = "./player_looksn.dsq looksn";
   sequence21 = "./player_lookms.dsq lookms";
   sequence22 = "./player_scoutroot.dsq scoutroot";
   sequence23 = "./player_headside.dsq headside";
   sequence24 = "./player_recoilde.dsq light_recoil";
   sequence25 = "./player_sitting.dsq sitting";
   sequence26 = "./player_celsalute.dsq celsalute";
   sequence27 = "./player_celwave.dsq celwave";
   sequence28 = "./player_standjump.dsq standjump";
   sequence29 = "./player_looknw.dsq looknw";
};         
