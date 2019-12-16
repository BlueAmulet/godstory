//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------
#ifndef _GUIATLASMONITORCTRL_H_
#define _GUIATLASMONITORCTRL_H_

#include "gui/containers/guiContainer.h"
#include "gfx/gfxTextureHandle.h"
#include "core/tVector.h"
#include "math/mPoint.h"
#include "atlas/core/atlasFile.h"

class AtlasInstance;

/// A utility widget that displays what's going on inside of Atlas.
///
/// To use it, just drag it onto your GUI and it will connect to the
/// first Atlas instance it finds on the server connection.  The control
/// will monitor all geometry and texture TOCs contained in the Atlas
/// file connected to the Atlas instance.
///
/// Please be aware that monitoring is not without its quirks.
/// Sampling a system means interacting with a system.  Also, since
/// the GUI runs within the same frame loop as Atlas, you will always see
/// a snapshot of a certain fixed point in Atlas's operation.  This is
/// important to keep in mind when evaluating the results of monitoring.
///
/// @note This control is agnostic as to the type of terrain used (blended
///    or unique).

class GuiAtlasMonitorCtrl : public GuiContainer
{
	typedef GuiContainer Parent;

protected:
	/// Enumeration of different stats that we can display.
	enum EStat
	{
        STAT_NumRequests,
        STAT_BytesPending,
        STAT_BytesLoading,
        STAT_LoadPending,
        STAT_UnpackPending,
        STAT_InstatePending,
        STAT_NumRequestsDOA,
        NUM_STATS
	};

	enum ETOCType
	{
		TOC_Geometry,
		TOC_Texture
	};

	/// Bookkeeping for a single TOC in the Atlas file we are
	/// monitoring.
	struct TOCMonitor
	{
		ETOCType                mType;
		const char*             mName;         ///< Name string displayed on top of monitor.
		AtlasTOC*               mTOC;          ///< The TOC we are monitoring.
		Vector< GFXTexHandle >  mTextures;     ///< One texture for each of the LOD levels.
		Point2I                 mExtent;       ///< The region the texture TOC monitor would cover if rendered with a 1:1 resolution.
		U32                     mDroppedLevels;///< The number of coarsest LODs to omit when displaying.

		~TOCMonitor();

		void init( ETOCType mType, const char* name, AtlasTOC* toc, U32 droppedLevels );
		void update();
		void render( GuiAtlasMonitorCtrl* ctrl, Point2I offset, Point2I extent );

	private:
		U32 getStubState( U32 level, Point2I pos );
	};

	SimObjectPtr< AtlasInstance > mAtlasInstance;	///< The Atlas instance we are monitoring.
	Vector< TOCMonitor >			   mTOCMonitors;
    AtlasFile::IOStatus           mIOStatus;

	void cleanup();
	void initialize();

	void initTOCMonitors();
	void freeTOCMonitors();

	void renderStats( Point2I offset, Point2I extent );
	void getStat( EStat stat, char* buffer, U32 bufferSize );

public:
	DECLARE_CONOBJECT( GuiAtlasMonitorCtrl );

	virtual ~GuiAtlasMonitorCtrl()
	{
		if( mAtlasInstance )
			cleanup();
	}

	virtual bool onAdd();
	virtual void onRemove();
	virtual bool onWake();
	virtual void onSleep();
	virtual void onRender( Point2I offset, const RectI& updateRect );
};

#endif // _GUIATLASMONITORCTRL_H_
