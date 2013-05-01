// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\client\splashGui.cs
// Copyright          :  
// Author             :  Alucard
// Created on         :  Wednesday, August 29, 2007 2:58 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function loadSplashScreen()
{
   //loadMyMission();
   splashScreen.done = "0";
   Canvas.setContent( splashScreen );
   schedule(100, 0, checkSplashDone );
}

function checkSplashDone()
{
   if (splashScreen.done == "1")
   {
		//loadMyMission();
   }
   else
      schedule(100, 0, checkSplashDone );
}