//-----------------------------------------------------------------------------
// PowerEngine
// Written by Melvyn May, Started on 4th August 2002.
//
// "My code is written for the PowerEngine community, so do your worst with it,
//	just don't rip-it-off and call it your own without even thanking me".
//
//	- Melv.
//
//
// Conversion to TSE By Brian "bzztbomb" Richardson 9/2005
//   This was a neat piece of code!  Thanks Melv!
//   I've switched this to use one large indexed primitive buffer.  All animation
//   is then done in the vertex shader.  This means we have a static vertex/primitive
//   buffer that never changes!  How spiff!  Because of this, the culling code was
//   changed to render out full quadtree nodes, we don't try to cull each individual
//   node ourselves anymore.  This means to get good performance, you probably need to do the 
//   following:
//     1.  If it's a small area to cover, turn off culling completely.
//     2.  You want to tune the parameters to make sure there are a lot of billboards within
//         each quadrant.
// 
// POTENTIAL TODO LIST:
//   TODO: Clamp item alpha to fog alpha
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "gfx/gfxDevice.h"
#include "gfx/PrimBuilder.h"	// Used for debug / mission edit rendering
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mRandom.h"
#include "math/mathIO.h"
#include "terrain/terrData.h"
#include "console/simBase.h"
#include "sceneGraph/sceneGraph.h"
#include "T3D/fx/fxFoliageReplicator.h"
#include "renderInstance/renderInstMgr.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
#include "renderInstance/RenderTranslucentMgr.h"
#include <Mmsystem.h>
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* fxFoliageReplicator::mAreaSB = NULL;
GFXStateBlock* fxFoliageReplicator::mSetSB = NULL;
GFXStateBlock* fxFoliageReplicator::mDebugSetSB = NULL;
GFXStateBlock* fxFoliageReplicator::mClearSB = NULL;

#pragma warning( push, 4 )
// I just could not get rid of this warning without flat out disabling it. ;/
// If you know how, I'm very curious, email me at bzzt@knowhere.net
// thanks
#pragma warning( disable : 4127 )

const U32 AlphaTexLen = 1024;

//------------------------------------------------------------------------------
//
//	Put the function in /example/common/editor/ObjectBuilderGui.gui [around line 458] ...
//
//	function ObjectBuilderGui::buildfxFoliageReplicator(%this)
//	{
//		%this.className = "fxFoliageReplicator";
//		%this.process();
//	}
//
//------------------------------------------------------------------------------
//
//	Put this in /example/common/editor/EditorGui.cs in [function Creator::init( %this )]
//	
//   %Environment_Item[8] = "fxFoliageReplicator";  <-- ADD THIS.
//
//------------------------------------------------------------------------------
//
//	Put this in /example/common/client/missionDownload.cs in [function clientCmdMissionStartPhase3(%seq,%missionName)] (line 65)
//	after codeline 'onPhase2Complete();'.
//
//	StartFoliageReplication();
//
//------------------------------------------------------------------------------
//
//	Put this in /engine/console/simBase.h (around line 509) in
//
//	namespace Sim
//  {
//	   DeclareNamedSet(fxFoliageSet)  <-- ADD THIS (Note no semi-colon).
//
//------------------------------------------------------------------------------
//
//	Put this in /engine/console/simBase.cc (around line 19) in
//
//  ImplementNamedSet(fxFoliageSet)  <-- ADD THIS (Note no semi-colon).
//
//------------------------------------------------------------------------------
//
//	Put this in /engine/console/simManager.cc [function void init()] (around line 269).
//
//	namespace Sim
//  {
//		InstantiateNamedSet(fxFoliageSet);  <-- ADD THIS (Including Semi-colon).
//
//------------------------------------------------------------------------------
extern bool gEditingMission;

//------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(fxFoliageReplicator);
IMPLEMENT_CO_NETOBJECT_V1(fxFireReplicator);
IMPLEMENT_CO_NETOBJECT_V1(VolumeFog);
DECLARE_SERIALIBLE(fxFoliageReplicator);

//------------------------------------------------------------------------------
//
// Trig Table Lookups.
//
//------------------------------------------------------------------------------
const F32 PeriodLen = (F32) 2.0f * (F32) M_PI;
const F32 PeriodLenMinus = (F32) (2.0f * M_PI) - 0.01f;

//------------------------------------------------------------------------------
//
// Class: fxFoliageRenderList
//
//------------------------------------------------------------------------------

void fxFoliageRenderList::SetupClipPlanes(SceneState* state, const F32 FarClipPlane)
{
	// Fetch Camera Position.
	CameraPosition  = state->getCameraPosition();
	// Calculate Perspective.
	F32 FarOverNear = FarClipPlane / (F32) state->getNearPlane();

	// Calculate Clip-Planes.
	FarPosLeftUp    = Point3F(	(F32) state->getBaseZoneState().frustum[0] * FarOverNear,
								FarClipPlane,
								(F32) state->getBaseZoneState().frustum[3] * FarOverNear);
	FarPosLeftDown  = Point3F(	(F32) state->getBaseZoneState().frustum[0] * FarOverNear,
								FarClipPlane,
								(F32) state->getBaseZoneState().frustum[2] * FarOverNear);
	FarPosRightUp   = Point3F(	(F32) state->getBaseZoneState().frustum[1] * FarOverNear,
								FarClipPlane,
								(F32) state->getBaseZoneState().frustum[3] * FarOverNear);
	FarPosRightDown = Point3F(	(F32) state->getBaseZoneState().frustum[1] * FarOverNear,
								FarClipPlane,
								(F32) state->getBaseZoneState().frustum[2] * FarOverNear);

	// Calculate our World->Object Space Transform.
	MatrixF InvXForm = state->mModelview;
	InvXForm.inverse();
	// Convert to Object-Space.
	InvXForm.mulP(FarPosLeftUp);
	InvXForm.mulP(FarPosLeftDown);
	InvXForm.mulP(FarPosRightUp);
	InvXForm.mulP(FarPosRightDown);

	// Calculate Bounding Box (including Camera).
	mBox.min = CameraPosition;
	mBox.min.setMin(FarPosLeftUp);
	mBox.min.setMin(FarPosLeftDown);
	mBox.min.setMin(FarPosRightUp);
	mBox.min.setMin(FarPosRightDown);
	mBox.max = CameraPosition;
	mBox.max.setMax(FarPosLeftUp);
	mBox.max.setMax(FarPosLeftDown);
	mBox.max.setMax(FarPosRightUp);
	mBox.max.setMax(FarPosRightDown);

	// Setup Our Viewplane.
	ViewPlanes[0].set(CameraPosition,	FarPosLeftUp,		FarPosLeftDown);
	ViewPlanes[1].set(CameraPosition,	FarPosRightUp,		FarPosLeftUp);
	ViewPlanes[2].set(CameraPosition,	FarPosRightDown,	FarPosRightUp);
	ViewPlanes[3].set(CameraPosition,	FarPosLeftDown,		FarPosRightDown);
	ViewPlanes[4].set(FarPosLeftUp,		FarPosRightUp,		FarPosRightDown);
}

//------------------------------------------------------------------------------


inline void fxFoliageRenderList::DrawQuadBox(const Box3F& QuadBox, const ColorF Colour)
{
	// Define our debug box.
	static Point3F BoxPnts[] = {
								  Point3F(0,0,0),
								  Point3F(0,0,1),
								  Point3F(0,1,0),
								  Point3F(0,1,1),
								  Point3F(1,0,0),
								  Point3F(1,0,1),
								  Point3F(1,1,0),
								  Point3F(1,1,1)
								};

	static U32 BoxVerts[][4] = {
								  {0,2,3,1},     // -x
								  {7,6,4,5},     // +x
								  {0,1,5,4},     // -y
								  {3,2,6,7},     // +y
								  {0,4,6,2},     // -z
								  {3,7,5,1}      // +z
								};

	static Point3F BoxNormals[] = {
								  Point3F(-1, 0, 0),
								  Point3F( 1, 0, 0),
								  Point3F( 0,-1, 0),
								  Point3F( 0, 1, 0),
								  Point3F( 0, 0,-1),
								  Point3F( 0, 0, 1)
								};

	// Project our Box Points.
	Point3F ProjectionPoints[8];

   for( U32 i=0; i<8; i++ )
	{
		ProjectionPoints[i].set(BoxPnts[i].x ? QuadBox.max.x : QuadBox.min.x,
								BoxPnts[i].y ? QuadBox.max.y : QuadBox.min.y,
								BoxPnts[i].z ? (mHeightLerp * QuadBox.max.z) + (1-mHeightLerp) * QuadBox.min.z : QuadBox.min.z);

	}

	PrimBuild::color(Colour);

	// Draw the Box.
	for(U32 x = 0; x < 6; x++)
	{
		// Draw a line-loop.
		PrimBuild::begin(GFXLineStrip, 5);

		for(U32 y = 0; y < 4; y++)
		{
			PrimBuild::vertex3f(ProjectionPoints[BoxVerts[x][y]].x,
						ProjectionPoints[BoxVerts[x][y]].y,
						ProjectionPoints[BoxVerts[x][y]].z);
		}
		PrimBuild::vertex3f(ProjectionPoints[BoxVerts[x][0]].x,
					ProjectionPoints[BoxVerts[x][0]].y,
					ProjectionPoints[BoxVerts[x][0]].z);
		PrimBuild::end();
	} 
}

//------------------------------------------------------------------------------
bool fxFoliageRenderList::IsQuadrantVisible(const Box3F VisBox, const MatrixF& RenderTransform)
{
	// Can we trivially accept the visible box?
	if (mBox.isOverlapped(VisBox))
	{
		// Yes, so calculate Object-Space Box.
		MatrixF InvXForm = RenderTransform;
		InvXForm.inverse();
		Box3F OSBox = VisBox;
		InvXForm.mulP(OSBox.min);
		InvXForm.mulP(OSBox.max);

		// Yes, so fetch Box Center.
		Point3F Center;
		OSBox.getCenter(&Center);

		// Scale.
		Point3F XRad(OSBox.len_x() * 0.5f, 0.0f, 0.0f);
		Point3F YRad(0.0f, OSBox.len_y() * 0.5f, 0.0f);
		Point3F ZRad(0.0f, 0.0f, OSBox.len_z() * 0.5f);

		// Render Transformation.
		RenderTransform.mulP(Center);
		RenderTransform.mulV(XRad);
		RenderTransform.mulV(YRad);
		RenderTransform.mulV(ZRad);

		// Check against View-planes.
		for (U32 i = 0; i < 5; i++)
		{
			// Reject if not visible.
			if (ViewPlanes[i].whichSideBox(Center, 
                                        XRad, YRad, ZRad, 
                                        Point3F(0, 0, 0)) == PlaneF::Back)
            return false;
		}

		// Visible.
		return true;
	}

	// Not visible.
	return false;
}



//------------------------------------------------------------------------------
//
// Class: fxFoliageCulledList
//
//------------------------------------------------------------------------------
fxFoliageCulledList::fxFoliageCulledList(Box3F SearchBox, fxFoliageCulledList* InVec)
{
	// Find the Candidates.
	FindCandidates(SearchBox, InVec);
}

//------------------------------------------------------------------------------

void fxFoliageCulledList::FindCandidates(Box3F SearchBox, fxFoliageCulledList* InVec)
{
	// Search the Culled List.
	for (U32 i = 0; i < InVec->GetListCount(); i++)
	{
		// Is this Box overlapping our search box?
		if (SearchBox.isOverlapped(InVec->GetElement(i)->FoliageBox))
		{
			// Yes, so add it to our culled list.
			mCulledObjectSet.push_back(InVec->GetElement(i));
		}
	}
}



//------------------------------------------------------------------------------
//
// Class: fxFoliageReplicator
//
//------------------------------------------------------------------------------

fxFoliageReplicator::fxFoliageReplicator()
{
	// Setup NetObject.
	mTypeMask |= StaticObjectType | StaticRenderedObjectType;
	mAddedToScene = false;
	mNetFlags.set(Ghostable | ScopeAlways);

	// Reset Client Replication Started.
	mClientReplicationStarted = false;

	// Reset Foliage Count.
	mCurrentFoliageCount = 0;

	// Reset Creation Area Angle Animation.
	mCreationAreaAngle = 0;

	// Reset Last Render Time.
	mLastRenderTime = 0;

	// Reset Foliage Nodes.
	mPotentialFoliageNodes = 0;
	// Reset Billboards Acquired.
	mBillboardsAcquired = 0;

	// Reset Frame Serial ID.
	mFrameSerialID = 0;

	mAlphaLookup = NULL;
	mWidth = 0; 
	mHeight = 0;
	mTextureCoordUV1 = NULL;
	mTextureCoordUV2 = NULL;
	mTextureCoordUV3 = NULL;
	mTextureCoordUV4 = NULL;
}

//------------------------------------------------------------------------------

