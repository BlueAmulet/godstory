//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "core/frameAllocator.h"
#include "core/fileStream.h"

#include "console/consoleTypes.h"
#include "gui/core/guiCanvas.h"
#include "gui/editor/guiEditCtrl.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
////#define STATEBLOCK
GFXStateBlock* GuiEditCtrl::mAlphaBlendSB = NULL;

IMPLEMENT_CONOBJECT(GuiEditCtrl);

GuiEditCtrl::GuiEditCtrl(): mCurrentAddSet(NULL),
mContentControl(NULL),
mGridSnap(0,0),
mDragBeginPoint(-1,-1)                            
{
	VECTOR_SET_ASSOCIATION(mSelectedControls);
	VECTOR_SET_ASSOCIATION(mDragBeginPoints);
	mSelectedControls.clear();
	mDragBeginPoints.clear();
	mActive = true;
}

bool GuiEditCtrl::onAdd()
{
	if(!Parent::onAdd())
		return false;

	mTrash.registerObject();
	mSelectedSet.registerObject();
	mUndoManager.registerObject();

	return true;
}

void GuiEditCtrl::onRemove()
{
	Parent::onRemove();
	mTrash.unregisterObject();
	mSelectedSet.unregisterObject();
	mUndoManager.unregisterObject();
}

ConsoleMethod( GuiEditCtrl, setRoot, void, 3, 3, "(GuiControl root)")
{
	GuiControl *ctrl;
	if(!Sim::findObject(argv[2], ctrl))
		return;
	object->setRoot(ctrl);
}


ConsoleMethod( GuiEditCtrl, addNewCtrl, void, 3, 3, "(GuiControl ctrl)")
{
	GuiControl *ctrl;
	if(!Sim::findObject(argv[2], ctrl))
		return;
	object->addNewControl(ctrl);
}
ConsoleMethod( GuiEditCtrl, addSelection, void, 3, 3, "selects a control.")
{
	S32 id = dAtoi(argv[2]);
	object->addSelection(id);
}
ConsoleMethod( GuiEditCtrl, removeSelection, void, 3, 3, "deselects a control.")
{
	S32 id = dAtoi(argv[2]);
	object->removeSelection(id);
}

ConsoleMethod( GuiEditCtrl, clearSelection, void, 2, 2, "Clear selected controls list.")
{
	object->clearSelection();
}
ConsoleMethod( GuiEditCtrl, select, void, 3, 3, "(GuiControl ctrl)")
{
	GuiControl *ctrl;

	if(!Sim::findObject(argv[2], ctrl))
		return;

	object->setSelection(ctrl, false);
}

ConsoleMethod( GuiEditCtrl, setCurrentAddSet, void, 3, 3, "(GuiControl ctrl)")
{
	GuiControl *addSet;

	if (!Sim::findObject(argv[2], addSet))
	{
		Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
		return;
	}
	object->setCurrentAddSet(addSet);
}

ConsoleMethod( GuiEditCtrl, getCurrentAddSet, S32, 2, 2, "Returns the set to which new controls will be added")
{
	const GuiControl* add = object->getCurrentAddSet();
	return add ? add->getId() : 0;
}

ConsoleMethod( GuiEditCtrl, toggle, void, 2, 2, "Toggle activation.")
{
	object->setEditMode(! object->mActive);
}

ConsoleMethod( GuiEditCtrl, justify, void, 3, 3, "(int mode)" )
{
	object->justifySelection((GuiEditCtrl::Justification)dAtoi(argv[2]));
}

ConsoleMethod( GuiEditCtrl, bringToFront, void, 2, 2, "")
{
	object->bringToFront();
}

ConsoleMethod( GuiEditCtrl, pushToBack, void, 2, 2, "")
{
	object->pushToBack();
}

ConsoleMethod( GuiEditCtrl, deleteSelection, void, 2, 2, "Delete the selected text.")
{
	object->deleteSelection();
}

ConsoleMethod( GuiEditCtrl, moveSelection, void, 4, 4, "(int deltax, int deltay)")
{
	object->moveAndSnapSelection(Point2I(dAtoi(argv[2]), dAtoi(argv[3])));
}

ConsoleMethod( GuiEditCtrl, saveSelection, void, 3, 3, "(string fileName)")
{
	object->saveSelection(argv[2]);
}

ConsoleMethod( GuiEditCtrl, loadSelection, void, 3, 3, "(string fileName)")
{
	object->loadSelection(argv[2]);
}

ConsoleMethod( GuiEditCtrl, selectAll, void, 2, 2, "()")
{
	object->selectAll();
}


ConsoleMethod( GuiEditCtrl, getSelected, S32, 2, 2, "() - Gets the GUI control(s) the editor is currently selecting" )
{
	return object->getSelectedSet().getId();
}

ConsoleMethod( GuiEditCtrl, getTrash, S32, 2, 2, "() - Gets the GUI controls(s) that are currently in the trash.")
{
	return object->getTrash().getId();
}

ConsoleMethod( GuiEditCtrl, getUndoManager, S32, 2, 2, "() - Gets the Gui Editor's UndoManager object")
{
	return object->getUndoManager().getId();
}

bool GuiEditCtrl::onWake()
{
	if (! Parent::onWake())
		return false;

	// Set GUI Controls to DesignTime mode
	GuiControl::smDesignTime = true;
	GuiControl::smEditorHandle = this;

	setEditMode(true);

	// TODO: paxorr: I'm not sure this is the best way to set these defaults.
	bool snap = Con::getBoolVariable("$pref::GuiEditor::snap2grid",false);
	U32 grid = Con::getIntVariable("$pref::GuiEditor::snap2gridsize",8);
	Con::setBoolVariable("$pref::GuiEditor::snap2grid", snap);
	Con::setIntVariable("$pref::GuiEditor::snap2gridsize",grid);

	setSnapToGrid( snap ? grid : 0);

	return true;
}

