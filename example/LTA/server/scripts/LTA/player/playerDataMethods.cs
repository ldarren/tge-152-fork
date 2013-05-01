// ============================================================
// Project            :  VR20
// File               :  .\LTA\server\scripts\LTA\player\player.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Thursday, July 19, 2007 2:36 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================
/*
datablock PlayerData(AlienBody : StandardBody)
{
	shapeFile = "~/data/shapes/player/player.dts";
};
*/
datablock PlayerData(FemaleBody : StandardBody)
{
	shapeFile = "~/data/shapes/Player/models/human_female/human_female_1.dts";
};

datablock PlayerData(MaleBody : StandardBody)
{
	shapeFile = "~/data/shapes/Player/models/human_male/human_male_1.dts";
};
/*
datablock PlayerData(ThomBody : StandardBody)
{
	shapeFile = "~/data/shapes/thomPlayer/player.dts";
};
*/