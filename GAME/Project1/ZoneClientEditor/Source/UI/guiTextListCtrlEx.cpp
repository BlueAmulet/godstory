//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/consoleTypes.h"
#include "console/console.h"
#include "gui/containers/guiScrollCtrl.h"
#include "gui/core/guiDefaultControlRender.h"
#include "gfx/gfxFontRenderBatcher.h"
#include "ui/guiTextListCtrlEx.h"

IMPLEMENT_CONOBJECT(GuiTextListCtrlEx);

static int sortColumn;
static bool sIncreasing;

static int getEndLinesInCell(const char *text)
{
	const char *pStart = text;
	int ret = 1;

	while(*pStart && *pStart != '\t')
	{
		if (*pStart == '\n')
			ret++;
		pStart++;
	}
	return ret;
}

static const char *getColumn(const char *text)
{
	int ct = sortColumn;
	while(ct--)
	{
		text = dStrchr(text, '\t');
		if(!text)
			return "";
		text++;
	}
	return text;
}

static const char* getColumn( int index, const char* text )
{
	int ct = index;
	while(ct--)
	{
		text = dStrchr(text, '\t');
		if(!text)
			return "";
		text++;
	}
	return text;	
}

static S32 QSORT_CALLBACK textCompare( const void* a, const void* b )
{
	GuiTextListCtrlEx::Entry *ea = (GuiTextListCtrlEx::Entry *) (a);
	GuiTextListCtrlEx::Entry *eb = (GuiTextListCtrlEx::Entry *) (b);
	S32 result = dStricmp( getColumn( ea->text ), getColumn( eb->text ) );
	return ( sIncreasing ? result : -result );
}

static S32 QSORT_CALLBACK numCompare(const void *a,const void *b)
{
	GuiTextListCtrlEx::Entry *ea = (GuiTextListCtrlEx::Entry *) (a);
	GuiTextListCtrlEx::Entry *eb = (GuiTextListCtrlEx::Entry *) (b);
	const char* aCol = getColumn( ea->text );
	const char* bCol = getColumn( eb->text );
	F32 result = dAtof(aCol) - dAtof(bCol);
	S32 res = result < 0 ? -1 : (result > 0 ? 1 : 0);

	return ( sIncreasing ? res : -res );
}

static S32 QSORT_CALLBACK idCompare(const void*a,const void* b)
{
	GuiTextListCtrlEx::Entry *ea = (GuiTextListCtrlEx::Entry *) (a);
	GuiTextListCtrlEx::Entry *eb = (GuiTextListCtrlEx::Entry *) (b);

	S32 result = ea->id - eb->id;
	return ( sIncreasing ? result : -result );
}

GuiTextListCtrlEx::GuiTextListCtrlEx()
{
	VECTOR_SET_ASSOCIATION(mList);
	VECTOR_SET_ASSOCIATION(mColumnOffsets);

	mActive = true;
	mEnumerate = false;
	mResizeCell = true;
	mSize.set(1, 0);
	mColumnOffsets.push_back(0);
	mFitParentWidth = true;
	mClipColumnText = false;
	m_TextListSelectImageFile = StringTable->insert("");
	m_TextListMouseOnImageFile = StringTable->insert("");
	m_TextListBackgroundImageFile1 = StringTable->insert("");
	m_TextListBackgroundTexture1 = NULL;
	m_TextListBackgroundImageFile2 = StringTable->insert("");
	m_TextListBackgroundTexture2 = NULL;
	m_TextListSelectTexture = NULL;
	m_TextListMouseOnTexture = NULL;
	mFont = NULL;
	mRowCount = 0;
}

GuiTextListCtrlEx::~GuiTextListCtrlEx()
{
	for (S32 i = 0; i < mList.size(); i++)
	{
		removeEntryByIndex(i);
	}
}

