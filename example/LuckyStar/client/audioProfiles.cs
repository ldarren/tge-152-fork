//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// Channel assignments (channel 0 is unused in-game).

$GuiAudioType     = 1; // gui
$SimAudioType     = 2; // background music
$MessageAudioType = 3; // effects

new AudioDescription(AudioGui)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = $GuiAudioType;
};

new AudioDescription(MusicGui)
{
   volume   = 1.0;
   isLooping= true;
   is3D     = false;
   type     = $SimAudioType;
};

new AudioDescription(AudioMessage)
{
   volume   = 1.0;
   isLooping= false;
   is3D     = false;
   type     = $MessageAudioType;
};

new AudioProfile(AudioResultWonderful)
{
	filename = "~/data/sound/result_wonderful.ogg";
	description = AudioMessage;
	preload = true;
};

new AudioProfile(AudioResultExcellent)
{
	filename = "~/data/sound/result_excellent.ogg";
	description = AudioMessage;
	preload = true;
};

new AudioProfile(AudioResultVeryGood)
{
	filename = "~/data/sound/result_vgood.ogg";
	description = AudioMessage;
	preload = true;
};

new AudioProfile(AudioResultGood)
{
	filename = "~/data/sound/result_good.ogg";
	description = AudioMessage;
	preload = true;
};

new AudioProfile(AudioTypeWrong)
{
	filename = "~/data/sound/type_wrong.ogg";
	description = AudioMessage;
	preload = true;
};

new AudioProfile(AudioTypeInvalid)
{
	filename = "~/data/sound/type_invalid.ogg";
	description = AudioMessage;
	preload = true;
};

new AudioProfile(AudioButtonOver)
{
	fileName = "~/data/sound/buttonOver.wav";
	description = AudioGui;
	preload = true;
};

new AudioProfile(AudioButtonDown)
{
	fileName = "~/data/sound/volumeTest.wav";
	description = AudioGui;
	preload = true;
};

new AudioProfile(AudioMessageBox)
{
	fileName = "~/data/sound/messageBoxSound.wav";
	description = AudioGui;
	preload = true;
};

new AudioProfile(AudioIntro)
{
	filename = "~/data/sound/Azuki.ogg";
	description = MusicGui;
	preload = false;
	//environment = 0; // EAX extension through AudioSampleEnvironment datablock
};

new AudioProfile(AudioLobby)
{
	filename = "~/data/sound/Haruhi.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioDebug)
{
	filename = "~/data/sound/LuckyStar.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioDance01)
{
	filename = "~/data/sound/dance01.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioDance02)
{
	filename = "~/data/sound/dance02.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioDance03)
{
	filename = "~/data/sound/dance03.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioDance04)
{
	filename = "~/data/sound/dance04.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioDance05)
{
	filename = "~/data/sound/SingNaNaNa.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioWin)
{
	filename = "~/data/sound/win.ogg";
	description = MusicGui;
	preload = false;
};

new AudioProfile(AudioLose)
{
	filename = "~/data/sound/lose.ogg";
	description = MusicGui;
	preload = false;
};

$musicHandle = 0;