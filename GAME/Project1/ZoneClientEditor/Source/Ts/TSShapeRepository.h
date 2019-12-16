//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#pragma once

#include "math/mMath.h"
#include "core/stream.h"
#include "ts/tsShape.h"
#include <string>
#include <vector>
#include <hash_map>
#include "Gameplay/GameObjects/GameObject.h"
#include "Util/BackgroundLoadMgr.h"

//using namespace std;
// ========================================================================================================================================
//	TSShapeInfo
// ========================================================================================================================================
//	TSShapeInfo.

//动态载入装备的模型信息
struct ImageShape
{
    U32 objId;
    U32 imageSlot;
    StringTableEntry shapeName;
    StringTableEntry mountPoint;
    StringTableEntry mountPointSelf;
};

enum EShapeType
{
    SHAPE_TYPE_IMAGE = 100,
    SHAPE_TYPE_DATABLOCK,
            
};

class TSShapeInfo:
    public CBackgroundBase
{
	// ------------------------------------------------------------------------------------------------------------------------------------
	//	Constants
	//
public:
	enum
	{
		MaxCollisionShapes = ShapeBaseData::MaxCollisionShapes,
		NumMountPoints = ShapeBaseData::NumMountPoints,
	};

	enum TSShapeTypes
	{
		BODY			= 0,							// 主体，包括角色身体、其他单独的物件等
		HEAD,
		HAIR,
		CAPE,
		WEAPON,
	};

	typedef stdext::hash_map<StringTableEntry,S32> NodeIndexMap;
	const static Box3F csBox;

	// ------------------------------------------------------------------------------------------------------------------------------------
	//	Nested structures.
	//

public:

	class SequenceFile
	{
	public:
		U32					m_action;					// 动作id，与GameObject::Animations相应
		StringTableEntry	m_Filename;					// 动作文件名
		StringTableEntry	m_szSoundFileName;			// 声音文件

		SequenceFile(U32 id,StringTableEntry s1,StringTableEntry s2)
		{
			m_action = id;
			m_Filename = StringTable->insert(s1);
			m_szSoundFileName = StringTable->insert(s2);
		}
	};
	typedef stdext::hash_map<U32,SequenceFile> SequenceFiles;
    
	// ------------------------------------------------------------------------------------------------------------------------------------
	//	Data Queries.
	//
public:
	TSShapeInfo											();
	~TSShapeInfo										();

	bool					IsInitialized				() const { return 0 != m_TSShape && m_Initialized;}

	// ------------------------------------------------------------------------------------------------------------------------------------
	//	Actor Manipulations
	//
public:
	TSShapeInfo*			InitializeTSShape			(bool bLoadSeq, bool computeCRC,bool isBackgroundLoad);
	S32						InitializeAction			(U32 action);
	void					Insert						(U32 id,StringTableEntry name);

    //method from CBackgroundBase
    virtual bool            BackgroundLoad(bool isIncludeModelFile);
    virtual void            OnLoaded(U32 type,U32 flag,Stream* pStream);
    virtual void            OnLoaded(U32 type,U32 flag,ResourceObject* pRes);

    Resource<TSShape>&      __LoadShape(void);
	void					onShapeLoaded();
	S32						getNodeIndex(StringTableEntry nodeName);
	// ------------------------------------------------------------------------------------------------------------------------------------
	//	Data Persistence.
	//
public:
	bool					Write						(Stream& stream) const;
	bool					Read						(Stream& stream);

	// ------------------------------------------------------------------------------------------------------------------------------------
	//	Data Members.
	//
public:

	bool					m_Initialized;
    bool                    m_isDynamicLoad;
    U32                     m_shapeType;
    U32                     m_resSeqId;
    //记录异步载入前的数据
    ImageShape              m_imageInfo;

	StringTableEntry		m_TSShapeId;				// 模型编号
	StringTableEntry		m_TSShapeFile;				// 模型文件名，不包括路径
	StringTableEntry		m_TSShapePath;				// 模型路径
	Box3F					m_Box;						// 模型BOX
	SequenceFiles			m_SequenceFiles;
	S32						m_ActionToSequence[GameObjectData::NumActionAnims];

	//

	NodeIndexMap mountPointNode;
	/// @name Collision Data
	/// @{
	Vector<S32>   collisionDetails;  ///< Detail level used to collide with.
	///
	/// These are detail IDs, see TSShape::findDetail()
	Vector<Box3F> collisionBounds;   ///< Detail level bounding boxes.

	Vector<S32>   LOSDetails;        ///< Detail level used to perform line-of-sight queries against.
	///
	/// These are detail IDs, see TSShape::findDetail()
	/// @}
    
    Resource<TSShape> GetShape(void) {return m_TSShape;}

    //清理数据
    void    Clear(void);
private:
    Resource<TSShape> m_TSShape;					// 模型资源
};


// ========================================================================================================================================
//	TSShapeRepository
// ========================================================================================================================================
//	TSShapeRepository保存了所有TSShape的信息
//

class TSShapeRepository
{
public:
	typedef stdext::hash_map<StringTableEntry, TSShapeInfo*> TSShapeInfoMap;

	TSShapeRepository									();
	~TSShapeRepository									();

	TSShapeInfo*			InitializeTSShape			(StringTableEntry TSShapeId, bool bLoadSeq,bool computeCRC,bool isBackgroundLoad,bool isDynamicLoad = true);
	S32						InitializeAction			(StringTableEntry TSShapeId, U32 action);

	TSShapeInfo*			GetTSShapeInfo				(StringTableEntry TSShapeId);
	S32						GetSequence					(StringTableEntry TSShapeId, U32 action);
	bool					Insert						(TSShapeInfo* pInfo);
	void					Clear						();

	bool					Read						();
    
    //资源的卸载管理
    void                    ClearRes(void);
    void                    Update(void);
private:
    void __DoClearShape(TSShapeInfo* pInfo);

	TSShapeInfoMap			m_TSShapeInfoMap;
   
    //缓冲地图切换时的资源卸载
    //资源的序列ID
    U32     m_resSeqId;
   
    bool    m_isNeedClr;
    //清理的时间
    unsigned int m_clrTime;
};

extern TSShapeRepository g_TSShapeRepository;
