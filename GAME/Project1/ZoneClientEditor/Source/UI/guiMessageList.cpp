#include <mbstring.h>
#include "UI/guiMessageList.h"
#include "gfx/gfxDrawUtil.h"
#include "console/consoleTypes.h"
#include "Common/ChatBase.h"

IMPLEMENT_CONOBJECT(guiMessageList);

guiMessageList::guiMessageList()
{
	
	m_TextOffset = 20;
	
	Clear();            

	m_ptContentSize.set(0,0);
	
	m_NodeCount		= 0;
	m_EntrySize.set(80, 20);
	m_ptRelativePosition.set(0,0);
	m_SelectedNode	= -1;
	m_HoverNode		= -1;
	pageHeight = m_EntrySize.y;

	for(S32 i = 0; i < CHAT_MSG_TYPE_UNKNOWN; i++)
	{
		m_TypeText[i] = StringTable->insert("");
	}

	m_reFont = NULL;
	m_ptPosition.set(0,0);

	m_ptInitPoint.set(0,0);
	m_ptInitExtent.set(0,0);

	mOffsetY = -1;
	m_MaxStorageLine = 20;
}

guiMessageList::~guiMessageList()
{

}

void guiMessageList::initPersistFields()
{
	Parent::initPersistFields();
	addField( "TextOffset",    TypeS32,          Offset( m_TextOffset, guiMessageList ) );
	addField( "EntrySize",     TypePoint2I,      Offset( m_EntrySize, guiMessageList ) );
	addField( "ContentSize",   TypePoint2I,      Offset( m_ptContentSize, guiMessageList ) );
	addField( "MaxStorageLine",   TypeS32,       Offset( m_MaxStorageLine, guiMessageList ) );
	addField( "Colors",		   TypeColorI,		 Offset(m_Colors, guiMessageList), CHAT_MSG_TYPE_UNKNOWN);
	addField( "TypeText",      TypeString,       Offset( m_TypeText, guiMessageList ),CHAT_MSG_TYPE_UNKNOWN );
	addField( "MaxLineNum",      TypeS32,			 Offset(m_MaxStorageLine, guiMessageList), CHAT_MSG_TYPE_UNKNOWN);
}

guiMessageList::Entry::Entry(S32 type, std::string& string/*, TagListL& tag_listL*/)
{
	m_Type		= type;
	m_Text		= string;
}

guiMessageList::Entry::~Entry()
{

}

void guiMessageList::onRender(Point2I offset, const RectI &updateRect)
{
	GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::SeparateAlphaBlend);
	m_DirtyFlag = false;
	Parent::onRender(offset,updateRect);
	GFX->getDrawUtil()->setBlendTyps(GFXDrawUtil::LerpAlphaBlend);
}

void guiMessageList::SetNodeCount(S32 count)
{
	m_NodeCount = count;

	Point2I new_extent(m_EntrySize.x, m_EntrySize.y * m_NodeCount);

	const RectI bounds = getBounds();

	m_ptSize = new_extent;

	if(new_extent.y <= bounds.extent.y)
		new_extent.y = bounds.extent.y;

	GuiControl::resize(bounds.point,new_extent);

	m_EntrySize.x = bounds.extent.x;
	pageHeight = m_EntrySize.y;
}