fxFoliageReplicator::~fxFoliageReplicator()
{
	if (mAlphaLookup)
		delete mAlphaLookup;
	SAFE_DELETE_ARRAY(mTextureCoordUV1);
	SAFE_DELETE_ARRAY(mTextureCoordUV2);
	SAFE_DELETE_ARRAY(mTextureCoordUV3);
	SAFE_DELETE_ARRAY(mTextureCoordUV4);

	//for(S32 i = 0; i < mSortFoliageItem.size(); i++)
	{
		//Vector<fxFoliageItem*> j = mSortFoliageItem[i];
		for (	Vector<fxFoliageItem*>::iterator QuadNodeItr = mSortFoliageItem.begin();
			QuadNodeItr != mSortFoliageItem.end();
			QuadNodeItr++ )
		{
			delete *QuadNodeItr;
		}
		//j.clear();
	}

	mSortFoliageItem.clear();
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();

	// Add out own persistent fields.
	addGroup( "Debugging" );	// MM: Added Group Header.
	addField( "UseDebugInfo",		TypeBool,		Offset( mFieldData.mUseDebugInfo,			fxFoliageReplicator ) );
	addField( "DebugBoxHeight",		TypeF32,		Offset( mFieldData.mDebugBoxHeight,			fxFoliageReplicator ) );
	addField( "HideFoliage",		TypeBool,		Offset( mFieldData.mHideFoliage,			fxFoliageReplicator ) );
	addField( "ShowPlacementArea",	TypeBool,		Offset( mFieldData.mShowPlacementArea,		fxFoliageReplicator ) );
	addField( "PlacementAreaHeight",TypeS32,		Offset( mFieldData.mPlacementBandHeight,	fxFoliageReplicator ) );
	addField( "PlacementColour",	TypeColorF,		Offset( mFieldData.mPlaceAreaColour,		fxFoliageReplicator ) );
	endGroup( "Debugging" );	// MM: Added Group Footer.

	addGroup( "Media" );	// MM: Added Group Header.
	addField( "Seed",				TypeS32,		Offset( mFieldData.mSeed,					fxFoliageReplicator ) );
	addField( "FoliageFile",		TypeFilename,	Offset( mFieldData.mFoliageFile,			fxFoliageReplicator ) );
	addField( "FoliageCount",		TypeS32,		Offset( mFieldData.mFoliageCount,			fxFoliageReplicator ) );
	addField( "FoliageRetries",		TypeS32,		Offset( mFieldData.mFoliageRetries,			fxFoliageReplicator ) );
	endGroup( "Media" );	// MM: Added Group Footer.

	addGroup( "Area" );	// MM: Added Group Header.
	addField( "InnerRadiusX",		TypeS32,		Offset( mFieldData.mInnerRadiusX,			fxFoliageReplicator ) );
	addField( "InnerRadiusY",		TypeS32,		Offset( mFieldData.mInnerRadiusY,			fxFoliageReplicator ) );
	addField( "OuterRadiusX",		TypeS32,		Offset( mFieldData.mOuterRadiusX,			fxFoliageReplicator ) );
	addField( "OuterRadiusY",		TypeS32,		Offset( mFieldData.mOuterRadiusY,			fxFoliageReplicator ) );
	endGroup( "Area" );	// MM: Added Group Footer.

	addGroup( "Dimensions" );	// MM: Added Group Header.
	addField( "MinWidth",			TypeF32,		Offset( mFieldData.mMinWidth,				fxFoliageReplicator ) );
	addField( "MaxWidth",			TypeF32,		Offset( mFieldData.mMaxWidth,				fxFoliageReplicator ) );
	addField( "MinHeight",			TypeF32,		Offset( mFieldData.mMinHeight,				fxFoliageReplicator ) );
	addField( "MaxHeight",			TypeF32,		Offset( mFieldData.mMaxHeight,				fxFoliageReplicator ) );
	addField( "FixAspectRatio",		TypeBool,		Offset( mFieldData.mFixAspectRatio,			fxFoliageReplicator ) );
	addField( "FixSizeToMax",		TypeBool,		Offset( mFieldData.mFixSizeToMax,			fxFoliageReplicator ) );
	addField( "OffsetZ",			TypeF32,		Offset( mFieldData.mOffsetZ,				fxFoliageReplicator ) );
	addField( "RandomFlip",			TypeBool,		Offset( mFieldData.mRandomFlip,				fxFoliageReplicator ) );
	endGroup( "Dimensions" );	// MM: Added Group Footer.

	addGroup( "Culling" );	// MM: Added Group Header.
	addField( "UseCulling",			TypeBool,		Offset( mFieldData.mUseCulling,				fxFoliageReplicator ) );
	addField( "CullResolution",		TypeS32,		Offset( mFieldData.mCullResolution,			fxFoliageReplicator ) );
	addField( "ViewDistance",		TypeF32,		Offset( mFieldData.mViewDistance,			fxFoliageReplicator ) );
	addField( "ViewClosest",		TypeF32,		Offset( mFieldData.mViewClosest,			fxFoliageReplicator ) );
	addField( "FadeInRegion",		TypeF32,		Offset( mFieldData.mFadeInRegion,			fxFoliageReplicator ) );
	addField( "FadeOutRegion",		TypeF32,		Offset( mFieldData.mFadeOutRegion,			fxFoliageReplicator ) );
	addField( "AlphaCutoff",		TypeF32,		Offset( mFieldData.mAlphaCutoff,			fxFoliageReplicator ) );
	addField( "GroundAlpha",		TypeF32,		Offset( mFieldData.mGroundAlpha,			fxFoliageReplicator ) );
	endGroup( "Culling" );	// MM: Added Group Footer.

	addGroup( "Animation" );	// MM: Added Group Header.
	addField( "SwayOn",				TypeBool,		Offset( mFieldData.mSwayOn,					fxFoliageReplicator ) );
	addField( "SwaySync",			TypeBool,		Offset( mFieldData.mSwaySync,				fxFoliageReplicator ) );
	addField( "SwayMagSide",		TypeF32,		Offset( mFieldData.mSwayMagnitudeSide,		fxFoliageReplicator ) );
	addField( "SwayMagFront",		TypeF32,		Offset( mFieldData.mSwayMagnitudeFront,		fxFoliageReplicator ) );
	addField( "MinSwayTime",		TypeF32,		Offset( mFieldData.mMinSwayTime,			fxFoliageReplicator ) );
	addField( "MaxSwayTime",		TypeF32,		Offset( mFieldData.mMaxSwayTime,			fxFoliageReplicator ) );
	endGroup( "Animation" );	// MM: Added Group Footer.

	addGroup( "Lighting" );	// MM: Added Group Header.
	addField( "LightOn",			TypeBool,		Offset( mFieldData.mLightOn,				fxFoliageReplicator ) );
	addField( "LightSync",			TypeBool,		Offset( mFieldData.mLightSync,				fxFoliageReplicator ) );
	addField( "MinLuminance",		TypeF32,		Offset( mFieldData.mMinLuminance,			fxFoliageReplicator ) );
	addField( "MaxLuminance",		TypeF32,		Offset( mFieldData.mMaxLuminance,			fxFoliageReplicator ) );
	addField( "LightTime",			TypeF32,		Offset( mFieldData.mLightTime,				fxFoliageReplicator ) );
	endGroup( "Lighting" );	// MM: Added Group Footer.

	addGroup( "Restrictions" );	// MM: Added Group Header.
	addField( "AllowOnTerrain",		TypeBool,		Offset( mFieldData.mAllowOnTerrain,			fxFoliageReplicator ) );
	addField( "AllowOnInteriors",	TypeBool,		Offset( mFieldData.mAllowOnInteriors,		fxFoliageReplicator ) );
	addField( "AllowOnStatics",		TypeBool,		Offset( mFieldData.mAllowStatics,			fxFoliageReplicator ) );
	addField( "AllowOnWater",		TypeBool,		Offset( mFieldData.mAllowOnWater,			fxFoliageReplicator ) );
	addField( "AllowWaterSurface",	TypeBool,		Offset( mFieldData.mAllowWaterSurface,		fxFoliageReplicator ) );
	addField( "AllowedTerrainSlope",TypeS32,		Offset( mFieldData.mAllowedTerrainSlope,	fxFoliageReplicator ) );
	endGroup( "Restrictions" );	// MM: Added Group Footer.
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::CreateFoliage(void)
{
	F32				HypX, HypY;
	F32				Angle;
	U32				RelocationRetry;
	Point3F			FoliagePosition;
	Point3F			FoliageStart;
	Point3F			FoliageEnd;
	Point3F			FoliageScale;
	bool			CollisionResult;
	RayInfo			RayEvent;

	// Let's get a minimum bounding volume.
	Point3F	MinPoint = Point3F( 10E30f, 10E30f, 10E30f);
	Point3F	MaxPoint = Point3F(-10E30f,-10E30f,-10E30f);

	// Check Host.
	AssertFatal(isClientObject(), "Trying to create Foliage on Server, this is bad!")

	// Cannot continue without Foliage Texture!
	//if (dStrlen(mFieldData.mFoliageFile) == 0) 
		//return;

	// Check that we can position somewhere!
	if (!(	mFieldData.mAllowOnTerrain ||
			mFieldData.mAllowOnInteriors ||
			mFieldData.mAllowStatics ||
			mFieldData.mAllowOnWater))
	{
		// Problem ...
		Con::warnf(ConsoleLogEntry::General, "fxFoliageReplicator - Could not place Foliage, All alloweds are off!");

		// Return here.
		return;
	}

	// Destroy Foliage if we've already got some.
	if (mCurrentFoliageCount != 0) DestroyFoliage();

	// Inform the user if culling has been disabled!
	if (!mFieldData.mUseCulling)
	{
		// Console Output.
		Con::printf("fxFoliageReplicator - Culling has been disabled!");
	}

	// ----------------------------------------------------------------------------------------------------------------------
	// > Calculate the Potential Foliage Nodes Required to achieve the selected culling resolution.
	// > Populate Quad-tree structure to depth determined by culling resolution.
	//
	// A little explanation is called for here ...
	//
	//			The approach to this problem has been choosen to make it *much* easier for
	//			the user to control the quad-tree culling resolution.  The user enters a single
	//			world-space value 'mCullResolution' which controls the highest resolution at
	//			which the replicator will check visibility culling.
	//
	//			example:	If 'mCullResolution' is 32 and the size of the replicated area is 128 radius
	//						(256 diameter) then this results in the replicator creating a quad-tree where
	//						there are 256/32 = 8x8 blocks.  Each of these can be checked to see if they
	//						reside within the viewing frustum and if not then they get culled therefore
	//						removing the need to parse all the billboards that occcupy that region.
	//						Most of the time you will get better than this as the culling algorithm will
	//						check the culling pyramid from the top to bottom e.g. the follow 'blocks'
	//						will be checked:-
	//
	//						 1 x 256 x 256 (All of replicated area)
	//						 4 x 128 x 128 (4 corners of above)
	//						16 x  64 x  64 (16 x 4 corners of above)
	//						etc.
	//
	//
	//	1.		First-up, the replicator needs to create a fixed-list of quad-tree nodes to work with.
	//
	//			To calculate this we take the largest outer-radius value set in the replicator and
	//			calculate how many quad-tree levels are required to achieve the selected 'mCullResolution'.
	//			One of the initial problems is that the replicator has seperate radii values for X & Y.
	//			This can lead to a culling resolution smaller in one axis than the other if there is a
	//			difference between the Outer-Radii.  Unfortunately, we just live with this as there is
	//			not much we can do here if we still want to allow the user to have this kind of
	//			elliptical placement control.
	//
	//			To calculate the number of nodes needed we using the following equation:-
	//
	//			Note:- We are changing the Logarithmic bases from 10 -> 2 ... grrrr!
	//
	//			Cr = mCullResolution
	//			Rs = Maximum Radii Diameter
	//
	//
	//				( Log10( Rs / Cr )       )
	//			int ( ---------------- + 0.5 )
	//				( Log10( 2 )             )
	//
	//					---------|
	//					 \
	//					  \			 n
	//					  /			4
	//					 /
	//					---------|
	//					   n = 0
	//
	//
	//			So basically we calculate the number of blocks in 1D at the highest resolution, then
	//			calculate the inverse exponential (base 2 - 1D) to achieve that quantity of blocks.
	//			We round that upto the next highest integer = e.  We then sum 4 to the power 0->e
	//			which gives us the correct number of nodes required.  e is also stored as the starting
	//			level value for populating the quad-tree (see 3. below).
	//
	//	2.		We then proceed to calculate the billboard positions as normal and calculate and assign
	//			each billboard a basic volume (rather than treat each as a point).  We need to take into
	//			account possible front/back swaying as well as the basic plane dimensions here.
	//			When all the billboards have been choosen we then proceed to populate the quad-tree.
	//
	//	3.		To populate the quad-tree we start with a box which completely encapsulates the volume
	//			occupied by all the billboards and enter into a recursive procedure to process that node.
	//			Processing this node involves splitting it into quadrants in X/Y untouched (for now).
	//			We then find candidate billboards with each of these quadrants searching using the
	//			current subset of shapes from the parent (this reduces the searching to a minimum and
	//			is very efficient).
	//
	//			If a quadrant does not enclose any billboards then the node is dropped otherwise it
	//			is processed again using the same procedure.
	//
	//			This happens until we have recursed through the maximum number of levels as calculated
	//			using the summation max (see equation above).  When level 0 is reached, the current list
	//			of enclosed objects is stored within the node (for the rendering algorithm).
	//
	//	4.		When this is complete we have finished here.  The next stage is when rendering takes place.
	//			An algorithm steps through the quad-tree from the top and does visibility culling on
	//			each box (with respect to the viewing frustum) and culls as appropriate.  If the box is
	//			visible then the next level is checked until we reach level 0 where the node contains
	//			a complete subset of billboards enclosed by the visible box.
	//
	//
	//	Using the above algorithm we can now generate *massive* quantities of billboards and (using the
	//	appropriate 'mCullResolution') only visible blocks of billboards will be processed.
	//
	//	- Melv.
	//
	// ----------------------------------------------------------------------------------------------------------------------



	// ----------------------------------------------------------------------------------------------------------------------
	// Step 1.
	// ----------------------------------------------------------------------------------------------------------------------

	// Calculate the maximum dimension.
	F32 MaxDimension = 2.0f * ( (mFieldData.mOuterRadiusX > mFieldData.mOuterRadiusY) ? mFieldData.mOuterRadiusX : mFieldData.mOuterRadiusY );

	// Let's check that our cull resolution is not greater than half our maximum dimension (and less than 1).
	if (mFieldData.mCullResolution > (MaxDimension/2) || mFieldData.mCullResolution < 8)
	{
		// Problem ...
		Con::warnf(ConsoleLogEntry::General, "fxFoliageReplicator - Could create Foliage, invalid Culling Resolution!");
		Con::warnf(ConsoleLogEntry::General, "fxFoliageReplicator - Culling Resolution *must* be >=8 or <= %0.2f!", (MaxDimension/2));

		// Return here.
		return;
	}

	// Take first Timestamp.
	F32 mStartCreationTime = (F32) Platform::getRealMilliseconds();

	// Calculate the quad-tree levels needed for selected 'mCullResolution'.
	mQuadTreeLevels = (U32)(mCeil(mLog( MaxDimension / mFieldData.mCullResolution ) / mLog( 2.0f )));

	// Calculate the number of potential nodes required.
	mPotentialFoliageNodes = 0;
	for (U32 n = 0; n <= mQuadTreeLevels; n++)
		mPotentialFoliageNodes += (U32)(mCeil(mPow(4.0f, (F32) n)));	// Ceil to be safe!

	// ----------------------------------------------------------------------------------------------------------------------
	// Step 2.
	// ----------------------------------------------------------------------------------------------------------------------

	// Set Seed.
	RandomGen.setSeed(mFieldData.mSeed);

	SAFE_DELETE_ARRAY(mTextureCoordUV1);
	SAFE_DELETE_ARRAY(mTextureCoordUV2);
	SAFE_DELETE_ARRAY(mTextureCoordUV3);
	SAFE_DELETE_ARRAY(mTextureCoordUV4);

	S32 TextureSplinterNum = 0;

	if ((mWidth > 0) && (mHeight > 0))
	{
		TextureSplinterNum = mWidth* mHeight;
		mTextureCoordUV1 = new Point2F[TextureSplinterNum];
		mTextureCoordUV2 = new Point2F[TextureSplinterNum];
		mTextureCoordUV3 = new Point2F[TextureSplinterNum];
		mTextureCoordUV4 = new Point2F[TextureSplinterNum];

		S32 temp_line, temp_col;
		F32 temp_f1 = 1.0f / F32(mWidth);
		F32 temp_f2 = 1.0f / F32(mHeight);

		for (S32 i = 0; i < TextureSplinterNum; i++)
		{
			temp_line = i / mWidth;
			temp_col = i % mWidth;

			mTextureCoordUV1[i].x = temp_col * temp_f1;
			mTextureCoordUV1[i].y = temp_line * temp_f2;
			mTextureCoordUV2[i].x = (temp_col + 1) * temp_f1;
			mTextureCoordUV2[i].y = mTextureCoordUV1[i].y;
			mTextureCoordUV3[i].x = mTextureCoordUV1[i].x;
			mTextureCoordUV3[i].y = (temp_line + 1) * temp_f2;
			mTextureCoordUV4[i].x = mTextureCoordUV2[i].x;
			mTextureCoordUV4[i].y = mTextureCoordUV3[i].y;
		}
	}

	// Add Foliage.
	for (U32 idx = 0; idx < mFieldData.mFoliageCount; idx++)
	{
		fxFoliageItem*	pFoliageItem;

		// Reset Relocation Retry.
		RelocationRetry = mFieldData.mFoliageRetries;

		// Find it a home ...
		do
		{
			// Get the fxFoliageReplicator Position.
			FoliagePosition = getPosition();

			// Calculate a random offset
			HypX	= RandomGen.randF((F32) mFieldData.mInnerRadiusX, (F32) mFieldData.mOuterRadiusX);
			HypY	= RandomGen.randF((F32) mFieldData.mInnerRadiusY, (F32) mFieldData.mOuterRadiusY);
			Angle	= RandomGen.randF(0, (F32) M_2PI);

			// Calcualte the new position.
			FoliagePosition.x += HypX * mCos(Angle);
			FoliagePosition.y += HypY * mSin(Angle);

			// Initialise RayCast Search Start/End Positions.
			FoliageStart = FoliageEnd = FoliagePosition;
			FoliageStart.z = 2000.f;
			FoliageEnd.z= -2000.f;

			// Perform Ray Cast Collision on Client.
			CollisionResult = gClientContainer.castRay(	FoliageStart, FoliageEnd, FXFOLIAGEREPLICATOR_COLLISION_MASK, &RayEvent);

			// Did we hit anything?
			if (CollisionResult)
			{
				// For now, let's pretend we didn't get a collision.
				CollisionResult = false;

				// Yes, so get it's type.
				U32 CollisionType = RayEvent.object->getTypeMask();

				// Check Illegal Placements, fail if we hit a disallowed type.
				if (((CollisionType & TerrainObjectType) && !mFieldData.mAllowOnTerrain)	||
					((CollisionType & InteriorObjectType) && !mFieldData.mAllowOnInteriors)	||
					((CollisionType & StaticTSObjectType) && !mFieldData.mAllowStatics)	||
					((CollisionType & WaterObjectType) && !mFieldData.mAllowOnWater) ) continue;

				// If we collided with water and are not allowing on the water surface then let's find the
				// terrain underneath and pass this on as the original collision else fail.
				if ((CollisionType & WaterObjectType) && !mFieldData.mAllowWaterSurface &&
					!gClientContainer.castRay( FoliageStart, FoliageEnd, FXFOLIAGEREPLICATOR_NOWATER_COLLISION_MASK, &RayEvent)) continue;

				// We passed with flying colour so carry on.
				CollisionResult = true;
			}

			// Invalidate if we are below Allowed Terrain Angle.
			if (RayEvent.normal.z < mSin(mDegToRad(90.0f-mFieldData.mAllowedTerrainSlope))) CollisionResult = false;

		// Wait until we get a collision.
		} while(!CollisionResult && --RelocationRetry);

		// Check for Relocation Problem.
		if (RelocationRetry > 0)
		{
			// Adjust Impact point.
			RayEvent.point.z += mFieldData.mOffsetZ;

			// Set New Position.
			FoliagePosition = RayEvent.point;
		}
		else
		{
			// Warning.
			Con::warnf(ConsoleLogEntry::General, "fxFoliageReplicator - Could not find satisfactory position for Foliage!");

			// Skip to next.
			continue;
		}

		// Create our Foliage Item.
		pFoliageItem = new fxFoliageItem;

		// Dump it into the vertex buffer
		S32 randnum = 0;
		S32 randnum1 = 0;
		float randnum2 = (float)(rand()%1000)/1000.0f;
		if(TextureSplinterNum)
		{
			randnum = rand()%TextureSplinterNum;
			randnum1 = randnum;
			while(randnum1 == randnum)
			{
				randnum1 = rand()%TextureSplinterNum;
			}
		}

		pFoliageItem->randnum = randnum;
		pFoliageItem->randnum1 = randnum1;
		pFoliageItem->randnum2 = randnum2;

		// Reset Frame Serial.
		pFoliageItem->LastFrameSerialID = 0;

		// Reset Transform.
		pFoliageItem->Transform.identity();

		// Set Position.
		pFoliageItem->Transform.setColumn(3, FoliagePosition);

		// Are we fixing size @ max?
		if (mFieldData.mFixSizeToMax)
		{
			// Yes, so set height maximum height.
			pFoliageItem->Height = mFieldData.mMaxHeight;
			// Is the Aspect Ratio Fixed?
			if (mFieldData.mFixAspectRatio)
				// Yes, so lock to height.
				pFoliageItem->Width = pFoliageItem->Height;
			else
				// No, so set width to maximum width.
				pFoliageItem->Width = mFieldData.mMaxWidth;
		}
		else
		{
			// No, so choose a new Scale.
			pFoliageItem->Height = RandomGen.randF(mFieldData.mMinHeight, mFieldData.mMaxHeight);
			// Is the Aspect Ratio Fixed?
			if (mFieldData.mFixAspectRatio)
				// Yes, so lock to height.
				pFoliageItem->Width = pFoliageItem->Height;
			else
				// No, so choose a random width.
				pFoliageItem->Width = RandomGen.randF(mFieldData.mMinWidth, mFieldData.mMaxWidth);
		}

		// Are we randomly flipping horizontally?
		if (mFieldData.mRandomFlip)
			// Yes, so choose a random flip for this object.
			pFoliageItem->Flipped = (RandomGen.randF(0, 1000) < 500.0f) ? false : true;
		else
			// No, so turn-off flipping.
			pFoliageItem->Flipped = false;		

		// Calculate Foliage Item World Box.
		// NOTE:-	We generate a psuedo-volume here.  It's basically the volume to which the
		//			plane can move and this includes swaying!
		//
		// Is Sway On?
		if (mFieldData.mSwayOn)
		{
			// Yes, so take swaying into account...
			pFoliageItem->FoliageBox.min =	FoliagePosition +
											Point3F(-pFoliageItem->Width / 2.0f - mFieldData.mSwayMagnitudeSide,
													-0.5f - mFieldData.mSwayMagnitudeFront,
													pFoliageItem->Height );

			pFoliageItem->FoliageBox.max =	FoliagePosition +
											Point3F(+pFoliageItem->Width / 2.0f + mFieldData.mSwayMagnitudeSide,
													+0.5f + mFieldData.mSwayMagnitudeFront,
													pFoliageItem->Height );
		}
		else
		{
			// No, so give it a minimum volume...
			pFoliageItem->FoliageBox.min =	FoliagePosition +
											Point3F(-pFoliageItem->Width / 2.0f,
													-0.5f,
													pFoliageItem->Height );

			pFoliageItem->FoliageBox.max =	FoliagePosition +
											Point3F(+pFoliageItem->Width / 2.0f,
													+0.5f,
													pFoliageItem->Height );
		}
		// Monitor the total volume.
		Point3F tmpMin = pFoliageItem->FoliageBox.min;
		Point3F tmpMax = pFoliageItem->FoliageBox.max;
		mWorldToObj.mulP(tmpMin);
		mWorldToObj.mulP(tmpMax);
		MinPoint.setMin(tmpMin);
		MaxPoint.setMax(tmpMax);

		// Store Shape in Replicated Shapes Vector.
		mReplicatedFoliage.push_back(pFoliageItem);

		// Increase Foliage Count.
		mCurrentFoliageCount++;
	}

	// Is Lighting On?
	if (mFieldData.mLightOn)
	{
		// Yes, so reset Global Light phase.
		mGlobalLightPhase = 0.0f;
		// Set Global Light Time Ratio.
		mGlobalLightTimeRatio = PeriodLenMinus / mFieldData.mLightTime;
		
		// Yes, so step through Foliage.
		for (U32 idx = 0; idx < mCurrentFoliageCount; idx++)
		{
			fxFoliageItem*	pFoliageItem;

			// Fetch the Foliage Item.
			pFoliageItem = mReplicatedFoliage[idx];

			// Do we have an item?
			if (pFoliageItem)
			{
				// Yes, so are lights syncronised?
				if (mFieldData.mLightSync)
				{
					pFoliageItem->LightTimeRatio = 1.0f;
					pFoliageItem->LightPhase = 0.0f;
				}
				else
				{
					// No, so choose a random Light phase.
					pFoliageItem->LightPhase = RandomGen.randF(0, PeriodLenMinus);
					// Set Light Time Ratio.
					pFoliageItem->LightTimeRatio = PeriodLenMinus / mFieldData.mLightTime;
				}
			}
		}

	}

	// Is Swaying Enabled?
	if (mFieldData.mSwayOn)
	{
		// Yes, so reset Global Sway phase.
		mGlobalSwayPhase = 0.0f;
		// Always set Global Sway Time Ratio.
		mGlobalSwayTimeRatio = PeriodLenMinus / RandomGen.randF(mFieldData.mMinSwayTime, mFieldData.mMaxSwayTime);
		
		// Yes, so step through Foliage.
		for (U32 idx = 0; idx < mCurrentFoliageCount; idx++)
		{			
			fxFoliageItem*	pFoliageItem;

			// Fetch the Foliage Item.
			pFoliageItem = mReplicatedFoliage[idx];
			// Do we have an item?
			if (pFoliageItem)
			{
				// Are we using Sway Sync?
				if (mFieldData.mSwaySync)
				{
					pFoliageItem->SwayPhase = 0;
					pFoliageItem->SwayTimeRatio = mGlobalSwayTimeRatio;
				}
				else
				{
					// No, so choose a random Sway phase.
					pFoliageItem->SwayPhase = RandomGen.randF(0, PeriodLenMinus);
					// Set to random Sway Time.
					pFoliageItem->SwayTimeRatio = PeriodLenMinus / RandomGen.randF(mFieldData.mMinSwayTime, mFieldData.mMaxSwayTime);
				}
			}
		}
	}

	// Update our Object Volume.
	mObjBox.min.set(MinPoint);
	mObjBox.max.set(MaxPoint);
	setTransform(mObjToWorld);

	// ----------------------------------------------------------------------------------------------------------------------
	// Step 3.
	// ----------------------------------------------------------------------------------------------------------------------

	// Reset Next Allocated Node to Stack base.
	mNextAllocatedNodeIdx = 0;

	// Allocate a new Node.
	fxFoliageQuadrantNode* pNewNode = new fxFoliageQuadrantNode;

	// Store it in the Quad-tree.
	mFoliageQuadTree.push_back(pNewNode);

	// Populate Initial Node.
	//
	// Set Start Level.
	pNewNode->Level = mQuadTreeLevels;
	// Calculate Total Foliage Area.
	pNewNode->QuadrantBox = getWorldBox();
	// Reset Quadrant child nodes.
	pNewNode->QuadrantChildNode[0] =
	pNewNode->QuadrantChildNode[1] =
	pNewNode->QuadrantChildNode[2] =
	pNewNode->QuadrantChildNode[3] = NULL;

	// Create our initial cull list with *all* billboards into.
	fxFoliageCulledList CullList;
	CullList.mCulledObjectSet = mReplicatedFoliage;

	// Move to next node Index.
	mNextAllocatedNodeIdx++;

	// Let's start this thing going by recursing it's children.
	ProcessNodeChildren(pNewNode, &CullList);

	// Calculate Elapsed Time and take new Timestamp.
	F32 ElapsedTime = (Platform::getRealMilliseconds() - mStartCreationTime) * 0.001f;

	// Console Output.
	Con::printf("fxFoliageReplicator - Lev: %d  PotNodes: %d  Used: %d  Objs: %d  Time: %0.4fs.",
				mQuadTreeLevels,
				mPotentialFoliageNodes,
				mNextAllocatedNodeIdx-1,
				mBillboardsAcquired,
				ElapsedTime);

	// Dump (*very*) approximate allocated memory.
	F32 MemoryAllocated = (F32) ((mNextAllocatedNodeIdx-1) * sizeof(fxFoliageQuadrantNode));
	MemoryAllocated		+=	mCurrentFoliageCount * sizeof(fxFoliageItem);
	MemoryAllocated		+=	mCurrentFoliageCount * sizeof(fxFoliageItem*);
	Con::printf("fxFoliageReplicator - Approx. %0.2fMb allocated.", MemoryAllocated / 1048576.0f);

	// ----------------------------------------------------------------------------------------------------------------------

	SetupBuffers();

	// Take first Timestamp.
	mLastRenderTime = Platform::getVirtualMilliseconds();
}

// Ok, what we do is let the older code setup the FoliageItem list and the QuadTree.
// Then we build the Vertex and Primitive buffers here.  It would probably be
// slightly more memory efficient to build the buffers directly, but we
// want to sort the items within the buffer by the quadtreenodes
void fxFoliageReplicator::SetupBuffers()
{
	// Following two arrays are used to build the vertex and primitive buffers.	
	Point3F basePoints[8];
	basePoints[0] = Point3F(-0.5f, 0.0f, 1.0f);
	basePoints[1] = Point3F(-0.5f, 0.0f, 0.0f);
	basePoints[2] = Point3F(0.5f, 0.0f, 0.0f);
	basePoints[3] = Point3F(0.5f, 0.0f, 1.0f);

	Point2F texCoords[4];
	texCoords[0] = Point2F(0.0, 0.0);
	texCoords[1] = Point2F(0.0, 1.0);
	texCoords[2] = Point2F(1.0, 1.0);
	texCoords[3] = Point2F(1.0, 0.0);	
	
	// Init our Primitive Buffer
	U32 indexSize = mFieldData.mFoliageCount * 6;
	U16* indices = new U16[indexSize];
	// Two triangles per particle
	for (U16 i = 0; i < mFieldData.mFoliageCount; i++) {
		U16* idx = &indices[i*6];		// hey, no offset math below, neat
		U16 vertOffset = i*4;
		idx[0] = vertOffset + 0;
		idx[1] = vertOffset + 1;
		idx[2] = vertOffset + 2;
		idx[3] = vertOffset + 2;
		idx[4] = vertOffset + 3;
		idx[5] = vertOffset + 0;
	}
	// Init the prim buffer and copy our indexes over
	U16 *ibIndices;
	mPrimBuffer.set(GFX, indexSize, 0, GFXBufferTypeStatic);
	mPrimBuffer.lock(&ibIndices);
	dMemcpy(ibIndices, indices, indexSize * sizeof(U16));
	mPrimBuffer.unlock();
	delete[] indices;

	S32 TextureSplinterNum = 0;

	if ((mWidth > 0) && (mHeight > 0))
		TextureSplinterNum = mWidth* mHeight;

	// Now, let's init the vertex buffer
	//for(S32 i = 0; i < mSortFoliageItem.size(); i++)
	{
		//Vector<fxFoliageItem*> j = mSortFoliageItem[i];
		for (	Vector<fxFoliageItem*>::iterator QuadNodeItr = mSortFoliageItem.begin();
			QuadNodeItr != mSortFoliageItem.end();
			QuadNodeItr++ )
		{
			delete *QuadNodeItr;
		}
		//j.clear();
	}

	mSortFoliageItem.clear();

	U32 currPrimitiveStartIndex = 0;
	mVertexBuffer.set(GFX, mFieldData.mFoliageCount * 4, GFXBufferTypeStatic);
	mVertexBuffer.lock();
	U32 idx = 0;	
	for (S32 qtIdx = 0; qtIdx < mFoliageQuadTree.size(); qtIdx++) {
		fxFoliageQuadrantNode* quadNode = mFoliageQuadTree[qtIdx];
		if (quadNode->Level == 0) {
			quadNode->startIndex = currPrimitiveStartIndex;
			quadNode->primitiveCount = 0;
			//Vector<fxFoliageItem*> temp_vec;
			// Ok, there should be data in here!
			for (S32 i = 0; i < quadNode->RenderList.size(); i++) {
				fxFoliageItem* pFoliageItem = quadNode->RenderList[i];
				if (pFoliageItem->LastFrameSerialID == 0) {
					pFoliageItem->LastFrameSerialID++;
                    fxFoliageItem* temp_ptr = new fxFoliageItem;
					memcpy(temp_ptr, pFoliageItem, sizeof(fxFoliageItem));
					mSortFoliageItem.push_back(temp_ptr);
					for (U32 vertIndex = 0; vertIndex < 4; vertIndex++) {
						GFXVertexFoliage *vert = &mVertexBuffer[(idx*4) + vertIndex];
						// This is the position of the billboard.
						vert->point = pFoliageItem->Transform.getPosition();			
						// Normal contains the point of the billboard (except for the y component, see below)
						vert->normal = basePoints[vertIndex];

						vert->normal.x *= pFoliageItem->Width;
						vert->normal.z *= pFoliageItem->Height;
						// Handle texture coordinates
						vert->texCoord = texCoords[vertIndex];				
						if (pFoliageItem->Flipped)
							vert->texCoord.x = 1.0f - vert->texCoord.x;
						// Handle sway. Sway is stored in a texture coord. The x coordinate is the sway phase multiplier, 
						// the y coordinate determines if this vertex actually sways or not.
						if ((vertIndex == 0) || (vertIndex == 3)) {
							vert->texCoord2.set(pFoliageItem->SwayTimeRatio / mGlobalSwayTimeRatio, 1.0f);
						} else {
							vert->texCoord2.set(0.0f, 0.0f);
						}
						// Handle lighting, lighting happens at the same time as global so this is just an offset.
						vert->normal.y = pFoliageItem->LightPhase;

                        Point3F temppoint3f; 

						if(TextureSplinterNum)
						{
							if(vertIndex == 0)
							{
								vert->texCoord5 = mTextureCoordUV1[pFoliageItem->randnum];
								temppoint3f = Point3F(mTextureCoordUV1[pFoliageItem->randnum1].x , mTextureCoordUV1[pFoliageItem->randnum1].y ,pFoliageItem->randnum2);
							}
							if(vertIndex == 1)
							{
								vert->texCoord5 = mTextureCoordUV3[pFoliageItem->randnum];
								temppoint3f = Point3F(mTextureCoordUV3[pFoliageItem->randnum1].x , mTextureCoordUV3[pFoliageItem->randnum1].y ,pFoliageItem->randnum2);
							}
							if(vertIndex == 2)
							{
								vert->texCoord5 = mTextureCoordUV4[pFoliageItem->randnum];
								temppoint3f = Point3F(mTextureCoordUV4[pFoliageItem->randnum1].x , mTextureCoordUV4[pFoliageItem->randnum1].y ,pFoliageItem->randnum2);
							}
							if(vertIndex == 3)
							{
								vert->texCoord5 = mTextureCoordUV2[pFoliageItem->randnum];
								temppoint3f = Point3F(mTextureCoordUV2[pFoliageItem->randnum1].x , mTextureCoordUV2[pFoliageItem->randnum1].y ,pFoliageItem->randnum2);
							}
							vert->texCoord4 = temppoint3f;
							vert->texCoord3.x = mTextureCoordUV4[pFoliageItem->randnum].y;
							vert->texCoord3.y = mTextureCoordUV4[pFoliageItem->randnum1].y;
						}
					}
					idx++;
					quadNode->primitiveCount += 2;
					currPrimitiveStartIndex += 6; 
				}
			}
			//mSortFoliageItem.push_back(temp_vec);
		}
	}
	mVertexBuffer.unlock();	

	DestroyFoliageItems();
}

//------------------------------------------------------------------------------

Box3F fxFoliageReplicator::FetchQuadrant(Box3F Box, U32 Quadrant)
{
	Box3F QuadrantBox;

	// Select Quadrant.
	switch(Quadrant)
	{
		// UL.
		case 0:
			QuadrantBox.min = Box.min + Point3F(0, Box.len_y()/2, 0);
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		// UR.
		case 1:
			QuadrantBox.min = Box.min + Point3F(Box.len_x()/2, Box.len_y()/2, 0);
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		// LL.
		case 2:
			QuadrantBox.min = Box.min;
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		// LR.
		case 3:
			QuadrantBox.min = Box.min + Point3F(Box.len_x()/2, 0, 0);
			QuadrantBox.max = QuadrantBox.min + Point3F(Box.len_x()/2, Box.len_y()/2, Box.len_z());
			break;

		default:
			return Box;
	}

	return QuadrantBox;
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::ProcessNodeChildren(fxFoliageQuadrantNode* pParentNode, fxFoliageCulledList* pCullList)
{
	// ---------------------------------------------------------------
	// Split Node into Quadrants and Process each.
	// ---------------------------------------------------------------

	// Process All Quadrants (UL/UR/LL/LR).
	for (U32 q = 0; q < 4; q++)
		ProcessQuadrant(pParentNode, pCullList, q);
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::ProcessQuadrant(fxFoliageQuadrantNode* pParentNode, fxFoliageCulledList* pCullList, U32 Quadrant)
{
	// Fetch Quadrant Box.
	const Box3F QuadrantBox = FetchQuadrant(pParentNode->QuadrantBox, Quadrant);

	// Create our new Cull List.
	fxFoliageCulledList CullList(QuadrantBox, pCullList);

	// Did we get any objects?
	if (CullList.GetListCount() > 0)
	{
		// Yes, so allocate a new Node.
		fxFoliageQuadrantNode* pNewNode = new fxFoliageQuadrantNode;

		// Store it in the Quad-tree.
		mFoliageQuadTree.push_back(pNewNode);

		// Move to next node Index.
		mNextAllocatedNodeIdx++;

		// Populate Quadrant Node.
		//
		// Next Sub-level.
		pNewNode->Level = pParentNode->Level - 1;
		// Calculate Quadrant Box.
		pNewNode->QuadrantBox = QuadrantBox;
		// Reset Child Nodes.
		pNewNode->QuadrantChildNode[0] =
		pNewNode->QuadrantChildNode[1] =
		pNewNode->QuadrantChildNode[2] =
		pNewNode->QuadrantChildNode[3] = NULL;

		// Put a reference in parent.
		pParentNode->QuadrantChildNode[Quadrant] = pNewNode;

		// If we're not at sub-level 0 then process this nodes children.
		if (pNewNode->Level != 0) ProcessNodeChildren(pNewNode, &CullList);
		// If we've reached sub-level 0 then store Cull List (for rendering).
		if (pNewNode->Level == 0)
		{
			// Store the render list from our culled object set.
			pNewNode->RenderList = CullList.mCulledObjectSet;
			// Keep track of the total billboard acquired.
			mBillboardsAcquired += CullList.GetListCount();
		}
	}
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::SyncFoliageReplicators(void)
{
	// Check Host.
	AssertFatal(isServerObject(), "We *MUST* be on server when Synchronising Foliage!")

	// Find the Replicator Set.
	SimSet *fxFoliageSet = dynamic_cast<SimSet*>(Sim::findObject("fxFoliageSet"));

	// Return if Error.
	if (!fxFoliageSet)
	{
		// Console Warning.
		Con::warnf("fxFoliageReplicator - Cannot locate the 'fxFoliageSet', this is bad!");
		// Return here.
		return;
	}

	// Parse Replication Object(s).
	for (SimSetIterator itr(fxFoliageSet); *itr; ++itr)
	{
		// Fetch the Replicator Object.
		fxFoliageReplicator* Replicator = static_cast<fxFoliageReplicator*>(*itr);
		// Set Foliage Replication Mask.
		if (Replicator->isServerObject())
		{
			Con::printf("fxFoliageReplicator - Restarting fxFoliageReplicator Object...");
			Replicator->setMaskBits(FoliageReplicationMask);
		}
	}

	// Info ...
	Con::printf("fxFoliageReplicator - Client Foliage Sync has completed.");
}


//------------------------------------------------------------------------------
// Lets chill our memory requirements out a little
void fxFoliageReplicator::DestroyFoliageItems()
{
	// Remove shapes.
	for (S32 idx = 0; idx < mReplicatedFoliage.size(); idx++)
	{
		fxFoliageItem*	pFoliageItem;

		// Fetch the Foliage Item.
		pFoliageItem = mReplicatedFoliage[idx];

		// Delete Shape.
		if (pFoliageItem) delete pFoliageItem;
	}
	// Clear the Replicated Foliage Vector.
	mReplicatedFoliage.clear();

	// Clear out old references also
	for (S32 qtIdx = 0; qtIdx < mFoliageQuadTree.size(); qtIdx++) {
		fxFoliageQuadrantNode* quadNode = mFoliageQuadTree[qtIdx];
		if (quadNode->Level == 0) {
			quadNode->RenderList.clear();
		}
	}
}

void fxFoliageReplicator::DestroyFoliage(void)
{
	// Check Host.
	AssertFatal(isClientObject(), "Trying to destroy Foliage on Server, this is bad!")

	// Destroy Quad-tree.
	mPotentialFoliageNodes = 0;
	// Reset Billboards Acquired.
	mBillboardsAcquired = 0;

	// Finish if we didn't create any shapes.
	if (mCurrentFoliageCount == 0) return;

	DestroyFoliageItems();

	// Let's remove the Quad-Tree allocations.
	for (	Vector<fxFoliageQuadrantNode*>::iterator QuadNodeItr = mFoliageQuadTree.begin();
			QuadNodeItr != mFoliageQuadTree.end();
			QuadNodeItr++ )
		{
			// Remove the node.
			delete *QuadNodeItr;
		}

	// Clear the Foliage Quad-Tree Vector.
	mFoliageQuadTree.clear();

	// Clear the Frustum Render Set Vector.
	mFrustumRenderSet.mVisObjectSet.clear();

	// Reset Foliage Count.
	mCurrentFoliageCount = 0;
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::StartUp(void)
{
	// Flag, Client Replication Started.
	mClientReplicationStarted = true;

	// Create foliage on Client.
	if (isClientObject()) CreateFoliage();
}

//------------------------------------------------------------------------------

bool fxFoliageReplicator::onAdd()
{
	if(!Parent::onAdd()) return(false);

	// Add the Replicator to the Replicator Set.
	dynamic_cast<SimSet*>(Sim::findObject("fxFoliageSet"))->addObject(this);

	// Set Default Object Box.
	mObjBox.min.set( -0.5, -0.5, -0.5 );
	mObjBox.max.set(  0.5,  0.5,  0.5 );
	// Reset the World Box.
	resetWorldBox();
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Add to Scene.
	addToScene();
	mAddedToScene = true;

	// Are we on the client?
    if ( isClientObject() )
	{
		// Yes, so load foliage texture.
		mFieldData.mFoliageTexture = GFXTexHandle( mFieldData.mFoliageFile, &GFXDefaultStaticDiffuseProfile );
		if ((GFXTextureObject*) mFieldData.mFoliageTexture == NULL)
			Con::printf("fxFoliageReplicator:  %s is an invalid or missing foliage texture file.", mFieldData.mFoliageFile);
		mAlphaLookup = new GBitmap(AlphaTexLen, 1);
		computeAlphaTex();

		// If we are in the editor then we can manually startup replication.
		if (gEditingMission) 
			mClientReplicationStarted = true;

		// Let's init the shader too! This is managed by the GFX->Shader manager, so I don't
		// need to free it myself. 
		mShader = GFX->createShader("gameres/shaders/fxFoliageReplicatorV.hlsl", "gameres/shaders/fxFoliageReplicatorP.hlsl", (F32) 1.4f);
	}
    
	// Return OK.
	return(true);
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::onRemove()
{
	// Remove the Replicator from the Replicator Set.
	dynamic_cast<SimSet*>(Sim::findObject("fxFoliageSet"))->removeObject(this);

	// Remove from Scene.
	removeFromScene();
	mAddedToScene = false;

	// Are we on the Client?
	if (isClientObject())
	{
		// Yes, so destroy Foliage.
		DestroyFoliage();

		// Remove Texture.
		mFieldData.mFoliageTexture = NULL;
	}

	// Do Parent.
	Parent::onRemove();
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::inspectPostApply()
{
	// Set Parent.
	Parent::inspectPostApply();

	// Set Foliage Replication Mask (this object only).
	setMaskBits(FoliageReplicationMask);
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::onEditorEnable()
{
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::onEditorDisable()
{
}

//------------------------------------------------------------------------------

ConsoleFunction(StartFoliageReplication, void, 1, 1, "StartFoliageReplication()")
{
	argv; argc; 
	// Find the Replicator Set.
	SimSet *fxFoliageSet = dynamic_cast<SimSet*>(Sim::findObject("fxFoliageSet"));

	// Return if Error.
	if (!fxFoliageSet)
	{
		// Console Warning.
		Con::warnf("fxFoliageReplicator - Cannot locate the 'fxFoliageSet', this is bad!");
		// Return here.
		return;
	}

	// Parse Replication Object(s).
   U32 startupCount = 0;
	for (SimSetIterator itr(fxFoliageSet); *itr; ++itr)
	{
		// Fetch the Replicator Object.
		fxFoliageReplicator* Replicator = static_cast<fxFoliageReplicator*>(*itr);
		
      // Start Client Objects Only.
		if (Replicator->isClientObject())
      {
         Replicator->StartUp();
         startupCount++;
      }
	}

	// Info ...
	Con::printf("fxFoliageReplicator - replicated client foliage for %d objects", startupCount);
}

//------------------------------------------------------------------------------

bool fxFoliageReplicator::prepRenderImage(SceneState* state, const U32 stateKey, const U32 /*startZone*/,
								const bool /*modifyBaseZoneState*/)
{
	// Return if last state.
	if (isLastState(state, stateKey)) return false;
	// Set Last State.
	setLastState(state, stateKey);

   // Is Object Rendered?
   if (state->isObjectRendered(this))
   {
      RenderInst *ri = gRenderInstManager.allocInst();
      ri->obj = this;
      ri->state = state;
      ri->type = RenderInstManager::RIT_Foliage;
      gRenderInstManager.addInst( ri );
   }

   return false;
}

//
// RENDERING
//
void fxFoliageReplicator::computeAlphaTex()
{
	// Distances used in alpha
	const F32	ClippedViewDistance		= mFieldData.mViewDistance;
	const F32	MaximumViewDistance		= ClippedViewDistance + mFieldData.mFadeInRegion;

	// This is used for the alpha computation in the shader.
	for (U32 i = 0; i < AlphaTexLen; i++) {
		F32 Distance = ((float) i / (float) AlphaTexLen) * MaximumViewDistance;
		F32 ItemAlpha = 1.0f;
		// Are we fading out?
		if (Distance < mFieldData.mViewClosest)
		{
			// Yes, so set fade-out.
			ItemAlpha = 1.0f - ((mFieldData.mViewClosest - Distance) * mFadeOutGradient);
		}
		// No, so are we fading in?
		else if (Distance > ClippedViewDistance)
		{
			// Yes, so set fade-in
			ItemAlpha = 1.0f - ((Distance - ClippedViewDistance) * mFadeInGradient);
		}

		// Clamp upper-limit to Fog Alpha.  TODO
//		F32 FogAlpha = 1.0f - state->getHazeAndFog(Distance, 
//			pFoliageItem->Transform.getPosition().z - state->getCameraPosition().z);
//		if (ItemAlpha > FogAlpha) 
//			ItemAlpha = FogAlpha;

		// Set texture info
		ColorI c((U8) (255.0f * ItemAlpha), 0, 0);
		mAlphaLookup->setColor(i, 0, c);
	}
	mAlphaTexture.set(mAlphaLookup, &GFXDefaultStaticDiffuseProfile, false);
}

// Renders a triangle stripped oval
void fxFoliageReplicator::renderArc(const F32 fRadiusX, const F32 fRadiusY)
{
	PrimBuild::begin(GFXTriangleStrip, 720);
	for (U32 Angle = mCreationAreaAngle; Angle < (mCreationAreaAngle+360); Angle++)
	{
		F32		XPos, YPos;

		// Calculate Position.
		XPos = fRadiusX * mCos(mDegToRad(-(F32)Angle));
		YPos = fRadiusY * mSin(mDegToRad(-(F32)Angle));

		// Set Colour.
		PrimBuild::color4f(mFieldData.mPlaceAreaColour.red,
					mFieldData.mPlaceAreaColour.green,
					mFieldData.mPlaceAreaColour.blue,
					AREA_ANIMATION_ARC * (Angle-mCreationAreaAngle));

		PrimBuild::vertex3f(XPos, YPos, -(F32)mFieldData.mPlacementBandHeight/2.0f);
		PrimBuild::vertex3f(XPos, YPos, +(F32)mFieldData.mPlacementBandHeight/2.0f);
	}			
	PrimBuild::end();
}

// This currently uses the primbuilder, could convert out, but why allocate the buffer if we
// never edit the misison?
void fxFoliageReplicator::renderPlacementArea(const F32 ElapsedTime)
{
	if (gEditingMission && mFieldData.mShowPlacementArea)
	{
		GFX->pushWorldMatrix();
		GFX->multWorld(getTransform());
#ifdef STATEBLOCK
		AssertFatal(mAreaSB, "fxFoliageReplicator::renderPlacementArea -- mAreaSB cannot be NULL.");
		mAreaSB->apply();  
#else
		GFX->setTextureStageColorOp(0, GFXTOPDisable);
		GFX->setTextureStageColorOp(1, GFXTOPDisable);
		GFX->setAlphaBlendEnable( true );
		GFX->setAlphaTestEnable( true );
		GFX->setSrcBlend(GFXBlendSrcAlpha);
		GFX->setDestBlend(GFXBlendInvSrcAlpha);
#endif

		// Do we need to draw the Outer Radius?
		if (mFieldData.mOuterRadiusX || mFieldData.mOuterRadiusY)
			renderArc((F32) mFieldData.mOuterRadiusX, (F32) mFieldData.mOuterRadiusY);
		// Inner radius?
		if (mFieldData.mInnerRadiusX || mFieldData.mInnerRadiusY)
			renderArc((F32) mFieldData.mInnerRadiusX, (F32) mFieldData.mInnerRadiusY);

		GFX->popWorldMatrix();
		mCreationAreaAngle = (U32)(mCreationAreaAngle + (1000 * ElapsedTime));
		mCreationAreaAngle = mCreationAreaAngle % 360;
	}
}

#ifdef STATEBLOCK  
void fxFoliageReplicator::renderObject(SceneState* state, RenderInst *)
{
	// If we're rendering and we haven't placed any foliage yet - do it.
	if(!mClientReplicationStarted)
	{
		Con::warnf("fxFoliageReplicator::renderObject - tried to render a non replicated fxFoliageReplicator; replicating it now...");

		StartUp();
	}

	// Calculate Elapsed Time and take new Timestamp.
	S32 Time = Platform::getVirtualMilliseconds();
	F32 ElapsedTime = (Time - mLastRenderTime) * 0.001f;
	mLastRenderTime = Time;	

	renderPlacementArea(ElapsedTime);

	if (mCurrentFoliageCount > 0) {

		if (!mFieldData.mHideFoliage) {
			// Animate Global Sway Phase (Modulus).  
			mGlobalSwayPhase = mGlobalSwayPhase + (mGlobalSwayTimeRatio * ElapsedTime);

			// Animate Global Light Phase (Modulus).
			mGlobalLightPhase = mGlobalLightPhase + (mGlobalLightTimeRatio * ElapsedTime);

			// Compute other light parameters
			const F32	LuminanceMidPoint		= (mFieldData.mMinLuminance + mFieldData.mMaxLuminance) / 2.0f;
			const F32	LuminanceMagnitude		= mFieldData.mMaxLuminance - LuminanceMidPoint;

			// Distances used in alpha
			const F32	ClippedViewDistance		= mFieldData.mViewDistance;
			const F32	MaximumViewDistance		= ClippedViewDistance + mFieldData.mFadeInRegion;

			// Set up our shader constants	
			// Projection matrix
			MatrixF proj = GFX->getProjectionMatrix();
			proj.transpose();
			GFX->setVertexShaderConstF(0, (float*)&proj, 4);
			// World transform matrix
			MatrixF world = GFX->getWorldMatrix();
			world.transpose();
			GFX->setVertexShaderConstF(4, (float*)&world, 4);
			// Current Global Sway Phase
			GFX->setVertexShaderConstF(8, (float*) &mGlobalSwayPhase, 1, 1);
			// Sway magntitudes
			GFX->setVertexShaderConstF(9, (float*) &mFieldData.mSwayMagnitudeSide, 1, 1);
			GFX->setVertexShaderConstF(10, (float*) &mFieldData.mSwayMagnitudeFront, 1, 1);
			// Light params
			GFX->setVertexShaderConstF(11, (float*) &mGlobalLightPhase, 1, 1);
			GFX->setVertexShaderConstF(12, (float*) &LuminanceMagnitude, 1, 1);
			GFX->setVertexShaderConstF(13, (float*) &LuminanceMidPoint, 1, 1);	
			// Alpha params
			Point3F camPos = state->getCameraPosition();
			GFX->setVertexShaderConstF(14, (float*) &MaximumViewDistance, 1, 1);
			GFX->setVertexShaderConstF(15, (float*) &camPos, 1, 3);
			// Pixel shader constants, this one is for ground alpha
			GFX->setPixelShaderConstF(1, (float*) &mFieldData.mGroundAlpha, 1, 1);			

			// Blend ops
			//GFX->setAlphaRef((U8) (255.0f * mFieldData.mAlphaCutoff));
			GFX->setRenderState(GFXRSAlphaRef, (U8) (255.0f * mFieldData.mAlphaCutoff));
			AssertFatal(mSetSB, "fxFoliageReplicator::renderObject -- mSetSB cannot be NULL.");
			mSetSB->apply();
			// Set up our texture and color ops.
			mShader->process();
			GFX->setTexture(0, mFieldData.mFoliageTexture);
			// computeAlphaTex();		// Uncomment if we figure out how to clamp to fogAndHaze
			GFX->setTexture(1, mAlphaTexture);

			// Setup our buffers
			GFX->setVertexBuffer(mVertexBuffer);
			GFX->setPrimitiveBuffer(mPrimBuffer);

			// If we use culling, we're going to send chunks of our buffers to the card
			if (mFieldData.mUseCulling)
			{
				// Setup the Clip-Planes.
				F32 FarClipPlane = getMin((F32)state->getFarPlane(), 
					mFieldData.mViewDistance + mFieldData.mFadeInRegion);
				mFrustumRenderSet.SetupClipPlanes(state, FarClipPlane);

				renderQuad(mFoliageQuadTree[0], getRenderTransform(), false);

				// Multipass, don't want to interrupt the vb state 
				if (mFieldData.mUseDebugInfo) 
				{
					// hey man, we're done, so it doesn't matter if we kill it to render the next part
					AssertFatal(mDebugSetSB, "fxFoliageReplicator::renderObject -- mDebugSetSB cannot be NULL.");
					mDebugSetSB->apply();
					GFX->disableShaders(); 
					renderQuad(mFoliageQuadTree[0], getRenderTransform(), true);
				}
			}
			else 
			{	
				// Draw the whole shebang!
				GFX->drawIndexedPrimitive(GFXTriangleList, 0, mVertexBuffer->mNumVerts, 
					0, mPrimBuffer->mIndexCount / 3);
			}

			// Reset some states 
			AssertFatal(mClearSB, "fxFoliageReplicator::renderObject -- mClearSB cannot be NULL.");
			mClearSB->apply();
			GFX->disableShaders();		// this fixes editor issue.
		}
	}
}

#else
void fxFoliageReplicator::renderObject(SceneState* state, RenderInst *)
{
   // If we're rendering and we haven't placed any foliage yet - do it.
   if(!mClientReplicationStarted)
   {
      Con::warnf("fxFoliageReplicator::renderObject - tried to render a non replicated fxFoliageReplicator; replicating it now...");

      StartUp();
   }

	// Calculate Elapsed Time and take new Timestamp.
	S32 Time = Platform::getVirtualMilliseconds();
	F32 ElapsedTime = (Time - mLastRenderTime) * 0.001f;
	mLastRenderTime = Time;	

	renderPlacementArea(ElapsedTime);

	if (mCurrentFoliageCount > 0) {

		if (!mFieldData.mHideFoliage) {
			// Animate Global Sway Phase (Modulus).  
			mGlobalSwayPhase = mGlobalSwayPhase + (mGlobalSwayTimeRatio * ElapsedTime);
				
			// Animate Global Light Phase (Modulus).
			mGlobalLightPhase = mGlobalLightPhase + (mGlobalLightTimeRatio * ElapsedTime);

			// Compute other light parameters
			const F32	LuminanceMidPoint		= (mFieldData.mMinLuminance + mFieldData.mMaxLuminance) / 2.0f;
			const F32	LuminanceMagnitude		= mFieldData.mMaxLuminance - LuminanceMidPoint;

			// Distances used in alpha
			const F32	ClippedViewDistance		= mFieldData.mViewDistance;
			const F32	MaximumViewDistance		= ClippedViewDistance + mFieldData.mFadeInRegion;

			// Set up our shader constants	
			// Projection matrix
			MatrixF proj = GFX->getProjectionMatrix();
			proj.transpose();
			GFX->setVertexShaderConstF(0, (float*)&proj, 4);
			// World transform matrix
			MatrixF world = GFX->getWorldMatrix();
			world.transpose();
			GFX->setVertexShaderConstF(4, (float*)&world, 4);
			// Current Global Sway Phase
			GFX->setVertexShaderConstF(8, (float*) &mGlobalSwayPhase, 1, 1);
			// Sway magntitudes
			GFX->setVertexShaderConstF(9, (float*) &mFieldData.mSwayMagnitudeSide, 1, 1);
			GFX->setVertexShaderConstF(10, (float*) &mFieldData.mSwayMagnitudeFront, 1, 1);
			// Light params
			GFX->setVertexShaderConstF(11, (float*) &mGlobalLightPhase, 1, 1);
			GFX->setVertexShaderConstF(12, (float*) &LuminanceMagnitude, 1, 1);
			GFX->setVertexShaderConstF(13, (float*) &LuminanceMidPoint, 1, 1);	
			// Alpha params
			Point3F camPos = state->getCameraPosition();
			GFX->setVertexShaderConstF(14, (float*) &MaximumViewDistance, 1, 1);
			GFX->setVertexShaderConstF(15, (float*) &camPos, 1, 3);
			// Pixel shader constants, this one is for ground alpha
			GFX->setPixelShaderConstF(1, (float*) &mFieldData.mGroundAlpha, 1, 1);			

			// Blend ops
			GFX->setAlphaBlendEnable( true );
			GFX->setAlphaTestEnable( true );
			GFX->setSrcBlend(GFXBlendSrcAlpha);
			GFX->setDestBlend(GFXBlendInvSrcAlpha);
			GFX->setAlphaFunc(GFXCmpGreater);
			GFX->setAlphaRef((U8) (255.0f * mFieldData.mAlphaCutoff));
			GFX->setCullMode(GFXCullNone);

			// Set up our texture and color ops.
			mShader->process();
			GFX->setTexture(0, mFieldData.mFoliageTexture);
			// computeAlphaTex();		// Uncomment if we figure out how to clamp to fogAndHaze
			GFX->setTexture(1, mAlphaTexture);
			GFX->setTextureStageColorOp(0, GFXTOPModulate);
			GFX->setTextureStageColorOp(1, GFXTOPModulate);  // am I needed? 
			GFX->setTextureStageAddressModeU(1, GFXAddressClamp);
			GFX->setTextureStageAddressModeV(1, GFXAddressClamp);

			// Setup our buffers
			GFX->setVertexBuffer(mVertexBuffer);
			GFX->setPrimitiveBuffer(mPrimBuffer);

			// If we use culling, we're going to send chunks of our buffers to the card
			if (mFieldData.mUseCulling)
			{
				// Setup the Clip-Planes.
				F32 FarClipPlane = getMin((F32)state->getFarPlane(), 
                           mFieldData.mViewDistance + mFieldData.mFadeInRegion);
				mFrustumRenderSet.SetupClipPlanes(state, FarClipPlane);

				renderQuad(mFoliageQuadTree[0], getRenderTransform(), false);

				// Multipass, don't want to interrupt the vb state 
				if (mFieldData.mUseDebugInfo) 
            {
					// hey man, we're done, so it doesn't matter if we kill it to render the next part
					GFX->setTextureStageColorOp(0, GFXTOPDisable);
					GFX->setTextureStageColorOp(1, GFXTOPDisable);
					GFX->disableShaders(); 
					renderQuad(mFoliageQuadTree[0], getRenderTransform(), true);
				}
			}
         else 
         {	
				// Draw the whole shebang!
				GFX->drawIndexedPrimitive(GFXTriangleList, 0, mVertexBuffer->mNumVerts, 
                                       0, mPrimBuffer->mIndexCount / 3);
			}

			// Reset some states 
			GFX->setAlphaBlendEnable( false );
			GFX->setAlphaTestEnable( false );
			GFX->setTextureStageColorOp(0, GFXTOPDisable);
			GFX->setTextureStageColorOp(1, GFXTOPDisable);  
			GFX->disableShaders();		// this fixes editor issue.
		}
	}
}

#endif


void fxFoliageReplicator::renderQuad(fxFoliageQuadrantNode* quadNode, const MatrixF& RenderTransform, const bool UseDebug)
{
	if (quadNode != NULL) {
		if (mFrustumRenderSet.IsQuadrantVisible(quadNode->QuadrantBox, RenderTransform))
		{
			// Draw the Quad Box (Debug Only).
			if (UseDebug) 
				mFrustumRenderSet.DrawQuadBox(quadNode->QuadrantBox, ColorF(0.0f, 1.0f, 0.1f, 1.0f));
			if (quadNode->Level != 0) {
				for (U32 i = 0; i < 4; i++)
					renderQuad(quadNode->QuadrantChildNode[i], RenderTransform, UseDebug);
			} else {
				if (!UseDebug)
               if(quadNode->primitiveCount)
					   GFX->drawIndexedPrimitive(GFXTriangleList, 0, mVertexBuffer->mNumVerts, 
						         quadNode->startIndex, quadNode->primitiveCount);
			}
		} else {
			// Use a different color to say "I think I'm not visible!"
			if (UseDebug) 
				mFrustumRenderSet.DrawQuadBox(quadNode->QuadrantBox, ColorF(1.0f, 0.8f, 0.1f, 1.0f));
		}
	}
}

//------------------------------------------------------------------------------
// NETWORK
//------------------------------------------------------------------------------

U64 fxFoliageReplicator::packUpdate(NetConnection * con, U64 mask, BitStream * stream)
{
	// Pack Parent.
	U64 retMask = Parent::packUpdate(con, mask, stream);

	// Write Foliage Replication Flag.
	if (stream->writeFlag(mask & FoliageReplicationMask))
	{
		stream->writeAffineTransform(mObjToWorld);						// Foliage Master-Object Position.

		stream->writeFlag(mFieldData.mUseDebugInfo);					// Foliage Debug Information Flag.
		stream->write(mFieldData.mDebugBoxHeight);						// Foliage Debug Height.
		stream->write(mFieldData.mSeed);								// Foliage Seed.
		stream->write(mFieldData.mFoliageCount);						// Foliage Count.
		stream->write(mFieldData.mFoliageRetries);						// Foliage Retries.
		stream->writeString(mFieldData.mFoliageFile);					// Foliage File.

		stream->write(mFieldData.mInnerRadiusX);						// Foliage Inner Radius X.
		stream->write(mFieldData.mInnerRadiusY);						// Foliage Inner Radius Y.
		stream->write(mFieldData.mOuterRadiusX);						// Foliage Outer Radius X.
		stream->write(mFieldData.mOuterRadiusY);						// Foliage Outer Radius Y.

		stream->write(mFieldData.mMinWidth);							// Foliage Minimum Width.
		stream->write(mFieldData.mMaxWidth);							// Foliage Maximum Width.
		stream->write(mFieldData.mMinHeight);							// Foliage Minimum Height.
		stream->write(mFieldData.mMaxHeight);							// Foliage Maximum Height.
		stream->write(mFieldData.mFixAspectRatio);						// Foliage Fix Aspect Ratio.
		stream->write(mFieldData.mFixSizeToMax);						// Foliage Fix Size to Max.
		stream->write(mFieldData.mOffsetZ);								// Foliage Offset Z.
		stream->write(mFieldData.mRandomFlip);							// Foliage Random Flip.

		stream->write(mFieldData.mUseCulling);							// Foliage Use Culling.
		stream->write(mFieldData.mCullResolution);						// Foliage Cull Resolution.
		stream->write(mFieldData.mViewDistance);						// Foliage View Distance.
		stream->write(mFieldData.mViewClosest);							// Foliage View Closest.
		stream->write(mFieldData.mFadeInRegion);						// Foliage Fade-In Region.
		stream->write(mFieldData.mFadeOutRegion);						// Foliage Fade-Out Region.
		stream->write(mFieldData.mAlphaCutoff);							// Foliage Alpha Cutoff.
		stream->write(mFieldData.mGroundAlpha);							// Foliage Ground Alpha.

		stream->writeFlag(mFieldData.mSwayOn);							// Foliage Sway On Flag.
		stream->writeFlag(mFieldData.mSwaySync);						// Foliage Sway Sync Flag.
		stream->write(mFieldData.mSwayMagnitudeSide);					// Foliage Sway Magnitude Side2Side.
		stream->write(mFieldData.mSwayMagnitudeFront);					// Foliage Sway Magnitude Front2Back.
		stream->write(mFieldData.mMinSwayTime);							// Foliage Minimum Sway Time.
		stream->write(mFieldData.mMaxSwayTime);							// Foliage Maximum way Time.

		stream->writeFlag(mFieldData.mLightOn);							// Foliage Light On Flag.
		stream->writeFlag(mFieldData.mLightSync);						// Foliage Light Sync
		stream->write(mFieldData.mMinLuminance);						// Foliage Minimum Luminance.
		stream->write(mFieldData.mMaxLuminance);						// Foliage Maximum Luminance.
		stream->write(mFieldData.mLightTime);							// Foliage Light Time.

		stream->writeFlag(mFieldData.mAllowOnTerrain);					// Allow on Terrain.
		stream->writeFlag(mFieldData.mAllowOnInteriors);				// Allow on Interiors.
		stream->writeFlag(mFieldData.mAllowStatics);					// Allow on Statics.
		stream->writeFlag(mFieldData.mAllowOnWater);					// Allow on Water.
		stream->writeFlag(mFieldData.mAllowWaterSurface);				// Allow on Water Surface.
		stream->write(mFieldData.mAllowedTerrainSlope);					// Foliage Offset Z.

		stream->writeFlag(mFieldData.mHideFoliage);						// Hide Foliage.
		stream->writeFlag(mFieldData.mShowPlacementArea);				// Show Placement Area Flag.
		stream->write(mFieldData.mPlacementBandHeight);					// Placement Area Height.
		stream->write(mFieldData.mPlaceAreaColour);						// Placement Area Colour.
	}

	// Were done ...
	return(retMask);
}

//------------------------------------------------------------------------------

void fxFoliageReplicator::unpackUpdate(NetConnection * con, BitStream * stream)
{
	// Unpack Parent.
	Parent::unpackUpdate(con, stream);

	// Read Replication Details.
	if(stream->readFlag())
	{
		MatrixF		ReplicatorObjectMatrix;

		stream->readAffineTransform(&ReplicatorObjectMatrix);			// Foliage Master Object Position.

		mFieldData.mUseDebugInfo = stream->readFlag();					// Foliage Debug Information Flag.
		stream->read(&mFieldData.mDebugBoxHeight);						// Foliage Debug Height.
		stream->read(&mFieldData.mSeed);								// Foliage Seed.
		stream->read(&mFieldData.mFoliageCount);						// Foliage Count.
		stream->read(&mFieldData.mFoliageRetries);						// Foliage Retries.
		mFieldData.mFoliageFile = stream->readSTString();				// Foliage File.

		stream->read(&mFieldData.mInnerRadiusX);						// Foliage Inner Radius X.
		stream->read(&mFieldData.mInnerRadiusY);						// Foliage Inner Radius Y.
		stream->read(&mFieldData.mOuterRadiusX);						// Foliage Outer Radius X.
		stream->read(&mFieldData.mOuterRadiusY);						// Foliage Outer Radius Y.

		stream->read(&mFieldData.mMinWidth);							// Foliage Minimum Width.
		stream->read(&mFieldData.mMaxWidth);							// Foliage Maximum Width.
		stream->read(&mFieldData.mMinHeight);							// Foliage Minimum Height.
		stream->read(&mFieldData.mMaxHeight);							// Foliage Maximum Height.
		stream->read(&mFieldData.mFixAspectRatio);						// Foliage Fix Aspect Ratio.
		stream->read(&mFieldData.mFixSizeToMax);						// Foliage Fix Size to Max.
		stream->read(&mFieldData.mOffsetZ);								// Foliage Offset Z.
		stream->read(&mFieldData.mRandomFlip);							// Foliage Random Flip.

		stream->read(&mFieldData.mUseCulling);							// Foliage Use Culling.
		stream->read(&mFieldData.mCullResolution);						// Foliage Cull Resolution.
		stream->read(&mFieldData.mViewDistance);						// Foliage View Distance.
		stream->read(&mFieldData.mViewClosest);							// Foliage View Closest.
		stream->read(&mFieldData.mFadeInRegion);						// Foliage Fade-In Region.
		stream->read(&mFieldData.mFadeOutRegion);						// Foliage Fade-Out Region.
		stream->read(&mFieldData.mAlphaCutoff);							// Foliage Alpha Cutoff.
		stream->read(&mFieldData.mGroundAlpha);							// Foliage Ground Alpha.

		mFieldData.mSwayOn = stream->readFlag();						// Foliage Sway On Flag.
		mFieldData.mSwaySync = stream->readFlag();						// Foliage Sway Sync Flag.
		stream->read(&mFieldData.mSwayMagnitudeSide);					// Foliage Sway Magnitude Side2Side.
		stream->read(&mFieldData.mSwayMagnitudeFront);					// Foliage Sway Magnitude Front2Back.
		stream->read(&mFieldData.mMinSwayTime);							// Foliage Minimum Sway Time.
		stream->read(&mFieldData.mMaxSwayTime);							// Foliage Maximum way Time.

		mFieldData.mLightOn = stream->readFlag();						// Foliage Light On Flag.
		mFieldData.mLightSync = stream->readFlag();						// Foliage Light Sync
		stream->read(&mFieldData.mMinLuminance);						// Foliage Minimum Luminance.
		stream->read(&mFieldData.mMaxLuminance);						// Foliage Maximum Luminance.
		stream->read(&mFieldData.mLightTime);							// Foliage Light Time.

		mFieldData.mAllowOnTerrain = stream->readFlag();				// Allow on Terrain.
		mFieldData.mAllowOnInteriors = stream->readFlag();				// Allow on Interiors.
		mFieldData.mAllowStatics = stream->readFlag();					// Allow on Statics.
		mFieldData.mAllowOnWater = stream->readFlag();					// Allow on Water.
		mFieldData.mAllowWaterSurface = stream->readFlag();				// Allow on Water Surface.
		stream->read(&mFieldData.mAllowedTerrainSlope);					// Allowed Terrain Slope.

		mFieldData.mHideFoliage = stream->readFlag();					// Hide Foliage.
		mFieldData.mShowPlacementArea = stream->readFlag();				// Show Placement Area Flag.
		stream->read(&mFieldData.mPlacementBandHeight);					// Placement Area Height.
		stream->read(&mFieldData.mPlaceAreaColour);

		// Calculate Fade-In/Out Gradients.
		mFadeInGradient		= 1.0f / mFieldData.mFadeInRegion;
		mFadeOutGradient	= 1.0f / mFieldData.mFadeOutRegion;

		// Set Transform.
		setTransform(ReplicatorObjectMatrix);

		// Load Foliage Texture on the client.
		mFieldData.mFoliageTexture = GFXTexHandle( mFieldData.mFoliageFile, &GFXDefaultStaticDiffuseProfile );
		if ((GFXTextureObject*) mFieldData.mFoliageTexture == NULL)
			Con::printf("fxFoliageReplicator:  %s is an invalid or missing foliage texture file.", mFieldData.mFoliageFile);

		// Set Quad-Tree Box Height Lerp.
		mFrustumRenderSet.mHeightLerp = mFieldData.mDebugBoxHeight;

		// Create Foliage (if Replication has begun).
		if (mClientReplicationStarted) 
			CreateFoliage();
	}
}




void fxFoliageReplicator::resetStateBlock()
{
	//mAreaSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->endStateBlock(mAreaSB);

	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, true);
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPModulate );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPModulate );
	GFX->setSamplerState(1, GFXSAMPAddressU, GFXAddressClamp);
	GFX->setSamplerState(1, GFXSAMPAddressV, GFXAddressClamp);
	GFX->setRenderState(GFXRSCullMode, GFXCullNone);
	//GFX->setRenderState(GFXRSAlphaRef, 0);//GFX->setAlphaRef((U8) (255.0f * mFieldData.mAlphaCutoff));
	GFX->endStateBlock(mSetSB);

	//mDebugSetSB
	GFX->beginStateBlock();
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mDebugSetSB);

	//mClearSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaTestEnable, false);
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->setTextureStageState( 0, GFXTSSColorOp, GFXTOPDisable );
	GFX->setTextureStageState( 1, GFXTSSColorOp, GFXTOPDisable );
	GFX->endStateBlock(mClearSB);
}


void fxFoliageReplicator::releaseStateBlock()
{
	if (mAreaSB)
	{
		mAreaSB->release();
	}

	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mDebugSetSB)
	{
		mDebugSetSB->release();
	}

	if (mClearSB)
	{
		mClearSB->release();
	}
}

void fxFoliageReplicator::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mAreaSB = new GFXD3D9StateBlock;
		mDebugSetSB = new GFXD3D9StateBlock;
		mClearSB = new GFXD3D9StateBlock;
		resetStateBlock();
	}
}

void fxFoliageReplicator::shutdown()
{
	SAFE_DELETE(mAreaSB);
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mDebugSetSB);
	SAFE_DELETE(mClearSB);
}


//addGroup( "Debugging" );	// MM: Added Group Header.
//addField( "UseDebugInfo",		TypeBool,		Offset( mFieldData.mUseDebugInfo,			fxFoliageReplicator ) );
//addField( "DebugBoxHeight",		TypeF32,		Offset( mFieldData.mDebugBoxHeight,			fxFoliageReplicator ) );
//addField( "HideFoliage",		TypeBool,		Offset( mFieldData.mHideFoliage,			fxFoliageReplicator ) );
//addField( "ShowPlacementArea",	TypeBool,		Offset( mFieldData.mShowPlacementArea,		fxFoliageReplicator ) );
//addField( "PlacementAreaHeight",TypeS32,		Offset( mFieldData.mPlacementBandHeight,	fxFoliageReplicator ) );
//addField( "PlacementColour",	TypeColorF,		Offset( mFieldData.mPlaceAreaColour,		fxFoliageReplicator ) );
//endGroup( "Debugging" );	// MM: Added Group Footer.
//
//addGroup( "Media" );	// MM: Added Group Header.
//addField( "Seed",				TypeS32,		Offset( mFieldData.mSeed,					fxFoliageReplicator ) );
//addField( "FoliageFile",		TypeFilename,	Offset( mFieldData.mFoliageFile,			fxFoliageReplicator ) );
//addField( "FoliageCount",		TypeS32,		Offset( mFieldData.mFoliageCount,			fxFoliageReplicator ) );
//addField( "FoliageRetries",		TypeS32,		Offset( mFieldData.mFoliageRetries,			fxFoliageReplicator ) );
//endGroup( "Media" );	// MM: Added Group Footer.
//
//addGroup( "Area" );	// MM: Added Group Header.
//addField( "InnerRadiusX",		TypeS32,		Offset( mFieldData.mInnerRadiusX,			fxFoliageReplicator ) );
//addField( "InnerRadiusY",		TypeS32,		Offset( mFieldData.mInnerRadiusY,			fxFoliageReplicator ) );
//addField( "OuterRadiusX",		TypeS32,		Offset( mFieldData.mOuterRadiusX,			fxFoliageReplicator ) );
//addField( "OuterRadiusY",		TypeS32,		Offset( mFieldData.mOuterRadiusY,			fxFoliageReplicator ) );
//endGroup( "Area" );	// MM: Added Group Footer.
//
//addGroup( "Dimensions" );	// MM: Added Group Header.
//addField( "MinWidth",			TypeF32,		Offset( mFieldData.mMinWidth,				fxFoliageReplicator ) );
//addField( "MaxWidth",			TypeF32,		Offset( mFieldData.mMaxWidth,				fxFoliageReplicator ) );
//addField( "MinHeight",			TypeF32,		Offset( mFieldData.mMinHeight,				fxFoliageReplicator ) );
//addField( "MaxHeight",			TypeF32,		Offset( mFieldData.mMaxHeight,				fxFoliageReplicator ) );
//addField( "FixAspectRatio",		TypeBool,		Offset( mFieldData.mFixAspectRatio,			fxFoliageReplicator ) );
//addField( "FixSizeToMax",		TypeBool,		Offset( mFieldData.mFixSizeToMax,			fxFoliageReplicator ) );
//addField( "OffsetZ",			TypeF32,		Offset( mFieldData.mOffsetZ,				fxFoliageReplicator ) );
//addField( "RandomFlip",			TypeBool,		Offset( mFieldData.mRandomFlip,				fxFoliageReplicator ) );
//endGroup( "Dimensions" );	// MM: Added Group Footer.
//
//addGroup( "Culling" );	// MM: Added Group Header.
//addField( "UseCulling",			TypeBool,		Offset( mFieldData.mUseCulling,				fxFoliageReplicator ) );
//addField( "CullResolution",		TypeS32,		Offset( mFieldData.mCullResolution,			fxFoliageReplicator ) );
//addField( "ViewDistance",		TypeF32,		Offset( mFieldData.mViewDistance,			fxFoliageReplicator ) );
//addField( "ViewClosest",		TypeF32,		Offset( mFieldData.mViewClosest,			fxFoliageReplicator ) );
//addField( "FadeInRegion",		TypeF32,		Offset( mFieldData.mFadeInRegion,			fxFoliageReplicator ) );
//addField( "FadeOutRegion",		TypeF32,		Offset( mFieldData.mFadeOutRegion,			fxFoliageReplicator ) );
//addField( "AlphaCutoff",		TypeF32,		Offset( mFieldData.mAlphaCutoff,			fxFoliageReplicator ) );
//addField( "GroundAlpha",		TypeF32,		Offset( mFieldData.mGroundAlpha,			fxFoliageReplicator ) );
//endGroup( "Culling" );	// MM: Added Group Footer.
//
//addGroup( "Animation" );	// MM: Added Group Header.
//addField( "SwayOn",				TypeBool,		Offset( mFieldData.mSwayOn,					fxFoliageReplicator ) );
//addField( "SwaySync",			TypeBool,		Offset( mFieldData.mSwaySync,				fxFoliageReplicator ) );
//addField( "SwayMagSide",		TypeF32,		Offset( mFieldData.mSwayMagnitudeSide,		fxFoliageReplicator ) );
//addField( "SwayMagFront",		TypeF32,		Offset( mFieldData.mSwayMagnitudeFront,		fxFoliageReplicator ) );
//addField( "MinSwayTime",		TypeF32,		Offset( mFieldData.mMinSwayTime,			fxFoliageReplicator ) );
//addField( "MaxSwayTime",		TypeF32,		Offset( mFieldData.mMaxSwayTime,			fxFoliageReplicator ) );
//endGroup( "Animation" );	// MM: Added Group Footer.
//
//addGroup( "Lighting" );	// MM: Added Group Header.
//addField( "LightOn",			TypeBool,		Offset( mFieldData.mLightOn,				fxFoliageReplicator ) );
//addField( "LightSync",			TypeBool,		Offset( mFieldData.mLightSync,				fxFoliageReplicator ) );
//addField( "MinLuminance",		TypeF32,		Offset( mFieldData.mMinLuminance,			fxFoliageReplicator ) );
//addField( "MaxLuminance",		TypeF32,		Offset( mFieldData.mMaxLuminance,			fxFoliageReplicator ) );
//addField( "LightTime",			TypeF32,		Offset( mFieldData.mLightTime,				fxFoliageReplicator ) );
//endGroup( "Lighting" );	// MM: Added Group Footer.
//
//addGroup( "Restrictions" );	// MM: Added Group Header.
//addField( "AllowOnTerrain",		TypeBool,		Offset( mFieldData.mAllowOnTerrain,			fxFoliageReplicator ) );
//addField( "AllowOnInteriors",	TypeBool,		Offset( mFieldData.mAllowOnInteriors,		fxFoliageReplicator ) );
//addField( "AllowOnStatics",		TypeBool,		Offset( mFieldData.mAllowStatics,			fxFoliageReplicator ) );
//addField( "AllowOnWater",		TypeBool,		Offset( mFieldData.mAllowOnWater,			fxFoliageReplicator ) );
//addField( "AllowWaterSurface",	TypeBool,		Offset( mFieldData.mAllowWaterSurface,		fxFoliageReplicator ) );
//addField( "AllowedTerrainSlope",TypeS32,		Offset( mFieldData.mAllowedTerrainSlope,	fxFoliageReplicator ) );
//endGroup( "Restrictions" );	// MM: Added Group Footer.

void fxFoliageReplicator::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream << mFieldData.mUseDebugInfo;
	stream << mFieldData.mDebugBoxHeight;
	stream << mFieldData.mHideFoliage;
	stream << mFieldData.mShowPlacementArea;
	stream << mFieldData.mPlacementBandHeight;
	stream << mFieldData.mPlaceAreaColour;

	stream << mFieldData.mSeed;
	stream.writeString( mFieldData.mFoliageFile );
	stream << mFieldData.mFoliageCount;
	stream << mFieldData.mFoliageRetries;

	stream << mFieldData.mInnerRadiusX;
	stream << mFieldData.mInnerRadiusY;
	stream << mFieldData.mOuterRadiusX;
	stream << mFieldData.mOuterRadiusY;

	stream << mFieldData.mMinWidth;
	stream << mFieldData.mMaxWidth;			
	stream << mFieldData.mMinHeight;			
	stream << mFieldData.mMaxHeight;			
	stream << mFieldData.mFixAspectRatio;		
	stream << mFieldData.mFixSizeToMax;		
	stream << mFieldData.mOffsetZ;				
	stream << mFieldData.mRandomFlip;	

	stream << mFieldData.mUseCulling;
	stream << mFieldData.mCullResolution;
	stream << mFieldData.mViewDistance;
	stream << mFieldData.mViewClosest;
	stream << mFieldData.mFadeInRegion;
	stream << mFieldData.mFadeOutRegion;
	stream << mFieldData.mAlphaCutoff;
	stream << mFieldData.mGroundAlpha;

	stream << mFieldData.mSwayOn;
	stream << mFieldData.mSwaySync;
	stream << mFieldData.mSwayMagnitudeSide;
	stream << mFieldData.mSwayMagnitudeFront;
	stream << mFieldData.mMinSwayTime;
	stream << mFieldData.mMaxSwayTime;

	stream << mFieldData.mLightOn;
	stream << mFieldData.mLightSync;
	stream << mFieldData.mMinLuminance;
	stream << mFieldData.mMaxLuminance;
	stream << mFieldData.mLightTime;

	stream << mFieldData.mAllowOnTerrain;
	stream << mFieldData.mAllowOnInteriors;
	stream << mFieldData.mAllowStatics;
	stream << mFieldData.mAllowOnWater;
	stream << mFieldData.mAllowWaterSurface;
	stream << mFieldData.mAllowedTerrainSlope;
}