void GuiEditCtrl::onSleep()
{
	// Set GUI Controls to run time mode
	GuiControl::smDesignTime = false;
	GuiControl::smEditorHandle = NULL;

	Parent::onSleep();
}
void GuiEditCtrl::setRoot(GuiControl *root)
{
	mContentControl = root;
	if( root != NULL ) root->mIsContainer = true;
	mCurrentAddSet = mContentControl;
	clearSelection();
}

enum GuiEditConstants {
	GUI_BLACK = 0,
	GUI_WHITE = 255,
	NUT_SIZE = 4
};

void GuiEditCtrl::setEditMode(bool value)
{
	mActive = value;

	clearSelection();
	if (mActive && mAwake)
		mCurrentAddSet = NULL;
}

void GuiEditCtrl::setCurrentAddSet(GuiControl *ctrl, bool doclearSelection)
{
	if (ctrl != mCurrentAddSet)
	{
		if(doclearSelection)
			clearSelection();

		mCurrentAddSet = ctrl;
	}
}

const GuiControl* GuiEditCtrl::getCurrentAddSet() const
{
	return mCurrentAddSet ? mCurrentAddSet : mContentControl;
}

void GuiEditCtrl::clearSelection(void)
{
	mSelectedControls.clear();
	if(isMethod("onClearSelected"))
		Con::executef(this, "onClearSelected");
}
void GuiEditCtrl::setSelection(GuiControl *ctrl, bool inclusive)
{
	//sanity check
	if (! ctrl)
		return;

	if(mContentControl == ctrl)
	{
		mCurrentAddSet = ctrl;
		clearSelection();
	}
	else
	{
		// otherwise, we hit a new control...
		GuiControl *newAddSet = ctrl->getParent();

		//see if we should clear the old selection set
		if (newAddSet != mCurrentAddSet || (! inclusive)) 
		{
			clearSelection();
		}

		//set the selection
		mCurrentAddSet = newAddSet;
		if (!(ctrl->isLocked())) 
		{
			mSelectedControls.push_back(ctrl);
			Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
		}
	}
}

void GuiEditCtrl::addNewControl(GuiControl *ctrl)
{
	if (! mCurrentAddSet)
		mCurrentAddSet = mContentControl;

	mCurrentAddSet->addObject(ctrl);
	clearSelection();
	if (!(ctrl->isLocked())) {
		mSelectedControls.push_back(ctrl);
		Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
	}
	// undo
	Con::executef(this, "onAddNewCtrl", Con::getIntArg(ctrl->getId()));
}

void GuiEditCtrl::drawNut(const Point2I &nut, ColorI &outlineColor, ColorI &nutColor)
{
	RectI r(nut.x - NUT_SIZE, nut.y - NUT_SIZE, 2 * NUT_SIZE, 2 * NUT_SIZE);
	r.inset(1, 1);
	GFX->getDrawUtil()->drawRectFill(r, nutColor);
	r.inset(-1, -1);
	GFX->getDrawUtil()->drawRect(r, outlineColor);
}

static inline bool inNut(const Point2I &pt, S32 x, S32 y)
{
	S32 dx = pt.x - x;
	S32 dy = pt.y - y;
	return dx <= NUT_SIZE && dx >= -NUT_SIZE && dy <= NUT_SIZE && dy >= -NUT_SIZE;
}

S32 GuiEditCtrl::getSizingHitKnobs(const Point2I &pt, const RectI &box)
{
	S32 lx = box.point.x, rx = box.point.x + box.extent.x - 1;
	S32 cx = (lx + rx) >> 1;
	S32 ty = box.point.y, by = box.point.y + box.extent.y - 1;
	S32 cy = (ty + by) >> 1;

	// adjust nuts, so they dont straddle the controls
	lx -= NUT_SIZE;
	ty -= NUT_SIZE;
	rx += NUT_SIZE;
	by += NUT_SIZE;

	if (inNut(pt, lx, ty))
		return sizingLeft | sizingTop;
	if (inNut(pt, cx, ty))
		return sizingTop;
	if (inNut(pt, rx, ty))
		return sizingRight | sizingTop;
	if (inNut(pt, lx, by))
		return sizingLeft | sizingBottom;
	if (inNut(pt, cx, by))
		return sizingBottom;
	if (inNut(pt, rx, by))
		return sizingRight | sizingBottom;
	if (inNut(pt, lx, cy))
		return sizingLeft;
	if (inNut(pt, rx, cy))
		return sizingRight;
	return sizingNone;
}

void GuiEditCtrl::drawNuts(RectI &box, ColorI &outlineColor, ColorI &nutColor)
{
	GFXDrawUtil *drawer = GFX->getDrawUtil();

	S32 lx = box.point.x, rx = box.point.x + box.extent.x - 1;
	S32 cx = (lx + rx) >> 1;
	S32 ty = box.point.y, by = box.point.y + box.extent.y - 1;
	S32 cy = (ty + by) >> 1;

	ColorF greenLine(0.0f, 1.0f, 0.0f, 0.6f);
	ColorF lightGreenLine(0.0f, 1.0f, 0.0f, 0.3f);

	if(lx > 0 && ty > 0)
	{
		drawer->drawLine(0, ty, lx, ty, greenLine);
		drawer->drawLine(lx, 0, lx, ty, greenLine);
	}

	if(lx > 0 && by > 0)
		drawer->drawLine(0, by, lx, by, greenLine);

	if(rx > 0 && ty > 0)
		drawer->drawLine(rx, 0, rx, ty, greenLine);

	Point2I extent = localToGlobalCoord(getExtent());

	if(lx < extent.x && by < extent.y)
		drawer->drawLine(lx, by, lx, extent.y, lightGreenLine);
	if(rx < extent.x && by < extent.y)
	{
		drawer->drawLine(rx, by, rx, extent.y, lightGreenLine);
		drawer->drawLine(rx, by, extent.x, by, lightGreenLine);
	}
	if(rx < extent.x && ty < extent.y)
		drawer->drawLine(rx, ty, extent.x, ty, lightGreenLine);

	// adjust nuts, so they dont straddle the controlslx -= NUT_SIZE;
	lx -= NUT_SIZE;
	ty -= NUT_SIZE;
	rx += NUT_SIZE;
	by += NUT_SIZE;

	drawNut(Point2I(lx, ty), outlineColor, nutColor);
	drawNut(Point2I(lx, cy), outlineColor, nutColor);
	drawNut(Point2I(lx, by), outlineColor, nutColor);
	drawNut(Point2I(rx, ty), outlineColor, nutColor);
	drawNut(Point2I(rx, cy), outlineColor, nutColor);
	drawNut(Point2I(rx, by), outlineColor, nutColor);
	drawNut(Point2I(cx, ty), outlineColor, nutColor);
	drawNut(Point2I(cx, by), outlineColor, nutColor);
}

