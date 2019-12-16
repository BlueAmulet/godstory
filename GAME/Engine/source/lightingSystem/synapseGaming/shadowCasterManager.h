//-----------------------------------------------
// Synapse Gaming - Lighting System
// Copyright ?Synapse Gaming 2003
// Written by John Kabus
//-----------------------------------------------

#ifndef _LIGHTINGSYSTEM_SHADOWCASTERMANAGER_H_
#define _LIGHTINGSYSTEM_SHADOWCASTERMANAGER_H_

class ShadowCaster;

// CodeReview - This may be better implemented as a singleton.  Something to consider
// for a refactor. -AlexS 5/02/07
class ShadowCasterManager
{
private:
   /// Array of registered ShadowCasters
   static Vector<ShadowCaster *> smShadowCasters;

   /// When we last cleaned up unused shadows
   static U32 smLastCleanup;

public:
   /// Register an existing ShadowCaster with us
   static void registerShadowCaster(ShadowCaster* shadowCaster);

   /// Unregister an existing ShadowCaster with us (no longer notified of cleanup)
   static void unregisterShadowCaster(ShadowCaster* shadowCaster);

   /// Loop through all registered ShadowCasters and have them cleanup unused shadows
   static void cleanupUnusedShadows();

   /// Loop through all registered ShadowCasters and have them delete all shadows
   static void clearAllShadows();
};

#endif