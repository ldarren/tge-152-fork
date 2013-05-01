echo("\c3--------- Loading Splash Screen  ---------");
//--------------------------------------------------------------------------
// Splash.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function SplashFadeinBitmap3::onRemove( %this ) {
   cancel(%this.event);
}


function SplashFadeinBitmap3::onWake( %this ) {
    %this.event = Canvas.schedule( 4200 , setContent ,MainMenu );
}

function SplashFadeinBitmap3::onSleep( %this ) 
{
   cancel(%this.event);
}


function SplashFadeinBitmap3::click( %this ) {
   cancel(%this.event);
   Canvas.setContent(MainMenu);
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./Splash.gui");



