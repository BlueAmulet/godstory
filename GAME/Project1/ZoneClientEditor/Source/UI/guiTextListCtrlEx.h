//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _GuiTextListCtrlEx_H_
#define _GuiTextListCtrlEx_H_

#ifndef _GUIARRAYCTRL_H_
#include "ui/guiArrayCtrlEx.h"
#endif

class GuiTextListCtrlEx : public GuiArrayCtrlEx
{
private:
	typedef GuiArrayCtrlEx Parent;

public:
	struct Entry
	{
		U32 id;
		char *text;		
		bool active;
	};

	Vector<Entry> mList;
	Vector<int>	  mRowList;
	int mRowCount;

	bool mEnumerate;
	bool mResizeCell;

	StringTableEntry	m_TextListSelectImageFile;		
	GFXTexHandle m_TextListSelectTexture;

	StringTableEntry	m_TextListMouseOnImageFile;		
	GFXTexHandle m_TextListMouseOnTexture;

	StringTableEntry	m_TextListBackgroundImageFile1;			//ÆæÊý±³¾°Í¼
	GFXTexHandle m_TextListBackgroundTexture1;
	StringTableEntry	m_TextListBackgroundImageFile2;			//Å¼Êý±³¾°Í¼
	GFXTexHandle m_TextListBackgroundTexture2;

protected:
	enum ScrollConst
	{
		UP = 0,
		DOWN = 1
	};
	enum {
		InvalidId = 0xFFFFFFFF
	};
	Vector<S32> mColumnOffsets;

	bool  mFitParentWidth;
	bool  mClipColumnText;

	U32 getRowWidth(Entry *row);
	void onCellSelected(Point2I cell);
	int calculateMaxRowInCell(Entry *row);

public:
	GuiTextListCtrlEx();
	~GuiTextListCtrlEx();

	DECLARE_CONOBJECT(GuiTextListCtrlEx);

	static void initPersistFields();

	virtual void setCellSize( const Point2I &size ){ mCellUnit = size; }
	virtual void getCellSize(       Point2I &size ){ size = mCellUnit; }

	const char *getScriptValue();
	void setScriptValue(const char *value);

	U32 getNumEntries();

	void clear();
	virtual void addEntry(U32 id, const char *text);
	virtual void insertEntry(U32 id, const char *text, S32 index);
	void setEntry(U32 id, const char *text);
	void setEntryActive(U32 id, bool active);
	S32 findEntryById(U32 id);
	S32 findEntryByText(const char *text);
	bool isEntryActive(U32 id);

	U32 getEntryId(U32 index);

	bool onWake();
	void removeEntry(U32 id);
	virtual void removeEntryByIndex(S32 id);
	virtual void sort(U32 column, bool increasing = true);
	virtual void sortNumerical(U32 column, bool increasing = true); 
	virtual void sortID(bool increasing = true);

	U32 getSelectedId();
	U32 getSelectedRow();
	const char *getSelectedText();

	bool onKeyDown(const GuiEvent &event);

	virtual int  getEntryIndex(S32 nRowIndex);
	virtual int  getRowNumber(S32 nCellIndex);
	virtual int  getRowCount(S32 nCellIndex = -1);
	virtual void onRenderCell(Point2I offset, Point2I cell, bool selected, bool mouseOver);

	void setSize(Point2I newSize);
	void onRemove();
	void addColumnOffset(S32 offset) { mColumnOffsets.push_back(offset); }
	void clearColumnOffsets() { mColumnOffsets.clear(); }
};

#endif //_GUI_TEXTLIST_CTRL_H
