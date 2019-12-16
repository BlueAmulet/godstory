#include "console/console.h"
#include "console/consoleTypes.h"
#include "Gameplay/Data/readDataFile.h"
#include "math/mPoint.h"
#include "Gameplay/Data/RouteRepository.h"
#ifdef  NTJ_CLIENT
#include "Gameplay/Ai/NaviGrid/NavigationBlock.h"
#include "Gameplay/ClientGameplayState.h"
#include "UI/guiViewMapCtrl.h"
#endif


#define GAME_STRIDEEDATA_FILE "gameres/data/repository/StrideServerpository.dat"

RouteRepository g_RouteRepository;

RouteRepository::RouteRepository()
{

}

RouteRepository::~RouteRepository()
{
	clear();
}

void RouteRepository::read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];
	RouteData* pRouteData = NULL;

	Platform::makeFullPathName(GAME_ROUTEDATA_FILE, filename, sizeof(filename));
	if(!op.readDataFile(filename))
		return;

	for(int i=0; i<op.RecordNum; ++i)
	{
		pRouteData = new RouteData;

		// 寻径编号
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "RouteRepository.dat::Read - failed to read mRouteID!", i);
		pRouteData->mRouteID = tempdata.m_U32;

		// 地图编号
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "RouteRepository.dat::Read - failed to read mMapID!", i);
		pRouteData->mMapID = tempdata.m_U32;

		// 坐标 1
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "RouteRepository.dat::Read - failed to read POS(1)!", i);
		AssertErrorMsg(pRouteData->addPosition(tempdata.m_string), "RouteRepository.dat::Read - failed to read POS(1)!", i);

		// 坐标 2
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "RouteRepository.dat::Read - failed to read POS(2) !", i);
		AssertErrorMsg(pRouteData->addPosition(tempdata.m_string), "RouteRepository.dat::Read - failed to read POS(2)!", i);

		// 坐标 3
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "RouteRepository.dat::Read - failed to read POS(3) !", i);
		AssertErrorMsg(pRouteData->addPosition(tempdata.m_string), "RouteRepository.dat::Read - failed to read POS(3)!", i);

		// 坐标 4
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "RouteRepository.dat::Read - failed to read POS(4) !", i);
		AssertErrorMsg(pRouteData->addPosition(tempdata.m_string), "RouteRepository.dat::Read - failed to read POS(4)!", i);

		// 坐标 5
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "RouteRepository.dat::Read - failed to read POS(5) !", i);
		AssertErrorMsg(pRouteData->addPosition(tempdata.m_string), "RouteRepository.dat::Read - failed to read POS(5)!", i);

		AssertErrorMsg(pRouteData->mPositionVec.size() > 0, "RouteRepository.dat::Read - No position specified!", i);

		//编号类型
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_enum8, "RouteRepository.dat::Read - failed to read mTargetType!", i);
		pRouteData->mTargetType = tempdata.m_Enum8;

		//编号
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_U32, "RouteRepository.dat::Read - failed to read mTargetID!", i);
		pRouteData->mTargetID = tempdata.m_U32;

		//行为标示
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_enum8, "RouteRepository.dat::Read - failed to read mBehavior!", i);
		pRouteData->mBehavior = tempdata.m_Enum8;

		//文本信息
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_string, "RouteRepository.dat::Read - failed to read mTextInfo!", i);
		//特殊处理，去除"传送至"前面的地图名称
		char* pDest = dStrstr(tempdata.m_string,"传送至");
		if(NULL == pDest)
			pRouteData->mTextInfo = StringTable->insert(tempdata.m_string);
		else
			pRouteData->mTextInfo = StringTable->insert(pDest);

		//类别
		op.GetData(tempdata);
		AssertErrorMsg(tempdata.m_Type == DType_enum8, "RouteRepository.dat::Read - failed to read mCategoryType!", i);
		pRouteData->mCategoryType = tempdata.m_Enum8;

		if (!insert(pRouteData))
            delete pRouteData;
	}

	//大地图寻径默认数据
	pRouteData = new RouteData;
	pRouteData->mRouteID  = 899999999;
	pRouteData->mPositionVec.push_back(Point3F(0,0,0));
	insert(pRouteData);
}

