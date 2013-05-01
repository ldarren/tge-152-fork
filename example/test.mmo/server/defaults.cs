// List of master servers to query, each one is tried in order
// until one responds
$Pref::Server::RegionMask = 2;
$pref::Master[0] = "2:165.21.82.242:47002";

// Information about the server
$Pref::Server::Name = $modDesc@" Game Server";
$Pref::Server::Info = "This is a "@$modDesc@" Game Server.";

// The connection error message is transmitted to the client immediatly
// on connection, if any further error occures during the connection
// process, such as network traffic mismatch, or missing files, this error
// message is display. This message should be replaced with information
// usefull to the client, such as the url or ftp address of where the
// latest version of the game can be obtained.
$Pref::Server::ConnectionError =
   "You do not have the correct version of "@$modDesc@
   " or the resources needed to play on this server ";

// The network port is also defined by the client, this value 
// overrides pref::net::port for dedicated servers
$Pref::Server::Port = 47000;

// If the password is set, clients must provide it in order
// to connect to the server
$Pref::Server::Password = "";

// Password for admin clients
$Pref::Server::AdminPassword = "";

// Misc server settings.
$Pref::Server::MaxPlayers = 64;
$Pref::Server::TimeLimit = 20;               // In minutes
$Pref::Server::KickBanTime = 300;            // specified in seconds
$Pref::Server::BanTime = 1800;               // specified in seconds
$Pref::Server::FloodProtectionEnabled = 1;
$Pref::Server::MaxChatLen = 120;

//----------------------------------------------------------------
//RPGDialog Code Begin
//----------------------------------------------------------------   
$Pref::Server::RPGDialog::ActionPath="~/data/dialogs/dla/";//RPGDialog Action Path.
//----------------------------------------------------------------
//RPGDialog Code End
//----------------------------------------------------------------   
