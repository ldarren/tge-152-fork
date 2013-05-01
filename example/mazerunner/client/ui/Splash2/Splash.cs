echo("\c3--------- Loading Splash Screen  ---------");
//--------------------------------------------------------------------------
// Splash.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function SplashFadeinBitmap2::onRemove( %this ) {
   cancel(%this.event);
}


function SplashFadeinBitmap2::onWake( %this ) {
 
    $lastSound = alxPlay(AudioStartup1);
    
    %this.event = Canvas.schedule( 9000 , setContent ,SplashScreen3);
}

function SplashFadeinBitmap2::onSleep( %this ) 
{
   cancel(%this.event);
   alxStop($lastSound);   
}


function SplashFadeinBitmap2::click( %this ) {
   alxStop($lastSound);
   cancel(%this.event);
   Canvas.setContent(SplashScreen3);
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./Splash.gui");



