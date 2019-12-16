//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

// List of master servers to query, each one is tried in order
// until one responds
$Pref::Server::RegionMask = 2;
//$pref::Master[0] = "2:www.firerain.com:10000";

// Information about the server
$Pref::Server::Name = "ZoneServer";
$Pref::Server::Info = "Power Engine Server.";

// The connection error message is transmitted to the client immediatly
// on connection, if any further error occures during the connection
// process, such as network traffic mismatch, or missing files, this error
// message is display. This message should be replaced with information
// usefull to the client, such as the url or ftp address of where the
// latest version of the game can be obtained.
$Pref::Server::ConnectionError = "version error";

// The network port is also defined by the client, this value 
// overrides pref::net::port for dedicated servers
$Pref::Server::Port = 0;

// If the password is set, clients must provide it in order
// to connect to the server
$Pref::Server::Password = "";

// Misc server settings.
$Pref::Server::MaxPlayers = 64;

$Pref::Net::BindAddress = "192.168.1.119";
$Pref::Net::Gate = "192.168.1.119:41001";

$Pref::Net::DataAgentIp = "192.168.1.119";
$Pref::Net::DataAgentPort = "60000";

$Pref::Net::LogIp = "192.168.1.119";
$Pref::Net::LogPort = "60008";