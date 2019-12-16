//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

// Set the name of our application
$appName = "µØÍ¼·þÎñÆ÷";

// The directory it is run from
$defaultGame = "gameres";

// Display the optional commandline arguements
$displayHelp = false;
enableWinConsole(true);
//-----------------------------------------------------------------------------
// Support functions used to manage the mod string

function pushFront(%list, %token, %delim)
{
   if (%list !$= "")
      return %token @ %delim @ %list;
   return %token;
}

function pushBack(%list, %token, %delim)
{
   if (%list !$= "")
      return %list @ %delim @ %token;
   return %token;
}

function popFront(%list, %delim)
{
   return nextToken(%list, unused, %delim);
}


//------------------------------------------------------------------------------
// Process command line arguments
$userDirs = $defaultGame;
$dirCount = 1;
$isDedicated = true;

for ($i = 1; $i < $Game::argc ; $i++)
{
   $arg = $Game::argv[$i];
   $nextArg = $Game::argv[$i+1];
   $hasNextArg = $Game::argc - $i > 1;
   $logModeSpecified = false;

   switch$ ($arg)
   {
      //--------------------
      case "-log":
         $argUsed[$i]++;
         if ($hasNextArg)
         {
            // Turn on console logging
            if ($nextArg != 0)
            {
               // Dump existing console to logfile first.
               $nextArg += 4;
            }
            setLogMode($nextArg);
            $logModeSpecified = true;
            $argUsed[$i+1]++;
            $i++;
         }
         else
            error("Error: Missing Command Line argument. Usage: -log <Mode: 0,1,2>");

     case "-mission":
        $argUsed[$i]++;
        if ($hasNextArg) {
           $missionArg = $nextArg;
           $ZoneID = SptParseZoneId( $missionArg );
           $argUsed[$i+1]++;
           $i++;
        }
        else
           error("Error: Missing Command Line argument. Usage: -mission <filename>");
               
      default:
         $argUsed[$i]++;
         if($userDirs $= "")
            $userDirs = $arg;
   }
}

if($dirCount == 0) {
      $userDirs = $defaultGame;
      $dirCount = 1;
}

//--------------------------------------------------------------------------

// Default to a new logfile each session.
if( !$logModeSpecified )
{
   if( $platform !$= "xbox" && $platform !$= "xenon" )
      setLogMode(6);
}

// Set the mod path which dictates which directories will be visible
// to the scripts and the resource engine.
setModPaths($userDirs);

// Get the first dir on the list, which will be the last to be applied... this
// does not modify the list.
nextToken($userDirs, currentMod, ";");

// Execute startup scripts for each mod, starting at base and working up
function loadDir(%dir)
{
   setModPaths(pushback($userDirs, %dir, ";"));
   exec(%dir @ "/main.cs");
}

echo("--------- Loading DIRS ---------");
function loadDirs(%dirPath)
{
   %dirPath = nextToken(%dirPath, token, ";");
   if (%dirPath !$= "")
      loadDirs(%dirPath);

   if(exec(%token @ "/main.cs") != true){
      error("Error: Unable to find specified directory: " @ %token );
      $dirCount--;
   }
}
loadDirs($userDirs);

if($dirCount == 0) {
   enableWinConsole(true);
   error("Error: Unable to load any specified directories");
   quit();
}

 onStart();
 echo("Engine initialized...");

// Display an error message for unused arguments
for ($i = 1; $i < $Game::argc; $i++)  {
   if (!$argUsed[$i])
      error("Error: Unknown command line argument: " @ $Game::argv[$i]);
}