void fxFoliageReplicator::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	stream >> mFieldData.mUseDebugInfo;
	stream >> mFieldData.mDebugBoxHeight;
	stream >> mFieldData.mHideFoliage;
	stream >> mFieldData.mShowPlacementArea;
	stream >> mFieldData.mPlacementBandHeight;
	stream >> mFieldData.mPlaceAreaColour;

	stream >> mFieldData.mSeed;
	char buf[1024];
	stream.readString( buf, 1024 );
	mFieldData.mFoliageFile = StringTable->insert( buf );
	stream >> mFieldData.mFoliageCount;
	stream >> mFieldData.mFoliageRetries;

	stream >> mFieldData.mInnerRadiusX;
	stream >> mFieldData.mInnerRadiusY;
	stream >> mFieldData.mOuterRadiusX;
	stream >> mFieldData.mOuterRadiusY;

	stream >> mFieldData.mMinWidth;
	stream >> mFieldData.mMaxWidth;			
	stream >> mFieldData.mMinHeight;			
	stream >> mFieldData.mMaxHeight;			
	stream >> mFieldData.mFixAspectRatio;		
	stream >> mFieldData.mFixSizeToMax;		
	stream >> mFieldData.mOffsetZ;				
	stream >> mFieldData.mRandomFlip;	

	stream >> mFieldData.mUseCulling;
	stream >> mFieldData.mCullResolution;
	stream >> mFieldData.mViewDistance;
	stream >> mFieldData.mViewClosest;
	stream >> mFieldData.mFadeInRegion;
	stream >> mFieldData.mFadeOutRegion;
	stream >> mFieldData.mAlphaCutoff;
	stream >> mFieldData.mGroundAlpha;

	stream >> mFieldData.mSwayOn;
	stream >> mFieldData.mSwaySync;
	stream >> mFieldData.mSwayMagnitudeSide;
	stream >> mFieldData.mSwayMagnitudeFront;
	stream >> mFieldData.mMinSwayTime;
	stream >> mFieldData.mMaxSwayTime;

	stream >> mFieldData.mLightOn;
	stream >> mFieldData.mLightSync;
	stream >> mFieldData.mMinLuminance;
	stream >> mFieldData.mMaxLuminance;
	stream >> mFieldData.mLightTime;

	stream >> mFieldData.mAllowOnTerrain;
	stream >> mFieldData.mAllowOnInteriors;
	stream >> mFieldData.mAllowStatics;
	stream >> mFieldData.mAllowOnWater;
	stream >> mFieldData.mAllowWaterSurface;
	stream >> mFieldData.mAllowedTerrainSlope;
}

