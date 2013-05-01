echo("\c2--------- Loading Maze Runner Profiles  ---------");
//--------------------------------------------------------------------------
// MazeRunnerProfiles.cs
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

new AudioDescription(AudioSim2DLooping)
{
    volume      = 1.0;
    isLooping   = true;
    is3D        = false;
    type        = $SimAudioType;
};

new AudioProfile(AudioStartup)
{
   filename = "./Sound/ggSplash.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(AudioStartup1)
{
   filename = "./Sound/GPGTSplash.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(AudioStartup2)
{
   filename = "./Sound/HOWSplash.ogg";
   description = "AudioGui";
   preload = true;
};

new AudioProfile(mainMenuMusic)
{
   filename = "./Sound/MainMenuLoop.ogg";
   description = "AudioSim2DLooping";
   preload = true;
};

new AudioProfile(levelLoop)
{
   filename = "./Sound/levelLoop.ogg";
   description = "AudioSim2DLooping";
   preload = true;
};


new AudioProfile(AudioMenuButtonPress)
{
    filename = "./Sound/buttonPress.ogg";
    description = "AudioGui";
    preload = true;
};

new AudioProfile(AudioMenuButtonOver)
{
    filename = "./Sound/buttonOver.ogg";
    description = "AudioGui";
    preload = true;
};


if(!isObject(MainMenuButtonProfile)) new GuiControlProfile (MainMenuButtonProfile)
{
    opaque = true;
    border = true;
    fontColor = "0 0 0";
    fontColorHL = $fontColorHL;
    fixedExtent = true;
    justify = "center";
    canKeyFocus = false;
    soundButtonOver = "AudioMenuButtonOver";
    soundButtonDown = "AudioMenuButtonPress";
};


if(!isObject(CreditsScrollProfile)) new GuiControlProfile (CreditsScrollProfile : GuiScrollProfile)
{
	border = 0;
	opaque    = false;
};

if(!isObject(CreditsMLTextProfile)) new GuiControlProfile (CreditsMLTextProfile)
{
	opaque    = false;
};

