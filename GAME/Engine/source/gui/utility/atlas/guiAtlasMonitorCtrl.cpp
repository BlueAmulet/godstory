//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "gui/utility/atlas/guiAtlasMonitorCtrl.h"
#include "T3D/gameConnection.h"
#include "gfx/gBitmap.h"
#include "gfx/gfxDevice.h"
#include "gfx/gfxTextureProfile.h"
#include "atlas/runtime/atlasInstance2.h"
#include "atlas/resource/atlasResourceTexTOC.h"
#include "atlas/resource/atlasResourceGeomTOC.h"
#include "atlas/core/atlasClassFactory.h"

GFX_ImplementTextureProfile( AtlasMonitorTextureProfile, 
                             GFXTextureProfile::DiffuseMap, 
                             GFXTextureProfile::KeepBitmap | GFXTextureProfile::NoMipmap,
                             GFXTextureProfile::None );

//TODO: allow switching Atlas instances
//TODO: use properties to control what is shown

IMPLEMENT_CONOBJECT( GuiAtlasMonitorCtrl );

//--------------------------------------------------------------------------------
//	Configuration.
//--------------------------------------------------------------------------------

enum {
	/// The number of coarsest LODs to not show in a
	/// texture TOC monitor.  Since these levels usually
	/// won't get paged out, they are not of interest.
	NUM_TEXTURE_LODS_TO_DROP = 2,

	/// The number of coarsest LODs to not show in a
	/// geometry TOC monitor.
	NUM_GEOMETRY_LODS_TO_DROP = 1,
};

/// These are the colors that will be used to indicate the different
/// loading states of a stub in the texture TOC monitor.

static U8 gStateColors[ 4 ][ 3 ] =
{
	{ 0x00, 0x00, 0x00 },	// Unloaded
	{ 0xFF, 0xFF, 0xFF },	// Pending
	{ 0xFF, 0x00, 0x00 },	// Loading
	{ 0x00, 0xFF, 0x00 },	// Loaded
};

//--------------------------------------------------------------------------------
//	Events.
//--------------------------------------------------------------------------------

bool GuiAtlasMonitorCtrl::onAdd()
{
	if( !Parent::onAdd() )
		return false;

	// Get the game connection and look up the first Atlas instance.

	GameConnection* connection = GameConnection::getConnectionToServer();
	if( !connection )
		Con::errorf( "GuiAtlasMonitorCtrl::onAdd -- no game connection" );
	else
	{
		for( SimSetIterator iter( connection ); *iter; ++ iter )
		{
			SimObject* object = *iter;
			if( dynamic_cast< AtlasInstance* >( object ) )
			{
				AtlasInstance* instance = ( AtlasInstance* ) object;
				Con::printf( "GuiAtlasMonitorCtrl::onAdd -- monitoring instance %i", object->getId() );
				mAtlasInstance = instance;
				initialize();
			}
		}

		if( !mAtlasInstance )
			Con::warnf( "GuiAtlasMonitorCtrl::onAdd -- could not find any Atlas instances" );
	}

	return true;
}

void GuiAtlasMonitorCtrl::onRemove()
{
	Parent::onRemove();
	cleanup();
}

void GuiAtlasMonitorCtrl::onSleep()
{
	Parent::onSleep();
	freeTOCMonitors();
}

bool GuiAtlasMonitorCtrl::onWake()
{
	if( !Parent::onWake() )
		return false;

	if( mAtlasInstance )
		initTOCMonitors();
	return true;
}

void GuiAtlasMonitorCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	Parent::onRender( offset, updateRect );

	if( mAtlasInstance )
	{
		U32 numTOCMonitors = mTOCMonitors.size();
		U32 widthPerComponent = getExtent().x / ( numTOCMonitors + 1 );
		Point2I rectPerComponent( widthPerComponent, getExtent().y );
		Point2I currentOffset = offset;

		// Update and render the TOC monitors.

		for( U32 i = 0; i < numTOCMonitors; ++ i )
		{
			mTOCMonitors[ i ].update();
			mTOCMonitors[ i ].render( this, currentOffset, rectPerComponent );
			currentOffset.x += widthPerComponent;
		}

		// Render the stats.

		renderStats( currentOffset, rectPerComponent );
	}
}

