//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "console/console.h"
#include "console/consoleTypes.h"
#include "core/stringTable.h"
#include "math/mPoint.h"
#include "core/color.h"
#include "console/simBase.h"



bool SkipSpaces(const char** pValues, const char *pEndValues, U32 count)
{
	U32 i = 0;
	const char *pNextValue = *pValues;

	while(i < count)
	{
		i++;
		pNextValue = dStrchr(*pValues, ' ');
		if (pNextValue != 0 && pNextValue < pEndValues)
			*pValues = pNextValue + 1;
		else
			return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// TypeColorFVector
//-----------------------------------------------------------------------------
ConsoleType( string, TypeColorFVector, Vector<ColorF> )

ConsoleGetType( TypeColorFVector )
{
	Vector<ColorF> *vec = (Vector<ColorF> *)dptr;
	S32 buffSize = ( vec->size() * 65 ) + 16 ;
	char* returnBuffer = Con::getReturnBuffer( buffSize );
	S32 maxReturn = buffSize;
	returnBuffer[0] = '\0';
	S32 returnLeng = 0;
	for (Vector<ColorF>::iterator itr = vec->begin(); itr != vec->end(); itr++)
	{
		// concatenate the next value onto the return string
		dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%g %g %g %g ", (*itr).red, (*itr).green, (*itr).blue, (*itr).alpha);
		// update the length of the return string (so far)
		returnLeng = dStrlen(returnBuffer);
	}
	// trim off that last extra space
	if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
		returnBuffer[returnLeng - 1] = '\0';
	return returnBuffer;
}

ConsoleSetType( TypeColorFVector )
{
	Vector<ColorF> *vec = (Vector<ColorF> *)dptr;
	// we assume the vector should be cleared first (not just appending)
	vec->clear();
	if(argc == 1)
	{
		const char *values = argv[0];
		const char *endValues = values + dStrlen(values);
		ColorF color;
		// advance through the string, pulling off S32's and advancing the pointer
		while (values < endValues && dSscanf(values, "%g %g %g %g", &color.red, &color.green, &color.blue, &color.alpha) != 0)
		{
			vec->push_back(color);
			if (!SkipSpaces(&values, endValues, 4))
				break;
		}
	}
	else if (argc > 1)
	{
		for (S32 i = 0; i < argc; i+=4)
		{
			ColorF color;
			color.red = dAtof(argv[i]);
			color.green = (argv[i+1] == NULL) ? 0.0f : dAtof(argv[i+1]);
			color.blue = (argv[i+2] == NULL) ? 0.0f : dAtof(argv[i+2]);
			color.alpha = (argv[i+3] == NULL) ? 1.0f : dAtof(argv[i+3]);

			vec->push_back(color);
		}
	}
	else
		Con::printf("Vector<Point2F> must be set as { r1 g1 b1 a1 r2 g2 b2 a2 ... } or \"r1 g1 b1 a1 r2 g2 b2 a2 ...\"");
}

//-----------------------------------------------------------------------------
// TypePoint2FVector
//-----------------------------------------------------------------------------
ConsoleType( string, TypePoint2FVector, Vector<Point2F> )

ConsoleGetType( TypePoint2FVector )
{
	Vector<Point2F> *vec = (Vector<Point2F> *)dptr;
	S32 buffSize = ( vec->size() * 35 ) + 16 ;
	char* returnBuffer = Con::getReturnBuffer( buffSize );
	S32 maxReturn = buffSize;
	returnBuffer[0] = '\0';
	S32 returnLeng = 0;
	for (Vector<Point2F>::iterator itr = vec->begin(); itr != vec->end(); itr++)
	{
		// concatenate the next value onto the return string
		dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%g %g ", (*itr).x, (*itr).y);
		// update the length of the return string (so far)
		returnLeng = dStrlen(returnBuffer);
	}
	// trim off that last extra space
	if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
		returnBuffer[returnLeng - 1] = '\0';
	return returnBuffer;
}

ConsoleSetType( TypePoint2FVector )
{
	Vector<Point2F> *vec = (Vector<Point2F> *)dptr;
	// we assume the vector should be cleared first (not just appending)
	vec->clear();
	if(argc == 1)
	{
		const char *values = argv[0];
		const char *endValues = values + dStrlen(values);
		Point2F point;
		// advance through the string, pulling off S32's and advancing the pointer
		while (values < endValues && dSscanf(values, "%g %g", &point.x, &point.y) != 0)
		{
			vec->push_back(point);
			if (!SkipSpaces(&values, endValues, 2))
				break;
		}
	}
	else if (argc > 1)
	{
		for (S32 i = 0; i < argc; i+=2)
		{
			Point2F point;
			point.x = dAtof(argv[i]);
			point.y = (argv[i+1] == NULL) ? 0.0f : dAtof(argv[i+1]);
			
			vec->push_back(point);
		}
	}
	else
		Con::printf("Vector<Point2F> must be set as { a1 b1 a2 b2 ... } or \"a1 b1 a2 b2 ...\"");
}


//-----------------------------------------------------------------------------
// TypePoint3FVector
//-----------------------------------------------------------------------------
ConsoleType( string, TypePoint3FVector, Vector<Point3F> )

ConsoleGetType( TypePoint3FVector )
{
	Vector<Point3F> *vec = (Vector<Point3F> *)dptr;
	S32 buffSize = ( vec->size() * 50 ) + 16 ;
	char* returnBuffer = Con::getReturnBuffer( buffSize );
	S32 maxReturn = buffSize;
	returnBuffer[0] = '\0';
	S32 returnLeng = 0;
	for (Vector<Point3F>::iterator itr = vec->begin(); itr != vec->end(); itr++)
	{
		// concatenate the next value onto the return string
		dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%g %g %g ", (*itr).x, (*itr).y, (*itr).z);
		// update the length of the return string (so far)
		returnLeng = dStrlen(returnBuffer);
	}
	// trim off that last extra space
	if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
		returnBuffer[returnLeng - 1] = '\0';
	return returnBuffer;
}

ConsoleSetType( TypePoint3FVector )
{
	Vector<Point3F> *vec = (Vector<Point3F> *)dptr;
	// we assume the vector should be cleared first (not just appending)
	vec->clear();
	if(argc == 1)
	{
		const char *values = argv[0];
		const char *endValues = values + dStrlen(values);
		Point3F point;
		// advance through the string, pulling off S32's and advancing the pointer
		while (values < endValues && dSscanf(values, "%g %g %g", &point.x, &point.y, &point.z) != 0)
		{
			vec->push_back(point);
			if (!SkipSpaces(&values, endValues, 3))
				break;
		}
	}
	else if (argc > 1)
	{
		for (S32 i = 0; i < argc; i+=3)
		{
			Point3F point;
			point.x = dAtof(argv[i]);
			point.y = (argv[i+1] == NULL) ? 0.0f : dAtof(argv[i+1]);
			point.z = (argv[i+2] == NULL) ? 0.0f : dAtof(argv[i+2]);

			vec->push_back(point);
		}
	}
	else
		Con::printf("Vector<Point3F> must be set as { a1 b1 c1 a2 b2 c2 ... } or \"a1 b1 c1 a2 b2 c2 ...\"");
}
//-----------------------------------------------------------------------------
// TypeString
//-----------------------------------------------------------------------------
ConsoleType( string, TypeString, const char* )

ConsoleGetType( TypeString )
{
   return *((const char **)(dptr));
}

ConsoleSetType( TypeString )
{
   if(argc == 1)
      *((const char **) dptr) = StringTable->insert(argv[0]);
   else
      Con::printf("(TypeString) Cannot set multiple args to a single string.");
}

//-----------------------------------------------------------------------------
// TypeCaseString
//-----------------------------------------------------------------------------
ConsoleType( caseString, TypeCaseString, const char* )

ConsoleSetType( TypeCaseString )
{
   if(argc == 1)
      *((const char **) dptr) = StringTable->insert(argv[0], true);
   else
      Con::printf("(TypeCaseString) Cannot set multiple args to a single string.");
}

ConsoleGetType( TypeCaseString )
{
   return *((const char **)(dptr));
}

//-----------------------------------------------------------------------------
// TypeFileName
//-----------------------------------------------------------------------------
ConsolePrepType( filename, TypeFilename, sizeof( const char* ) )

ConsoleSetType( TypeFilename )
{
   if(argc == 1)
   {
      char buffer[1024];
      if(argv[0][0] == '$')
      {
         dMemcpy(buffer, argv[0], sizeof(buffer) - 1);
         buffer[sizeof(buffer)-1] = 0;
      }
      else if (! Con::expandScriptFilename(buffer, 1024, argv[0]))
      {
         Con::warnf("(TypeFilename) illegal filename detected: %s", argv[0]);
         return;
      }

      *((const char **) dptr) = StringTable->insert(buffer);
   }
   else
      Con::printf("(TypeFilename) Cannot set multiple args to a single filename.");
}

ConsoleGetType( TypeFilename )
{
   return *((const char **)(dptr));
}

ConsoleProcessData( TypeFilename )
{
   if( Con::expandScriptFilename( buffer, bufferSz, data ) )
      return buffer;
   else
   {
      Con::warnf("(TypeFilename) illegal filename detected: %s", data);
      return data;
   }
}

//-----------------------------------------------------------------------------
// TypeS8
//-----------------------------------------------------------------------------
ConsoleType( char, TypeS8, S8 )

ConsoleGetType( TypeS8 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d", *((U8 *) dptr) );
   return returnBuffer;
}

ConsoleSetType( TypeS8 )
{
   if(argc == 1)
      *((U8 *) dptr) = dAtoi(argv[0]);
   else
      Con::printf("(TypeU8) Cannot set multiple args to a single S32.");
}

//-----------------------------------------------------------------------------
// TypeS32
//-----------------------------------------------------------------------------
ConsoleType( int, TypeS32, S32 )
ImplementConsoleTypeCasters(TypeS32, S32)

ConsoleGetType( TypeS32 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d", *((S32 *) dptr) );
   return returnBuffer;
}

ConsoleSetType( TypeS32 )
{
   if(argc == 1)
      *((S32 *) dptr) = dAtoi(argv[0]);
   else
      Con::printf("(TypeS32) Cannot set multiple args to a single S32.");
}

//-----------------------------------------------------------------------------
// TypeS32Vector
//-----------------------------------------------------------------------------
ConsoleType( intList, TypeS32Vector, Vector<S32> )

ConsoleGetType( TypeS32Vector )
{
   Vector<S32> *vec = (Vector<S32> *)dptr;
   S32 buffSize = ( vec->size() * 15 ) + 16 ;
   char* returnBuffer = Con::getReturnBuffer( buffSize );
   S32 maxReturn = buffSize;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<S32>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%d ", *itr);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleSetType( TypeS32Vector )
{
   Vector<S32> *vec = (Vector<S32> *)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if(argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      S32 value;
      // advance through the string, pulling off S32's and advancing the pointer
      while (values < endValues && dSscanf(values, "%d", &value) != 0)
      {
         vec->push_back(value);
         const char *nextValues = dStrchr(values, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < argc; i++)
         vec->push_back(dAtoi(argv[i]));
   }
   else
      Con::printf("Vector<S32> must be set as { a, b, c, ... } or \"a b c ...\"");
}

//-----------------------------------------------------------------------------
// TypeF32
//-----------------------------------------------------------------------------
ConsoleType( float, TypeF32, F32 )
ImplementConsoleTypeCasters(TypeF32, F32)

ConsoleGetType( TypeF32 )
{
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%g", *((F32 *) dptr) );
   return returnBuffer;
}
ConsoleSetType( TypeF32 )
{
   if(argc == 1)
      *((F32 *) dptr) = dAtof(argv[0]);
   else
      Con::printf("(TypeF32) Cannot set multiple args to a single F32.");
}

//-----------------------------------------------------------------------------
// TypeF32Vector
//-----------------------------------------------------------------------------
ConsoleType( floatList, TypeF32Vector, Vector<F32> )

ConsoleGetType( TypeF32Vector )
{
   Vector<F32> *vec = (Vector<F32> *)dptr;
   S32 buffSize = ( vec->size() * 15 ) + 16 ;
   char* returnBuffer = Con::getReturnBuffer( buffSize );
   S32 maxReturn = buffSize;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<F32>::iterator itr = vec->begin(); itr != vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%g ", *itr);
      // update the length of the return string (so far)
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return returnBuffer;
}

ConsoleSetType( TypeF32Vector )
{
   Vector<F32> *vec = (Vector<F32> *)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if(argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      F32 value;
      // advance through the string, pulling off F32's and advancing the pointer
      while (values < endValues && dSscanf(values, "%g", &value) != 0)
      {
         vec->push_back(value);
         const char *nextValues = dStrchr(values, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < argc; i++)
         vec->push_back(dAtof(argv[i]));
   }
   else
      Con::printf("Vector<F32> must be set as { a, b, c, ... } or \"a b c ...\"");
}

//-----------------------------------------------------------------------------
// TypeBool
//-----------------------------------------------------------------------------
ConsoleType( bool, TypeBool, bool )

ConsoleGetType( TypeBool )
{
   return *((bool *) dptr) ? "1" : "0";
}

ConsoleSetType( TypeBool )
{
   if(argc == 1)
      *((bool *) dptr) = dAtob(argv[0]);
   else
      Con::printf("(TypeBool) Cannot set multiple args to a single bool.");
}


//-----------------------------------------------------------------------------
// TypeBoolVector
//-----------------------------------------------------------------------------
ConsoleType( boolList, TypeBoolVector, Vector<bool> )

ConsoleGetType( TypeBoolVector )
{
   Vector<bool> *vec = (Vector<bool>*)dptr;
   char* returnBuffer = Con::getReturnBuffer(1024);
   S32 maxReturn = 1024;
   returnBuffer[0] = '\0';
   S32 returnLeng = 0;
   for (Vector<bool>::iterator itr = vec->begin(); itr < vec->end(); itr++)
   {
      // concatenate the next value onto the return string
      dSprintf(returnBuffer + returnLeng, maxReturn - returnLeng, "%d ", (*itr == true ? 1 : 0));
      returnLeng = dStrlen(returnBuffer);
   }
   // trim off that last extra space
   if (returnLeng > 0 && returnBuffer[returnLeng - 1] == ' ')
      returnBuffer[returnLeng - 1] = '\0';
   return(returnBuffer);
}

ConsoleSetType( TypeBoolVector )
{
   Vector<bool> *vec = (Vector<bool>*)dptr;
   // we assume the vector should be cleared first (not just appending)
   vec->clear();
   if (argc == 1)
   {
      const char *values = argv[0];
      const char *endValues = values + dStrlen(values);
      S32 value;
      // advance through the string, pulling off bool's and advancing the pointer
      while (values < endValues && dSscanf(values, "%d", &value) != 0)
      {
         vec->push_back(value == 0 ? false : true);
         const char *nextValues = dStrchr(values, ' ');
         if (nextValues != 0 && nextValues < endValues)
            values = nextValues + 1;
         else
            break;
      }
   }
   else if (argc > 1)
   {
      for (S32 i = 0; i < argc; i++)
         vec->push_back(dAtob(argv[i]));
   }
   else
      Con::printf("Vector<bool> must be set as { a, b, c, ... } or \"a b c ...\"");
}

//-----------------------------------------------------------------------------
// TypeEnum
//-----------------------------------------------------------------------------
ConsoleType( enumval, TypeEnum, S32 )

ConsoleGetType( TypeEnum )
{
   AssertFatal(tbl, "Null enum table passed to getDataTypeEnum()");
   S32 dptrVal = *(S32*)dptr;
   for (S32 i = 0; i < tbl->size; i++)
   {
      if (dptrVal == tbl->table[i].index)
      {
         return tbl->table[i].label;
      }
   }

   //not found
   return "";
}

ConsoleSetType( TypeEnum )
{
   AssertFatal(tbl, "Null enum table passed to setDataTypeEnum()");
   if (argc != 1) return;

   S32 val = 0;
   for (S32 i = 0; i < tbl->size; i++)
   {
      if (! dStricmp(argv[0], tbl->table[i].label))
      {
         val = tbl->table[i].index;
         break;
      }
   }
   *((S32 *) dptr) = val;
}

//-----------------------------------------------------------------------------
// TypeFlag
//-----------------------------------------------------------------------------
ConsoleType( flag, TypeFlag, S32 )

ConsoleGetType( TypeFlag )
{
   BitSet32 tempFlags = *(BitSet32 *)dptr;
   if (tempFlags.test(flag)) return "true";
   else return "false";
}

ConsoleSetType( TypeFlag )
{
   bool value = true;
   if (argc != 1)
   {
      Con::printf("flag must be true or false");
   }
   else
   {
      value = dAtob(argv[0]);
   }
   ((BitSet32 *)dptr)->set(flag, value);
}

//-----------------------------------------------------------------------------
// TypeColorF
//-----------------------------------------------------------------------------
ConsoleType( ColorF, TypeColorF, ColorF )

ConsoleGetType( TypeColorF )
{
   ColorF * color = (ColorF*)dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%g %g %g %g", color->red, color->green, color->blue, color->alpha);
   return(returnBuffer);
}

ConsoleSetType( TypeColorF )
{
   ColorF *tmpColor = (ColorF *) dptr;
   if(argc == 1)
   {
      tmpColor->set(0, 0, 0, 1);
      F32 r,g,b,a;
      S32 args = dSscanf(argv[0], "%g %g %g %g", &r, &g, &b, &a);
      tmpColor->red = r;
      tmpColor->green = g;
      tmpColor->blue = b;
      if (args == 4)
         tmpColor->alpha = a;
   }
   else if(argc == 3)
   {
      tmpColor->red    = dAtof(argv[0]);
      tmpColor->green  = dAtof(argv[1]);
      tmpColor->blue   = dAtof(argv[2]);
      tmpColor->alpha  = 1.f;
   }
   else if(argc == 4)
   {
      tmpColor->red    = dAtof(argv[0]);
      tmpColor->green  = dAtof(argv[1]);
      tmpColor->blue   = dAtof(argv[2]);
      tmpColor->alpha  = dAtof(argv[3]);
   }
   else
      Con::printf("Color must be set as { r, g, b [,a] }");
}

//-----------------------------------------------------------------------------
// TypeColorI
//-----------------------------------------------------------------------------
ConsoleType( ColorI, TypeColorI, ColorI )

ConsoleGetType( TypeColorI )
{
   ColorI *color = (ColorI *) dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%d %d %d %d", color->red, color->green, color->blue, color->alpha);
   return returnBuffer;
}

ConsoleSetType( TypeColorI )
{
   ColorI *tmpColor = (ColorI *) dptr;
   if(argc == 1)
   {
      tmpColor->set(0, 0, 0, 255);
      S32 r,g,b,a;
      S32 args = dSscanf(argv[0], "%d %d %d %d", &r, &g, &b, &a);
      tmpColor->red = r;
      tmpColor->green = g;
      tmpColor->blue = b;
      if (args == 4)
         tmpColor->alpha = a;
   }
   else if(argc == 3)
   {
      tmpColor->red    = dAtoi(argv[0]);
      tmpColor->green  = dAtoi(argv[1]);
      tmpColor->blue   = dAtoi(argv[2]);
      tmpColor->alpha  = 255;
   }
   else if(argc == 4)
   {
      tmpColor->red    = dAtoi(argv[0]);
      tmpColor->green  = dAtoi(argv[1]);
      tmpColor->blue   = dAtoi(argv[2]);
      tmpColor->alpha  = dAtoi(argv[3]);
   }
   else
      Con::printf("Color must be set as { r, g, b [,a] }");
}

//-----------------------------------------------------------------------------
// TypeSimObjectPtr
//-----------------------------------------------------------------------------
ConsoleType( SimObjectPtr, TypeSimObjectPtr, SimObject* )

ConsoleSetType( TypeSimObjectPtr )
{
   if(argc == 1)
   {
      SimObject **obj = (SimObject **)dptr;
      *obj = Sim::findObject(argv[0]);
   }
   else
      Con::printf("(TypeSimObjectPtr) Cannot set multiple args to a single S32.");
}

ConsoleGetType( TypeSimObjectPtr )
{
   SimObject **obj = (SimObject**)dptr;
   char* returnBuffer = Con::getReturnBuffer(256);
   dSprintf(returnBuffer, 256, "%s", *obj ? (*obj)->getName() ? (*obj)->getName() : (*obj)->getIdString() : "");
   return returnBuffer;
}

//-----------------------------------------------------------------------------
// TypeSimObjectName
//-----------------------------------------------------------------------------
ConsoleType( SimObjectPtr, TypeSimObjectName, SimObject* )

ConsoleSetType( TypeSimObjectName )
{
   if(argc == 1)
   {
      SimObject **obj = (SimObject **)dptr;
      *obj = Sim::findObject(argv[0]);
   }
   else
      Con::printf("(TypeSimObjectName) Cannot set multiple args to a single S32.");
}

ConsoleGetType( TypeSimObjectName )
{
   SimObject **obj = (SimObject**)dptr;
   char* returnBuffer = Con::getReturnBuffer(128);
   dSprintf(returnBuffer, 128, "%s", *obj && (*obj)->getName() ? (*obj)->getName() : "");
   return returnBuffer;
}

//-----------------------------------------------------------------------------
// TypeColorFVector
//-----------------------------------------------------------------------------
ConsoleType( stringList, TypeStringVector, Vector< const char * > )

ConsoleSetType( TypeStringVector )
{
	Vector< const char * > *vec = (Vector< const char * > *)dptr;
	
	Vector< const char * >::iterator it;
	for (it = vec->begin(); it != vec->end(); ++it)
	{
		if (*it)
		{
			delete [] (*it);
			*it = NULL;
		}
	}
	vec->clear();

	if (argc == 1)
	{
		const char *pStart = argv[0];
		const char *pEnd = pStart;
		
		while( *pEnd )
		{
			pEnd = dStrchr(pStart, ' ');
			if (pEnd == NULL)
			{
				break;
			}
			
			S32 len = pEnd - pStart;
			if (len)
			{
				char *pContent = new char[ len + 1 ];
				dStrncpy(pContent, len + 1, pStart, len);
				pContent[len] = 0;
				vec->push_back(pContent);
			}

			pStart = pEnd;
			pEnd++;
		}
	}
	else
	{
		Con::printf("Vector< const char * > must be set as \"a b c ...\"");
	}
}

ConsoleGetType( TypeStringVector )
{
	Vector< const char * > *vec = (Vector< const char * > *)dptr;
	
	Vector< const char * >::iterator it;
	S32 buffSize = 0;
	for (it = vec->begin(); it != vec->end(); ++it)
	{
		if (*it)
		{
			buffSize += dStrlen(*it) + 1;
		}
	}

	char* returnBuffer = Con::getReturnBuffer( buffSize );
	returnBuffer[0] = 0;
	char* pBuffer = returnBuffer;
	S32 nLeftSize = buffSize;
	for (it = vec->begin(); it != vec->end(); ++it)
	{
		if (*it == NULL)
			continue;
		
		int nNumCopy = 0;
		if (it != vec->end())
		{
			nNumCopy = dSprintf(pBuffer, nLeftSize, "%s ", (*it));
		}
		else
		{
			nNumCopy = dSprintf(pBuffer, nLeftSize, "%s", (*it));
		}

		nLeftSize -= nNumCopy;
	}

	return returnBuffer;
}