bool fxFireReplicator::onAdd()
{
	if(!RenderableSceneObject::onAdd()) return(false);

	// Add the Replicator to the Replicator Set.
	dynamic_cast<SimSet*>(Sim::findObject("fxFoliageSet"))->addObject(this);

	// Set Default Object Box.
	mObjBox.min.set( -0.5, -0.5, -0.5 );
	mObjBox.max.set(  0.5,  0.5,  0.5 );
	// Reset the World Box.
	resetWorldBox();
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Add to Scene.
	addToScene();
	mAddedToScene = true;

	// Are we on the client?
	if ( isClientObject() )
	{
		// Yes, so load foliage texture.
		mFieldData.mFoliageTexture = GFXTexHandle( mFieldData.mFoliageFile, &GFXDefaultStaticDiffuseProfile );
		if ((GFXTextureObject*) mFieldData.mFoliageTexture == NULL)
			Con::printf("fxFoliageReplicator:  %s is an invalid or missing foliage texture file.", mFieldData.mFoliageFile);
		//mAlphaLookup = new GBitmap(AlphaTexLen, 1);
		//computeAlphaTex();

		// If we are in the editor then we can manually startup replication.
		if (gEditingMission) 
			mClientReplicationStarted = true;

		// Let's init the shader too! This is managed by the GFX->Shader manager, so I don't
		// need to free it myself. 
	}
	// Return OK.
	return(true);
}

