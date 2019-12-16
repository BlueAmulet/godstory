//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Game start / end events sent from the server
//----------------------------------------------------------------------------

function clientCmdGameStart(%seq)
{
}

function clientCmdGameEnd(%seq)
{
   // Stop local activity... the game will be destroyed on the server
   sfxStopAll();
}
