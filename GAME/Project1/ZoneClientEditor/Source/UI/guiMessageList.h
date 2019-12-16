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
		TYPE_TOOTHER,			//��������
		TYPE_TOME,				//���˷����Լ�
		TYPE_SYS,			    //һ����ʾ��Ϣ 
		TYPE_SYS_WARNING,		//ϵͳ����
		TYPE_CHATEMOTE,
		TYPE_PERSON,
		TYPE_GM_MSG,
		TYPE_P2P,               //���Ѵ���˽��
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
	Point2I m_ptContentSize;			// ��������Ĵ�С
	int mOffsetY;
	
	CommonFontEX*  m_reFont;

	EntryListL	m_EntryListL;

	S32	m_NodeCount;
	S32	m_SelectedNode;
	S32	m_HoverNode;

	ColorI m_Colors[CHAT_MSG_TYPE_UNKNOWN];
	StringTableEntry m_TypeText[CHAT_MSG_TYPE_UNKNOWN];

	Point2I m_ptSize;				// �ؼ��ĳߴ�
	S32	 pageHeight;

	S32	m_MaxStorageLine;		// ����������

	Point2I m_ptRelativePosition;	// ���ӿؼ�����ϵ��, �����������Ͻǵ����λ��.
	Point2I m_ptPosition;			// �ؼ����Ͻǵ�����

	Point2I m_ptInitPoint;		// 
	Point2I m_ptInitExtent;	// ���浱ǰ�ؼ��ĳ�ʼ������

	std::list< std::string > mStringList;
};

#endif