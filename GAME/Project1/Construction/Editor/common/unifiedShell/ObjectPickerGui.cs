//------------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FireRain.com, Inc.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global vars
//------------------------------------------------------------------------------

$PICKER::ROW_OBJECT  =  0; ///< The row used to pick ojbects
$PICKER::ROW_MOUNT   =  1; ///< The row used to pick the mounted object

//$PICKER::MODEL["Default"]   =  $defaultGame @ "/data/shapes/players/SpaceOrc/SpaceOrc.dts";
//
//$PICKER::WEAPON["Default"]  = $defaultGame @ "/data/shapes/weapons/SwarmGun/swarmgun.dts";
//$PICKER::WEAPON["Rocket Launcher"] = $defaultGame @ "/data/shapes/weapons/SwarmGun/swarmgun.dts";
//$PICKER::WEAPON["Crossbow"] = $defaultGame @ "/data/shapes/weapons/crossbow/weapon.dts";

//------------------------------------------------------------------------------
// PickerMenu methods
//------------------------------------------------------------------------------

/// Callback when this gui is added to the sim.
function PickerMenu::onAdd(%this)
{
   %this.addRow("Model", "Default", true, "onModelChange");
   %this.addRow("Weapon", "Default", true, "onMountChange");
}

/// Callback when the control wakes up.
function PickerMenu::onWake(%this)
{
   // For now PlayerDatasGroup is currently being populated
   // by loadPlayerPickerData() which is implemented in
   // <$defaultGame>/client/init.cs
      
   %this.modelList = "";
   
   if (isObject(PlayerDatasGroup))
   {
      for (%i = 0; %i < PlayerDatasGroup.getCount(); %i++)
      {
         %obj = PlayerDatasGroup.getObject(%i);
         %name = %obj.getName();
         
         %name = strreplace(%name, "Data", "");
         
         $PICKER::MODEL[%name]   =  %obj.shapeFile;
         
         %this.modelList = %this.modelList @ %name @ "\t";
      }
   }
   
   %this.setOptions($PICKER::ROW_OBJECT, %this.modelList);

   %this.setOptions($PICKER::ROW_MOUNT, "Default\tRocket Launcher\tCrossbow");

   // Initialize the currently selected object here
   if ($pref::Player:PlayerDB $= "")
      $pref::Player:PlayerDB = getField(%this.modelList, 0) @ "Data";
      
   %db = strreplace($pref::Player:PlayerDB, "Data", "");

   PickerObjectView.setModel($PICKER::MODEL[%db]);
   
   if ($pref::Player:Weapon $= "")
      $pref::Player:Weapon = "Default";

   
   PickerObjectView.setMount($PICKER::WEAPON[$pref::Player:Weapon],0);
   PickerObjectView.setOrbitDistance(3);
   
   %this.selectOption($PICKER::ROW_OBJECT, %db);
   %this.selectOption($PICKER::ROW_MOUNT, $pref::Player:Weapon);
}

function PickerMenu::setPlayer(%this)
{
   %selected = PickerMenu.getCurrentOption($PICKER::ROW_OBJECT);
   
   if (%selected $= "Default")
      %selected = "DefaultPlayer";
   
   $pref::Player:PlayerDB = %selected @ "Data";

   %selected = PickerMenu.getCurrentOption($PICKER::ROW_MOUNT);
   
   $pref::Player:Weapon = %selected;
   
   Canvas.setContent(UnifiedMainMenuGui);
}

//------------------------------------------------------------------------------
// callbacks from PickerMenu
//------------------------------------------------------------------------------

/// Callback when the primary object model is changed.
///
/// \param %direction (string) "LEFT" or "RIGHT" indicating the direction the
/// option changed.
function onModelChange(%direction)
{
   %selected = PickerMenu.getCurrentOption($PICKER::ROW_OBJECT);
   PickerObjectView.setModel($PICKER::MODEL[%selected]);
   PickerObjectView.setOrbitDistance(3);
   PickerInfoDisplay.update();
}

/// Callback when the mounted object model is changed.
///
/// \param %direction (string) "LEFT" or "RIGHT" indicating the direction the
/// option changed.
function onMountChange(%direction)
{
   %selected = PickerMenu.getCurrentOption($PICKER::ROW_MOUNT);
   PickerObjectView.setMount($PICKER::WEAPON[%selected],0);
   $pref::Player:Weapon = %selected;
   
   PickerInfoDisplay.update();
}

//------------------------------------------------------------------------------
// PickerInfoDisplay methods
//------------------------------------------------------------------------------

/// Updates the information display to show information about the currently
/// selected objects.
function PickerInfoDisplay::update(%this)
{
   %objectName = PickerMenu.getCurrentOption($PICKER::ROW_OBJECT);
   %mountName = PickerMenu.getCurrentOption($PICKER::ROW_MOUNT);
   %this.setText("");
   %this.addText("<color:666666><tab:80>", false);
   %this.addText("Selected Objects<br>", false);
   %this.addText("Object:" TAB %objectName @ "<br>", false);
   %this.addText("Mounted Object:" TAB %mountName @ "<br>", false);
   %this.forceReflow();
}

/// Callback when this control wakes up
function PickerInfoDisplay::onWake(%this)
{
   %this.update();
}

//------------------------------------------------------------------------------
// PickerButtonHolder methods
//------------------------------------------------------------------------------

function PickerButtonHolder::onWake(%this)
{
   %this.add(GamepadButtonsGui);

   GamepadButtonsGui.setButton($BUTTON_A, "Accept", PickerMenu.CallbackOnA);
   GamepadButtonsGui.setButton($BUTTON_B, "Go Back", PickerMenu.CallbackOnB);
}