//--------------------------------------------------------------------------------
//	Internal.
//--------------------------------------------------------------------------------

void GuiAtlasMonitorCtrl::initialize()
{
	initTOCMonitors();
}

void GuiAtlasMonitorCtrl::cleanup()
{
	freeTOCMonitors();
	mAtlasInstance = NULL;
}

void GuiAtlasMonitorCtrl::initTOCMonitors()
{
	AtlasFile* file = mAtlasInstance->getAtlasFile();
	U32 numTOCs = file->getTOCCount();

	for( U32 i = 0; i < numTOCs; ++ i )
	{
		AtlasTOC* toc = file->getTOC( i );
		const char* name = AtlasClassFactory::getTOCName( toc );

		bool isGeomTOC = ( dStrcmp( name, "Geom" ) == 0 );
		bool isTexTOC = ( dStrcmp( name, "Tex" ) == 0 );

      if( isTexTOC )
         name = static_cast< AtlasResourceTexTOC* >( toc )->mTextureName;

		if( isGeomTOC || isTexTOC )
		{
			mTOCMonitors.push_back( TOCMonitor() );
			TOCMonitor& monitor = mTOCMonitors.last();
			monitor.init( isTexTOC ? TOC_Texture : TOC_Geometry, name, toc,
				isTexTOC ? NUM_TEXTURE_LODS_TO_DROP : NUM_GEOMETRY_LODS_TO_DROP );
		}
	}
}

void GuiAtlasMonitorCtrl::freeTOCMonitors()
{
	mTOCMonitors.clear();
}

void GuiAtlasMonitorCtrl::getStat( EStat stat, char* buffer, U32 bufferSize )
{
	AtlasFile* file = mAtlasInstance->getAtlasFile();
   file->getIOStatus( mIOStatus );

	U32 value = 0;
	const char* formatString = "";

	switch( stat )
	{
   case STAT_NumRequests:     value = mIOStatus.mNumRequests;              formatString = "Requests   : %i"; break;
   case STAT_NumRequestsDOA:  value = mIOStatus.mNumRequestsDOA;           formatString = "DOA        : %i"; break;
   case STAT_BytesPending:    value = mIOStatus.mBytesPending;             formatString = "BytesPend  : %i"; break;
   case STAT_BytesLoading:    value = mIOStatus.mBytesLoading;             formatString = "BytesLoad  : %i"; break;
   case STAT_LoadPending:     value = mIOStatus.mLoadQueueLength;          formatString = "Q/Load     : %i"; break;
   case STAT_UnpackPending:   value = mIOStatus.mDeserializeQueueLength;   formatString = "Q/Unpack   : %i"; break;
   case STAT_InstatePending:  value = mIOStatus.mInstatementQueueLength;   formatString = "Q/Instate  : %i"; break;
	}

	dSprintf( buffer, bufferSize, formatString, value );
}

void GuiAtlasMonitorCtrl::renderStats( Point2I offset, Point2I extent )
{
	char textBuffer[ 512 ];
	U32 heightPerStat = extent.y / NUM_STATS;
	extent.y = heightPerStat;

	// Nudge a bit to the right.
	offset.x += 4;
	extent.x -= 4;

	for( U32 i = 0; i < NUM_STATS; ++ i )
	{
		getStat( ( EStat ) i, textBuffer, sizeof( textBuffer ) );
		renderJustifiedText( offset, extent, textBuffer );
		offset.y += heightPerStat;
	}
}

//--------------------------------------------------------------------------------
//	TOC Monitors.
//--------------------------------------------------------------------------------

/// Set up the TOC monitor to show the load status and
/// interest region of each individual LOD.

void GuiAtlasMonitorCtrl::TOCMonitor::init( ETOCType type, const char* name, AtlasTOC* toc, U32 droppedLevels )
{
	U32 treeDepth = toc->getTreeDepth();
	U32 regionHeight = 0;

   if( droppedLevels >= treeDepth )
      droppedLevels = treeDepth - 1;
	U32 numMonitoredLevels = treeDepth - droppedLevels;
	mTextures.setSize( numMonitoredLevels );

	for( U32 i = 0; i < numMonitoredLevels; ++ i )
	{
		U32 edgeWidth = BIT( i + droppedLevels );
		regionHeight += edgeWidth;
		GBitmap* bitmap = new GBitmap( edgeWidth, edgeWidth );
		mTextures[ i ].set( bitmap, &AtlasMonitorTextureProfile, false );
	}

	mType = type;
	mName = name;
	mExtent = Point2I( BIT( treeDepth - 1 ), regionHeight );
	mTOC = toc;
	mDroppedLevels = droppedLevels;
}