void GuiEditCtrl::getDragRect(RectI &box)
{
	box.point.x = getMin(mLastMousePos.x, mSelectionAnchor.x);
	box.extent.x = getMax(mLastMousePos.x, mSelectionAnchor.x) - box.point.x + 1;
	box.point.y = getMin(mLastMousePos.y, mSelectionAnchor.y);
	box.extent.y = getMax(mLastMousePos.y, mSelectionAnchor.y) - box.point.y + 1;
}

void GuiEditCtrl::onPreRender()
{
	setUpdate();
}

void GuiEditCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	m_DirtyFlag = false;

	Point2I ctOffset;
	Point2I cext;
	bool keyFocused = isFirstResponder();

	GFXDrawUtil *drawer = GFX->getDrawUtil();

	if (mActive)
	{
		if (mCurrentAddSet)
		{
			// draw a white frame inset around the current add set.
			cext = mCurrentAddSet->getExtent();
			ctOffset = mCurrentAddSet->localToGlobalCoord(Point2I(0,0));
			RectI box(ctOffset.x, ctOffset.y, cext.x, cext.y);

			box.inset(-5, -5);
			drawer->drawRect(box, ColorI(50, 101, 152,160));
			box.inset(1,1);
			drawer->drawRect(box, ColorI(50, 101, 152,170));
			box.inset(1,1);
			drawer->drawRect(box, ColorI(50, 101, 152,180));
			box.inset(1,1);
			drawer->drawRect(box, ColorI(50, 101, 152,190));
			box.inset(1,1);
			drawer->drawRect(box, ColorI(50, 101, 152,200));
		}
		Vector<GuiControl *>::iterator i;
		bool multisel = mSelectedControls.size() > 1;
		for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
		{
			GuiControl *ctrl = (*i);
			cext = ctrl->getExtent();
			ctOffset = ctrl->localToGlobalCoord(Point2I(0,0));
			RectI box(ctOffset.x,ctOffset.y, cext.x, cext.y);
			ColorI nutColor = multisel ? ColorI(255,255,255) : ColorI(0,0,0);
			ColorI outlineColor = multisel ? ColorI(0,0,0) : ColorI(255,255,255);
			if(!keyFocused)
				nutColor.set(128,128,128);

			drawNuts(box, outlineColor, nutColor);
		}
		if (mMouseDownMode == DragSelecting)
		{
			RectI b;
			getDragRect(b);
			b.point += offset;
			drawer->drawRect(b, ColorI(255, 255, 255));
		}
	}

	renderChildControls(offset, updateRect);

	if(mActive && mCurrentAddSet && (mGridSnap.x && mGridSnap.y) && 
		(mMouseDownMode == MovingSelection || mMouseDownMode == SizingSelection))
	{
		Point2I cext = mCurrentAddSet->getExtent();
		Point2I coff = mCurrentAddSet->localToGlobalCoord(Point2I(0,0));
		// create point-dots
		Point2I snap = mGridSnap;
		if(snap.x < 6)
			snap *= 2;
		if(snap.x < 6)
			snap *= 2;
		U32 maxdot = (U32)(mCeil(cext.x / (F32)snap.x) - 1) * (U32)(mCeil(cext.y / (F32)snap.y) - 1);


		if( mDots.isNull() || maxdot != mDots->mNumVerts)
		{
			mDots.set(GFX, maxdot, GFXBufferTypeStatic);

			U32 ndot = 0;
			mDots.lock();
			for(U32 ix = snap.x; ix < cext.x; ix += snap.x)
			{ 
				for(U32 iy = snap.y; ndot < maxdot && iy < cext.y; iy += snap.y)
				{
					mDots[ndot].color.set( 50, 50, 254, 200 );
					mDots[ndot].point.x = F32(ix + coff.x);
					mDots[ndot].point.y = F32(iy + coff.y);
					ndot++;
				}
			}
			mDots.unlock();
			AssertFatal(ndot <= maxdot, "dot overflow");
			AssertFatal(ndot == maxdot, "dot underflow");
		}
#ifdef STATEBLOCK
		AssertFatal(mAlphaBlendSB, "GuiEditCtrl::onRender -- mAlphaBlendSB cannot be NULL.");
		mAlphaBlendSB->apply();
#else
		// draw the points.
		GFX->setAlphaBlendEnable( true );
		GFX->setSrcBlend( GFXBlendSrcAlpha );
		GFX->setDestBlend( GFXBlendInvSrcAlpha );
		// CodeReview: This seems to be a very heavy-handed onRender method [7/2/2007 Pat]
		for(S32 i=GFX->getNumSamplers(); i>=0; i--)
		{
			GFX->setTextureStageColorOp(i, GFXTOPDisable);
		}

#endif
		GFX->setVertexBuffer( mDots );
		GFX->drawPrimitive( GFXPointList, 0, mDots->mNumVerts );
	}
}

