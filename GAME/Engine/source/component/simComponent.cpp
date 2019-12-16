//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "component/simComponent.h"
#include "core/stream.h"

SimComponent::SimComponent() : mOwner( NULL )
{
   mComponentList.clear();
   
   mEnabled = true;

}

SimComponent::~SimComponent()
{
}

void SimComponent::_registerInterfaces( SimComponent *owner )
{
   // First call this to expose the interfaces that this component will cache
   // before examining the list of subcomponents
   registerInterfaces( owner );

   // Early out to avoid mutex lock and such
   if( !hasComponents() )
      return;

   VectorPtr<SimComponent *> & components = getComponentList();
   for( SimComponentIterator i = components.begin(); i != components.end(); i++ )
   {
      (*i)->mOwner = owner;

      // Recurse
      (*i)->_registerInterfaces( owner );
   }
}

bool SimComponent::_registerComponents( SimComponent *owner )
{
   // This method will return true if the object contains no components. See the
   // documentation for SimComponent::onComponentRegister for more information
   // on this behavior.

   // If this doesn't contain components, don't even lock the list.
   if( hasComponents() )
   {
      VectorPtr<SimComponent *> & components = getComponentList();
      for( SimComponentIterator i = components.begin(); i != components.end(); i++ )
      {
         // Register child then recurse it's children
         if( !(*i)->onComponentRegister(owner) || !(*i)->_registerComponents( owner ) )
            return false;

         AssertFatal( (*i)->_getOwner() == owner, "Component failed to call parent onComponentRegister!" );
         AssertFatal( (*i)->_getOwner() != (*i), "Component cannot be it's own owner!");
      }
   }

   return true;
}

bool SimComponent::_postRegisterComponents()
{
   if( !onComponentPostRegister() )
      return false;

   // If we have components, recursively call onComponentPostRegister on them all
   if( hasComponents() )
   {
      VectorPtr<SimComponent *> & components = getComponentList();
      for( SimComponentIterator i = components.begin(); i != components.end(); i++ )
      {
         // Recurse
         if( !(*i)->_postRegisterComponents() )
            return false;
      }
   }

   return true;
}

void SimComponent::_unregisterComponents()
{
   if( !hasComponents() )
      return;

   VectorPtr<SimComponent *> & components = getComponentList();
   for( SimComponentIterator i = components.begin(); i != components.end(); i++ )
   {
      (*i)->onComponentUnRegister();

      AssertFatal( (*i)->mOwner == NULL, "Component failed to call parent onUnRegister" );

      // Recurse
      (*i)->_unregisterComponents();
   }
}

//------------------------------------------------------------------------------

bool SimComponent::getInterfaces( ComponentInterfaceList *list, const char *type /* = NULL */, const char *name /* = NULL  */,
                                 const SimComponent *owner /* = NULL */, bool notOwner /* = false */ )
{
   AssertFatal( list != NULL, "Passing NULL for a list is not supported functionality for SimComponents." );
   return ( mInterfaceCache.enumerate( list, type, name, owner, notOwner ) > 0 );
}

bool SimComponent::registerCachedInterface( const char *type, const char *name, SimComponent *interfaceOwner, ComponentInterface *cinterface )
{
   // Assign the interface, and let it's overrides return false to stop further
   // stuff from happening.
   if( !cinterface->assign( interfaceOwner ) )
      return false;

   if( mInterfaceCache.add( type, name, interfaceOwner, cinterface ) )
   {
      // Recurse
      if( mOwner != NULL )
         return mOwner->registerCachedInterface( type, name, interfaceOwner, cinterface );

      return true;
   }

   // So this is not a good assert, because it will get triggered due to the recursive
   // nature of interface caching. I want to keep it here, though, just so nobody
   // else thinks, "Oh I'll add an assert here."
   //
   //AssertFatal( false, avar( "registerCachedInterface failed, probably because interface with type '%s', name '%s' and owner with SimObjectId '%d' already exists",
   //   type, name, interfaceOwner->getId() ) );

   return false;
}

void SimComponent::registerUnCachedInterface( const char *type, const char *name, SimComponent *interfaceOwner, ComponentInterface *cinterface )
{
   cinterface->unassign();

   if( mOwner != NULL )
      mOwner->registerUnCachedInterface( type, name, interfaceOwner, cinterface );
}

//-----------------------------------------------------------------------------
// Component Management
//-----------------------------------------------------------------------------

// Add Component to this one
bool SimComponent::addComponent( SimComponent *component )
{
   AssertFatal( dynamic_cast<SimComponent*>(component), "SimComponent - Cannot add non SimComponent derived components!" );

   for( SimComponentIterator nItr = mComponentList.begin(); nItr != mComponentList.end(); nItr++ )
   {
      SimComponent *pComponent = dynamic_cast<SimComponent*>(*nItr);
      AssertFatal( pComponent, "SimComponent::addComponent - NULL component in list!" );
      if( pComponent == component )
         return true;
   }

   if(component->onComponentAdd(this))
   {
      component->mOwner = this;
      mComponentList.push_back( component );
      return true;
   }

   return false;
}

// Remove Component from this one
bool SimComponent::removeComponent( SimComponent *component )
{
   for( SimComponentIterator nItr = mComponentList.begin(); nItr != mComponentList.end(); nItr++ )
   {
      SimComponent *pComponent = dynamic_cast<SimComponent*>(*nItr);
      AssertFatal( pComponent, "SimComponent::removeComponent - NULL component in list!" );
      if( pComponent == component )
      {
         AssertFatal( component->mOwner == this, "Somehow we contain a component who doesn't think we are it's owner." );
         (*nItr)->onComponentRemove(this);
         component->mOwner = NULL;
         mComponentList.erase( nItr );
         return true;
      }
   }
   return false;
}

//-----------------------------------------------------------------------------

ComponentInterface *SimComponent::getInterface(const char *type /* = NULL */, const char *name /* = NULL */, 
                                               const SimComponent *owner /* = NULL */, bool notOwner /* = false  */)
{
   ComponentInterfaceList iLst;

   if( getInterfaces( &iLst, type, name, owner, notOwner ) )
      return iLst[0];

   return NULL;
}