void RouteRepository::clear()
{
	for(RouteMap::iterator it = m_RouteMap.begin(); it != m_RouteMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	m_RouteMap.clear();
}

bool RouteRepository::insert(RouteData* pRouteData)
{
	if(!pRouteData)
		return false;

	return m_RouteMap.insert( RouteMap::value_type(pRouteData->mRouteID, pRouteData) ).second;
}

RouteData* RouteRepository::getRouteData(U32 nRouteId)
{
	RouteMap::iterator it = m_RouteMap.find(nRouteId);

	if (it != m_RouteMap.end())
		return it->second;

	return NULL;
}


void RouteRepository::AssertErrorMsg(bool bAssert, const char *msg, int rowNum)
{
	char buf[256];

	if (!bAssert)
	{
		dSprintf(buf, sizeof(buf), "RowNum: %d  ", rowNum);
		dStrcat(buf, sizeof(buf), msg);
		AssertRelease(false, buf);
	}
}

bool RouteData::addPosition( char *string )
{
	F32 x, y, z;
	char *pStart = string;
	char *pEnd = NULL;
	
	// string没有数据
	if (string == NULL || *string == 0 || strcmp(string, " ") == 0)
		return true;

	pEnd = strchr(pStart, ' ');
	if (pEnd == NULL || *pEnd == 0)
		return false;

	*pEnd = 0;
	x = atof(pStart);

	pStart = pEnd + 1;
	if (pStart == NULL || *pStart == 0)
		return false;

	pEnd = strchr(pStart, ' ');

	if (pEnd == NULL || *pEnd == 0)
		return false;

	*pEnd = 0;
	y = atof(pStart);

	pStart = pEnd + 1;
	if (pStart == NULL || *pStart == 0)
		return false;

	z = atof(pStart);

	mPositionVec.push_back( Point3F(x, y, z) );
	
	return true;
}

static char* categoryName[8] =
{
	"",
	"功能性NPC",
	"非功能性NPC",
	"怪物列表",
	"洞天福地",
	"地宫入口",
	"地图传送点",
	"资源采集点"
};

StringTableEntry RouteData::getCategoryName()
{
	return categoryName[mCategoryType];
}

Point3F& RouteData::GetRoutePosition()
{
	S32 nPosCount = mPositionVec.size();

	S32 nPosIndex = mRandI(1, nPosCount);
	return mPositionVec[nPosIndex - 1];
}

U32 RouteRepository::getMapID(U32 MapId)
{
	return MapId/10000%10000;
}
#ifdef NTJ_CLIENT

ConsoleFunction(getRoute_Behavior, S32, 2, 2, "getRoute_Behavior(nRouteID);" )
{
	U32 nRouteID = atoi(argv[1]);

	RouteData *pRouteData = g_RouteRepository.getRouteData(nRouteID);
	if (pRouteData == NULL)
	{
		return 0;
	}

	return pRouteData->mBehavior;
}

ConsoleFunction(getRoute_MapID, S32, 2, 2, "getRoute_MapID(nRouteID);" )
{
	U32 nRouteID = atoi(argv[1]);

	RouteData *pRouteData = g_RouteRepository.getRouteData(nRouteID);
	if (pRouteData == NULL)
	{
		return 0;
	}

	return pRouteData->mMapID;
}

ConsoleFunction(getRoute_TargetType, S32, 2, 2, "getRoute_TargetType(nRouteID);" )
{
	U32 nRouteID = atoi(argv[1]);

	RouteData *pRouteData = g_RouteRepository.getRouteData(nRouteID);
	if (pRouteData == NULL)
	{
		return 0;
	}

	return pRouteData->mTargetType;
}

ConsoleFunction(getRoute_TargetID, S32, 2, 2, "getRoute_TargetID(nRouteID);" )
{
	U32 nRouteID = atoi(argv[1]);

	RouteData *pRouteData = g_RouteRepository.getRouteData(nRouteID);
	if (pRouteData == NULL)
	{
		return 0;
	}

	return pRouteData->mTargetID;
}

ConsoleFunction(getRoute_TextInfo, StringTableEntry, 2, 2, "getRoute_TextInfo(nRouteID);" )
{
	U32 nRouteID = atoi(argv[1]);

	RouteData *pRouteData = g_RouteRepository.getRouteData(nRouteID);
	if (pRouteData == NULL)
	{
		return 0;
	}

	return pRouteData->mTextInfo;
}

ConsoleFunction(getRoute_Position, StringTableEntry, 2, 2, "getRoute_Position(nRouteID);" )
{
	char *strRet = Con::getReturnBuffer(64);
	U32 nRouteID = atoi(argv[1]);

	RouteData *pRouteData = g_RouteRepository.getRouteData(nRouteID);
	if (pRouteData == NULL)
	{
		return "";
	}
	g_ClientGameplayState->setFindPathID(nRouteID);
	Point3F position = pRouteData->GetRoutePosition();
	dSprintf(strRet, 64, "%f %f %f", position.x, position.y, position.z);	
	return strRet;
}

ConsoleFunction(getPath_Position, StringTableEntry, 2, 2, "getPath_Position(nRouteID);")
{
	char *strRet = Con::getReturnBuffer(64);
	Point3F pos = getPath_Position(dAtoi(argv[1]));
	if(!pos.isZero())
	{
		dSprintf(strRet,64,"%f %f %f",pos.x,pos.y,pos.x);
		return strRet;
	}

	return "";
}
#endif

//跨服寻径
StrideRepository g_StrideRepository;

StrideRepository::StrideRepository()
{

}

StrideRepository::~StrideRepository()
{
	clear();
}

void StrideRepository::read()
{
	CDataFile op;
	RData tempdata;
	char filename[256];
	StrideServerPath* pStrideData = NULL;

	Platform::makeFullPathName(GAME_STRIDEEDATA_FILE, filename, sizeof(filename));
	if(!op.readDataFile(filename))
		return;

	for(int i=0; i<op.RecordNum; ++i)
	{
		pStrideData = new StrideServerPath;

		// 当前地图ID
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "StrideRepository.dat::Read - failed to read mCurrentZone!");
		pStrideData->mCurrentZone = tempdata.m_U32;

		// 可传送得地图ID
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "StrideRepository.dat::Read - failed to read mDestZone!");
		pStrideData->mDestZone = tempdata.m_U32;

		// 寻径ID
		op.GetData(tempdata);
		AssertRelease(tempdata.m_Type == DType_U32, "StrideRepository.dat::Read - failed to read mFindPathID!");
		pStrideData->mFindPathID = tempdata.m_U32;

		insert(avar("%d_%d",pStrideData->mCurrentZone, pStrideData->mDestZone), pStrideData);
	}
}

