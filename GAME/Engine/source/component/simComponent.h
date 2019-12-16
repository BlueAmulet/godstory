//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _SIMCOMPONENT_H_
#define _SIMCOMPONENT_H_

#include "core/tVector.h"
#include "core/stringTable.h"
#include "component/componentInterface.h"
#include "Util/TinySerializer/TinySerializer.h"

class SimComponent : public ITinySerialable
{
private:
   VectorPtr<SimComponent *> mComponentList; ///< The Component List

   SimComponent *mOwner;        ///< The component which owns this one.

protected:

   typedef VectorPtr<SimComponent *>::iterator SimComponentIterator;

   /// This is called internally to instruct the component to iterate over it's
   /// list of components and recursively call _registerInterfaces on their lists
   /// of components.
   void _registerInterfaces( SimComponent *owner );

   bool _registerComponents( SimComponent *owner );
   bool _postRegisterComponents();
   void _unregisterComponents();

   ComponentInterfaceCache mInterfaceCache;  ///< Stores the interfaces exposed by this component. 
   
   // Non-const getOwner for derived classes
   SimComponent *_getOwner() { return mOwner; }

   /// Returns a reference to private mComponentList
   VectorPtr<SimComponent *> & getComponentList() { return mComponentList; }

   /// onComponentRegister is called on each component by it's owner. If a component
   /// has no owner, onComponentRegister will not be called on it. The purpose
   /// of onComponentRegister is to allow a component to check for any external
   /// interfaces, or other dependencies which it needs to function. If any component
   /// in a component hierarchy returns false from it's onComponentRegister call
   /// the entire hierarchy is invalid, and SimObject::onAdd will fail on the
   /// top-level component. To put it another way, if a component contains other
   /// components, it will be registered successfully with Sim iff each subcomponent
   /// returns true from onComponentRegister. If a component does not contain
   /// other components, it will not receive an onComponentRegister call.
   ///
   /// When onComponentRegister is called on a component, it can be assumed that
   /// all *interfaces* are registered, but not that all components are registered.
   /// To perform initialization operations, please use onComponentPostRegister.
   /// @see onComponentPostRegister
   ///
   /// Overloads of this method must pass the call along to their parent, as is
   /// shown in the example below. 
   ///
   /// @code
   /// bool FooComponent::onComponentRegister( SimComponent *owner )
   /// {
   ///    if( !Parent::onComponentRegister( owner ) )
   ///       return false;
   ///    ...
   /// }
   /// @endcode
   virtual bool onComponentRegister( SimComponent *owner )
   {
      mOwner = owner;
      return true;
   }

   /// onComponentPostRegister occurs after onComponentRegister, and the purpose
   /// of the method is to allow for initialization which occurs after components
   /// are registered. This is different than onComponentRegister, which occurs
   /// after all interfaces are registered. 
   ///
   /// Overloads of this method must pass the call along to their parent in the
   /// same manner as onComponentRegister.
   ///
   /// @see onComponentRegister
   virtual bool onComponentPostRegister()
   {
      return true;
   }

   /// onUnregister is called when the owner is unregistering. Your object should
   /// do cleanup here, as well as pass a call up the chain to the parent.
   virtual void onComponentUnRegister()
   {
      mOwner = NULL;
   }

   /// registerInterfaces is called on each component as it's owner is registering
   /// it's interfaces. This is called before onComponentRegister, and should be used to
   /// register all interfaces exposed by your component, as well as all callbacks
   /// needed by your component.
   virtual void registerInterfaces( SimComponent *owner )
   {

   }

public:
   bool mEnabled;

   /// Constructor
   /// Add this component
   SimComponent();

   /// Destructor
   /// Remove this component and destroy child references
   virtual ~SimComponent();

public:

   /// Will return true if this object contains components.
   bool hasComponents() const { return ( mComponentList.size() > 0 ); };

   /// The component which owns this object
   const SimComponent *getOwner() const { return mOwner; };

   // Component Information
   inline virtual StringTableEntry  getComponentName() { return StringTable->insert(""); };

   /// Add Component to this one
   virtual bool addComponent( SimComponent *component );

   /// Remove Component from this one
   virtual bool removeComponent( SimComponent *component );

   /// Clear Child components of this one
   virtual bool clearComponents() { mComponentList.clear(); return true; };

   virtual bool onComponentAdd(SimComponent *target) { return true; };
   virtual void onComponentRemove(SimComponent *target) { };

   S32 getComponentCount()                   { return mComponentList.size(); }
   SimComponent *getComponent(S32 idx)       { return mComponentList[idx]; }

