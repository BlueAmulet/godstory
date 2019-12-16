#pragma once

#include <list>
#include "math/mPoint.h"
#include "math/mMatrix.h"
#include "T3D/shapeBase.h"
#include "T3D/item.h"
#include "Gameplay/GameObjects/GameObject.h"

struct PrizeBoxData : public ShapeBaseData
{
private:
	typedef ShapeBaseData Parent;

public:
	
};

class ItemShortcut;
class DropItemList;

class PrizeBox : public Item
{
	typedef ShapeBase Parent;

public:
	const static U64 SendMask		= Parent::NextFreeMask;

	PrizeBox();
	~PrizeBox();
	
	void	clear();

	void	processTick(const Move * move);
	bool	onAdd();
	void	onRemove();
	void	onEnabled();	
	bool	onNewDataBlock(GameBaseData* dptr);

public:
	virtual bool canAddToScope(NetConnection *con);

#ifdef NTJ_SERVER
	U64			packUpdate  (NetConnection *conn, U64 mask, BitStream *stream);
	static bool CreatePrizeBox(NpcObject *obj, DropItemList *pList);

	DropItemList*         m_DropItemList;
#endif


#ifdef NTJ_CLIENT
	void unpackUpdate(NetConnection *conn, BitStream *stream);
	
	F32 mAddSpeed;
	F32 mSpeed;
	F32 mZDiff;
	bool mCanPickup;
	bool m_IsBox;
	ItemShortcut *m_pItemShortcut;
	MatrixF m_mat;
	Point3F m_scale;
#endif

	SimTime m_currentTime;
	SimTime m_dropTime;

	F32  mGroundZDiff;

	DECLARE_CONOBJECT(PrizeBox);
};

class PrizeBoxManager
{
public:
#ifdef NTJ_CLIENT
	struct PrizeBoxPickup
	{
		PrizeBoxPickup(PrizeBox *pPrizeBox = NULL, bool pHadPicked = false) 
					: m_PrizeBox(pPrizeBox), m_hadPicked(pHadPicked) { }

		PrizeBox *m_PrizeBox;
		bool m_hadPicked;
	};

	typedef std::list<PrizeBoxPickup *> PrizeBoxList;
#endif

#ifdef NTJ_SERVER
	typedef std::list<PrizeBox *> PrizeBoxList;
#endif
#ifdef NTJ_EDITOR
	typedef std::list<PrizeBox *> PrizeBoxList;
#endif

	PrizeBoxManager();
	~PrizeBoxManager();
	void		Clear();
	S32			GetCount();
	void		DeletePrizeBox(PrizeBox *pPrizeBox);
	void		PushBack(PrizeBox *pPrizeBox);
	void		PopFront();

#ifdef NTJ_CLIENT
	void		ClearHadPicked();
	void		SetHadPicked(PrizeBox *pPrizeBox);
	PrizeBox*	GetNearest(GameObject *pObj, F32 radius);
#endif

private:
	PrizeBoxList m_list;
#ifdef NTJ_SERVER
	U32 m_nMaxCount;
#endif
};

extern PrizeBoxManager g_PrizeBoxManager;