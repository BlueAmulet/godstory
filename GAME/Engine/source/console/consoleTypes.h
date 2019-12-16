//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _CONSOLETYPES_H_
#define _CONSOLETYPES_H_

#ifndef _DYNAMIC_CONSOLETYPES_H_
#include "console/dynamicTypes.h"
#endif

#ifndef Offset
#if defined(POWER_COMPILER_GCC) && (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define Offset(m,T) ((int)(&((T *)1)->m) - 1)
#else
#define Offset(x, cls) ((dsize_t)((const char *)&(((cls *)0)->x)-(const char *)0))
#endif
#endif


class Point2F;
class Point3F;
class ColorI;
class ColorF;
class GFXShader;
class GFXCubemap;
class CustomMaterial;
class ProjectileData;
class ParticleEmitterData;
struct GameBaseData;


// Define Core Console Types
DefineConsoleType( TypeBool, bool )
DefineConsoleType( TypeBoolVector, Vector<bool>)
DefineConsoleType( TypeS8,  S8 )
DefineConsoleType( TypeS32, S32 )
DefineConsoleType( TypeS32Vector, Vector<S32> )
DefineConsoleType( TypeF32, F32 )
DefineConsoleType( TypeF32Vector, Vector<F32> )
DefineConsoleType( TypeString, const char * )
DefineConsoleType( TypeCaseString, const char * )
DefineConsoleType( TypeFilename, const char * )
DefineConsoleType( TypeEnum, S32 )
DefineConsoleType( TypeFlag, S32 )
DefineConsoleType( TypePoint3F, Point3F )
DefineConsoleType( TypeColorI, ColorI )
DefineConsoleType( TypeColorF, ColorF )
DefineConsoleType( TypeSimObjectPtr, SimObject* )
DefineConsoleType( TypeSimObjectName, SimObject* )
DefineConsoleType( TypeShader, GFXShader * )
DefineConsoleType( TypeCustomMaterial, CustomMaterial * )
DefineConsoleType( TypeCubemap, GFXCubemap * )
DefineConsoleType( TypeProjectileDataPtr, ProjectileData* )
DefineConsoleType( TypeParticleEmitterDataPtr, ParticleEmitterData* );
DefineConsoleType( TypeGameBaseDataPtr, GameBaseData* );
DefineConsoleType( TypePoint2FVector, Vector<Point2F> );
DefineConsoleType( TypePoint3FVector, Vector<Point3F> );
DefineConsoleType( TypeColorFVector, Vector<ColorF> );
DefineConsoleType( TypeStringVector, Vector< const char * > );
#endif
