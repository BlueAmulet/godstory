//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#include <crtdbg.h>
#include "TS/TSShapeRepository.h"
#include "math/MathIO.h"
#include "ts/tsShape.h"
//#include "DatBuilder/DatBuilder.h"
#include "Gameplay/Data/readDataFile.h"
#include <time.h>

TSShapeRepository g_TSShapeRepository;
const Box3F TSShapeInfo::csBox(-0.5f, -0.5f, 0.0f, 0.5f, 0.5f, 1.0f);

// ========================================================================================================================================
//	TSShapeInfo - Data Persistence
// ========================================================================================================================================

TSShapeInfo::TSShapeInfo() : m_Initialized(false),m_isDynamicLoad(false),m_Box(csBox)
{
	dMemset(m_ActionToSequence, -1, sizeof(m_ActionToSequence));
}

TSShapeInfo::~TSShapeInfo()
{
}

enum SHAPE_LOAD_TYPE
{
    SHAPE_lOAD_MODEL,
    SHAPE_lOAD_SEQ,
};

bool TSShapeInfo::BackgroundLoad(bool isIncludeModelFile)
{
    //模型文件
    if(!m_TSShape && isIncludeModelFile)
    {
        char filename[1024];
        char path[1024];
        dSprintf(path, sizeof(path), "%s%s", m_TSShapePath, m_TSShapeFile);
        Platform::makeFullPathName(path,filename,sizeof(filename));

        if (!ResourceManager->HasLoaded(filename))
        {
            CBackgroundLoadMgr::Instance()->Add(this,false,SHAPE_lOAD_MODEL,0,filename);
        }
		else
		{
			__LoadShape();
		}
    }

    //动画文件
    for(SequenceFiles::const_iterator itr = m_SequenceFiles.begin(); itr != m_SequenceFiles.end(); ++itr)
    {
        const SequenceFile& SeqFile = itr->second;

        if(SeqFile.m_action >= GameObjectData::NumActionAnims)
            continue;

        if(m_ActionToSequence[SeqFile.m_action] != -1)
            continue;

        StringTableEntry name = GameObjectData::ActionAnimationList[SeqFile.m_action].name;

        char filename[1024];
        char path[1024];
        dSprintf(path, sizeof(path), "%s%s", m_TSShapePath, SeqFile.m_Filename);
        Platform::makeFullPathName(path,filename,sizeof(filename));

        //动画文件这里是直接打开文件流读取的，不存在资源操作
        CBackgroundLoadMgr::Instance()->Add(this,true,SHAPE_lOAD_SEQ,SeqFile.m_action,filename);
    }

    return true;
}

void TSShapeInfo::OnLoaded(U32 type,U32 flag,ResourceObject* pRes)
{
    if (type != SHAPE_lOAD_MODEL)
        return;

    m_TSShape = pRes;

    if (!m_TSShape)
        return;

    //TODO 判断是否是当前的资源

    m_TSShape->initNodesMatters("Bip01 Spine1");
    onShapeLoaded();
}

void TSShapeInfo::OnLoaded(U32 type,U32 flag,Stream* pStream)
{
    if (type != SHAPE_lOAD_SEQ)
        return;

    U32 action = flag;

    if(action >= GameObjectData::NumActionAnims)
        return;

    if(m_ActionToSequence[action] != -1)
        return;

    if (0 == m_TSShape)
        return;

    StringTableEntry name = GameObjectData::ActionAnimationList[action].name;

    if (!m_TSShape->importSequences(pStream) || pStream->getStatus()!=Stream::Ok)
    {
        Con::warnf("Load sequence %s failed for %s", name, m_TSShapeFile);
        return;
    }

    m_TSShape->sequences.last().nameIndex = m_TSShape->findName(name);

    if (m_TSShape->sequences.last().nameIndex == -1)
    {
        m_TSShape->sequences.last().nameIndex = m_TSShape->names.size();
        m_TSShape->names.increment();
        m_TSShape->names.last() = StringTable->insert(name,false);
    }

    m_ActionToSequence[action] = m_TSShape->sequences.size()-1;
}

TSShapeInfo* TSShapeInfo::InitializeTSShape(bool bLoadSeq, bool computeCRC,bool isBackgroundLoad)
{
    //已经载入
    if (IsInitialized())
    {
        return this;
    }

    if (!isBackgroundLoad)
    {
        __LoadShape();
    }
    else
    {
#ifdef NTJ_ENABLE_BACKGROUNDLOAD
        //异步载入动画数据
        BackgroundLoad(true);
#else 
        __LoadShape();
#endif
    }

    return this;
}

