//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "component/dynamicConsoleMethodComponent.h"

IMPLEMENT_CONOBJECT(DynamicConsoleMethodComponent);

//-----------------------------------------------------------
// Function name:  SimComponent::handlesConsoleMethod
// Summary:
//-----------------------------------------------------------
bool DynamicConsoleMethodComponent::handlesConsoleMethod( const char *fname, S32 *routingId )
{
   // CodeReview: Host object is now given priority over components for method
   // redirection. [6/23/2007 Pat]

   // On this object?
   if( isMethod( fname ) )
   {
      *routingId = -1; // -1 denotes method on object
      return true;
   }

   // on this objects components?
   VectorPtr<SimComponent*> & componentList = getComponentList();
   for( SimComponentIterator nItr = componentList.begin(); nItr != componentList.end(); nItr++ )
   {
      SimObject *pComponent = dynamic_cast<SimObject*>(*nItr);
      if( pComponent != NULL && pComponent->isMethod( fname ) )
      {
         *routingId = -2; // -2 denotes method on component
         return true;
      }
   }

   return false;
}

const char *DynamicConsoleMethodComponent::callMethod( S32 argc, const char* methodName, ... )
{
   const char *argv[128];
   methodName = StringTable->insert( methodName );

   argc++;

   va_list args;
   va_start(args, methodName);
   for(S32 i = 0; i < argc; i++)
      argv[i+2] = va_arg(args, const char *);
   va_end(args);

   // FIXME: the following seems a little excessive. I wonder why it's needed?
   argv[0] = methodName;
   argv[1] = methodName;
   argv[2] = methodName;

   return callMethodArgList( argc , argv );
}

const char* DynamicConsoleMethodComponent::callMethodArgList( U32 argc, const char *argv[], bool callThis /* = true  */ )
{
   return _callMethod( argc, argv, callThis );
}

// Call all components that implement methodName giving them a chance to operate
// Components are called in reverse order of addition

#ifdef POWER_DEBUG
struct MethodReturnDbg
{
   SimObject *simObj;
   StringTableEntry retVal;
};
#endif

const char *DynamicConsoleMethodComponent::_callMethod( U32 argc, const char *argv[], bool callThis /* = true  */ )
{
   // Set Owner
   SimObject *pThis = dynamic_cast<SimObject *>( this );
   AssertFatal( pThis, "DynamicConsoleMethodComponent::callMethod : this should always exist!" );

   const char *cbName = StringTable->insert(argv[0]);
   const char* result = "";

#ifdef POWER_DEBUG
   VectorPtr<MethodReturnDbg> dbgMethodCalledOn;
#endif

   if( getComponentCount() > 0 )
   {
      for( int i = getComponentCount() - 1; i >= 0; i-- )
      {
         argv[0] = cbName;

         SimComponent *pComponent = dynamic_cast<SimComponent *>( getComponent( i ) );
         AssertFatal( pComponent, "DynamicConsoleMethodComponent::callMethod - NULL component in list!" );

         DynamicConsoleMethodComponent *pThisComponent = dynamic_cast<DynamicConsoleMethodComponent*>( pComponent );
         AssertFatal( pThisComponent, "DynamicConsoleMethodComponent::callMethod - Non DynamicConsoleMethodComponent component attempting to callback!");

         // Only call on first depth components
         if( pComponent->isEnabled() && pThisComponent->isMethod( cbName ) )
         {
            result = Con::execute( pThisComponent, argc, argv );

#ifdef POWER_DEBUG
            dbgMethodCalledOn.increment();
            dbgMethodCalledOn.last().simObj = pThisComponent;
            dbgMethodCalledOn.last().retVal = StringTable->insert( result );
#endif
         }
      }
   }

   if( callThis )
   {
      result = Con::execute( pThis, argc, argv );

#ifdef POWER_DEBUG
      dbgMethodCalledOn.increment();
      dbgMethodCalledOn.last().simObj = this;
      dbgMethodCalledOn.last().retVal = StringTable->insert( result );
#endif
   }

#ifdef POWER_DEBUG
   if( dbgMethodCalledOn.size() > 1 )
   {
      Con::errorf( "Error: Method '%s' called via method redirection has multiple return values:", cbName );

      for( int i = 0; i < dbgMethodCalledOn.size(); i++ )
         Con::warnf( "--SimObject Name: '%s', return value '%s'", dbgMethodCalledOn[i].simObj->getName(), dbgMethodCalledOn[i].retVal );

      Con::warnf( "Last return value (%s) is being returned.", result );
   }
#endif

   return result;
}

ConsoleMethod( DynamicConsoleMethodComponent, callMethod, void, 3, 64 , "%obj.callMethod( %methodName, %arg1, %arg2, ... );")
{
   object->callMethodArgList( argc - 1, argv + 2 );
}

//-----------------------------------------------------------------------------