void GuiTextListCtrlEx::initPersistFields()
{
	Parent::initPersistFields();
	addField("enumerate",               TypeBool, Offset(mEnumerate, GuiTextListCtrlEx));
	addField("resizeCell",              TypeBool, Offset(mResizeCell, GuiTextListCtrlEx));
	addField("columns",                 TypeS32Vector, Offset(mColumnOffsets, GuiTextListCtrlEx));
	addField("fitParentWidth",          TypeBool, Offset(mFitParentWidth, GuiTextListCtrlEx));
	addField("clipColumnText",          TypeBool, Offset(mClipColumnText, GuiTextListCtrlEx));
	addField("TextListSelectImageFile",  TypeString, Offset(m_TextListSelectImageFile,GuiTextListCtrlEx));
	addField("TextListMouseOnImageFile", TypeString, Offset(m_TextListMouseOnImageFile,GuiTextListCtrlEx));
	addField("TextListBackgroundImageFile1", TypeString, Offset(m_TextListBackgroundImageFile1,GuiTextListCtrlEx));
	addField("TextListBackgroundImageFile2", TypeString, Offset(m_TextListBackgroundImageFile2,GuiTextListCtrlEx));
}

bool GuiTextListCtrlEx::onWake()
{
	if(!Parent::onWake())
		return false;

	if(m_TextListSelectImageFile && m_TextListSelectImageFile[0])
		m_TextListSelectTexture =  GFXTexHandle(m_TextListSelectImageFile, &GFXDefaultStaticDiffuseProfile, true);

	if(m_TextListMouseOnImageFile && m_TextListMouseOnImageFile[0])
		m_TextListMouseOnTexture =  GFXTexHandle(m_TextListMouseOnImageFile, &GFXDefaultStaticDiffuseProfile, true);

	if(m_TextListBackgroundImageFile1 && m_TextListBackgroundImageFile1[0])
		m_TextListBackgroundTexture1 =  GFXTexHandle(m_TextListBackgroundImageFile1, &GFXDefaultStaticDiffuseProfile, true);
	if(m_TextListBackgroundImageFile2 && m_TextListBackgroundImageFile2[0])
		m_TextListBackgroundTexture2 =  GFXTexHandle(m_TextListBackgroundImageFile2, &GFXDefaultStaticDiffuseProfile, true);

	setSize(mSize);
	return true;
}

U32 GuiTextListCtrlEx::getSelectedId()
{
	if (mSelectedCell.y == -1)
		return InvalidId;

	return mList[mSelectedCell.y].id;
}

U32 GuiTextListCtrlEx::getSelectedRow()
{
	return mSelectedCell.y;
}
void GuiTextListCtrlEx::onCellSelected(Point2I cell)
{
	Con::executef(this, "onSelect", Con::getIntArg(mList[cell.y].id), mList[cell.y].text);

	if (mConsoleCommand[0])
	{
		char buf[16];
		dSprintf(buf, sizeof(buf), "%d", getId());
		Con::setVariable("$ThisControl", buf);
		Con::evaluate(mConsoleCommand, false);
	}
}

int GuiTextListCtrlEx::calculateMaxRowInCell(Entry *row)
{
	int nMaxRow = 1;
	if (!row->text || !row->text[0])
	{
		return nMaxRow;
	}
	char *pStart = row->text;
	char *pEnd = pStart;
	while(*pStart)
	{
		pEnd = strchr(pStart, '\t');
		int nEndOfLines = 0;
		while(pStart != pEnd && *pStart)
		{
			if (*pStart == '\n')
				nEndOfLines++;
			pStart++;
		}

		nMaxRow = (nMaxRow < (nEndOfLines + 1)) ? (nEndOfLines + 1) : nMaxRow;
		if (pEnd && *pEnd)
			pStart++;
	}
	return nMaxRow;
}

int GuiTextListCtrlEx::getEntryIndex(S32 nRowIndex)
{
	int count = 0;
	for (int i = 0; i < mRowList.size(); i++)
	{
		count += mRowList[i];
		if ( count >= (nRowIndex + 1) )
			return i;
	}

	return 0;
}

