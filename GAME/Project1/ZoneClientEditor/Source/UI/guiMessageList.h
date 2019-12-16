#ifndef  _GUIMESSAGELIST_H_
#define _GUIMESSAGELIST_H_

#include <vector>
#include <list>

#ifndef _GUICONTROL_H_
#include "gui/core/guiControl.h"
#endif

#ifndef CHAT_BASE_H
#include "Common/ChatBase.h"
#endif

#include "guiRichTextCtrl.h"

class guiMessageList : public GuiRichTextCtrl
{
	typedef GuiRichTextCtrl Parent;
public:

	enum ChatType
	{
		TYPE_CHANNEL,
		TYPE_TOOTHER,			//发给别人
		TYPE_TOME,				//别人发给自己
		TYPE_SYS,			    //一般提示信息 
		TYPE_SYS_WARNING,		//系统公告
		TYPE_CHATEMOTE,
		TYPE_PERSON,
		TYPE_GM_MSG,
		TYPE_P2P,               //仙友窗口私聊
		CHAT_COUNT,	
	};

	class Entry;
	typedef std::vector<Entry*> EntryListL;

	guiMessageList();
	~guiMessageList();

	DECLARE_CONOBJECT(guiMessageList);

	static void initPersistFields();

	void AddMsg	(const char* channel,const char* msg, S32 reserveID,const char* name/*1,const char* name2*/);

	void Clear();

	void onRender(Point2I offset, const RectI &updateRect);

    void AddEntry(S32 type, const char* prefix, const char *text,  S32 otherType, S32 reserveID,const char* name);

	void ScrollTo(S32 x, S32 y);
	void ScrollUpArrow();
	void ScrollDownArrow();
	void ScrollDown();
	void ScrollUp();

	//--------------------------------------------------------------------------------------------------------------------------------------

	class Entry
	{
	public:
		
		Entry (S32 type, std::string& string);

		~Entry ();

	public:

		S32			m_Type;

		std::string			m_Text;						// Real text. Node list is parsed from the text.

	};

	Point2I	m_EntrySize;
	S32	m_TextOffset;

protected:

	void SetNodeCount (S32 count);
	void AppendText( char* str );
	void Refresh();
private:
	char m_string[100];
	Point2I m_ptContentSize;			// 内容区域的大小
	int mOffsetY;
	
	CommonFontEX*  m_reFont;

	EntryListL	m_EntryListL;

	S32	m_NodeCount;
	S32	m_SelectedNode;
	S32	m_HoverNode;

	ColorI m_Colors[CHAT_MSG_TYPE_UNKNOWN];
	StringTableEntry m_TypeText[CHAT_MSG_TYPE_UNKNOWN];

	Point2I m_ptSize;				// 控件的尺寸
	S32	 pageHeight;

	S32	m_MaxStorageLine;		// 最大存贮行数

	Point2I m_ptRelativePosition;	// 在子控件坐标系中, 内容区域左上角的相对位置.
	Point2I m_ptPosition;			// 控件左上角的坐标

	Point2I m_ptInitPoint;		// 
	Point2I m_ptInitExtent;	// 保存当前控件的初始化坐标

	std::list< std::string > mStringList;
};

#endif