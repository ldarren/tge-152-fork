new AudioDescription(DialogSound)
{
   volume   = 1.2;
   isLooping= false;
   is3D     = false;
   type     = $MessageAudioType;
};

new AudioProfile(NPCVoice01)
{
   filename = $Pref::RPGDialog::Client::SoundsPath@"test.wav";
   description = "DialogSound";
   preload = false;
};

new AudioProfile(NPCVoice02)
{
   filename = $Pref::RPGDialog::Client::SoundsPath@"test2.wav";
   description = "DialogSound";
   preload = false;
};