void fxFireReplicator::renderObject(SceneState* state, RenderInst *)
{
	if(gClientSceneGraph->isReflectPass())
		return;

	// If we're rendering and we haven't placed any foliage yet - do it.
	if(!mClientReplicationStarted)
	{
		Con::warnf("fxFireReplicator::renderObject - tried to render a non replicated fxFireReplicator; replicating it now...");

		StartUp();
	}

	// Calculate Elapsed Time and take new Timestamp.
	S32 Time = Platform::getVirtualMilliseconds();
	F32 ElapsedTime = (Time - mLastRenderTime) * 0.001f;
	mLastRenderTime = Time;	

	renderPlacementArea(ElapsedTime);

	if (mCurrentFoliageCount > 0) {

		if (!mFieldData.mHideFoliage) {
			MySort();
			// Animate Global Sway Phase (Modulus).  
			mGlobalSwayPhase = mGlobalSwayPhase + (mGlobalSwayTimeRatio * ElapsedTime);

			// Animate Global Light Phase (Modulus).
			mGlobalLightPhase = mGlobalLightPhase + (mGlobalLightTimeRatio * ElapsedTime);

			// Compute other light parameters
			const F32	LuminanceMidPoint		= (mFieldData.mMinLuminance + mFieldData.mMaxLuminance) / 2.0f;
			const F32	LuminanceMagnitude		= mFieldData.mMaxLuminance - LuminanceMidPoint;

			// Distances used in alpha
			const F32	ClippedViewDistance		= mFieldData.mViewDistance;
			const F32	MaximumViewDistance		= ClippedViewDistance + mFieldData.mFadeInRegion;

			// Set up our shader constants	
			// Projection matrix
			GFX->setVertexShaderConstF(0, (float*)&layer_speed, 1);
			Point4F temp_vec4;
            temp_vec4 = Point4F(mLastRenderTime * 0.001f, 0.0f, 0.0f, 0.0f);
            GFX->setVertexShaderConstF(1, (float*)&temp_vec4, 1);
			// World transform matrix
			MatrixF proj = GFX->getProjectionMatrix();
			proj.transpose();
			GFX->setVertexShaderConstF(9, (float*) &proj, 4);
			MatrixF world = GFX->getWorldMatrix();
			world.transpose();
			GFX->setVertexShaderConstF(2, (float*)&world, 4);
			// Current Global Sway Phase
			GFX->setVertexShaderConstF(6, (float*) &mGlobalSwayPhase, 1, 1);
			// Sway magntitudes
			GFX->setVertexShaderConstF(7, (float*) &mFieldData.mSwayMagnitudeSide, 1, 1);
			GFX->setVertexShaderConstF(8, (float*) &mFieldData.mSwayMagnitudeFront, 1, 1);

			// Pixel shader constants, this one is for ground alpha
			temp_vec4 = Point4F(distortion_amount2, 0.0f, 0.0f, 0.0f);
			GFX->setPixelShaderConstF(0, (float*) &temp_vec4, 1);	
            temp_vec4 = Point4F(distortion_amount1, 0.0f, 0.0f, 0.0f);
			GFX->setPixelShaderConstF(2, (float*) &temp_vec4, 1);	
			temp_vec4 = Point4F(distortion_amount0, 0.0f, 0.0f, 0.0f);
			GFX->setPixelShaderConstF(3, (float*) &temp_vec4, 1);	
			GFX->setPixelShaderConstF(4, (float*) &myColor, 1);	
			temp_vec4 = Point4F(bumpscale, 0.0f, 0.0f, 0.0f);
			GFX->setPixelShaderConstF(5, (float*) &temp_vec4, 1);	

			// Blend ops
			GFX->setAlphaBlendEnable( true );
			GFX->setAlphaTestEnable( true );
			GFX->setSrcBlend(GFXBlendSrcAlpha);
			GFX->setDestBlend(GFXBlendOne);
			GFX->setAlphaFunc(GFXCmpGreater);
			GFX->setAlphaRef((U8) (255.0f * mFieldData.mAlphaCutoff));
			GFX->setCullMode(GFXCullNone);

			// Set up our texture and color ops.
			GFX->setShader(fireshader->getShader());
			GFX->setTexture(0, mTexture);
			// computeAlphaTex();		// Uncomment if we figure out how to clamp to fogAndHaze
			GFX->setTexture(1, mTexture2);
			GFX->setTextureStageColorOp(0, GFXTOPModulate);
			GFX->setTextureStageColorOp(1, GFXTOPModulate);  // am I needed? 
			GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
			GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
			GFX->setTextureStageAddressModeU(1, GFXAddressWrap);
			GFX->setTextureStageAddressModeV(1, GFXAddressWrap);
			GFX->setTextureStageMagFilter( 0, GFXTextureFilterLinear );
			GFX->setTextureStageMinFilter( 0, GFXTextureFilterLinear );
			GFX->setTextureStageMipFilter( 0, GFXTextureFilterLinear );
			GFX->setTextureStageMagFilter( 1, GFXTextureFilterLinear );
			GFX->setTextureStageMinFilter( 1, GFXTextureFilterLinear );
			GFX->setTextureStageMipFilter( 1, GFXTextureFilterLinear );
			// Setup our buffers
			GFX->setVertexBuffer(mVertexBuffer);
			GFX->setPrimitiveBuffer(mPrimBuffer);

			// If we use culling, we're going to send chunks of our buffers to the card
			//if (mFieldData.mUseCulling)
			//{
			//	// Setup the Clip-Planes.
			//	F32 FarClipPlane = getMin((F32)state->getFarPlane(), 
			//		mFieldData.mViewDistance + mFieldData.mFadeInRegion);
			//	mFrustumRenderSet.SetupClipPlanes(state, FarClipPlane);

			//	renderQuad(mFoliageQuadTree[0], getRenderTransform(), false);

			//	// Multipass, don't want to interrupt the vb state 
			//	if (mFieldData.mUseDebugInfo) 
			//	{
			//		// hey man, we're done, so it doesn't matter if we kill it to render the next part
			//		GFX->setTextureStageColorOp(0, GFXTOPDisable);
			//		GFX->setTextureStageColorOp(1, GFXTOPDisable);
			//		GFX->disableShaders(); 
			//		renderQuad(mFoliageQuadTree[0], getRenderTransform(), true);
			//	}
			//}
			//else 
			{	
				// Draw the whole shebang!
				GFX->drawIndexedPrimitive(GFXTriangleList, 0, mVertexBuffer->mNumVerts, 
					0, mPrimBuffer->mIndexCount / 3);
			}

			// Reset some states 
			GFX->setAlphaBlendEnable( false );
			GFX->setAlphaTestEnable( false );
			GFX->setTextureStageColorOp(0, GFXTOPDisable);
			GFX->setTextureStageColorOp(1, GFXTOPDisable);  
			GFX->disableShaders();		// this fixes editor issue.
			GFX->setTextureStageMagFilter( 0, GFXTextureFilterLinear );
			GFX->setTextureStageMinFilter( 0, GFXTextureFilterLinear );
			GFX->setTextureStageMipFilter( 0, GFXTextureFilterNone );
			GFX->setTextureStageMagFilter( 1, GFXTextureFilterLinear );
			GFX->setTextureStageMinFilter( 1, GFXTextureFilterLinear );
			GFX->setTextureStageMipFilter( 1, GFXTextureFilterNone );
		}
	}
}

