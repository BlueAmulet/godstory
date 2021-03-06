//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "core/findMatch.h"

//--------------------------------------------------------------------------------
// NAME
//   FindMatch::FindMatch( const char *_expression, S32 maxNumMatches )
//
// DESCRIPTION
//   Class to match regular expressions (file names)
//   only works with '*','?', and 'chars'
//
// ARGUMENTS
//   _expression  -  The regular expression you intend to match (*.??abc.bmp)
//   _maxMatches  -  The maximum number of strings you wish to match.
//
// RETURNS
//
// NOTES
//
//--------------------------------------------------------------------------------

FindMatch::FindMatch( U32 _maxMatches )
{
   VECTOR_SET_ASSOCIATION(matchList);

   expression = NULL;
   maxMatches = _maxMatches;
   matchList.reserve( maxMatches );
}

FindMatch::FindMatch( char *_expression, U32 _maxMatches )
{
   VECTOR_SET_ASSOCIATION(matchList);

   expression = NULL;
   setExpression( _expression );
   maxMatches = _maxMatches;
   matchList.reserve( maxMatches );
}

FindMatch::~FindMatch()
{
   delete [] expression;
   matchList.clear();
}

void FindMatch::setExpression( const char *_expression )
{
   delete [] expression;

   S32 length = dStrlen(_expression) + 1;
   expression = new char[length];
   dStrcpy(expression, length, _expression);
   dStrupr(expression);
}

bool FindMatch::findMatch( const char *str, bool caseSensitive )
{
   if ( isFull() )
      return false;

   char nstr[512];
   dStrcpy( nstr, sizeof(nstr), str );
   dStrupr(nstr);
   if ( isMatch( expression, nstr, caseSensitive ) )
   {
      matchList.push_back( (char*)str );
      return true;
   }
   return false;
}

bool FindMatch::isMatch( const char *exp, const char *str, bool caseSensitive )
{
   const char  *e=exp;
   const char  *s=str;
   bool  match=true;

   while ( match && *e && *s )
   {
      switch( *e )
      {
         case '*':
               e++;
               match = false;
               while( ((s=dStrchr(s,*e)) !=NULL) && !match )
               {
                  match = isMatch( e, s, caseSensitive );
                  s++;
               }
               return( match );
         case '?':
            e++;
            s++;
            break;
         default:
            if (caseSensitive) match = ( *e++ == *s++ );
            else match = ( dToupper(*e++) == dToupper(*s++) );

            break;
      }
   }

   if (*e != *s) // both exp and str should be at '\0' if match was successfull
      match = false;

   return ( match );
}


bool FindMatch::isMatchMultipleExprs( const char *exps, const char *str, bool caseSensitive )
{
   char *tok = 0;
   int len = dStrlen(exps);

   char *e = new char[len+1];
   dStrcpy(e, len+1, exps);

   // [tom, 12/18/2006] This no longer supports space seperated expressions as
   // they don't work when the paths have spaces in.

   // search for each expression. return true soon as we see one.
   char *context = NULL;
   for( tok = dStrtok(e,"\t", &context); tok != NULL; tok = dStrtok(NULL,"\t", &context))
   {
      if( isMatch( tok, str, caseSensitive) )
      {
         delete []e;
         return true;
      }
   }

   delete []e;
   return false;
}