bool GuiEditCtrl::selectionContains(GuiControl *ctrl)
{
	Vector<GuiControl *>::iterator i;
	for (i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
		if (ctrl == *i) return true;
	return false;
}

void GuiEditCtrl::onRightMouseDown(const GuiEvent &event)
{
	if (! mActive || !mContentControl)
	{
		Parent::onRightMouseDown(event);
		return;
	}
	setFirstResponder();

	//search for the control hit in any layer below the edit layer
	GuiControl *hitCtrl = mContentControl->findHitControl(globalToLocalCoord(event.mousePoint), mLayer - 1);
	if (hitCtrl != mCurrentAddSet)
	{
		clearSelection();
		mCurrentAddSet = hitCtrl;
	}
	// select the parent if we right-click on the current add set 
	else if( mCurrentAddSet != mContentControl)
	{
		mCurrentAddSet = hitCtrl->getParent();
		select(hitCtrl);
	}

	//Design time mouse events
	GuiEvent designEvent = event;
	designEvent.mousePoint = mLastMousePos;
	hitCtrl->onRightMouseDownEditor( designEvent, localToGlobalCoord( Point2I(0,0) ) );

}
void GuiEditCtrl::select(GuiControl *ctrl)
{
	clearSelection();
	if(ctrl != mContentControl) {
		if (!(ctrl->isLocked())) {
			mSelectedControls.push_back(ctrl);
			Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
		}
	}
	else
		mCurrentAddSet = mContentControl;
}

void GuiEditCtrl::getCursor(GuiCursor *&cursor, bool &showCursor, const GuiEvent &lastGuiEvent)
{
	GuiCanvas *pRoot = getRoot();
	if( !pRoot )
		return;

	showCursor = false;
	cursor = NULL;

	Point2I ctOffset;
	Point2I cext;
	GuiControl *ctrl;

	Point2I mousePos  = globalToLocalCoord(lastGuiEvent.mousePoint);

	PlatformWindow *pWindow = static_cast<GuiCanvas*>(getRoot())->getPlatformWindow();
	AssertFatal(pWindow != NULL,"GuiControl without owning platform window!  This should not be possible.");
	PlatformCursorController *pController = pWindow->getCursorController();
	AssertFatal(pController != NULL,"PlatformWindow without an owned CursorController!");

	S32 desiredCursor = PlatformCursorController::curArrow;

	// first see if we hit a sizing knob on the currently selected control...
	if (mSelectedControls.size() == 1 )
	{
		ctrl = mSelectedControls.first();
		cext = ctrl->getExtent();
		ctOffset = globalToLocalCoord(ctrl->localToGlobalCoord(Point2I(0,0)));
		RectI box(ctOffset.x,ctOffset.y,cext.x, cext.y);

		GuiEditCtrl::sizingModes sizeMode = (GuiEditCtrl::sizingModes)getSizingHitKnobs(mousePos, box);

		if( mMouseDownMode == SizingSelection )
		{
			if ( ( mSizingMode == ( sizingBottom | sizingRight ) ) || ( mSizingMode == ( sizingTop | sizingLeft ) ) )
				desiredCursor = PlatformCursorController::curResizeNWSE;
			else if (  ( mSizingMode == ( sizingBottom | sizingLeft ) ) || ( mSizingMode == ( sizingTop | sizingRight ) ) )
				desiredCursor = PlatformCursorController::curResizeNESW;
			else if ( mSizingMode == sizingLeft || mSizingMode == sizingRight ) 
				desiredCursor = PlatformCursorController::curResizeVert;
			else if (mSizingMode == sizingTop || mSizingMode == sizingBottom )
				desiredCursor = PlatformCursorController::curResizeHorz;
		}
		else
		{
			// Check for current mouse position after checking for actual sizing mode
			if ( ( sizeMode == ( sizingBottom | sizingRight ) ) || ( sizeMode == ( sizingTop | sizingLeft ) ) )
				desiredCursor = PlatformCursorController::curResizeNWSE;
			else if ( ( sizeMode == ( sizingBottom | sizingLeft ) ) || ( sizeMode == ( sizingTop | sizingRight ) ) )
				desiredCursor = PlatformCursorController::curResizeNESW;
			else if (sizeMode == sizingLeft || sizeMode == sizingRight )
				desiredCursor = PlatformCursorController::curResizeVert;
			else if (sizeMode == sizingTop || sizeMode == sizingBottom )
				desiredCursor = PlatformCursorController::curResizeHorz;
		}
	}

	if( mMouseDownMode == MovingSelection && cursor == NULL )
		desiredCursor = PlatformCursorController::curResizeAll;

	if( pRoot->mCursorChanged != desiredCursor )
	{
		// We've already changed the cursor, 
		// so set it back before we change it again.
		if(pRoot->mCursorChanged != -1)
			pController->popCursor();

		// Now change the cursor shape
		pController->pushCursor(pController->getSysCursor(desiredCursor));
		pRoot->mCursorChanged = desiredCursor;
	}
}

void GuiEditCtrl::onMouseDown(const GuiEvent &event)
{
	if (! mActive)
	{
		Parent::onMouseDown(event);
		return;
	}
	if(!mContentControl)
		return;

	setFirstResponder();
	//lock the mouse
	mouseLock();

	Point2I ctOffset;
	Point2I cext;
	GuiControl *ctrl;

	mLastMousePos = globalToLocalCoord(event.mousePoint);

	// first see if we hit a sizing knob on the currently selected control...
	if (mSelectedControls.size() == 1)
	{
		ctrl = mSelectedControls.first();
		cext = ctrl->getExtent();
		ctOffset = globalToLocalCoord(ctrl->localToGlobalCoord(Point2I(0,0)));
		RectI box(ctOffset.x,ctOffset.y,cext.x, cext.y);

		if ((mSizingMode = (GuiEditCtrl::sizingModes)getSizingHitKnobs(mLastMousePos, box)) != 0)
		{
			mMouseDownMode = SizingSelection;
			// undo
			Con::executef(this, "onPreEdit", Con::getIntArg(getSelectedSet().getId()));
			return;
		}
	}

	if(!mCurrentAddSet)
		mCurrentAddSet = mContentControl;

	//find the control we clicked
	ctrl = mContentControl->findHitControl(mLastMousePos, mCurrentAddSet->mLayer);

	bool handledEvent = ctrl->onMouseDownEditor( event, localToGlobalCoord( Point2I(0,0) ) );
	if( handledEvent == true )
	{
		// The Control handled the event and requested the edit ctrl
		// *NOT* act on it.  The dude abides.
		return;
	}
	else if ( selectionContains(ctrl) )
	{
		//if we're holding shift, de-select the clicked ctrl
		if (event.modifier & SI_SHIFT)
		{
			Vector<GuiControl *>::iterator i;
			for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			{
				if (*i == ctrl)
				{
					Con::executef(this, "onRemoveSelected", Con::getIntArg(ctrl->getId()));
					break;
				}
			}

			//set the mode
			mMouseDownMode = Selecting;
		}
		else //else we hit a ctrl we've already selected, so set the mode to moving
		{
			// For calculating mouse delta
			mDragBeginPoint = event.mousePoint;

			// Allocate enough space for our selected controls
			mDragBeginPoints.reserve( mSelectedControls.size() );

			// For snapping to origin
			Vector<GuiControl *>::iterator i;
			for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
				mDragBeginPoints.push_back( (*i)->getPosition() );

			// Set Mouse Mode
			mMouseDownMode = MovingSelection;
			// undo
			Con::executef(this, "onPreEdit", Con::getIntArg(getSelectedSet().getId()));
		}
	}

	//else we clicked on an unselected control
	else
	{
		//if we clicked in the current add set
		if (ctrl == mCurrentAddSet)
		{
			// start dragging a rectangle
			// if the shift is not down, nuke prior selection
			if (!(event.modifier & SI_SHIFT)) {
				clearSelection();
			}
			mSelectionAnchor = mLastMousePos;
			mMouseDownMode = DragSelecting;
		}
		else
		{
			//find the new add set
			GuiControl *newAddSet = ctrl->getParent();

			//if we're holding shift and the ctrl is in the same add set
			if (event.modifier & SI_SHIFT && newAddSet == mCurrentAddSet)
			{            	
				if (!(ctrl->isLocked())) {
					mSelectedControls.push_back(ctrl);
					Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
				}
				mMouseDownMode = Selecting;
			}
			else if (ctrl != mContentControl)
			{
				//find and set the new add set
				mCurrentAddSet = ctrl->getParent();

				//clear and set the selected controls
				clearSelection();
				if (!(ctrl->isLocked())) {
					mSelectedControls.push_back(ctrl);
					Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
				}
				mMouseDownMode = Selecting;
			}
			else
				mMouseDownMode = Selecting;
		}
	}
}
void GuiEditCtrl::addSelection(S32 id)
{
	GuiControl * ctrl;
	if(Sim::findObject(id, ctrl))
		mSelectedControls.push_back(ctrl);

}
void GuiEditCtrl::removeSelection(S32 id)
{
	GuiControl * ctrl;
	if (Sim::findObject(id, ctrl)) {
		Vector<GuiControl *>::iterator i;
		for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
		{
			if (*i == ctrl)
			{
				mSelectedControls.erase(i);
				break;
			}
		}
	}
}
void GuiEditCtrl::onMouseUp(const GuiEvent &event)
{
	if (! mActive || !mContentControl || !mCurrentAddSet )
	{
		Parent::onMouseUp(event);
		return;
	}

	//find the control we clicked
	GuiControl *ctrl = mContentControl->findHitControl(mLastMousePos, mCurrentAddSet->mLayer);

	bool handledEvent = ctrl->onMouseUpEditor( event, localToGlobalCoord( Point2I(0,0) ) );
	if( handledEvent == true )
	{
		// The Control handled the event and requested the edit ctrl
		// *NOT* act on it.  The dude abides.
		return;
	}

	//unlock the mouse
	mouseUnlock();

	// Reset Drag Axis Alignment Information
	mDragBeginPoint.set(-1,-1);
	mDragBeginPoints.clear();

	mLastMousePos = globalToLocalCoord(event.mousePoint);
	if (mMouseDownMode == DragSelecting)
	{
		RectI b;
		getDragRect(b);
		GuiControl::iterator i;
		for(i = mCurrentAddSet->begin(); i != mCurrentAddSet->end(); i++)
		{
			GuiControl *ctrl = dynamic_cast<GuiControl *>(*i);
			Point2I upperL = globalToLocalCoord(ctrl->localToGlobalCoord(Point2I(0,0)));
			Point2I lowerR = upperL + ctrl->getExtent() - Point2I(1, 1);

			if (b.pointInRect(upperL) && b.pointInRect(lowerR) && !selectionContains(ctrl)) {
				if (!(ctrl->isLocked())) {
					mSelectedControls.push_back(ctrl);
					Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
				}
			}
		}
	}
	if (mSelectedControls.size() == 1)
		Con::executef(this, "onSelect", Con::getIntArg(mSelectedControls[0]->getId()));


	// deliver post edit event if we've been editing
	// note: paxorr: this may need to be moved earlier, if the selection has changed.
	// undo
	if(mMouseDownMode == SizingSelection || mMouseDownMode == MovingSelection)
		Con::executef(this, "onPostEdit", Con::getIntArg(getSelectedSet().getId()));

	//reset the mouse mode
	setFirstResponder();
	mMouseDownMode = Selecting;
}

void GuiEditCtrl::onMouseDragged(const GuiEvent &event)
{
	if (! mActive || !mContentControl || !mCurrentAddSet)
	{
		Parent::onMouseDragged(event);
		return;
	}

	if(!mCurrentAddSet)
		mCurrentAddSet = mContentControl;

	Point2I mousePoint = globalToLocalCoord(event.mousePoint);

	//find the control we clicked
	GuiControl *ctrl = mContentControl->findHitControl(mousePoint, mCurrentAddSet->mLayer);

	bool handledEvent = ctrl->onMouseDraggedEditor( event, localToGlobalCoord( Point2I(0,0) ) );
	if( handledEvent == true )
	{
		// The Control handled the event and requested the edit ctrl
		// *NOT* act on it.  The dude abides.
		return;
	}

	if (mMouseDownMode == SizingSelection)
	{
		if (mGridSnap.x)
			mousePoint.x -= mousePoint.x % mGridSnap.x;
		if (mGridSnap.y)
			mousePoint.y -= mousePoint.y % mGridSnap.y;

		GuiControl *ctrl = mSelectedControls.first();
		Point2I ctrlPoint = mCurrentAddSet->globalToLocalCoord(event.mousePoint);
		if (mGridSnap.x)
			ctrlPoint.x -= ctrlPoint.x % mGridSnap.x;
		if (mGridSnap.y)
			ctrlPoint.y -= ctrlPoint.y % mGridSnap.y;

		Point2I newPosition = ctrl->getPosition();
		Point2I newExtent = ctrl->getExtent();
		Point2I minExtent = ctrl->getMinExtent();

		RectI ctrlBounds = ctrl->getBounds();

		if (mSizingMode & sizingLeft)
		{
			newPosition.x = ctrlPoint.x;
			newExtent.x = newExtent.x + newPosition.x - ctrlPoint.x;

			if(newExtent.x < minExtent.x)
			{
				newPosition.x -= minExtent.x - newExtent.x;
				newExtent.x = minExtent.x;
			}
		}
		else if (mSizingMode & sizingRight)
		{
			newExtent.x = ctrlPoint.x - newPosition.x;
			if(mGridSnap.x)
				newExtent.x -= newExtent.x % mGridSnap.x;

			if(newExtent.x < minExtent.x)
				newExtent.x = minExtent.x;
		}

		if (mSizingMode & sizingTop)
		{
			newPosition.y = ctrlPoint.y;
			newExtent.y = newExtent.y + newPosition.y - ctrlPoint.y;
			if(newExtent.y < minExtent.y)
			{
				newPosition.y -= minExtent.y - newExtent.y;
				newExtent.y = minExtent.y;
			}
		}
		else if (mSizingMode & sizingBottom)
		{
			newExtent.y = ctrlPoint.y - newPosition.y;
			if(newExtent.y < minExtent.y)
				newExtent.y = minExtent.y;
		}

		if(mGridSnap.x)
		{
			newPosition.x -= newPosition.x % mGridSnap.x;
			newExtent.x -= newExtent.x % mGridSnap.x;
		}
		if(mGridSnap.y)
		{
			newPosition.y -= newPosition.y % mGridSnap.y;
			newExtent.y -= newExtent.y % mGridSnap.y;
		}

		ctrl->resize(newPosition, newExtent);
		mCurrentAddSet->childResized(ctrl);
		if(mSelectedControls.size() > 0)
			Con::executef(this, "onSelect", Con::getIntArg(mSelectedControls[0]->getId()));
	}
	else if (mMouseDownMode == MovingSelection && mSelectedControls.size())
	{
		Vector<GuiControl *>::iterator i = mSelectedControls.begin();
		Point2I minPos = (*i)->getPosition();
		for(; i != mSelectedControls.end(); i++)
		{
			Point2I iPos = (**i).getPosition();
			if (iPos.x < minPos.x)
				minPos.x = iPos.x;
			if (iPos.y < minPos.y)
				minPos.y = iPos.y;
		}
		Point2I delta = mousePoint - mLastMousePos;
		delta += minPos; // find new minPos;

		if (mGridSnap.x)
			delta.x -= delta.x % mGridSnap.x;
		if (mGridSnap.y)
			delta.y -= delta.y % mGridSnap.y;

		delta -= minPos;

		// Do we want to align this drag to the X and Y axes within a certain threshold?
		if( event.modifier & SI_SHIFT )
		{
			Point2I dragTotalDelta = event.mousePoint - mDragBeginPoint;
			if( dragTotalDelta.y < 10 && dragTotalDelta.y > -10 )
			{
				for(S32 i = 0; i < mSelectedControls.size(); i++)
				{
					Point2I selCtrlPos = mSelectedControls[i]->getPosition();
					Point2I snapBackPoint( selCtrlPos.x, mDragBeginPoints[i].y);
					// This is kind of nasty but we need to snap back if we're not at origin point with selection - JDD
					if( selCtrlPos.y != mDragBeginPoints[i].y )
						mSelectedControls[i]->setPosition( snapBackPoint );
				}
				delta.y = 0;
			}
			if( dragTotalDelta.x < 10 && dragTotalDelta.x > -10 )
			{
				for(S32 i = 0; i < mSelectedControls.size(); i++)
				{
					Point2I selCtrlPos = mSelectedControls[i]->getPosition();
					Point2I snapBackPoint( mDragBeginPoints[i].x, selCtrlPos.y);
					// This is kind of nasty but we need to snap back if we're not at origin point with selection - JDD
					if( selCtrlPos.x != mDragBeginPoints[i].x )
						mSelectedControls[i]->setPosition( snapBackPoint );
				}
				delta.x = 0;
			}


		}

		moveSelection(delta);

		// find the current control under the mouse but not in the selected set.
		// setting a control invisible makes sure it wont be seen by findHitControl()
		for(int i = 0; i< mSelectedControls.size(); i++)
			mSelectedControls[i]->setVisible(false);
		GuiControl *inCtrl = mContentControl->findHitControl(mousePoint, mCurrentAddSet->mLayer);
		for(int i = 0; i< mSelectedControls.size(); i++)
			mSelectedControls[i]->setVisible(true);

		// find the nearest control up the heirarchy from the control the mouse is in
		// that is flagged as a container.
		/*while(! inCtrl->mIsContainer)
			inCtrl = inCtrl->getParent();*/

		if( inCtrl->getParent() && inCtrl->getParent()->mIsContainer )
			inCtrl  = inCtrl->getParent();

		// if the control under the mouse is not our parent, move the selected controls
		// into the new parent.
		if(mSelectedControls[0]->getParent() != inCtrl && inCtrl->mIsContainer)
		{
			moveSelectionToCtrl(inCtrl);
			setCurrentAddSet(inCtrl,false);
		}

		mLastMousePos += delta;
	}
	else
		mLastMousePos = mousePoint;
}

void GuiEditCtrl::moveSelectionToCtrl(GuiControl *newParent)
{
	for(int i = 0; i < mSelectedControls.size(); i++)
	{
		GuiControl* ctrl = mSelectedControls[i];
		if(ctrl->getParent() == newParent)
			continue;

		Point2I globalpos = ctrl->localToGlobalCoord(Point2I(0,0));
		newParent->addObject(ctrl);
		Point2I newpos = ctrl->globalToLocalCoord(globalpos) + ctrl->getPosition();
		ctrl->setPosition(newpos);
	}
}

static Point2I snapPoint(Point2I point, Point2I delta, Point2I gridSnap)
{ 
	S32 snap;
	if(gridSnap.x && delta.x)
	{
		snap = point.x % gridSnap.x;
		point.x -= snap;
		if(delta.x > 0 && snap != 0)
			point.x += gridSnap.x;
	}
	if(gridSnap.y && delta.y)
	{
		snap = point.y % gridSnap.y;
		point.y -= snap;
		if(delta.y > 0 && snap != 0)
			point.y += gridSnap.y;
	}
	return point;
}
//-----------------
void GuiEditCtrl::moveAndSnapSelection(const Point2I &delta)
{
	// move / nudge gets a special callback so that multiple small moves can be
	// coalesced into one large undo action.
	// undo
	Con::executef(this, "onPreSelectionNudged", Con::getIntArg(getSelectedSet().getId()));

	Vector<GuiControl *>::iterator i;
	Point2I newPos;
	for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
	{
		newPos = (*i)->getPosition() + delta;
		newPos = snapPoint(newPos, delta, mGridSnap);
		(*i)->setPosition(newPos);
	}

	// undo
	Con::executef(this, "onPostSelectionNudged", Con::getIntArg(getSelectedSet().getId()));

	// allow script to update the inspector
	if (mSelectedControls.size() == 1)
		Con::executef(this, "onSelectionMoved", Con::getIntArg(mSelectedControls[0]->getId()));
}

void GuiEditCtrl::moveSelection(const Point2I &delta)
{
	Vector<GuiControl *>::iterator i;
	for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
	{
		(*i)->setPosition((*i)->getPosition() + delta);
	}

	// allow script to update the inspector
	if (mSelectedControls.size() == 1)
		Con::executef(this, "onSelectionMoved", Con::getIntArg(mSelectedControls[0]->getId()));
}

void GuiEditCtrl::justifySelection(Justification j)
{
	S32 minX, maxX;
	S32 minY, maxY;
	S32 extentX, extentY;

	if (mSelectedControls.size() < 2)
		return;

	Vector<GuiControl *>::iterator i = mSelectedControls.begin();
	minX = (*i)->getLeft();
	maxX = minX + (*i)->getWidth();
	minY = (*i)->getTop();
	maxY = minY + (*i)->getHeight();
	extentX = (*i)->getWidth();
	extentY = (*i)->getHeight();
	i++;
	for(;i != mSelectedControls.end(); i++)
	{
		minX = getMin(minX, (*i)->getLeft());
		maxX = getMax(maxX, (*i)->getLeft() + (*i)->getWidth());
		minY = getMin(minY, (*i)->getTop());
		maxY = getMax(maxY, (*i)->getTop() + (*i)->getHeight());
		extentX += (*i)->getWidth();
		extentY += (*i)->getHeight();
	}
	S32 deltaX = maxX - minX;
	S32 deltaY = maxY - minY;
	switch(j)
	{
	case JUSTIFY_LEFT:
		for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			(*i)->setLeft( minX );
		break;
	case JUSTIFY_TOP:
		for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			(*i)->setTop( minY );
		break;
	case JUSTIFY_RIGHT:
		for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			(*i)->setLeft( maxX - (*i)->getWidth() + 1 );
		break;
	case JUSTIFY_BOTTOM:
		for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			(*i)->setTop( maxY - (*i)->getHeight() + 1 );
		break;
	case JUSTIFY_CENTER:
		for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			(*i)->setLeft( minX + ((deltaX - (*i)->getWidth()) >> 1 ));
		break;
	case SPACING_VERTICAL:
		{
			Vector<GuiControl *> sortedList;
			Vector<GuiControl *>::iterator k;
			for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			{
				for(k = sortedList.begin(); k != sortedList.end(); k++)
				{
					if ((*i)->getTop() < (*k)->getTop())
						break;
				}
				sortedList.insert(k, *i);
			}
			S32 space = (deltaY - extentY) / (mSelectedControls.size() - 1);
			S32 curY = minY;
			for(k = sortedList.begin(); k != sortedList.end(); k++)
			{
				(*k)->setTop( curY );
				curY += (*k)->getHeight() + space;
			}
		}
		break;
	case SPACING_HORIZONTAL:
		{
			Vector<GuiControl *> sortedList;
			Vector<GuiControl *>::iterator k;
			for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
			{
				for(k = sortedList.begin(); k != sortedList.end(); k++)
				{
					if ((*i)->getLeft() < (*k)->getLeft())
						break;
				}
				sortedList.insert(k, *i);
			}
			S32 space = (deltaX - extentX) / (mSelectedControls.size() - 1);
			S32 curX = minX;
			for(k = sortedList.begin(); k != sortedList.end(); k++)
			{
				(*k)->setLeft( curX );
				curX += (*k)->getWidth() + space;
			}
		}
		break;
	}
}

void GuiEditCtrl::deleteSelection(void)
{
	// undo
	Con::executef(this, "onTrashSelection", Con::getIntArg(getSelectedSet().getId()));

	Vector<GuiControl *>::iterator i;
	for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
	{
		mTrash.addObject(*i);
	}
	clearSelection();
}

void GuiEditCtrl::loadSelection(const char* filename)
{
	if (! mCurrentAddSet)
		mCurrentAddSet = mContentControl;

	Con::executef("exec", filename);
	SimSet *set;
	if(!Sim::findObject("guiClipboard", set))
		return;

	if(set->size())
	{
		clearSelection();
		for(U32 i = 0; i < set->size(); i++)
		{
			GuiControl *ctrl = dynamic_cast<GuiControl *>((*set)[i]);
			if(ctrl)
			{
				mCurrentAddSet->addObject(ctrl);
				mSelectedControls.push_back(ctrl);
				Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
			}
		}
		// Undo 
		Con::executef(this, "onAddNewCtrlSet", Con::getIntArg(getSelectedSet().getId()));
	}
	set->deleteObject();
}

void GuiEditCtrl::saveSelection(const char* filename)
{
	// if there are no selected objects, then don't save
	if (mSelectedControls.size() == 0)
		return;

	FileStream stream;
	if(!ResourceManager->openFileForWrite(stream, filename))
		return;
	SimSet *clipboardSet = new SimSet;
	clipboardSet->registerObject();
	Sim::getRootGroup()->addObject(clipboardSet, "guiClipboard");

	Vector<GuiControl *>::iterator i;
	for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
		clipboardSet->addObject(*i);

	clipboardSet->write(stream, 0);
	clipboardSet->deleteObject();
}

void GuiEditCtrl::selectAll()
{
	GuiControl::iterator i;
	if (!mCurrentAddSet)
		return;

	clearSelection();
	for(i = mCurrentAddSet->begin(); i != mCurrentAddSet->end(); i++)
	{
		GuiControl *ctrl = dynamic_cast<GuiControl *>(*i);
		if (!(ctrl->isLocked())) {
			mSelectedControls.push_back(ctrl);
			Con::executef(this, "onAddSelected", Con::getIntArg(ctrl->getId()));
		}
	}
}

void GuiEditCtrl::bringToFront()
{
	// undo
	if (mSelectedControls.size() != 1)
		return;

	GuiControl *ctrl = *(mSelectedControls.begin());
	mCurrentAddSet->pushObjectToBack(ctrl);
}

void GuiEditCtrl::pushToBack()
{
	// undo
	if (mSelectedControls.size() != 1)
		return;

	GuiControl *ctrl = *(mSelectedControls.begin());
	mCurrentAddSet->bringObjectToFront(ctrl);
}

bool GuiEditCtrl::onKeyDown(const GuiEvent &event)
{
	if (! mActive)
		return Parent::onKeyDown(event);

	if (!(event.modifier & SI_CTRL))
	{
		switch(event.keyCode)
		{
		case KEY_BACKSPACE:
		case KEY_DELETE:
			deleteSelection();
			Con::executef(this, "onDelete");
			return true;
		}
	}
	return false;
}

ConsoleMethod(GuiEditCtrl, setSnapToGrid, void, 3, 3, "GuiEditCtrl.setSnapToGrid(gridsize)")
{
	U32 gridsize = dAtoi(argv[2]);
	object->setSnapToGrid(gridsize);
}

void GuiEditCtrl::setSnapToGrid(U32 gridsize)
{
	mGridSnap.set(gridsize, gridsize);
}


void GuiEditCtrl::controlInspectPreApply(GuiControl* object)
{
	// undo
	Con::executef(this, "onControlInspectPreApply", Con::getIntArg(object->getId()));
}

void GuiEditCtrl::controlInspectPostApply(GuiControl* object)
{
	// undo
	Con::executef(this, "onControlInspectPostApply", Con::getIntArg(object->getId()));
}


void GuiEditCtrl::updateSelectedSet()
{
	mSelectedSet.clear();
	Vector<GuiControl*>::iterator i;
	for(i = mSelectedControls.begin(); i != mSelectedControls.end(); i++)
	{
		mSelectedSet.addObject(*i);
	}
}


void GuiEditCtrl::resetStateBlock()
{
	//mAlphaBlendSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendInvSrcAlpha);
	for(S32 i=GFX->getNumSamplers(); i>=0; i--)
	{
		GFX->setTextureStageState(0, GFXTSSColorOp, GFXTOPDisable);
	}
	GFX->endStateBlock(mAlphaBlendSB);
}


void GuiEditCtrl::releaseStateBlock()
{
	if (mAlphaBlendSB)
	{
		mAlphaBlendSB->release();
	}
}

void GuiEditCtrl::init()
{
	if (mAlphaBlendSB == NULL)
	{
		mAlphaBlendSB = new GFXD3D9StateBlock;
		mAlphaBlendSB->registerResourceWithDevice(GFX);
		mAlphaBlendSB->mZombify = &releaseStateBlock;
		mAlphaBlendSB->mResurrect = &resetStateBlock;

		resetStateBlock();
	}
}

void GuiEditCtrl::shutdown()
{
	SAFE_DELETE(mAlphaBlendSB);
}


// -----------------------------------------------------------------------------
// GuiEditor Ruler
// -----------------------------------------------------------------------------
class GuiEditorRuler : public GuiControl {
	StringTableEntry refCtrl;
	typedef GuiControl Parent;
public:

	void onPreRender()
	{
		setUpdate();
	}
	void onRender(Point2I offset, const RectI &updateRect)
	{
		GFX->getDrawUtil()->drawRectFill(updateRect, ColorF(1,1,1,1));
		GuiScrollCtrl *ref;
		SimObject *o = Sim::findObject(refCtrl);

		//Sim::findObject(refCtrl, &ref);
		ref = dynamic_cast<GuiScrollCtrl *>(o);
		Point2I choffset(0,0);
		if(ref)
			choffset = ref->getChildPos();
		if(getWidth() > getHeight())
		{
			// it's horizontal.
			for(U32 i = 0; i < getWidth(); i++)
			{
				S32 x = offset.x + i;
				S32 pos = i - choffset.x;
				if(!(pos % 10))
				{
					S32 start = 6;
					if(!(pos %20))
						start = 4;
					if(!(pos % 100))
						start = 1;
					GFX->getDrawUtil()->drawLine(x, offset.y + start, x, offset.y + 10, ColorF(0,0,0,1));
				}
			}
		}
		else
		{
			// it's vertical.
			for(U32 i = 0; i < getHeight(); i++)
			{
				S32 y = offset.y + i;
				S32 pos = i - choffset.y;
				if(!(pos % 10))
				{
					S32 start = 6;
					if(!(pos %20))
						start = 4;
					if(!(pos % 100))
						start = 1;
					GFX->getDrawUtil()->drawLine(offset.x + start, y, offset.x + 10, y, ColorF(0,0,0,1));
				}
			}
		}
	}
	static void initPersistFields()
	{
		Parent::initPersistFields();
		addField("refCtrl", TypeString, Offset(refCtrl, GuiEditorRuler));
	}
	DECLARE_CONOBJECT(GuiEditorRuler);
};

IMPLEMENT_CONOBJECT(GuiEditorRuler);