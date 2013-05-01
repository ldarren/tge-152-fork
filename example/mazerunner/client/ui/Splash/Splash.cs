echo("\c3--------- Loading Splash Screen  ---------");
//--------------------------------------------------------------------------
// Splash.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function SplashFadeinBitmap::onRemove( %this ) {
   cancel(%this.event);
}


function SplashFadeinBitmap::onWake( %this ) {
 
    %this.event = Canvas.schedule( 7000 , setContent ,SplashScreen2 );
}

function SplashFadeinBitmap::onSleep( %this ) 
{
   cancel(%this.event);
   alxStop($lastSound);
}


function SplashFadeinBitmap::click( %this ) {
   cancel(%this.event);
   Canvas.setContent(SplashScreen2);
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./Splash.gui");



