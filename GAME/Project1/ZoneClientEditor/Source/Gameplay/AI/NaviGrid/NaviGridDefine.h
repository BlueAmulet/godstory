#pragma once

#include "math/mPoint.h"
#include "math/mPlane.h"
#include <vector>
#include <hash_map>
#include "Gameplay/ai/NaviGrid/table.h"

//#define NaviGridSize			2.0f
//#define NaviGridHeightToler		14.5f
//
//// 单张地图的数据
//#define singleTerrainOffest		256.0f		// 地图的中心点 2.0f * 128
//#define singleTableWidth		256			// 2.0f * 256 / NaviGridSize
//#define singleTableSize			65536		// singleTableWidth*singleTableWidth

#define NaviGridSize			0.5f
#define NaviGridHeightToler		NaviGridSize

// 单张地图的数据
#define singleTerrainOffest	 	 256.0f										// 地图的中心点 2.0f * 128
#define singleTableWidth	    (2.0f * 256 / NaviGridSize)
#define singleTableSize			(singleTableWidth * singleTableWidth)		// singleTableWidth*singleTableWidth


#define NaviMeshID				('N'|'A'<<8|'V'<<16|'G'<<24)
#define NaviOldMeshVer			0x1
#define NaviMeshVer				0x3

class NaviGrid
{
public:
	static S32 TableSize;
	static S32 TableWidthX;
	static S32 TableWidthY;
	static F32 TerrainOffestX;
	static F32 TerrainOffestY;

	static void InitNaviGridInfo(U32 x, U32 y, F32 offsetX, F32 offsetY)
	{
		TableSize = singleTableSize*x*y;
		TableWidthX = singleTableWidth*x;
		TableWidthY = singleTableWidth*y;
		TerrainOffestX = singleTerrainOffest*x - offsetX;
		TerrainOffestY = singleTerrainOffest*y - offsetY;
	}

	static inline void PosToGrid(const VectorF& pos, int& x, int& y)
	{
		float fx = pos.x + TerrainOffestX;
		float fy = pos.y + TerrainOffestY;
		x = mFloor(fx / NaviGridSize);
		y = mFloor(fy / NaviGridSize);
	}

	static inline void PosToTerrain(const VectorF& pos, U32& width, U32& height)
	{
		float fx = pos.x + TerrainOffestX;
		float fy = pos.y + TerrainOffestY;
		width = int(fx / NaviGridSize) / singleTableWidth;
		height = int(fy / NaviGridSize) / singleTableWidth;
	}

	static inline void IdToGrid(int id, int& x, int& y)
	{
		x = id % TableWidthX;
		y = id / TableWidthX;
	}

	static inline int GridToID(unsigned int x, unsigned int y)
	{
		if (x > TableWidthX || y > TableWidthY)
			return -1;

		return TableWidthX * y + x;
	}	

	static inline int GenerateID(const VectorF& pos)
	{
		int x, y;
		PosToGrid(pos, x, y);
		return GridToID(x, y);
	}
	
	static inline VectorF GeneratePosition(int x, int y)
	{
		VectorF pos;
		pos.x = x * NaviGridSize - TerrainOffestX;
		pos.y = y * NaviGridSize - TerrainOffestY;
		pos.z = 0;
		return pos;
	}
};

/*------------------------------------------------------------*/

class GridPointBase :  protected std::vector<VectorF>
{
public:
	GridPointBase(void)
	{
		//m_IndexMap.rehash(singleTableSize);
	}

	int			GetSize			()	{ return (int)size(); }
	void		Resize			(int size)	{ resize(size); }

	int			AddPoint		(const VectorF& pos);
	VectorF	GetPoint		(int index);
	void		UpdatePoint		(int index, VectorF& pos);
	void		Clear			();
#ifdef NTJ_EDITOR
	void		BuildIndexMap	();
	stdext::hash_multimap<int, int>  m_IndexMap;
#endif
protected:

};

inline int GridPointBase::AddPoint(const Point3F& pos)
{
#ifdef NTJ_EDITOR
	int id = NaviGrid::GenerateID(pos);
	if (m_IndexMap.find(id) != m_IndexMap.end())
	{
		stdext::hash_multimap<int, int>::iterator it, end;
		it = m_IndexMap.lower_bound(id);
		end = m_IndexMap.upper_bound(id);
		for(;it != end; ++it)
		{
			int i = it->second;
			if( ( mFabs( pos.x - (*this)[i].x ) < POINT_EPSILON ) &&
				( mFabs( pos.y - (*this)[i].y ) < POINT_EPSILON ) &&
				( mFabs( pos.z - (*this)[i].z ) < NaviGridHeightToler ) )
				return i;
		}
	}
	m_IndexMap.insert(std::make_pair(id, (int)size()));

#else
	for (int i = 0; i < size(); i++)
	{
		if( ( mFabs( pos.x - (*this)[i].x ) < POINT_EPSILON ) &&
			( mFabs( pos.y - (*this)[i].y ) < POINT_EPSILON ) &&
			( mFabs( pos.z - (*this)[i].z ) < NaviGridHeightToler ) )
			return i;
	}
#endif

	push_back(pos);
	return (int)size() -1;
}

inline VectorF GridPointBase::GetPoint(int index)
{
	AssertFatal(index >= 0 && index < size(), "Invalid index in GridPointBase"); 
	return this->at(index);
}

inline void GridPointBase::UpdatePoint(int index, VectorF& pos)
{
	AssertFatal(index >= 0 && index < size(), "Invalid index in GridPointBase"); 
	(*this)[index] = pos;
}

inline void GridPointBase::Clear()
{
	clear();
#ifdef NTJ_EDITOR
	m_IndexMap.clear();
#endif
}

#ifdef NTJ_EDITOR
inline void GridPointBase::BuildIndexMap()
{
	for (int i = 0; i < size(); i++)
	{
		int id = NaviGrid::GenerateID((*this)[i]);
		m_IndexMap.insert(std::make_pair(id, i));
	}
}
#endif
