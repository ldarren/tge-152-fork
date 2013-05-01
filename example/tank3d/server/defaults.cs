//-----------------------------------------------------------------------------
// Torque Game Engine 
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

// List of master servers to query, each one is tried in order
// until one responds
$Pref::Server::RegionMask = 2;
$pref::Master[0] = "";//"2:master.garagegames.com:28002";

// Information about the server
$Pref::Server::Name = "Tank Attack 3D TTB Server";
$Pref::Server::Info = "This is a Tank Attack 3D Game Engine Test Server.";

// The connection error message is transmitted to the client immediatly
// on connection, if any further error occures during the connection
// process, such as network traffic mismatch, or missing files, this error
// message is display. This message should be replaced with information
// usefull to the client, such as the url or ftp address of where the
// latest version of the game can be obtained.
$Pref::Server::ConnectionError =
   "You do not have the correct version of the Tank Attack 3D Game Engine or "@
   "the related art needed to connect to this server, please contact "@
   "the server operator to obtain the latest version of this game.";

// The network port is also defined by the client, this value 
// overrides pref::net::port for dedicated servers
$Pref::Server::Port = 28000;

// If the password is set, clients must provide it in order
// to connect to the server
$Pref::Server::Password = "";

// Password for admin clients
$Pref::Server::AdminPassword = "";

// Misc server settings.
$Pref::Server::MaxPlayers = 64;
$Pref::Server::FloodProtectionEnabled = 1;
$Pref::Server::MaxChatLen = 120;

