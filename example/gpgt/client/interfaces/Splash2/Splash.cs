echo("\c3--------- Loading Splash Screen  ---------");
//--------------------------------------------------------------------------
// Splash.cs
//--------------------------------------------------------------------------
new AudioProfile(GGAudioStartup)
{
   filename = "./GGSplash.ogg";
   description = "AudioGui";
   preload = true;
};

//--------------------------------------------------------------------------
// Scripts
//--------------------------------------------------------------------------
function SplashFadeinBitmap2::onRemove( %this ) {
   cancel(%this.event);
}


function SplashFadeinBitmap2::onWake( %this ) {
 
    $lastSound = alxPlay(GGAudioStartup);
    
    %this.event = Canvas.schedule( 4200 , setContent ,MainMenuGui );
}

function SplashFadeinBitmap2::onSleep( %this ) 
{
   cancel(%this.event);
   alxStop($lastSound);
}


function SplashFadeinBitmap2::click( %this ) {
   cancel(%this.event);
   alxStop($lastSound);
   Canvas.setContent( MainMenuGui );
}



//--------------------------------------------------------------------------
// Load Interface Definition
//--------------------------------------------------------------------------
exec("./Splash.gui");