//获得nCellIndex当前拥有的行数
int GuiTextListCtrlEx::getRowNumber(S32 nCellIndex)
{
	if (nCellIndex < 0 || nCellIndex >= mList.size())
		return 0;

	return mRowList[nCellIndex];
}

//计算nCellIndex之前的所有行数
int GuiTextListCtrlEx::getRowCount(S32 nCellIndex/* = -1*/)
{
	if (nCellIndex == -1 || nCellIndex >= mList.size())
		nCellIndex = mList.size();

	int count = 0;
	for(int i = 0; i < nCellIndex; i++)
	{
		count += mRowList[i];
	}
	return count;
}

void GuiTextListCtrlEx::onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver)
{
	int nRowNum = getRowNumber(cell.y);
	RectI highlightRect = RectI(offset.x, offset.y, mCellUnit.x, mCellUnit.y * mRowList[cell.y]);
	highlightRect.inset( 0, -1 );

	if ( mList[cell.y].active )
	{
		GFXTexHandle textListBackgroundTexture = (cell.y % 2 == 0) ? m_TextListBackgroundTexture1 : m_TextListBackgroundTexture2;
		if (textListBackgroundTexture)
		{
			GFX->getDrawUtil()->clearBitmapModulation();
			GFX->getDrawUtil()->drawBitmapStretch(textListBackgroundTexture, highlightRect);
		}
		if (selected || (mProfile->mMouseOverSelected && mouseOver))
		{
			if (m_TextListSelectTexture)
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->drawBitmapStretch(m_TextListSelectTexture,highlightRect);
			}
			else
				renderFilledBorder( highlightRect, mProfile->mBorderColorHL, mProfile->mFillColorHL);

			GFX->getDrawUtil()->setBitmapModulation(mProfile->mFontColorHL);

		}
		else
		{
			if (mouseOver && m_TextListMouseOnTexture)
			{
				GFX->getDrawUtil()->clearBitmapModulation();
				GFX->getDrawUtil()->drawBitmapStretch(m_TextListMouseOnTexture,highlightRect);
			}
			GFX->getDrawUtil()->setBitmapModulation(mouseOver ? mProfile->mFontColorHL : mProfile->mFontColor);

		}
	}
	else
		GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColorNA );
	const char *text = mList[cell.y].text;
	for(U32 index = 0; index < mColumnOffsets.size(); index++)
	{
		int nLineInCell = getEndLinesInCell(text);
		const char *nextCol = dStrchr(text, '\t');
		if(mColumnOffsets[index] < 0)
			break;
		dsize_t slen;
		////////             123\n456\n789\t			////////////
		const char *pStart = text;
		const char *pEnd = pStart;
		int nLineNumber = 0;
		while(pStart != nextCol)
		{			
			//搜索换行符'\n'
			while(*pEnd != '\n' && *pEnd && pEnd != nextCol)
				pEnd++;
			//画从pEnd到pStart之间的文字
			if(*pEnd == '\n' || *pEnd == '\t')
				slen = pEnd - pStart;
			else
				slen = dStrlen(pStart);
			////////////////////////////////////////////////////////////////////////////////////////////////
			int offSetY = (nRowNum - nLineInCell) * mCellUnit.y / (nLineInCell+1) * (nLineNumber+1) + nLineNumber * mCellUnit.y;
			Point2I pos(offset.x + 4 + mColumnOffsets[index], offset.y + offSetY);
			RectI saveClipRect;
			bool clipped = false;
			if(mClipColumnText && (index != (mColumnOffsets.size() - 1)))
			{
				saveClipRect = GFX->getClipRect();
				RectI clipRect(pos, Point2I(mColumnOffsets[index+1] - mColumnOffsets[index] - 4, mCellUnit.y));
				if(clipRect.intersect(saveClipRect))
				{
					clipped = true;
					GFX->setClipRect( clipRect );
				}
			}

			if ( mList[cell.y].active )
			{
				if (selected || (mProfile->mMouseOverSelected && mouseOver))
				{
					GFX->getDrawUtil()->setBitmapModulation(mProfile->mFontColorHL);
				}
				else
				{
					GFX->getDrawUtil()->setBitmapModulation(mouseOver ? mProfile->mFontColorHL : mProfile->mFontColor);
				}
			}
			else
				GFX->getDrawUtil()->setBitmapModulation( mProfile->mFontColorNA );
			//[2009/7/14][thinking]:修改文本显示错误问题
			Point2I clipExtent;
			if(index < mColumnOffsets.size()-1)
				clipExtent = Point2I(mColumnOffsets[index+1] - mColumnOffsets[index] - 2,mCellUnit.y);
			else
				clipExtent = Point2I(getBounds().extent.x - mColumnOffsets[index] - 2,mCellUnit.y);

			//GFX->getDrawUtil()->drawTextN(mFont, pos, text, slen, mProfile->mFontColors);
			//renderJustifiedTextN(pos,clipExtent,text,slen);
			renderJustifiedTextN(pos, clipExtent, pStart, slen);

			if(clipped)
				GFX->setClipRect( saveClipRect );
			////////////////////////////////////////////////////////////////////////////////////////////////

			nLineNumber++;			//表示当前Cell的第几行文字,用于计算offset
			
			//准备进行下一次搜索换行符
			if (*pEnd == '\n')
			{
				pEnd++;
				pStart = pEnd;
			}
			else
				break;
		}

		if(!nextCol)
			break;
		text = nextCol+1;
	}
	GFX->getDrawUtil()->setBitmapModulation(ColorI(255,255,255));
}