Resource<TSShape>& TSShapeInfo::__LoadShape(void)
{
    if (!m_TSShape)
    {
        char filename[1024];
        char path[1024];
        dSprintf(path, sizeof(path), "%s%s", m_TSShapePath, m_TSShapeFile);
        Platform::makeFullPathName(path,filename,sizeof(filename));

        m_TSShape = ResourceManager->load(filename,false);

        if(m_TSShape)
            m_TSShape->initNodesMatters("Bip01 Spine1");
		onShapeLoaded();
    }

    return m_TSShape;
}

void TSShapeInfo::onShapeLoaded()
{
	if(!m_TSShape)
		return;

	S32 i = 0;
	// Resolve details and camera node indexes.
	for (i = 0; i < m_TSShape->details.size(); i++)
	{
		char* name = (char*)m_TSShape->names[m_TSShape->details[i].nameIndex];

		if (dStrstr((const char *)dStrlwr(name), "collision-"))
		{
			collisionDetails.push_back(i);
			collisionBounds.increment();

			m_TSShape->computeBounds(collisionDetails.last(), collisionBounds.last());
			m_TSShape->getAccelerator(collisionDetails.last());

			if (!m_TSShape->bounds.isContained(collisionBounds.last()))
			{
				Con::warnf("Warning: shape %s collision detail %d (Collision-%d) bounds exceed that of shape.", m_TSShapeId, collisionDetails.size() - 1, collisionDetails.last());
				collisionBounds.last() = m_TSShape->bounds;
			}
			else if (collisionBounds.last().isValidBox() == false)
			{
				Con::errorf("Error: shape %s-collision detail %d (Collision-%d) bounds box invalid!", m_TSShapeId, collisionDetails.size() - 1, collisionDetails.last());
				collisionBounds.last() = m_TSShape->bounds;
			}

			// The way LOS works is that it will check to see if there is a LOS detail that matches
			// the the collision detail + 1 + MaxCollisionShapes (this variable name should change in
			// the future). If it can't find a matching LOS it will simply use the collision instead.
			// We check for any "unmatched" LOS's further down
			LOSDetails.increment();

			char buff[128];
			dSprintf(buff, sizeof(buff), "LOS-%d", i + 1 + MaxCollisionShapes);
			U32 los = m_TSShape->findDetail(buff);
			if (los == -1)
				LOSDetails.last() = i;
			else
				LOSDetails.last() = los;
		}
	}

	// Snag any "unmatched" LOS details
	for (i = 0; i < m_TSShape->details.size(); i++)
	{
		char* name = (char*)m_TSShape->names[m_TSShape->details[i].nameIndex];

		if (dStrstr((const char *)dStrlwr(name), "los-"))
		{
			// See if we already have this LOS
			bool found = false;
			for (U32 j = 0; j < LOSDetails.size(); j++)
			{
				if (LOSDetails[j] == i)
				{
					found = true;
					break;
				}
			}

			if (!found)
				LOSDetails.push_back(i);
		}
	}

	// Resolve mount point node indexes
	//for (i = 0; i < NumMountPoints; i++) 
	//{
	//	char fullName[256];
	//	dSprintf(fullName,sizeof(fullName),"Link%02dPoint",i);
	//	mountPointNode[i] = m_TSShape->findNode(fullName);
	//}
	m_Initialized = true;
}

S32 TSShapeInfo::getNodeIndex(StringTableEntry nodeName)
{
	if(!m_TSShape)
		return -1;
	static NodeIndexMap::iterator it;
	nodeName = StringTable->insert(nodeName);
	it = mountPointNode.find(nodeName);
	if(it == mountPointNode.end())
	{
		S32 ni = m_TSShape->findNode(nodeName);
		mountPointNode.insert(NodeIndexMap::value_type(nodeName, ni));
		return ni;
	}
	return it->second;
}

