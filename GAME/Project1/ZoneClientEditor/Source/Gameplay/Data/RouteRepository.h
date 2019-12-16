#pragma once
#ifndef _ROUTEREPOSITORY_H_
#define _ROUTEREPOSITORY_H_

#include <deque>

struct RouteData
{
	U32 mRouteID;
	U32 mMapID;
	Vector<Point3F> mPositionVec;
	U8 mTargetType;
	U32 mTargetID;
	U8 mBehavior;
	StringTableEntry mTextInfo;
	U8 mCategoryType;                 //用于大地图分类查询显示

	Point3F&              GetRoutePosition();
	bool                  addPosition(char *string);
	StringTableEntry      getCategoryName();

	RouteData() { dMemset(this,0,sizeof(RouteData));}
};

class RouteRepository
{
public:
	typedef stdext::hash_map<U32, RouteData *> RouteMap;

	RouteRepository();
	~RouteRepository();

	void                      read();
	void                      clear();
	bool                      insert(RouteData* pRouteData);
	RouteData*                getRouteData(U32 nRouteId);
	inline U32	              getMapID(U32 mapId);
	inline RouteMap&          getData() { return m_RouteMap; }

private:
	void AssertErrorMsg(bool bAssert, const char *msg, int rowNum);

private:
	RouteMap m_RouteMap;
};
//跨服寻径
struct  StrideServerPath
{
	U32					mCurrentZone;		//当前地图ID
	U32					mDestZone;			//可传送地图ID
	//StringTableEntry	mTransimissionPos;	//传送坐标
	U32					mFindPathID;		//寻径ID
};

struct Xnode
{
	Xnode*	parentNode;
	U32		zone;
};

class StrideRepository
{
public:
	typedef stdext::hash_map<std::string, StrideServerPath*> StrideMap;
	typedef std::list<Xnode*> NODELIST;
	NODELIST openList,closeList;
	StrideRepository();
	~StrideRepository();
	
	bool isInlist(Xnode* node, NODELIST& findList);
	void findNode(Xnode* node, NODELIST& findList);
	void clearNode();
	Xnode* SearchNode(U32 startzone, U32 endzone);
	//void tempInsert(U32 startzone, U32 endzone, U32 pathid);
	void ParseNode(Xnode* node, std::deque<std::string>& PathMap);

	void read();
	void clear();
	bool insert(StringTableEntry index, StrideServerPath* path);
	StrideServerPath* getStrideServerPath(StringTableEntry zoneID);
private:
	StrideMap mStrideMap;
};
#ifdef NTJ_CLIENT
Point3F getPath_Position(U32 routeId);
#endif

extern RouteRepository g_RouteRepository;
extern StrideRepository g_StrideRepository;

#endif