U32 GuiTextListCtrlEx::getRowWidth(Entry *row)
{
	U32 width = 1;
	const char *text = row->text;
	for(U32 index = 0; index < mColumnOffsets.size(); index++)
	{
		const char *nextCol = dStrchr(text, '\t');
		U32 textWidth;
		if(nextCol)
			textWidth = mFont->getStrNWidth(( UTF8*)text, nextCol - text);
		else
			textWidth = mFont->getStrWidth(( UTF8*)text);
		if(mColumnOffsets[index] >= 0)
			width = getMax(width, mColumnOffsets[index] + textWidth);
		if(!nextCol)
			break;
		text = nextCol+1;
	}
	return width;
}

void GuiTextListCtrlEx::insertEntry(U32 id, const char *text, S32 index)
{
	Entry e;
	e.text = dStrdup(text);
	e.id = id;
	e.active = true;
	int nMaxRow = calculateMaxRowInCell(&e);
	if(!mList.size())
		mList.push_back(e);
	else
	{
		if(index > mList.size())
			index = mList.size();
		mList.insert(index);
		mList[index] = e;
	}
	mRowList.push_back(nMaxRow);
	mRowCount = getRowCount();
	setSize(Point2I(1, mList.size()));
}

void GuiTextListCtrlEx::addEntry(U32 id, const char *text)
{
	Entry e;
	e.text = dStrdup(text);
	e.id = id;
	e.active = true;
	int nMaxRow = calculateMaxRowInCell(&e);
	mList.push_back(e);
	mRowList.push_back(nMaxRow);
	mRowCount = getRowCount();
	setSize(Point2I(1, mList.size()));
}

void GuiTextListCtrlEx::setEntry(U32 id, const char *text)
{
	S32 e = findEntryById(id);
	if(e == -1)
		addEntry(id, text);
	else
	{
		dFree(mList[e].text);
		mList[e].text = dStrdup(text);

		// Still have to call this to make sure cells are wide enough for new values:
		setSize( Point2I( 1, mList.size() ) );
	}
	setUpdate();
}