DECLARE_SERIALIBLE(VolumeFog);

fxFireReplicator::fxFireReplicator() : fxFoliageReplicator()
{
	layer_speed = Point4F(0.68753f, 0.52f, 0.7534f, 1.0f);
	distortion_amount2 = 0.0723f;
	distortion_amount1 = 0.091f;
	distortion_amount0 = 0.123f;
	myColor = ColorF(0.0147f, 0.0147f, 0.0147f, 0.8f);
	mTexture = NULL;
	mTexture2 = NULL;
	fireshader = NULL;
	bumpscale = 1.5f;
	mWidth = 3;
	mHeight = 4;
	m_TextureName = "~/data/environments/precipitation/001.dds";
	m_Texture2Name = "~/data/environments/precipitation/distortion.dds";	
	if ( !Sim::findObject( "fireblaze", fireshader ) )
		Con::warnf("fireblaze - failed to locate fireblaze shader fireblazeBlendData!");
}

fxFireReplicator::~fxFireReplicator()
{
	mTexture = NULL;
	mTexture2 = NULL;
}

void fxFireReplicator::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();

	addField( "layer_speed",				TypePoint4F,		Offset( layer_speed,					fxFireReplicator ) );
	addField( "distortion_amount2",				TypeF32,		Offset( distortion_amount2,					fxFireReplicator ) );
	addField( "distortion_amount1",				TypeF32,		Offset( distortion_amount1,					fxFireReplicator ) );
	addField( "distortion_amount0",				TypeF32,		Offset( distortion_amount0,					fxFireReplicator ) );
	addField( "myColor",				TypeColorF,		Offset( myColor,					fxFireReplicator ) );
	addField("TextureName",                TypeString,             Offset(m_TextureName,                     fxFireReplicator));
	addField("TextureName2",                TypeString,             Offset(m_Texture2Name,                     fxFireReplicator));
    addField("bumpscale",                TypeF32,              Offset(bumpscale,                     fxFireReplicator));
	addField("Width",                   TypeS32,                  Offset(mWidth,                        fxFireReplicator));
	addField("Height",                TypeS32,                 Offset(mHeight,                       fxFireReplicator));
}