S32 TSShapeInfo::InitializeAction(U32 action)
{
	if(!m_TSShape)
		return -1;

	if(action >= GameObjectData::NumActionAnims)
		return -1;
	// 已经载入该动作了
	if(m_ActionToSequence[action] != -1)
		return m_ActionToSequence[action];

	SequenceFiles::const_iterator itr = m_SequenceFiles.find(action);
	// 没有该动作
	if(itr == m_SequenceFiles.end())
		return -1;

	StringTableEntry name = GameObjectData::ActionAnimationList[action].name;
	char filename[1024];
	char path[1024];
	dSprintf(path, sizeof(path), "%s%s", m_TSShapePath, itr->second.m_Filename);
	Platform::makeFullPathName(path,filename,sizeof(filename));

	Stream* stream;
	stream = ResourceManager->openStream(filename);
	if (!stream)
	{
		Con::warnf("Missing sequence %s for %s", filename, m_TSShapeFile);
		return -1;
	}

	if (!m_TSShape->importSequences(stream) || stream->getStatus() != Stream::Ok)
	{			
		ResourceManager->closeStream(stream);
		Con::warnf("Load sequence %s failed for %s", filename, m_TSShapeFile);
		return -1;
	}
	ResourceManager->closeStream(stream);

	m_TSShape->sequences.last().nameIndex = m_TSShape->findName(name);
	if (m_TSShape->sequences.last().nameIndex == -1)
	{
		m_TSShape->sequences.last().nameIndex = m_TSShape->names.size();
		m_TSShape->names.increment();
		m_TSShape->names.last() = StringTable->insert(name,false);
	}

	m_ActionToSequence[action] = m_TSShape->sequences.size()-1;

	return m_ActionToSequence[action];
}

void TSShapeInfo::Insert(U32 id,StringTableEntry name)
{
	m_SequenceFiles.insert(SequenceFiles::value_type(id,SequenceFile(id,name,"")));
}

bool TSShapeInfo::Write(Stream& stream) const
{
	return true;
}


bool TSShapeInfo::Read(Stream& stream)
{
	return true;
}

void TSShapeInfo::Clear(void)
{
    //清理模型数据
    m_Initialized = false;
    m_TSShape     = 0;

    //清理动画
    for (int i = 0; i < GameObjectData::NumActionAnims; ++i)
    {
        m_ActionToSequence[i] = -1;
    }
}

// ========================================================================================================================================
//	TSShapeRepository
// ========================================================================================================================================

TSShapeRepository::TSShapeRepository():
    m_resSeqId(1),
    m_isNeedClr(false)
{
}

TSShapeRepository::~TSShapeRepository()
{
    Clear();
}

TSShapeInfo* TSShapeRepository::GetTSShapeInfo(StringTableEntry TSShapeId)
{
	TSShapeId = StringTable->insert(TSShapeId);
	TSShapeInfoMap::const_iterator itr = m_TSShapeInfoMap.find(TSShapeId);
	if(itr == m_TSShapeInfoMap.end())
		return NULL;

	return itr->second;
}

S32 TSShapeRepository::GetSequence(StringTableEntry TSShapeId, U32 action)
{
	if(action >= GameObjectData::NumActionAnims)
		return -1;
#ifdef NTJ_SERVER
	return action;
#endif

	TSShapeId = StringTable->insert(TSShapeId);

	TSShapeInfoMap::const_iterator itr = m_TSShapeInfoMap.find(TSShapeId);

	if(itr == m_TSShapeInfoMap.end() || !itr->second)
		return -1;

#if 1
	if(-2 == itr->second->m_ActionToSequence[action])
		return -1;
#endif

    if (0 > itr->second->m_ActionToSequence[action])
    {
        if(0 > InitializeAction(TSShapeId,action))
		{
			itr->second->m_ActionToSequence[action] = -2;
			return -1;
		}
    }

	return itr->second->m_ActionToSequence[action];
}

bool TSShapeRepository::Insert(TSShapeInfo* pInfo)
{
	if(!pInfo)
		return false;

	return m_TSShapeInfoMap.insert(TSShapeInfoMap::value_type(pInfo->m_TSShapeId, pInfo)).second;
}

void TSShapeRepository::Clear()
{
	TSShapeInfoMap::const_iterator itr = m_TSShapeInfoMap.begin();
	for(;itr != m_TSShapeInfoMap.end(); itr++)
		if(itr->second)
			delete itr->second;
	m_TSShapeInfoMap.clear();
}

TSShapeInfo* TSShapeRepository::InitializeTSShape(StringTableEntry TSShapeId, bool bLoadSeq, bool computeCRC,bool isBackgroundLoad,bool isDynamicLoad)
{
	TSShapeId = StringTable->insert(TSShapeId);
	TSShapeInfoMap::iterator itr = m_TSShapeInfoMap.find(TSShapeId);
	if(itr == m_TSShapeInfoMap.end())
		return NULL;

	if(!itr->second)
		return NULL;
    
    itr->second->m_isDynamicLoad = isDynamicLoad;
    itr->second->m_resSeqId      = m_resSeqId;
	return itr->second->InitializeTSShape(bLoadSeq, computeCRC,isBackgroundLoad);
}