void GuiTextListCtrlEx::setEntryActive(U32 id, bool active)
{
	S32 index = findEntryById( id );
	if ( index == -1 )
		return;

	if ( mList[index].active != active )
	{
		mList[index].active = active;

		// You can't have an inactive entry selected...
		if ( !active && mSelectedCell.y >= 0 && mSelectedCell.y < mList.size()
			&& mList[mSelectedCell.y].id == id )
			setSelectedCell( Point2I( -1, -1 ) );

		setUpdate();
	}
}

S32 GuiTextListCtrlEx::findEntryById(U32 id)
{
	for(U32 i = 0; i < mList.size(); i++)
		if(mList[i].id == id)
			return i;
	return -1;
}

S32 GuiTextListCtrlEx::findEntryByText(const char *text)
{
	for(U32 i = 0; i < mList.size(); i++)
		if(!dStricmp(mList[i].text, text))
			return i;
	return -1;
}

bool GuiTextListCtrlEx::isEntryActive(U32 id)
{
	S32 index = findEntryById( id );
	if ( index == -1 )
		return( false );

	return( mList[index].active );
}

void GuiTextListCtrlEx::setSize(Point2I newSize)
{
	mSize = newSize;

	if ( mFont )
	{
		if ( mSize.x == 1 && mFitParentWidth )
		{
			GuiScrollCtrl* parent = dynamic_cast<GuiScrollCtrl *>(getParent());
			if ( parent )
				mCellUnit.x = parent->getContentExtent().x;
		}
		else
		{
			// Find the maximum width cell:
			S32 maxWidth = 1;
			for ( U32 i = 0; i < mList.size(); i++ )
			{
				U32 rWidth = getRowWidth( &mList[i] );
				if ( rWidth > maxWidth )
					maxWidth = rWidth;
			}

			mCellUnit.x = maxWidth + 8;
		}

		if ( mCellUnit.y <= mFont->getHeight())
		{
			mCellUnit.y = mFont->getHeight() + 2;
		}
	}

	Point2I newExtent(/* newSize.x * */mCellUnit.x/* + mHeaderDim.x*/, mRowCount * mCellUnit.y + mHeaderDim.y );
	setExtent( newExtent );
}

void GuiTextListCtrlEx::clear()
{
	while (mList.size())
		removeEntry(mList[0].id);

	mMouseOverCell.set( -1, -1 );
	setSelectedCell(Point2I(-1, -1));
}

void GuiTextListCtrlEx::sort(U32 column, bool increasing)
{
	if (getNumEntries() < 2)
		return;
	sortColumn = column;
	sIncreasing = increasing;
	dQsort((void *)&(mList[0]), mList.size(), sizeof(Entry), textCompare);
}

void GuiTextListCtrlEx::sortNumerical( U32 column, bool increasing )
{
	if ( getNumEntries() < 2 )
		return;

	sortColumn = column;
	sIncreasing = increasing;
	dQsort( (void*) &( mList[0] ), mList.size(), sizeof( Entry ), numCompare );
}

void GuiTextListCtrlEx::sortID(bool increasing /* = true */)
{
	if ( getNumEntries() < 2 )
		return;

	sIncreasing = increasing;
	dQsort( (void*) &( mList[0] ), mList.size(), sizeof( Entry ), idCompare);
}

void GuiTextListCtrlEx::onRemove()
{
	clear();
	Parent::onRemove();
}

U32 GuiTextListCtrlEx::getNumEntries()
{
	return mList.size();
}

void GuiTextListCtrlEx::removeEntryByIndex(S32 index)
{
	if(index < 0 || index >= mList.size())
		return;
	dFree(mList[index].text);
	mList.erase(index);
	mRowList.erase(index);
	mRowCount = getRowCount();

	setSize(Point2I( 1, mList.size()));
	setSelectedCell(Point2I(-1, -1));
}

void GuiTextListCtrlEx::removeEntry(U32 id)
{
	S32 index = findEntryById(id);
	removeEntryByIndex(index);
}

const char *GuiTextListCtrlEx::getSelectedText()
{
	if (mSelectedCell.y == -1)
		return NULL;

	return mList[mSelectedCell.y].text;
}