void fxFireReplicator::packData(BitStream* stream)
{
	stream->writeString(m_TextureName);
	stream->writeString(m_Texture2Name);
	stream->writeBits(128, &layer_speed);
	stream->writeBits(32, &distortion_amount2);
	stream->writeBits(32, &distortion_amount1);
	stream->writeBits(32, &distortion_amount0);
	stream->writeBits(128, &myColor);
    stream->writeBits(32, &bumpscale);
	stream->writeBits(32, &mWidth);
	stream->writeBits(32, &mHeight);
}

void     fxFireReplicator::unpackData(BitStream* stream)
{
	m_TextureName = stream->readSTString();
	m_Texture2Name = stream->readSTString();
	stream->readBits(128, &layer_speed);
	stream->readBits(32, &distortion_amount2);
	stream->readBits(32, &distortion_amount1);
	stream->readBits(32, &distortion_amount0);
	stream->readBits(128, &myColor);
	stream->readBits(32, &bumpscale);
	stream->readBits(32, &mWidth);
	stream->readBits(32, &mHeight);	

	mTexture = NULL;
	mTexture2 = NULL;
	mTexture = GFXTexHandle(m_TextureName, &GFXDefaultStaticDiffuseProfile);
	mTexture2 = GFXTexHandle(m_Texture2Name, &GFXDefaultStaticDiffuseProfile);
}

