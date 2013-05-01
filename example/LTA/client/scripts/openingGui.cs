// ============================================================
// Project            :  VR20
// File               :  .\LTA\client\scripts\openingGui.cs
// Copyright          :  
// Author             :  Darren
// Created on         :  Thursday, June 14, 2007 4:22 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================
function loadOpening()
{
   Canvas.setContent( OpeningGui );
}

function OpeningGui::onWake(%this)
{
	Canvas.hideCursor();
	Theora.setFile("LTA/data/video/opening.ogg");
	//Theora.mStopOnSleep = true; // stop on sleep
	schedule(10000, 0, checkOpeningDone );
}

//-------------------------------------
function OpeningGui::onSleep(%this)
{
   //alxStop($audioHandle);

   Theora.stop();
}

//-------------------------------------
function checkOpeningDone()
{
	%currentTheoraTime = Theora.getCurrentTime();
	//echo(%currentTheoraTime);
	if ( %currentTheoraTime >= 51.3569) // not a good method
	{
		Theora.done = true;
		loadMainMenu();
	}
	else
	{
		schedule(1000, 0, checkOpeningDone );
	}
}