const char *GuiTextListCtrlEx::getScriptValue()
{
	return getSelectedText();
}

void GuiTextListCtrlEx::setScriptValue(const char *val)
{
	S32 e = findEntryByText(val);
	if(e == -1)
		setSelectedCell(Point2I(-1, -1));
	else
		setSelectedCell(Point2I(0, e));
}

bool GuiTextListCtrlEx::onKeyDown( const GuiEvent &event )
{
	//if this control is a dead end, make sure the event stops here
	if ( !mVisible || !mActive || !mAwake )
		return true;

	S32 yDelta = 0;
	switch( event.keyCode )
	{
	case KEY_RETURN:
		if ( mAltConsoleCommand[0] )
			Con::evaluate( mAltConsoleCommand, false );
		break;
	case KEY_LEFT:
	case KEY_UP:
		if ( mSelectedCell.y > 0 )
		{
			mSelectedCell.y--;
			yDelta = -mCellUnit.y;
		}
		break;
	case KEY_DOWN:
	case KEY_RIGHT:
		if ( mSelectedCell.y < ( mList.size() - 1 ) )
		{
			mSelectedCell.y++;
			yDelta = mCellUnit.y;
		}
		break;
	case KEY_HOME:
		if ( mList.size() )
		{
			mSelectedCell.y = 0;
			yDelta = -(mCellUnit.y * mList.size() + 1 );
		}
		break;
	case KEY_END:
		if ( mList.size() )
		{
			mSelectedCell.y = mList.size() - 1;
			yDelta = (mCellUnit.y * mList.size() + 1 );
		}
		break;
	case KEY_DELETE:
		if ( mSelectedCell.y >= 0 && mSelectedCell.y < mList.size() )
			Con::executef( this, "onDeleteKey", Con::getIntArg( mList[mSelectedCell.y].id ) );
		break;
	default:
		return( Parent::onKeyDown( event ) );
		break;
	};

	GuiScrollCtrl* parent = dynamic_cast<GuiScrollCtrl *>(getParent());
	if ( parent )
		parent->scrollDelta( 0, yDelta );

	return ( true );



}

//-----------------------------------------------------------------------------
// Console Methods
//-----------------------------------------------------------------------------

ConsoleMethod(GuiTextListCtrlEx, getSelectedId, S32, 2, 2, "Get the ID of the currently selected item.")
{
	return object->getSelectedId();
}

ConsoleMethod( GuiTextListCtrlEx, setSelectedById, void, 3, 3, "(int id)"
			  "Finds the specified entry by id, then marks its row as selected.")
{
	S32 index = object->findEntryById(dAtoi(argv[2]));
	if(index < 0)
		return ;

	object->setSelectedCell(Point2I(0, index));
}

ConsoleMethod( GuiTextListCtrlEx, setSelectedRow, void, 3, 3, "(int rowNum)"
			  "Selects the specified row.")
{
	object->setSelectedCell( Point2I( 0, dAtoi( argv[2] ) ) );
}

ConsoleMethod( GuiTextListCtrlEx, getSelectedRow, S32, 2, 2, "getSelectedRow - returns row index (not ID)")
{
	return object->getSelectedRow();
}
ConsoleMethod( GuiTextListCtrlEx, clearSelection, void, 2, 2, "Set the selection to nothing.")
{
	object->setSelectedCell(Point2I(-1, -1));
}

ConsoleMethod(GuiTextListCtrlEx, addRow, S32, 4, 5, "(int id, string text, int index=0)"
			  "Returns row number of the new item.")
{
	S32 ret = object->mList.size();
	if(argc < 5)
		object->addEntry(dAtoi(argv[2]), argv[3]);
	else
		object->insertEntry(dAtoi(argv[2]), argv[3], dAtoi(argv[4]));

	return ret;
}

ConsoleMethod( GuiTextListCtrlEx, setRowById, void, 4, 4, "(int id, string text)")
{
	object->setEntry(dAtoi(argv[2]), argv[3]);
}

