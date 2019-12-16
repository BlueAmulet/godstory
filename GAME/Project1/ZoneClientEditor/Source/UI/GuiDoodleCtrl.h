#ifndef GUI_DOODLE_CTRL_H
#define GUI_DOODLE_CTRL_H

#ifdef NTJ_CLIENT

#include <list>

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef _GUITYPES_H_
#include "gui/core/guiTypes.h"
#endif

struct D3DXVECTOR2;

class GuiDoodleCtrl : public GuiControl
{
public:
	GuiDoodleCtrl();
	virtual ~GuiDoodleCtrl();

	typedef GuiControl Parent;

	DECLARE_CONOBJECT( GuiDoodleCtrl );

	bool onWake();
	void onSleep();

	void onRender(Point2I offset, const RectI &updateRect);

	void onMouseDown(const GuiEvent &event);
	void onMouseUp(const GuiEvent &event);

	void onMouseDragged(const GuiEvent &event);

	void onMouseMove(const GuiEvent &event);

	void clear();

	typedef std::list< Point2I > NodeList;

	struct Line
	{
		Line();
		virtual ~Line();

		void addNode( Point2I point );

		unsigned long mColor;

		D3DXVECTOR2* getVector();
		int getPointSize();

		__declspec( property( get = getVector ) ) D3DXVECTOR2* VectorPtr;
		__declspec( property( get = getPointSize ) ) int PointCount;

	private:
		void build();

		bool isDirty;
		NodeList mNodeList;		

		D3DXVECTOR2* mVectorPtr;
	};

private:

	bool isMouseDown;
	Point2I mLastMousePos;

	typedef std::list< Line* > LineList;

	LineList mLineList;
};

#endif

#endif