void StrideRepository::clear()
{
	for(StrideMap::iterator it = mStrideMap.begin(); it != mStrideMap.end(); ++it)
	{
		if(it->second)
			delete it->second;
	}
	mStrideMap.clear();
}

bool StrideRepository::insert(StringTableEntry index, StrideServerPath* path)
{
	if(!path)
		return false;

	mStrideMap.insert( StrideMap::value_type(index, path));
	return true;
}

StrideServerPath* StrideRepository::getStrideServerPath(StringTableEntry zoneID)
{
	StrideMap::iterator it = mStrideMap.find(zoneID);
	if( it != mStrideMap.end())
	{
		return it->second;
	}
	return NULL;
}

bool StrideRepository::isInlist(Xnode* node, NODELIST& findList)
{
	NODELIST::iterator itB = findList.begin();
	for(; itB != findList.end(); ++itB)
	{
		if(node->zone == (*itB)->zone)
			return true;
	}
	return false;
}

void StrideRepository::findNode(Xnode* node, NODELIST& findList)
{
	StrideMap::iterator itB = mStrideMap.begin();
	for(; itB != mStrideMap.end(); ++itB)
	{
		StrideServerPath* pathNode = itB->second;
		if(pathNode->mCurrentZone == node->zone)
		{
			Xnode* fnode = new Xnode;
			fnode->parentNode = node;
			fnode->zone = pathNode->mDestZone;
			findList.push_back(fnode);
		}
	}
}

void StrideRepository::clearNode()
{
	NODELIST::iterator itB = openList.begin();
	for(; itB != openList.end(); ++itB)
	{
		Xnode* node = *itB;
		if(node)
			delete node;
	}
	openList.clear();
	
	itB = closeList.begin();
	for(; itB != closeList.end(); ++itB)
	{
		Xnode* node = *itB;
		if(node)
			delete node;
	}
	closeList.clear();
}