ConsoleMethod( GuiTextListCtrlEx, sort, void, 3, 4, "(int columnID, bool increasing=false)"
			  "Performs a standard (alphabetical) sort on the values in the specified column.")
{
	if ( argc == 3 )
		object->sort(dAtoi(argv[2]));
	else
		object->sort( dAtoi( argv[2] ), dAtob( argv[3] ) );
}

ConsoleMethod(GuiTextListCtrlEx, sortNumerical, void, 3, 4, "(int columnID, bool increasing=false)"
			  "Perform a numerical sort on the values in the specified column.")
{
	if ( argc == 3 )
		object->sortNumerical( dAtoi( argv[2] ) );
	else
		object->sortNumerical( dAtoi( argv[2] ), dAtob( argv[3] ) );
}

ConsoleMethod( GuiTextListCtrlEx, clear, void, 2, 2, "Clear the list.")
{
	object->clear();
}

ConsoleMethod( GuiTextListCtrlEx, rowCount, S32, 2, 2, "Get the number of rows.")
{
	return object->getNumEntries();
}

ConsoleMethod( GuiTextListCtrlEx, getRowId, S32, 3, 3, "(int index)"
			  "Get the row ID for an index.")
{
	U32 index = dAtoi(argv[2]);
	if(index >= object->getNumEntries())
		return -1;

	return object->mList[index].id;
}

ConsoleMethod( GuiTextListCtrlEx, getRowTextById, const char*, 3, 3, "(int id)"
			  "Get the text of a row with the specified id.")
{
	S32 index = object->findEntryById(dAtoi(argv[2]));
	if(index < 0)
		return "";
	return object->mList[index].text;
}

ConsoleMethod( GuiTextListCtrlEx, getRowNumById, S32, 3, 3, "(int id)"
			  "Get the row number for a specified id.")
{
	S32 index = object->findEntryById(dAtoi(argv[2]));
	if(index < 0)
		return -1;
	return index;
}

ConsoleMethod( GuiTextListCtrlEx, getRowText, const char*, 3, 3, "(int index)"
			  "Get the text of the row with the specified index.")
{
	U32 index = dAtoi(argv[2]);
	if(index < 0 || index >= object->mList.size())
		return "";
	return object->mList[index].text;
}

ConsoleMethod( GuiTextListCtrlEx, removeRowById, void, 3, 3,"(int id)"
			  "Remove row with the specified id.")
{
	object->removeEntry(dAtoi(argv[2]));
}

ConsoleMethod( GuiTextListCtrlEx, removeRow, void, 3, 3, "(int index)"
			  "Remove a row from the table, based on its index.")
{
	U32 index = dAtoi(argv[2]);
	object->removeEntryByIndex(index);
}

ConsoleMethod( GuiTextListCtrlEx, scrollVisible, void, 3, 3, "(int rowNum)"
			  "Scroll so the specified row is visible.")
{
	object->scrollCellVisible(Point2I(0, dAtoi(argv[2])));
}

ConsoleMethod( GuiTextListCtrlEx, findTextIndex, S32, 3, 3, "(string needle)"
			  "Find needle in the list, and return the row number it was found in.")
{
	return( object->findEntryByText( argv[2] ) );
}

ConsoleMethod( GuiTextListCtrlEx, setRowActive, void, 4, 4, "(int rowNum, bool active)"
			  "Mark a specified row as active/not.")
{
	object->setEntryActive( U32( dAtoi( argv[2] ) ), dAtob( argv[3] ) );
}

ConsoleMethod( GuiTextListCtrlEx, isRowActive, bool, 3, 3, "(int rowNum)"
			  "Is the specified row currently active?")
{
	return( object->isEntryActive( U32( dAtoi( argv[2] ) ) ) );
}

ConsoleMethod(GuiTextListCtrlEx,sortID,void,3,3,"(bool increasing )")
{
	return(object->sortID(atoi(argv[2])));
}