void guiMessageList::AddMsg(const char* channel,const char* msg, S32 reserveID,const char* name/*1,const char* name2*/)
{
	S32 nChannel = atoi(channel);
	switch(nChannel)
	{
	case CHAT_MSG_TYPE_SYSTEM:// 系统
		AddEntry(nChannel,"<i s='gameres/gui/images/GUIWindow15_1_038.png' />",msg,TYPE_SYS_WARNING,reserveID,name);
		break;
	case CHAT_MSG_TYPE_WORLDPLUS:// 跑马
		AddEntry(nChannel,"<i s='gameres/gui/images/GUIWindow15_1_038.png' />", msg,TYPE_SYS,reserveID,name);
		break;
	case CHAT_MSG_TYPE_WORLD:// 跨服
		AddEntry(nChannel,"",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_LINE:// 全服
		AddEntry(nChannel,"",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_RACE:// 门宗
		AddEntry(nChannel,"",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_ORG:// 帮会
		AddEntry(nChannel,"",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_SQUAD:// 团
		AddEntry(nChannel,"",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_TEAM:// 队伍
		AddEntry(nChannel,"<i s='gameres/gui/images/GUIWindow15_1_031.png' />",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_GROUP:// 群消息
		AddEntry(nChannel,"",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_REALCITY:// 同城
		AddEntry(nChannel,"<i s='gameres/gui/images/GUIWindow15_1_034.png' />",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_NEARBY:// 附近
		AddEntry(nChannel,"<i s='gameres/gui/images/GUIWindow15_1_030.png' />",msg,TYPE_CHANNEL,reserveID,name);
		break;
	case CHAT_MSG_TYPE_PRIVATE:// 私聊
		AddEntry(nChannel,"",msg,TYPE_TOME,reserveID,name);
		break;
	case CHAT_MSG_TYPE_GM:// GM
		AddEntry(nChannel,"",msg,TYPE_GM_MSG,reserveID,name);
		break;
	case CHAT_MSG_TYPE_PERSON:
		AddEntry(nChannel,"<i s='gameres/gui/images/GUIWindow15_1_037.png' />",msg,TYPE_PERSON,reserveID,name);
		break;
	case CHAT_MSG_TYPE_UNKNOWN:
		AddEntry(nChannel,"",msg,TYPE_PERSON,reserveID,name);
		break;
	case CHAT_MSG_TYPE_P2P:  //仙友窗口私聊
        AddEntry(nChannel,"",msg,TYPE_P2P,reserveID,name);
		break;
	}
}

void guiMessageList::Clear()
{
	for(int i = 0 ; i < m_EntryListL.size(); i++)
		delete m_EntryListL[i];
	m_EntryListL.clear();

	if(!this->isAwake())
		return;

	mStringList.clear();

	setContent( "" );

	Refresh();

	m_NodeCount = 0;
	m_ptRelativePosition.set(0,0);
	//ScrollTo(0,0x7FFFFFFF);
	m_DirtyFlag = true;
}


void guiMessageList::AddEntry(S32 type,  const char* prefix, const char *text, S32 otherType, S32 reserveID,const char* name)
{
	m_DirtyFlag = true;

	SimObject* pObject = Sim::findObject("ChatWnd_ShowMessage2");
	guiMessageList* pMsgList = NULL;
	if (NULL != pObject)
	{
		pMsgList = dynamic_cast<guiMessageList*>(pObject);
	}

	char nameDisplay[1024] = { 0 };

	dStrcat( nameDisplay, sizeof( nameDisplay), prefix );
	
	switch(otherType)
	{
	case TYPE_CHANNEL:
		dStrcat(nameDisplay, sizeof(nameDisplay), "<t o='0x2b2b2bff'>[");
		dStrcat(nameDisplay, sizeof(nameDisplay), name);
		dStrcat(nameDisplay, sizeof(nameDisplay), "]说:</t>");
		break;
	case TYPE_TOOTHER:
		dStrcat(nameDisplay, sizeof(nameDisplay), "<t>你对[");
		dStrcat(nameDisplay, sizeof(nameDisplay), name);
		dStrcat(nameDisplay, sizeof(nameDisplay), "]说:</t>");
		break;
	case TYPE_TOME:
		dStrcat(nameDisplay, sizeof(nameDisplay), "<t o='0x2b2b2bff'>[");
		dStrcat(nameDisplay, sizeof(nameDisplay), name);
		dStrcat(nameDisplay, sizeof(nameDisplay), "]对你说:</t>");
		break;
	case TYPE_SYS_WARNING:
		dStrcat(nameDisplay, sizeof(nameDisplay), "");	    
		break;
	case TYPE_SYS:
		dStrcat(nameDisplay, sizeof(nameDisplay), "");
		break;
	case TYPE_PERSON:
		dStrcat(nameDisplay, sizeof(nameDisplay), "");
		break;
	case TYPE_CHATEMOTE:
		dStrcat(nameDisplay, sizeof(nameDisplay), "");
		break;
	case TYPE_GM_MSG:
		dStrcat(nameDisplay, sizeof(nameDisplay), "");
		break;
	case TYPE_P2P:
		dStrcat(nameDisplay, sizeof(nameDisplay), "<t o='0x2b2b2bff'>[");
		dStrcat(nameDisplay, sizeof(nameDisplay), name);
		dStrcat(nameDisplay, sizeof(nameDisplay), "]说:</t><b/>");
		break;
	default:
		dStrcat(nameDisplay, sizeof(nameDisplay), "");
		break;
	}	

	if( otherType != TYPE_SYS_WARNING && otherType != TYPE_SYS && otherType != TYPE_PERSON )
	{
		if( strstr( text, "<t" ) != NULL )
		{
			dStrcat( nameDisplay, sizeof(nameDisplay), text);
		}
		else
		{
			dStrcat( nameDisplay, sizeof(nameDisplay), "<t o = '0x2b2b2bff'>");
			dStrcat( nameDisplay, sizeof(nameDisplay), text);	
			dStrcat( nameDisplay, sizeof(nameDisplay), "</t>");
		}
	}
	else
	{
		dStrcat( nameDisplay, sizeof(nameDisplay), text);
	}

	m_reFont = mProfile->mFont;

	AppendText( nameDisplay );

	Refresh();

	ScrollTo(0,0);

}

void guiMessageList::ScrollTo(S32 x, S32 y)
{
	RectI bounds = getBounds();
	
	setUpdate();

	S32 delta1 = getHeight() - this->getParent()->getHeight();
	if (delta1 < 0)
	{
		delta1 = 0;
		y = 0;
	}
	S32 delta2 = delta1 - bounds.point.y;
	if (y == 0)
	{
		bounds.point.y = -delta1;
	}
	else
	{
		bounds.point.y += y;
		if (bounds.point.y >= 0)
		{
			bounds.point.y = 0;
		}
		else if (bounds.point.y <= -delta1)
		{
			bounds.point.y = -delta1;
		}
	}

	bounds.extent.x = getParent()->getBounds().extent.x - 20;
	
	setBounds(bounds);

}

void guiMessageList::ScrollUpArrow()
{
	ScrollTo(0, 16);
}
void guiMessageList::ScrollDownArrow()
{
	ScrollTo(0, -16);
}

void guiMessageList::ScrollDown()
{
	ScrollTo(0,0);
}
void guiMessageList::ScrollUp()
{
	ScrollTo(0,0x0);
}

void guiMessageList::AppendText( char* str )
{
	if(m_MaxStorageLine > 0)
	{
		if( mStringList.size() >= m_MaxStorageLine )
			mStringList.erase( mStringList.begin() );

	}
	mStringList.push_back( str );	
}

void guiMessageList::Refresh()
{
	std::stringstream ss;
	std::list< std::string >::iterator it;
	int i = 0;
	for( it = mStringList.begin(); it != mStringList.end(); it++, i++ )
	{
		if( i == mStringList.size() - 1 )
			ss<< *it;
		else
			ss<< *it << "<b/>";
	}
	setContent( ss.str().c_str() );
}

ConsoleMethod( guiMessageList, AddMsg, void, 6, 6, "obj.AddMsg( type, newText, id ,name)" )
{
	argc;
	object->AddMsg( argv[2], argv[3], atoi(argv[4]), argv[5]);
}

ConsoleMethod( guiMessageList, ScrollUpArrow,void,2,2, "obj.ScrollUpArrow()")
{
	object->ScrollUpArrow();
}

ConsoleMethod( guiMessageList, ScrollDownArrow,void,2,2, "obj.ScrollDownArrow()")
{
	object->ScrollDownArrow();
}

ConsoleMethod( guiMessageList, ScrollDown,void,2,2, "obj.ScrollDown()")
{
	object->ScrollDown();
}

ConsoleMethod( guiMessageList, Clear, void, 2,2, "obj.Clear()")
{
	object->Clear();
}