Xnode* StrideRepository::SearchNode(U32 startzone, U32 endzone)
{
	Xnode* firstnode = new Xnode;
	firstnode->parentNode = NULL;
	firstnode->zone = startzone;
	openList.push_back(firstnode);

	NODELIST::iterator itB = openList.begin();
	while(!openList.empty())
	{
		Xnode* node = *itB;
		if(node->zone == endzone)
		{
			return node;
		}
		else
		{
			closeList.push_back(node);

			NODELIST findList;
			findNode(node, findList);
			NODELIST::iterator itA = findList.begin();
			for(; itA != findList.end();)
			{
				Xnode* fnode = *itA;
				if(!isInlist(fnode, openList) && !isInlist(fnode, closeList))
				{
					openList.push_back(fnode);
				}
				else
				{
					delete fnode;
				}
				findList.erase(itA++);
			}
			
			openList.erase(itB++);
		}
	}
	return NULL;
}

//void StrideRepository::tempInsert(U32 startzone, U32 endzone, U32 pathid)
//{
//	char szStr[32];
//	sprintf(szStr, "%d_%d", startzone, endzone);
//	StrideServerPath* pathNode;
//	pathNode->mCurrentZone = startzone;
//	pathNode->mDestZone = endzone;
//	pathNode->mFindPathID = pathid;
//	mStrideMap.insert(StrideMap::value_type(szStr, pathNode));
//}

void StrideRepository::ParseNode(Xnode* node, std::deque<std::string>& PathMap)
{
	PathMap.clear();
	char szStr[32];
	U32 lastZone = node->zone;
	node = node->parentNode;
	while(node)
	{
		dSprintf(szStr, sizeof(szStr), "%d_%d", node->zone, lastZone);
		lastZone = node->zone;
		PathMap.push_front(szStr);
		node = node->parentNode;		
	}
}
#ifdef NTJ_CLIENT
Point3F getPath_Position(U32 routeId)
{	
	Point3F position(0,0,0);
	StringTableEntry npcName = StringTable->insert("");

	Player* pPlayer =  g_ClientGameplayState->GetControlPlayer();
	if(NULL == pPlayer)
		return position;

	if(routeId == 0)
	{
		AssertWarn(false, "Cannt find routeId");
		return position;
	}
	
	RouteData *pRouteData = g_RouteRepository.getRouteData(routeId);
	if (NULL == pRouteData)
	{
		AssertWarn(false, "Cannt find routeData");
		return position;
	}
		
	S32 mapId = g_ViewMap.convertZoneIdToMapId(g_ClientGameplayState->getCurrentZoneId());
	if(!g_ViewMap.isValidMapId(mapId))
		return position;

	U32 iDestMapId = g_RouteRepository.getMapID(pRouteData->mMapID);
	npcName = pRouteData->mTextInfo;

	g_ClientGameplayState->clearPath();
	if(mapId == iDestMapId)
	{
		g_ClientGameplayState->setFindPathID(routeId);
		position = pRouteData->GetRoutePosition();
		if(pPlayer->getPosition() == position)//位置相同不处理
			return position;

		//修正Z轴值，防修改地形网格后与data数据不一至
		position = g_NavigationManager->SlerpPosition(position);
		Con::executef("OpenFindStridePathDialog",
			Con::getIntArg(routeId),
			Con::getIntArg(iDestMapId), 
			npcName);

		g_ClientGameplayState->setDarwPath(true);
		g_StrideRepository.clearNode();
	}
	else
	{
		Xnode* node = g_StrideRepository.SearchNode(mapId, iDestMapId);
		if(!node)
		{
			g_StrideRepository.clearNode();
			return position;
		}
		std::deque<std::string> PathMap;
		std::vector<U32> pathID;
		g_StrideRepository.ParseNode(node, PathMap);
		g_StrideRepository.clearNode();
		//delete node;
		for(int i=0; i<PathMap.size(); ++i)
		{
			StrideServerPath* serverPath = g_StrideRepository.getStrideServerPath(PathMap[i].c_str());
			if(serverPath)
				pathID.push_back(serverPath->mFindPathID);
		}
		pathID.push_back(routeId);
		g_ClientGameplayState->setStrideServerPath(pathID);
		RouteData *pData = g_RouteRepository.getRouteData(pathID[0]);
		if(pData)
		{
			g_ClientGameplayState->setFindPathID(pathID[0]);
			g_ClientGameplayState->setStrideServerPathFlag(true);
			position = pData->GetRoutePosition();

			Con::executef("OpenFindStridePathDialog",
				Con::getIntArg(routeId),
				Con::getIntArg(iDestMapId), 
				npcName);

			g_ClientGameplayState->setDarwPath(true);
		}
	}

	//修正Z轴值，防修改地形网格后与data数据不一至
	position = g_NavigationManager->SlerpPosition(position);
	return position;
}
#endif
