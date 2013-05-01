function loadOpening()
{
   Canvas.setContent( OpeningGui );
}

function OpeningGui::onWake(%this)
{
	Canvas.hideCursor();
	Theora.setFile($modname@"/data/video/opening.ogg");
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