S32 TSShapeRepository::InitializeAction(StringTableEntry TSShapeId, U32 action)
{
	TSShapeId = StringTable->insert(TSShapeId);
	TSShapeInfoMap::iterator itr = m_TSShapeInfoMap.find(TSShapeId);
	if(itr == m_TSShapeInfoMap.end())
		return -1;

	TSShapeInfo* pInfo = itr->second;
	if(!pInfo)
		return -1;

	if (!pInfo->IsInitialized())
		pInfo->InitializeTSShape(false,false,false);

	return pInfo->InitializeAction(action);
}


bool TSShapeRepository::Read()
{
	CDataFile file;
	RData tempData;
    char filename[1024] = {0};

	// 读取模型数据
	Platform::makeFullPathName(GAME_MODEDATA_FILE, filename, sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : ModelRepository.dat!");

	for (S32 i=0; i<file.RecordNum; i++)
	{
		TSShapeInfo* pInfo = new TSShapeInfo;

		// m_TSShapeId
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModelRepository.dat::Read - failed to read model Id!");
		pInfo->m_TSShapeId = StringTable->insert(tempData.m_string);

		// m_TSShapeFile
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModelRepository.dat::Read - failed to read filename!");
		pInfo->m_TSShapeFile = StringTable->insert(tempData.m_string);

		// m_TSShapePath
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ModelRepository.dat::Read - failed to read filepath!");
		pInfo->m_TSShapePath = StringTable->insert(tempData.m_string);

		// box
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "ModelRepository.dat::Read - failed to read box.min.x!");
		pInfo->m_Box.min.x = tempData.m_F32;
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "ModelRepository.dat::Read - failed to read box.min.y!");
		pInfo->m_Box.min.y = tempData.m_F32;
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "ModelRepository.dat::Read - failed to read box.min.z!");
		pInfo->m_Box.min.z = tempData.m_F32;
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "ModelRepository.dat::Read - failed to read box.max.x!");
		pInfo->m_Box.max.x = tempData.m_F32;
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "ModelRepository.dat::Read - failed to read box.max.y!");
		pInfo->m_Box.max.y = tempData.m_F32;
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_F32, "ModelRepository.dat::Read - failed to read box.max.z!");
		pInfo->m_Box.max.z = tempData.m_F32;

		if(!pInfo->m_Box.isValidBox())
			pInfo->m_Box = TSShapeInfo::csBox;

		if (!g_TSShapeRepository.Insert(pInfo))
        {
            delete pInfo;
        }
	}
	file.ReadDataClose();

	// 读取动作数据
	Platform::makeFullPathName(GAME_ACTIONDATA_FILE,filename,sizeof(filename));
	file.ReadDataInit();
	AssertRelease(file.readDataFile(filename), "can NOT read file : ActionRepository.dat!");

	for (S32 i=0; i<file.RecordNum; i++)
	{
		StringTableEntry id = NULL;
		U32 action = 0;

		// m_TSShapeId
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ActionRepository.dat::Read - failed to read model id!");
		id = StringTable->insert(tempData.m_string);

		// m_action
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_enum16, "ActionRepository.dat::Read - failed to read action id!");
		action = ClampAction(tempData.m_Enum16);

		// m_Filename
		file.GetData(tempData);
		AssertRelease(tempData.m_Type == DType_string, "ActionRepository.dat::Read - failed to read filename!");

		TSShapeInfo* pInfo = g_TSShapeRepository.GetTSShapeInfo(id);
		if(pInfo)
			pInfo->Insert(action,tempData.m_string);
		else
			Con::errorf("can NOT find tsShape: %s",id);
	}
	file.ReadDataClose();

	return true;
}

//重新设置模型资源的当前序列号，用来统计哪些资源需要被删除
void TSShapeRepository::ClearRes(void)
{
    m_resSeqId++;
    
    //暂时在10秒后清理数据
    m_clrTime   = time(0) + 10;
    m_isNeedClr = true;
}

void TSShapeRepository::Update(void)
{
    if (!m_isNeedClr)
        return;

    int curTime = time(0);

    if (curTime >= m_clrTime)
    {
        m_isNeedClr = false;
        
        //遍历资源做卸载操作
        for(TSShapeInfoMap::iterator itr = m_TSShapeInfoMap.begin(); itr != m_TSShapeInfoMap.end();++itr)
        {
            TSShapeInfo* pShape = itr->second;

            if(!pShape)
                continue;

            __DoClearShape(pShape);
        }

        //通知资源管理器删除
        ResourceManager->purge();
    }
}

void TSShapeRepository::__DoClearShape(TSShapeInfo* pShape)
{
    if (0 == pShape)
        return;

    if (pShape->m_isDynamicLoad && pShape->m_resSeqId != m_resSeqId)
    {
        pShape->Clear();
    }
}
