#ifndef _NAVIGATIONOPTIMIZE_H_
#define _NAVIGATIONOPTIMIZE_H_

#include "gfx/gfxDevice.h"

#include <vector>

#define NAV_LAYER_COUNT 8
#define NAV_MERGE_HEIGHT_TOLER 0.1f

/************************************************************************/
/* 对网格进行合并优化
/************************************************************************/
template<typename _NavigationMesh,typename _NavigationCell,typename _NavigationGenCell>
class CNavigationOptimize
{
#ifdef NTJ_EDITOR
	//不同精细的网格层
	class CNavLayer
	{
	public:
		CNavLayer(void) {}

		void	AddCell(_NavigationCell* pCell)
		{
			if (0 == pCell)
				return;

			m_cells.push_back( pCell);
		}

		void    Clear(void)
		{
			m_cells.clear();
		}


		void	Render(void)
		{
			if(!m_cells.size())
				return;

			int count = 0;				// 需渲染的网格计数
			ColorI iColor(0,0,255,127);

			Vector<GFXVertexPC> vertexBuffer;

			//边缘线
			GFX->setBaseRenderState();
			GFX->setZEnable(true);
			GFX->setAlphaBlendEnable(false);

			count = 0;
			vertexBuffer.setSize(m_cells.size() * 8);

			for (size_t i = 0; i < m_cells.size(); ++i)
			{
				GFXVertexPC *verts = &(vertexBuffer[count * 8]);
				const _NavigationCell* cell = m_cells[i];

				for (int i=0;i<4;++i)
				{
					verts[i*2].color.set(0,255,255,255);
					verts[i*2+1].color.set(0,255,255,255);

					verts[i*2].point   = cell->GetVertex(i)  + VectorF(0,0,0.2f);;
					verts[i*2+1].point = cell->GetVertex((i+1)%4)  + VectorF(0,0,0.2f);;
				}
				++count;
			}

			GFXVertexBufferHandle<GFXVertexPC> VB_Line(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
			VB_Line.lock(0, vertexBuffer.size());
			// Copy stuff
			dMemcpy((void*)&VB_Line[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
			VB_Line.unlock();

			GFX->setVertexBuffer(VB_Line);
			GFX->drawPrimitive( GFXLineList, 0, count * 4);
		}

		typedef std::vector<_NavigationCell*> CELL_ARRAY;
		CELL_ARRAY m_cells;
	};
#endif /*NTJ_EDITOR*/

public:
	CNavigationOptimize(void):
	  m_navMesh(0)
    {
	}
	
	bool	Build(_NavigationMesh* pNavIn,_NavigationMesh* pOut)
	{
		if (0 == pNavIn || 0 == pOut)
			return false;

		m_navMesh = pOut;

		__Clear();
		m_navMesh->InitNavGridFlag();
		m_navMesh->ClrGenCells();

		for (int i = 0; i <= NAV_LAYER_COUNT; ++i)
		{
			__GenLayerGrid(i,pNavIn);
		}

		//建立各个层节点间的联系
		__LinkGrids(pNavIn);
		__OptimizeGridsHeight();

		return true;
	}

#ifdef NTJ_EDITOR
	void	Render(void)
	{
		for (int i = 0; i <= NAV_LAYER_COUNT; ++i)
		{
			m_layers[i].Render();
		}

		if (0 == m_navMesh)
			return;

		//绘制连接关系
		//Vector<GFXVertexPC> vertexBuffer;
		//GFXVertexPC vert;
		//int count = 0;

		//_NavigationMesh::CELL_MAP& cells = m_navMesh->GetGenCells();

		//for (_NavigationMesh::CELL_MAP::iterator iter = cells.begin(); iter != cells.end(); ++iter)
		//{
		//	_NavigationCell* pCell = iter->second;

		//	std::vector<unsigned int> neighbours = pCell->GetNeighbours();

		//	for (size_t iNeigubour = 0; iNeigubour < neighbours.size(); ++iNeigubour)
		//	{
		//		_NavigationMesh::CELL_MAP::iterator iterFound = cells.find(neighbours[iNeigubour]);

		//		if (iterFound !=  cells.end())
		//		{
		//			vert.color.set(255,0,0,255);
		//			vert.point = pCell->GetCenterPoint();

		//			vertexBuffer.push_back(vert);

		//			vert.color.set(255,0,0,255);
		//			vert.point = iterFound->second->GetCenterPoint();

		//			vertexBuffer.push_back(vert);

		//			count++;
		//		}
		//	}
		//}

		//if (count > 0)
		//{
		//	GFXVertexBufferHandle<GFXVertexPC> VB_Line(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
		//	VB_Line.lock(0, vertexBuffer.size());
		//	// Copy stuff
		//	dMemcpy((void*)&VB_Line[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
		//	VB_Line.unlock();

		//	GFX->setVertexBuffer(VB_Line);
		//	GFX->drawPrimitive( GFXLineList, 0, count);
		//}	
	}
#endif /*NTJ_EDITOR*/

private:
	void	__Clear(void)
	{
#ifdef NTJ_EDITOR
		for (int i = 0; i <= NAV_LAYER_COUNT; ++i)
		{
			m_layers[i].Clear();
		}
#endif
	}

	void	__GenLayerGrid(int layer,_NavigationMesh* pNavIn)
	{
		float navGridSize  = NaviGridSize * powl(2,NAV_LAYER_COUNT - layer);
		float navGridWidth = (2.0f * 256 / navGridSize);
		float navGridCount = navGridWidth * navGridWidth;

		for (int row = 0; row < navGridWidth; ++row)
		{
			for (int col = 0; col < navGridWidth; ++col)
			{
				//检测每个格子是否完整
				int perWidth     = powl(2,NAV_LAYER_COUNT - layer);
				int r			 = row * perWidth;
				int c			 = col * perWidth;
				bool isMergeable = true;

				_NavigationCell* pFoundCell = 0;
				bool  isHeightSet  = false;
				float minNavHeight = 0;
				float maxNavHeight = 0;
				float curNavHeight = 0;

				for (int subRow = r; isMergeable && subRow < r + perWidth; ++subRow)
				{
					for (int subCol = c; isMergeable && subCol < c + perWidth; ++subCol)
					{
						if (0 != m_navMesh->GetGridFlag(subRow,subCol))
						{
							isMergeable = false;
							break;
						}

						pFoundCell = pNavIn->FindCell(NaviGrid::GridToID(subCol,subRow));

						if (0 == pFoundCell)
						{
							isMergeable = false;
							break;
						}

						curNavHeight = pFoundCell->GetCenterHeight();

						if (isHeightSet)
						{
							if (curNavHeight < minNavHeight)
								minNavHeight = curNavHeight;

							if (curNavHeight > maxNavHeight)
								maxNavHeight = curNavHeight;
						}
						else
						{
							minNavHeight = curNavHeight;
							maxNavHeight = curNavHeight;

                            isHeightSet = true;
						}
					}
				}

				//把格子加入到当前层,并设置标志位
				if (isMergeable)
				{
					bool isFlat = true;

					if (fabs(minNavHeight - maxNavHeight) > NAV_MERGE_HEIGHT_TOLER || 1 == perWidth)
					{
						isFlat = false;
					}

					m_navMesh->AddGenCell(__GenCellId(layer,row,col),isFlat ? maxNavHeight : -0xffffff);

					for (int subRow = r; subRow < r + perWidth; ++subRow)
					{
						for (int subCol = c; subCol < c + perWidth; ++subCol)
						{
							m_navMesh->SetGridFlag(subRow,subCol,__GenCellId(layer,row,col));
						}
					}
				}
			}
		}
	}

	void	__LinkGrids(_NavigationMesh* pNavIn)
	{
		/*for (int i = 0; i <= NAV_LAYER_COUNT; ++i)
		{
			for (int iCell = 0; iCell < m_layers[i].m_cells.size(); ++iCell)
			{
				__LinkCell(pNavIn,m_layers[i].m_cells[iCell],i);
			}
		}*/
		
		_NavigationMesh::CELL_MAP& cells = m_navMesh->GetGenCells();

		for (_NavigationMesh::CELL_MAP::iterator iter = cells.begin(); iter != cells.end(); ++iter)
		{
			__LinkCell(pNavIn,iter->first,iter->second);
		}
	}

	void	__LinkCell(_NavigationMesh* pNavIn,unsigned int id,_NavigationGenCell& pCell)
	{
		__LinkCell(pNavIn,id,&pCell);
	}

	void	__LinkCell(_NavigationMesh* pNavIn,unsigned int id,_NavigationGenCell* pCell)
	{
		if (0 == pCell)
			return;

		//查找cell所占据的基础格子
		int   row,col,layer; 
		__GetIdInfo(id,layer,row,col);

		int perWidth = powl(2,NAV_LAYER_COUNT - layer);

		int r	= row * perWidth;
		int c	= col * perWidth;

		stdext::hash_map<unsigned int,bool> neighbourIds;

		for (int subRow = r; subRow < r + perWidth; ++subRow)
		{
			for (int subCol = c; subCol < c + perWidth; ++subCol)
			{
				_NavigationCell* pFoundCell = pNavIn->FindCell(NaviGrid::GridToID(subCol,subRow));

				if (0 == pFoundCell)
					continue;

				for (int iLink = 0; iLink < 4; ++iLink)
				{
					_NavigationCell* pLink = pFoundCell->GetLink(iLink);

					if (0 == pLink)
						continue;

					int linkRow = subRow + yoff[iLink];
					int linkCol = subCol + xoff[iLink];

					if (linkCol < 0 || linkRow < 0 || linkCol >= singleTableWidth || linkRow >= singleTableWidth)
					{
						continue;
					}

					unsigned int neighbourId = m_navMesh->GetGridFlag(linkRow,linkCol);

					if (neighbourId == id)
						continue;
                        
                    ////检测和邻接格子的高度差是否超过阀值
                    //_NavigationCell* pTestNeighbour = pNavIn->FindCell(NaviGrid::GridToID(linkCol,linkRow));
                    //
                    //if (pTestNeighbour && fabs(pFoundCell->GetCenterHeight() - pTestNeighbour->GetCenterHeight()) >= 0.5f * NaviGridSize)
                    //    continue;

					neighbourIds.insert(std::make_pair(neighbourId,true));
				}
			}
		}

		//填充CELL的邻接格子
		for (stdext::hash_map<unsigned int,bool>::iterator iter = neighbourIds.begin(); iter != neighbourIds.end(); ++iter)
		{
			pCell->AddNeighbours(iter->first);

			/*_NavigationGenCell* pNeighbour = m_navMesh->GetGenCell(iter->first);

			if (0 != pNeighbour)
			{
				pNeighbour->AddNeighbours(id);
			}*/
		}
	}
	
	void __OptimizeGridsHeight(void)
	{
		_NavigationMesh::CELL_MAP& cells = m_navMesh->GetGenCells();

		for (_NavigationMesh::CELL_MAP::iterator iter = cells.begin(); iter != cells.end(); ++iter)
		{
			__OptimizeCellHeight(iter->first,iter->second);
		}
	}

	void __OptimizeCellHeight(unsigned int id,_NavigationGenCell& pCell)
	{
		__OptimizeCellHeight(id,&pCell);
	}

	void __OptimizeCellHeight(unsigned int id,_NavigationGenCell* pCell)
	{
		if (0 == pCell || !pCell->IsFlat())
			return;

		//查找cell所占据的基础格子
		int   row,col,layer; 
		__GetIdInfo(id,layer,row,col);

		int perWidth = powl(2,NAV_LAYER_COUNT - layer);

		int r	= row * perWidth;
		int c	= col * perWidth;

		for (int subRow = r; subRow < r + perWidth; ++subRow)
		{
			for (int subCol = c; subCol < c + perWidth; ++subCol)
			{
				m_navMesh->DelCell(NaviGrid::GridToID(subCol,subRow));
			}
		}
	}

	unsigned int __GenCellId(int layer,int row,int col)
	{
		return unsigned int((layer << 24) | (row << 12) | col); 
	}

	void		 __GetIdInfo(unsigned int id,int& layer,int& row,int& col)
	{
		layer = (id >> 24);
		row   = ((id & 0x00ffffff) >> 12);
		col   = id & 0x00000fff;
	}

#ifdef NTJ_EDITOR
	CNavLayer m_layers[NAV_LAYER_COUNT + 1];
#endif

	//保存合成的nav网格
	_NavigationMesh* m_navMesh;
};

#endif /*_NAVIGATIONOPTIMIZE_H_*/