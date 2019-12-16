#ifdef NTJ_CLIENT


#include <d3dx9math.h>
#include "console/console.h"
#include "console/consoleTypes.h"
#include "gfx/D3D9/gfxD3D9Device.h"

#include "GuiDoodleCtrl.h"

LPD3DXLINE g_Line;

IMPLEMENT_CONOBJECT(GuiDoodleCtrl);

void GuiDoodleCtrl::Line::addNode( Point2I point )
{
	isDirty = true;
	mNodeList.push_back( point );
}

D3DXVECTOR2* GuiDoodleCtrl::Line::getVector()
{
	if( isDirty )
	{
		build();
		isDirty = false;
	}

	return mVectorPtr;
}

GuiDoodleCtrl::Line::Line()
{
	mVectorPtr = NULL;
}

GuiDoodleCtrl::Line::~Line()
{
	if( mVectorPtr )
		delete[] mVectorPtr;
}

void GuiDoodleCtrl::Line::build()
{
	if( mVectorPtr )
	{
		delete[] mVectorPtr;
		mVectorPtr = NULL;
	}

	if( mNodeList.size() == 0 )
		return ;

	mVectorPtr = new D3DXVECTOR2[mNodeList.size()];

	NodeList::iterator it = mNodeList.begin();
	for( int i = 0; it != mNodeList.end(); it++, i++ )
	{
		mVectorPtr[i].x = it->x;
		mVectorPtr[i].y = it->y;
	}
}

int GuiDoodleCtrl::Line::getPointSize()
{
	return mNodeList.size();
}

void GuiDoodleCtrl::onRender( Point2I offset, const RectI &updateRect )
{

	for each( Line* line in mLineList )
	{
		g_Line->Begin();
		g_Line->Draw( line->VectorPtr, line->PointCount, 0xffffffff );
		g_Line->End();
	}
}

void GuiDoodleCtrl::onMouseDown( const GuiEvent &event )
{
	isMouseDown = true;

	Line* pLine = new Line();
	mLineList.push_back( pLine );
}

void GuiDoodleCtrl::onMouseUp( const GuiEvent &event )
{
	isMouseDown = false;
}

GuiDoodleCtrl::~GuiDoodleCtrl()
{
	clear();
}

void GuiDoodleCtrl::clear()
{
	for each( Line* pLine in mLineList )
	{
		delete pLine;
	}

	mLineList.clear();
}

void GuiDoodleCtrl::onMouseMove( const GuiEvent &event )
{
	//if( mLineList.size() == 0 || !isMouseDown )
	//	return ;

	//Line* pLine = *mLineList.rbegin();

	//const Point2I& curPos = event.mousePoint;

	//pLine->addNode( curPos );
}

bool GuiDoodleCtrl::onWake()
{

	static LPDIRECT3DDEVICE9 pDevice = dynamic_cast<GFXD3D9Device *>(GFX)->getDevice();

	D3DXCreateLine( pDevice, &g_Line );

	g_Line->SetWidth( 4 );

	return Parent::onWake();
}

void GuiDoodleCtrl::onSleep()
{
	g_Line->Release();

	Parent::onSleep();
}

GuiDoodleCtrl::GuiDoodleCtrl()
{

}

void GuiDoodleCtrl::onMouseDragged( const GuiEvent &event )
{
	if( mLineList.size() == 0 )
		return ;

	Line* pLine = *mLineList.rbegin();

	const Point2I& curPos = event.mousePoint;

	pLine->addNode( curPos );
}
#endif