   /// getComponent by type T
   template <class T>
   T *getComponent();


   static bool setEnabled( void* obj, const char* data ) { static_cast<SimComponent*>(obj)->setEnabled( dAtob( data ) ); return false; };
   virtual void setEnabled( const bool enabled ) { mEnabled = enabled; }
   bool isEnabled() const { return mEnabled; }

   /// getInterfaces allows the caller to enumerate the interfaces exposed by
   /// this component. This method can be overwritten to expose interfaces
   /// which are not cached on the object, before passing the call to the Parent.
   /// This can be used delay interface creation until it is queried for, instead
   /// of creating it on initialization, and caching it. Returns false if no results
   /// were found
   ///
   /// @param list The list that this method will append search results on to.
   /// @param type An expression which the 'type' field on an added object must match to be included in results
   /// @param name An expression which the 'name' field on an added object must match to be included in results
   /// @param owner Limit results to components owned/not-owned by this SimComponent (see next param)
   /// @param notOwner If set to true, this will enumerate only interfaces NOT owned by 'owner'
   virtual bool getInterfaces( ComponentInterfaceList *list, const char *type = NULL, const char *name = NULL, const SimComponent *owner = NULL, bool notOwner = false ); // const omission intentional

   template <class T>
   bool getInterfaces( ComponentInterfaceList *list, const char *type = NULL, const char *name = NULL, const SimComponent *owner = NULL, bool notOwner = false );
   
   /// These two methods allow for easy query of component interfaces if you know
   /// exactly what you are looking for, and don't mind being passed back the first
   /// matching result.
   ComponentInterface *getInterface( const char *type = NULL, const char *name = NULL, const SimComponent *owner = NULL, bool notOwner = false );

   template <class T>
   T *getInterface( const char *type = NULL, const char *name = NULL, const SimComponent *owner = NULL, bool notOwner = false );

   /// Add an interface to the cache. This function will return true if the interface
   /// is added successfully. An interface will not be added successfully if an entry
   /// in this components cache with the same values for 'type' and 'name' is present.
   /// 
   /// @param type Type of the interface being added. If NULL is passed, it will match any type string queried.
   /// @param name Name of interface being added. If NULL is passed, it will match any name string queried.
   /// @param interfaceOwner The component which owns the interface being cached
   /// @param cinterface The ComponentInterface being cached
   bool registerCachedInterface( const char *type, const char *name, SimComponent *interfaceOwner, ComponentInterface *cinterface );

   /// Inform component hierarchy of the existence of a new, uncached interface
   void registerUnCachedInterface( const char *type, const char *name, SimComponent *interfaceOwner, ComponentInterface *cinterface );
};

//-----------------------------------------------------------------------------

template <class T>
bool SimComponent::getInterfaces(ComponentInterfaceList *list, const char *type /* = NULL */, 
                                 const char *name /* = NULL */, const SimComponent *owner /* = NULL */, 
                                 bool notOwner /* = false  */)
{
   bool ret = false;

   AssertFatal( list != NULL, "Passing NULL for a list is not supported functionality for SimComponents." );
   list->clear();

   ComponentInterfaceList iLst;

   if( getInterfaces( &iLst, type, name, owner, notOwner ) )
   {
      ComponentInterfaceListIterator itr = iLst.begin();

      for( itr; itr != iLst.end(); itr++ )
      {
         T *iface = dynamic_cast<T *>( *itr );
         if( iface != NULL )
         {
            ret |= true;
            list->push_back( iface );
         }
      }
   }

   return ret;
}

template <class T>
T *SimComponent::getInterface( const char *type /* = NULL */, const char *name /* = NULL */, 
                              const SimComponent *owner /* = NULL */, bool notOwner /* = false  */ )
{
   ComponentInterfaceList iLst;

   if( getInterfaces( &iLst, type, name, owner, notOwner ) )
   {
      ComponentInterfaceListIterator itr = iLst.begin();

      for( itr; itr != iLst.end(); itr++ )
      {
         T *ret = dynamic_cast<T *>( *itr );
         if( ret != NULL )
            return ret;
      }
   }

   return NULL;
}

template <class T>
T * SimComponent::getComponent()
{
   if( !hasComponents() )
      return NULL;

   T *retComponent = NULL;
   VectorPtr<SimComponent *> & components = getComponentList();
   for( S32 i = 0; i < components.size(); i++ )
   {
      T *matchComponent = dynamic_cast<T*>(components[i]);
      if( matchComponent )
      {
         retComponent = matchComponent;
         break;
      }
   }

   return retComponent;
}


#endif // _SIMCOMPONENT_H_