void fxFireReplicator::unpackUpdate(NetConnection *conn, BitStream *stream)
{
    Parent::unpackUpdate(conn, stream);
    unpackData(stream);
}

U64 fxFireReplicator::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);
	packData(stream);
	return retMask;
}

//static S32 fxFireReplicatorFunc(const void *a, const void *b)
//{
//	const fxFoliageItem *aS = (const fxFoliageItem*)a;
//	const fxFoliageItem *bS = (const fxFoliageItem*)b;
//
//	return (aS->mViewZ > bS->mViewZ ? 1 : -1);
//}

void    fxFireReplicator::MySort()
{
	MatrixF matView = GFX->getWorldMatrix();
	//matView.transpose();
	Point3F TempViewPos;

	//for(S32 i = 0; i < mSortFoliageItem.size(); i++)
	{
		//Vector<fxFoliageItem*> j = mSortFoliageItem[i];
		for (	Vector<fxFoliageItem*>::iterator QuadNodeItr = mSortFoliageItem.begin();
			QuadNodeItr != mSortFoliageItem.end();
			QuadNodeItr++ )
		{
			matView.mulP((*QuadNodeItr)->Transform.getPosition(), &TempViewPos);
			(*QuadNodeItr)->mViewZ = TempViewPos.z;
		}
	}

	//dQsort(mSortFoliageItem.address(), mSortFoliageItem.size(), sizeof(fxFoliageItem*), fxFireReplicatorFunc);
	//for(S32 k = 0; k < mSortFoliageItem.size(); k++)
	{
		//Vector<fxFoliageItem*> ppp = mSortFoliageItem[k];
		for(	S32 i = 0; i < mSortFoliageItem.size() - 1; i++)
		{
			Vector<fxFoliageItem*>::iterator temp = mSortFoliageItem.begin();
			for(S32 j = 0; j < mSortFoliageItem.size() - i - 1; j++)
			{
				if(temp != mSortFoliageItem.end())
				{
					Vector<fxFoliageItem*>::iterator temp2 = temp;
					temp2++;
					if ((*temp)->mViewZ < (*temp2)->mViewZ)
					{
						fxFoliageItem* temp_prt = (*temp);
						(*temp) = (*temp2);
						(*temp2) = temp_prt;
					}
					else
					{
						temp++;
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	Point3F basePoints[8];
	basePoints[0] = Point3F(-0.5f, 0.0f, 1.0f);
	basePoints[1] = Point3F(-0.5f, 0.0f, 0.0f);
	basePoints[2] = Point3F(0.5f, 0.0f, 0.0f);
	basePoints[3] = Point3F(0.5f, 0.0f, 1.0f);

	Point2F texCoords[4];
	texCoords[0] = Point2F(0.0, 0.0);
	texCoords[1] = Point2F(0.0, 1.0);
	texCoords[2] = Point2F(1.0, 1.0);
	texCoords[3] = Point2F(1.0, 0.0);	

	S32 TextureSplinterNum = 0;

	if ((mWidth > 0) && (mHeight > 0))
		TextureSplinterNum = mWidth* mHeight;

	//mVertexBuffer.set(GFX, mFieldData.mFoliageCount * 4, GFXBufferTypeStatic);
	mVertexBuffer.lock();
	U32 idx = 0;	
	//for(S32 k = 0; k < mSortFoliageItem.size(); k++)
	{
		//Vector<fxFoliageItem*> ppp = mSortFoliageItem[k];
		for (	Vector<fxFoliageItem*>::iterator QuadNodeItr = mSortFoliageItem.begin();
			QuadNodeItr != mSortFoliageItem.end();
			QuadNodeItr++ ) 
		{
			for (U32 vertIndex = 0; vertIndex < 4; vertIndex++) 
			{
				GFXVertexFoliage *vert = &mVertexBuffer[(idx*4) + vertIndex];
				vert->point = (*QuadNodeItr)->Transform.getPosition();			
				vert->normal = basePoints[vertIndex];

				vert->normal.x *= (*QuadNodeItr)->Width;
				vert->normal.z *= (*QuadNodeItr)->Height;

				vert->texCoord = texCoords[vertIndex];				
				if ((*QuadNodeItr)->Flipped)
					vert->texCoord.x = 1.0f - vert->texCoord.x;

				if ((vertIndex == 0) || (vertIndex == 3)) {
					vert->texCoord2.set((*QuadNodeItr)->SwayTimeRatio / mGlobalSwayTimeRatio, 1.0f);
				} else {
					vert->texCoord2.set(0.0f, 0.0f);
				}

				vert->normal.y = (*QuadNodeItr)->LightPhase;

				Point3F temppoint3f; 

				if(TextureSplinterNum)
				{
					if(vertIndex == 0)
					{
						vert->texCoord5 = mTextureCoordUV1[(*QuadNodeItr)->randnum];
						temppoint3f = Point3F(mTextureCoordUV1[(*QuadNodeItr)->randnum1].x , mTextureCoordUV1[(*QuadNodeItr)->randnum1].y ,(*QuadNodeItr)->randnum2);
					}
					if(vertIndex == 1)
					{
						vert->texCoord5 = mTextureCoordUV3[(*QuadNodeItr)->randnum];
						temppoint3f = Point3F(mTextureCoordUV3[(*QuadNodeItr)->randnum1].x , mTextureCoordUV3[(*QuadNodeItr)->randnum1].y ,(*QuadNodeItr)->randnum2);
					}
					if(vertIndex == 2)
					{
						vert->texCoord5 = mTextureCoordUV4[(*QuadNodeItr)->randnum];
						temppoint3f = Point3F(mTextureCoordUV4[(*QuadNodeItr)->randnum1].x , mTextureCoordUV4[(*QuadNodeItr)->randnum1].y ,(*QuadNodeItr)->randnum2);
					}
					if(vertIndex == 3)
					{
						vert->texCoord5 = mTextureCoordUV2[(*QuadNodeItr)->randnum];
						temppoint3f = Point3F(mTextureCoordUV2[(*QuadNodeItr)->randnum1].x , mTextureCoordUV2[(*QuadNodeItr)->randnum1].y ,(*QuadNodeItr)->randnum2);
					}
					vert->texCoord4 = temppoint3f;
					vert->texCoord3.x = mTextureCoordUV4[(*QuadNodeItr)->randnum].y;
					vert->texCoord3.y = mTextureCoordUV4[(*QuadNodeItr)->randnum1].y;
				}
			}
			idx++;
		}
	}

	mVertexBuffer.unlock();	
}

VolumeFog::VolumeFog() 
{
	mTypeMask |= StaticObjectType | StaticRenderedObjectType;
	mNetFlags.set(Ghostable | ScopeAlways);

	m_pShader = NULL;
	color_amount = Point4F(0.6, 0.6f, 0,1);
	m_FogColor = ColorF(1,1,1,1);
	m_TexName = StringTable->insert("");
	m_pTexDepth = NULL;
	m_fTime = 0;
    m_dwNowTime = 0;
	m_vSpeed = Point3F(0.033f, 0.033f, 1.0f);
	m_fRadius = 20.0f;
	m_fAttenuationStart = 18.0f;

	// Set Default Object Box.
	mObjBox.min.set( -1, -1, -1 );
	mObjBox.max.set(  1,  1,  1 );
}

VolumeFog::~VolumeFog()
{
	m_pTexDepth = NULL;
}

void VolumeFog::shutdown()
{
	
}

bool VolumeFog::onAdd()
{
	if(!Parent::onAdd()) return(false);

	if (!m_pShader)
	{
		if ( !Sim::findObject( "FogPolygonVolumes1", m_pShader ) )
			Con::warnf("FogPolygonVolumes1 - failed to locate FogPolygonVolumes1 shader");
	}

	// Reset the World Box.
	resetWorldBox();
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Add to Scene.
	addToScene();

	return true;
}

bool VolumeFog::prepRenderImage(SceneState *state, const U32 stateKey, const U32 startZone, const bool modifyBaseZoneState/* =false */)
{
	if(gClientSceneGraph->isReflectPass())
		return false;

	const MatrixF &objTrans = GFX->getWorldMatrix();
	MatrixF proj     = GFX->getProjectionMatrix();

	RenderInst *ri = gRenderInstManager.allocInst();
	ri->worldXform = gRenderInstManager.allocXform();
	*ri->worldXform = objTrans;

	ri->objXform = gRenderInstManager.allocXform();
	*ri->objXform = proj;

	ri->obj = this;

	ri->translucent = true;
	ri->type = RenderInstManager::RIT_Translucent;
	ri->calcSortPoint( this, state->getCameraPosition() );
	ri->particles = RenderTranslucentMgr::RENDERTRANSLUCENT_TYPE_VOLUMEFOG;

	gRenderInstManager.addInst( ri );

	return true;
}

void VolumeFog::onRemove()
{
	removeFromScene();
	Parent::onRemove();
}

void VolumeFog::renderObject( SceneState *state, RenderInst *ri )
{
	if(!m_pTexDepth)
	{
		m_pTexDepth = GFXTexHandle(m_TexName, &GFXDefaultStaticDiffuseProfile);
	}
	GFX->setTexture( 0, m_pTexDepth );
	GFX->copyBBToSfxBuff();
	GFX->setTexture(1, GFX->getSfxBackBuffer());
	GFX->setShader(m_pShader->getShader());
	MatrixF mat =(*ri->objXform) * (*ri->worldXform)* getRenderTransform();
	mat.transpose();
	GFX->setVertexShaderConstF(0, (float*)&mat, 4);

    Point4F cons = Point4F(m_vSpeed.x, m_vSpeed.y,0,0);
	GFX->setVertexShaderConstF(4, (float*) &cons, 1);

	U32 nowtime = timeGetTime();
	m_fTime += (float)(nowtime - m_dwNowTime)/1000.0f; 
	m_dwNowTime = nowtime;
	cons = Point4F(m_fTime, 0, 0,0);
    GFX->setVertexShaderConstF(5, (float*) &cons, 1);
	cons = Point4F(m_fRadius, m_fAttenuationStart, 0,0);
	GFX->setPixelShaderConstF(6, (float*) &cons, 1);
    GFX->setPixelShaderConstF(0, color_amount, 1);
    GFX->setPixelShaderConstF(4, m_FogColor, 1);
	GFX->setAlphaBlendEnable(false);
	GFX->setCullMode(GFXCullNone);
	//GFX->setSrcBlend( GFXBlendSrcAlpha);
	//GFX->setDestBlend(GFXBlendInvSrcAlpha);
	GFX->setTextureStageAddressModeU(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeV(0, GFXAddressWrap);
	GFX->setTextureStageAddressModeW(0, GFXAddressWrap);

	PrimBuild::color(ColorF(1.0f, 1.0f, 1.0f));
	PrimBuild::begin(GFXTriangleStrip, 4);
	PrimBuild::texCoord2f(0.0f, 0.0f);
	PrimBuild::vertex3fv(Point3F(-m_fRadius, m_fRadius, 0));
	PrimBuild::texCoord2f(m_vSpeed.z, 0.0f);
	PrimBuild::vertex3fv(Point3F(m_fRadius, m_fRadius, 0));
	PrimBuild::texCoord2f(0.0f, m_vSpeed.z);
	PrimBuild::vertex3fv(Point3F(-m_fRadius, -m_fRadius, 0));
	PrimBuild::texCoord2f(m_vSpeed.z, m_vSpeed.z);
	PrimBuild::vertex3fv(Point3F(m_fRadius, -m_fRadius, 0));
	PrimBuild::end(false);

    GFX->disableShaders();
	GFX->setCullMode(GFXCullCCW);
	GFX->setTextureStageAddressModeU(0, GFXAddressClamp);
	GFX->setTextureStageAddressModeV(0, GFXAddressClamp);
	GFX->setTextureStageAddressModeW(0, GFXAddressClamp);
	TSShapeInstance::isNormalSkin = false;
}

void VolumeFog::unpackUpdate(NetConnection *conn, BitStream *stream)
{
	Parent::unpackUpdate(conn, stream);

	unpackData(stream);

	MatrixF		ObjectMatrix;
	stream->readAffineTransform(&ObjectMatrix);			// Foliage Master Object Position.
	setTransform(ObjectMatrix);
}

U64 VolumeFog::packUpdate(NetConnection *conn, U64 mask, BitStream *stream)
{
	U64 retMask = Parent::packUpdate(conn, mask, stream);

	packData(stream);

	stream->writeAffineTransform(mObjToWorld);						// Foliage Master-Object Position.
	return retMask;
}

void VolumeFog::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();
	addField( "color_amount",				TypePoint4F,		Offset( color_amount,					VolumeFog ) );
    addField( "TexName",				TypeFilename,		    Offset( m_TexName,					VolumeFog ) );
    addField( "FogColor",				TypeColorF,		Offset( m_FogColor,					VolumeFog ) );
	addField( "Radius",				    TypeF32,		Offset( m_fRadius,					VolumeFog ) );
    addField( "Speed",				    TypePoint3F,		Offset( m_vSpeed,					VolumeFog ) );
    addField( "AttenuationStart",	    TypeF32,		Offset( m_fAttenuationStart,					VolumeFog ) );
}

void VolumeFog::packData(BitStream* stream)
{
	stream->writeBits(128, &color_amount);
    stream->writeString(m_TexName);
    stream->writeBits(128, &m_FogColor);
	stream->writeBits(32, &m_fRadius);
    stream->writeBits(96, &m_vSpeed);
    stream->writeBits(32, &m_fAttenuationStart);

	mObjBox.min =  Point3F(-m_fRadius,-m_fRadius,-1.0f);
	mObjBox.max =  Point3F(m_fRadius,m_fRadius,1.0f);
}

void VolumeFog::unpackData(BitStream* stream)
{
	stream->readBits(128, &color_amount);
	m_TexName = stream->readSTString();
	stream->readBits(128, &m_FogColor);
	stream->readBits(32, &m_fRadius);
	stream->readBits(96, &m_vSpeed);
    stream->readBits(32, &m_fAttenuationStart);
	mObjBox.min =  Point3F(-m_fRadius,-m_fRadius,-1.0f);
    mObjBox.max =  Point3F(m_fRadius,m_fRadius,1.0f);
}

void VolumeFog::OnSerialize( CTinyStream& stream )
{
	__super::OnSerialize( stream );

	stream << color_amount;
	stream.writeString( m_TexName );
	stream << m_FogColor;
	stream << m_fRadius;
	stream << m_vSpeed;
	stream << m_fAttenuationStart;
}

void VolumeFog::OnUnserialize( CTinyStream& stream )
{
	__super::OnUnserialize( stream );

	stream >> color_amount;
	char buf[256];
	stream.readString( buf, 256 );
	m_TexName = StringTable->insert( buf );

	stream >> m_FogColor;
	stream >> m_fRadius;
	stream >> m_vSpeed;
	stream >> m_fAttenuationStart;
}
//#pragma warning( default : 4127 )
#pragma warning( pop ) 