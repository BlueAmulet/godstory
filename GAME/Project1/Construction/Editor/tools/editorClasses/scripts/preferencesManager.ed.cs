//-----------------------------------------------------------------------------
// PowerEngine2D Editor Classes
// Copyright (C) FireRain.com, Inc.
//-----------------------------------------------------------------------------

//*** Initializes the Preferences Manager
function initPreferencesManager()
{
   // FIXME TGEA doesnt currently have these due to the way it's built
   return;
   
   //*** Create the Preferences Manager singleton
   %pm = new PreferencesManager(pref);
   registerPreferencesManager(%pm.getId());
}