/// Free the resources held by the TOC monitor.

GuiAtlasMonitorCtrl::TOCMonitor::~TOCMonitor()
{
	for( U32 i = 0; i < mTextures.size(); ++ i )
		mTextures[ i ].free();
	mTextures.clear();
}

/// Update all the bitmaps for the texture TOC monitor and
/// issue reloads on the associated textures.

void GuiAtlasMonitorCtrl::TOCMonitor::update()
{
	for( U32 i = 0; i < mTextures.size(); ++ i )
	{
		U32 level = i + mDroppedLevels;
		U32 edgeWidth = BIT( level );
		GBitmap* bitmap = mTextures[ i ].getBitmap();

		for( U32 x = 0; x < edgeWidth; ++ x )
			for( U32 y = 0; y < edgeWidth; ++ y )
			{
				U32 state = getStubState( level, Point2I( x, y ) );
				ColorI color( gStateColors[ state ][ 0 ], gStateColors[ state ][ 1 ], gStateColors[ state ][ 2 ] );
				bitmap->setColor( x, y, color );
			}

		mTextures[ i ].refresh();
	}
}

void GuiAtlasMonitorCtrl::TOCMonitor::render( GuiAtlasMonitorCtrl* ctrl, Point2I offset, Point2I extent )
{
	// Calculate the width and height ratio and take the smaller ratio as the
	// one to render our content with.

	F32 widthRatio = F32( extent.x ) / F32( mExtent.x );
	F32 heightRatio = F32( extent.y ) / F32( mExtent.y );
	F32 ratio = getMin( widthRatio, heightRatio );

	// Now walk the texture LOD table upside down and draw the rectangles.

	U32 currentEdgeWidthInScreenCoords = ceil( F32( mExtent.x ) * ratio );
	RectI rectangle( offset.x, offset.y,
		currentEdgeWidthInScreenCoords, currentEdgeWidthInScreenCoords );
	
	U32 heightRendered = 0;
	for( S32 i = mTextures.size() - 1; i >= 0; -- i )
	{
		GFX->getDrawUtil()->drawBitmapStretch( mTextures[ i ], rectangle );
		heightRendered += rectangle.extent.y;

		// Update our region for the next level.

		U32 edgeWidthNextLevelInScreenCoords = ceil( F32( BIT( i + mDroppedLevels - 1 ) ) * ratio );
		
		rectangle.point.y += currentEdgeWidthInScreenCoords;
		rectangle.point.x += edgeWidthNextLevelInScreenCoords - 1;
		rectangle.extent.x = edgeWidthNextLevelInScreenCoords;
		rectangle.extent.y = edgeWidthNextLevelInScreenCoords;

		currentEdgeWidthInScreenCoords = edgeWidthNextLevelInScreenCoords;
	}

	// Render the label in bottom left corner.

	GFX->getDrawUtil()->drawText( ctrl->mProfile->mFont,
		Point2I( offset.x + 4, offset.y + heightRendered - 10 ),
		mName, ctrl->mProfile->mFontColors );
}

U32 GuiAtlasMonitorCtrl::TOCMonitor::getStubState( U32 level, Point2I pos )
{
	U32 state;
	if( mType == TOC_Texture )
	{
		AtlasResourceTexTOC* toc = ( AtlasResourceTexTOC* ) mTOC;
		state = toc->getStub( level, pos )->mState;
	}
	else
	{
		AssertFatal( dynamic_cast< AtlasResourceGeomTOC* >( mTOC ) != 0,
			"GuiAtlasMonitorCtrl::TOCMonitor::getStubState -- unknown type of TOC" );
		AtlasResourceGeomTOC* toc = ( AtlasResourceGeomTOC* ) mTOC;
		state = toc->getStub( level, pos )->mState;
	}
	return state;
}
