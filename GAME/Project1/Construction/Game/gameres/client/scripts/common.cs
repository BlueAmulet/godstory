//-----------------------------------------------------------------------------
// PowerEngine 
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------
// initializeCommon
// Initializes common game functionality.
//---------------------------------------------------------------------------------------------
function initializeCommon()
{
   // Not Reentrant
   if( $commonInitialized == true )
      return;
      
   // Common keybindings.
   GlobalActionMap.bind(keyboard, tilde, toggleConsole);
   GlobalActionMap.bind(keyboard, "PrintScreen", doScreenShot);
   GlobalActionMap.bindcmd(keyboard, "alt enter", "Canvas.toggleFullScreen();","");
   GlobalActionMap.bindcmd(keyboard, "alt k", "cls();",  "");
   GlobalActionMap.bindCmd(keyboard, "escape", "", "handleEscape();");
   
   // Very basic functions used by everyone.
   exec("./audio.cs");
   exec("./canvas.cs");
   exec("./cursor.cs");

   // Content.
   exec("~/gui/profiles.cs");
   exec("~/gui/cursors.cs");

   // Seed the random number generator.
   setRandomSeed();
   // Set up networking.
   setNetPort(0);
   // Initialize the canvas.
   initializeCanvas("NewGame");
   
   // Common Guis.
   exec("~/gui/console.gui");
   
   // Screenshot
   exec("~/client/Scripts/Screenshot.cs");
  
   // Client scripts
   exec("~/client/metrics.cs");
   
   exec("~/client/Scripts/shaders.cs");
   exec("~/client/Scripts/materials.cs");

   // Set a default cursor.
   Canvas.setCursor(DefaultCursor);
   
   $commonInitialized = true;

}

//---------------------------------------------------------------------------------------------
// shutdownCommon
// Shuts down common game functionality.
//---------------------------------------------------------------------------------------------
function shutdownCommon()
{      
   sfxShutdown();
}

//---------------------------------------------------------------------------------------------
// dumpKeybindings
// Saves of all keybindings.
//---------------------------------------------------------------------------------------------
function dumpKeybindings()
{
   // Loop through all the binds.
   for (%i = 0; %i < $keybindCount; %i++)
   {
      // If we haven't dealt with this map yet...
      if (isObject($keybindMap[%i]))
      {
         // Save and delete.
         $keybindMap[%i].save(getPrefsPath("bind.cs"), %i == 0 ? false : true);
         $keybindMap[%i].delete();
      }
   }
}

function handleEscape()
{
  // if (PlayGui.isAwake())	
  //    escapeFromGame();	
  toggleSystemSetting();
}
