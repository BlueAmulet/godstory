
#include "ui/guiBitmapWindowCtrl.h"
#include "gui/core/guiCanvas.h"

IMPLEMENT_CONOBJECT(GuiBitmapWindowCtrl);

GuiBitmapWindowCtrl::GuiBitmapWindowCtrl()
{
	m_MoveHotspot.set(1, 1, 0, 0);
	m_IsMoving = false;
	m_MouseDownPosition.set(0, 0);
}

void GuiBitmapWindowCtrl::initPersistFields()
{
  Parent::initPersistFields();

  addField("MoveHotspot", TypeRectI, Offset(m_MoveHotspot,GuiBitmapWindowCtrl));
}


bool GuiBitmapWindowCtrl::onWake()
{
	if ( !Parent::onWake())
		return false;
	return true;
}

void GuiBitmapWindowCtrl::onMouseUp(const GuiEvent &event)
{
	mouseUnlock();
    m_IsMoving = false;
}

void GuiBitmapWindowCtrl::onMouseDragged(const GuiEvent &event)
{
	GuiControl* parent = getParent();
	if(!parent)
		return;

	GuiCanvas *root = getRoot();
	if ( !root ) return;

	Point2I delta_mouse_pos = event.mousePoint - m_MouseDownPosition;

	Point2I new_pos = getPosition();
	Point2I new_extent = getExtent();

	if(m_IsMoving && parent)
	{
		new_pos.x = getMax(0, getMin(parent->getWidth() - getWidth(), m_OriginalBoundary.point.x + delta_mouse_pos.x));
		new_pos.y = getMax(0, getMin(parent->getHeight() - getHeight(), m_OriginalBoundary.point.y + delta_mouse_pos.y));

		Point2I pos = parent->localToGlobalCoord(getPosition());
		root->addUpdateRegion(pos, getExtent());
		resize(new_pos, new_extent);
	}
}

void GuiBitmapWindowCtrl::onMouseDown(const GuiEvent &event)
{
	m_OriginalBoundary = getBounds();
	m_MouseDownPosition = event.mousePoint;
	Point2I local_position = globalToLocalCoord(m_MouseDownPosition);

	//select this window - move it to the back
	GuiControl *parent = getParent();
	if (parent)
	{
		parent->pushObjectToBack(this);
	}

	//also set the first responder to be the one within this window
	//	ActivateInteraction(m_pInteractingControl);

	if(m_MoveHotspot.pointInRect(local_position))
	{
		m_IsMoving = true;
		mouseLock();
	}
}

void GuiBitmapWindowCtrl::onRender( Point2I offset, const RectI &updateRect )
{
	GFX->getDrawUtil()->lockAlpha( mAlpha );
	if (!mTextureObject)
	{
		RectI rect(offset.x, offset.y, getExtent().x, getExtent().y);
		GFX->getDrawUtil()->drawRect(rect,ColorI(0,0,0,0));
	}
	else
		Parent::onRender(offset,updateRect);

	intoRound();

	renderChildControls(offset, updateRect);

	GFX->getDrawUtil()->unlockAlpha();
}

void GuiBitmapWindowCtrl::intoRound()
{
	GuiControl *parent = getParent();
	if (parent)
	{
		RectI mRound = parent->getBounds();//Parent::getBounds();
		RectI mSelf = getBounds();
		if (mSelf.point.x < mRound.point.x)
			mSelf.point.x = mRound.point.x;
		if (mSelf.point.y < mRound.point.y)
			mSelf.point.y = mRound.point.y;
		if (mSelf.point.x + mSelf.extent.x > mRound.extent.x)
			mSelf.point.x = mRound.extent.x - mSelf.extent.x;
		if (mSelf.point.y + mSelf.extent.y > mRound.extent.y)
			mSelf.point.y = mRound.extent.y - mSelf.extent.y;
		setBounds(mSelf);
	}
}

void GuiBitmapWindowCtrl::onMouseEnter( const GuiEvent &event )
{
	Con::executef( this, "onMouseEnter" );
}

void GuiBitmapWindowCtrl::onMouseLeave( const GuiEvent &event )
{
	Con::executef( this, "onMouseLeave" );
}

void GuiBitmapWindowCtrl::onRightMouseDown( const GuiEvent &event )
{
	Con::executef( this, "onRightMouseDown" );
}