// The master server is declared with the server defaults, which is
// loaded on both clients & dedicated servers.  If the server mod
// is not loaded on a client, then the master must be defined. 
// $pref::Master[0] = "2:master.garagegames.com:28002";

$pref::Player::Name = "VR-10";
$pref::Player::defaultFov = 90;
$pref::Player::zoomSpeed = 0;

$pref::Net::LagThreshold = 400;
$pref::Net::Port = 47000;

$pref::shadows = "2";
$pref::HudMessageLogSize = 40;
$pref::ChatHudLength = 1;

$pref::timeManagerProcessInterval = 33; // 30fps

$pref::Input::LinkMouseSensitivity = 1;
// DInput keyboard, mouse, and joystick prefs
$pref::Input::KeyboardEnabled = 1;
$pref::Input::MouseEnabled = 1;
$pref::Input::JoystickEnabled = 0;
$pref::Input::KeyboardForwardSpeed = 0.3;
$pref::Input::KeyboardBackwardSpeed = 0.1;
$pref::Input::KeyboardTurnSpeed = 0.03;

$pref::sceneLighting::cacheSize = 20000;
$pref::sceneLighting::purgeMethod = "lastCreated";
$pref::sceneLighting::cacheLighting = 1;
$pref::sceneLighting::terrainGenerateLevel = 1;

$pref::ts::detailAdjust = 0.45;

$pref::Terrain::DynamicLights = 1;
$pref::Interior::TexturedFog = 0;

$pref::Video::displayDevice = "OpenGL";
$pref::Video::allowOpenGL = 1;
$pref::Video::allowD3D = 1;
$pref::Video::preferOpenGL = 1;
$pref::Video::appliedPref = 0;
$pref::Video::disableVerticalSync = 1;
$pref::Video::monitorNum = 0;
$pref::Video::windowedRes = "1024 768";
$pref::Video::fullScreen = "0";
$Pref::Video::Resolution = "1024 768 32";

$pref::Video::screenShotSession = 0;
$pref::Video::screenShotFormat = "PNG";

$pref::OpenGL::force16BitTexture = "0";
$pref::OpenGL::forcePalettedTexture = "0";
$pref::OpenGL::maxHardwareLights = 3;
$pref::VisibleDistanceMod = 1.0;

$pref::Audio::driver = "OpenAL";
$pref::Audio::forceMaxDistanceUpdate = 0;
$pref::Audio::environmentEnabled = 0;
$pref::Audio::masterVolume   = 0.8;
$pref::Audio::channelVolume1 = 0.8;
$pref::Audio::channelVolume2 = 0.8;
$pref::Audio::channelVolume3 = 0.8;
$pref::Audio::channelVolume4 = 0.8;
$pref::Audio::channelVolume5 = 0.8;
$pref::Audio::channelVolume6 = 0.8;
$pref::Audio::channelVolume7 = 0.8;
$pref::Audio::channelVolume8 = 0.8;

$pref::Game::effectOn = 1;
$pref::Game::modeEasy = 1;
$pref::Game::saveOn = 1;
$pref::Game::soundOn = 1;
 
//----------------------------------------------------------------
//RPGDialog Code Begin
//----------------------------------------------------------------   
$Pref::RPGDialog::Client::QuestionPath="~/data/dialogs/dlq/";//RPGDialog Question Path.
$Pref::RPGDialog::Client::PortraitsPath="~/data/dialogs/portraits/";//RPGDialog Images Path.
$Pref::RPGDialog::Client::SoundsPath="~/data/dialogs/sounds/";//RPGDialog Sounds Path.
$Pref::RPGDialog::MaxOptions = 10;//RPGDialog Max Options.
$Pref::RPGDialog::ChatHudQuestionColor="\c1";//Question text Color on the chat hud.
$Pref::RPGDialog::ChatHudAnswerColor="\c5";//Answer text Color on the chat hud.
//----------------------------------------------------------------
//RPGDialog Code End
//----------------------------------------------------------------   

// AFX
$pref::Player::corpsesHiddenFromRayCast = false;
$pref::afxSpellButton::unknownSpellBitmap = "~/client/ui/icons/unknown";
$pref::afxSpellButton::spellCooldownBitmaps = "~/client/ui/icons/cooldown/cooldown";
$pref::AFX::advancedFXLighting = !afxLegacyLighting();
$pref::AFX::fxLevelOfDetail = "2";
$pref::AFX::clickToTargetSelf = false;
$pref::AFX::destinationMask = $TypeMasks::TerrainObjectType | $TypeMasks::InteriorObjectType;
$pref::AFX::targetSelectionMask =   //$TypeMasks::GameBaseObjectType |
									//$TypeMasks::ShapeBaseObjectType |
									//$TypeMasks::StaticShapeObjectType |
									$TypeMasks::PlayerObjectType |
									$TypeMasks::CorpseObjectType |
									$TypeMasks::ItemObjectType |
									$TypeMasks::VehicleObjectType |
									//$TypeMasks::VehicleBlockerObjectType |
									//$TypeMasks::StaticTSObjectType |
									$TypeMasks::AIObjectType; //|
									//$TypeMasks::StaticRenderedObjectType; // ground and mall
$pref::AFX::missileCollisionMask = $TypeMasks::PlayerObjectType | 
                                   $TypeMasks::CorpseObjectType | 
                                   $TypeMasks::InteriorObjectType |
                                   $TypeMasks::StaticObjectType |
                                   $TypeMasks::DamagableItemObjectType |
                                   $TypeMasks::VehicleObjectType;
// DARREN only if absolutely necessary
//$pref::Net::PacketRateToClient = "32";
//$pref::Net::PacketRateToServer = "32";
//$pref::Net::PacketSize = "450";
// AFX


//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------

$pref::DRL::enable = "1";
$pref::DRL::bloomQuality = "3";
$pref::Material::qualityLevel = "500";

//------------------------------------------------------------------
// TGE Modernization Kit
//------------------------------------------------------------------


