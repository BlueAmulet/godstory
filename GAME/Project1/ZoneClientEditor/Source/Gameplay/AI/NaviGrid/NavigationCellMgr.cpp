#include "NavigationCellMgr.h"
#include "core/frameAllocator.h"
#include "core/fileStream.h"
#include "core/bitStream.h"
#include "core/memstream.h"
#include "gfx/primBuilder.h"
#include "interior/interiorInstance.h"
#include "gui/missionEditor/editTSCtrl.h"
#include "Gameplay/ai/NaviGrid/NavigationBlock.h"
#include "NavigationMeshEx.h"

#include <map>
#include <string>
#include <windows.h>
#include "gfx/D3D9/gfxD3D9StateBlock.h"
//===================================================================================
//---------------------------------------------------------------------------
//静态成员和宏定义
//状态块
//---------------------------------------------------------------------------
//////#define STATEBLOCK
GFXStateBlock* NavigationCellMgr::mSetSB = NULL;
GFXStateBlock* NavigationCellMgr::mZTrueSB = NULL;
GFXStateBlock* NavigationCellMgr::mZFalseSB = NULL;
GFXStateBlock* NavigationCellMgr::mAlphaBlendFalseSB = NULL;


GridPointBase	NavigationCellMgr::s_GridPointBase;
NavigationCellMgr	g_NavigationMesh;

S32 NaviGrid::TableSize = singleTableSize;
S32 NaviGrid::TableWidthX = singleTableWidth;
S32 NaviGrid::TableWidthY = singleTableWidth;
F32 NaviGrid::TerrainOffestX = singleTerrainOffest;
F32 NaviGrid::TerrainOffestY = singleTerrainOffest;

// test
//S64 testStart = 0;
//S64 testEnd = 0;
//S64 testDelta = 0;

//--------static functions----------------------------------------------------------------

int NavigationCellMgr::sLinePoint[6][2]= 
{
	{NavigationCell::VERT_A, NavigationCell::VERT_B}, {NavigationCell::VERT_B, NavigationCell::VERT_C},
	{NavigationCell::VERT_D, NavigationCell::VERT_C}, {NavigationCell::VERT_A, NavigationCell::VERT_D},
	{NavigationCell::VERT_A, NavigationCell::VERT_C}, {NavigationCell::VERT_B, NavigationCell::VERT_D},
};

//----------------------------------------------------------------------------------------

NavigationCellMgr::NavigationCellMgr(void):
	m_CellArray(0),
	m_isShowOptimized(false),
	m_isEnableZBuffer(false)
{
	m_CellArray.clear();
}

void NavigationCellMgr::InitCellArray(int initSize)
{
	m_CellArray.init(initSize);
}

NavigationCell* NavigationCellMgr::AddCell(const VectorF& a, const VectorF& b, const VectorF& c, const VectorF& d,bool isNavFixed,std::string line)
{
	//TODO 检查单元是否超出范围

	//1.检查是否存在
	Point3F cen = (a + b + c + d) / 4.0f;
	int id = NaviGrid::GenerateID(cen);
	if (id < 0) return NULL;

	//检查是否相同单元,同一个格子不能有重复点
	CELL_ARRAY::iterator it, end;
	it = m_CellArray.find(id);
	end = m_CellArray.end();

	std::vector<NavigationCell*> cellToDelete;

	for(;it != end; it = it->next)
	{
		NavigationCell* cell = it->value;

		if (cell->GetVertex(0).equal(a) || cell->GetVertex(1).equal(b) ||
			cell->GetVertex(2).equal(c) || cell->GetVertex(3).equal(d))
        {
			return cell;
        }
		else
		{
			cellToDelete.push_back(cell);
		}
	}

	for (size_t i = 0; i < cellToDelete.size(); ++i)
	{
		DeleteCell(cellToDelete[i]);
	}

	//2.加入新单元
	NavigationCell* newCell = new NavigationCell;
	newCell->Initialize(a, b, c, d);
	m_CellArray.insert(newCell->m_nId, newCell);
	LinkNeighbor(newCell);

	m_isCellChanged = true;

	return newCell;
}


void NavigationCellMgr::DeleteCell(const VectorF& a, const VectorF& b, const VectorF& c, const VectorF& d)
{
#define EQ(a, b) (mFabs(a.x - b.x) < 1e-4 && mFabs(a.y - b.y) < 1e-4 && mFabs(a.z - b.z) < 12.f)
	
	m_isCellChanged = true;

	//1.检查是否存在
	Point3F cen = (a + b + c + d) / 4.0f;
	int id = NaviGrid::GenerateID(cen);

	//检查是否相同单元,同一个格子不能有重复点
	CELL_ARRAY::iterator it;
	it = m_CellArray.find(id);
	for(;it != m_CellArray.end(); it = it->next)
	{
		NavigationCell* cell = it->value;
		if ( EQ(cell->GetVertex(0), a) || EQ(cell->GetVertex(1), b) ||
			EQ(cell->GetVertex(2), c) || EQ(cell->GetVertex(3), d) )
		{
			//删除关联
			m_CellArray.erase(id, it);
			UnlinkNeighbor(cell);
			delete cell;
			break;
		}
	}
}

void NavigationCellMgr::DeleteCell(const NavigationCell* cellDel)
{
	m_isCellChanged = true;

	CELL_ARRAY::iterator it, end;
	int id = cellDel->m_nId;
	it = m_CellArray.find(id);
	end = m_CellArray.end();
	for(;it != end; it = it->next)
	{
		NavigationCell* cell = it->value;
		if (cell == cellDel)
		{
			//删除关联
			m_CellArray.erase(id, it);
			UnlinkNeighbor(cell);
			delete cell;
			break;
		}
	}
}


static int xoff[4] = {-1, +0, +1, -0};
static int yoff[4] = {-0, +1, +0, -1};

void NavigationCellMgr::LinkNeighbor(NavigationCell* cell)
{
	int x, y;
	NaviGrid::IdToGrid(cell->m_nId, x, y);	

	for (int i = 0; i < 4; i++)
	{
		int id = NaviGrid::GridToID(x + xoff[i], y + yoff[i]);

		CELL_ARRAY::iterator it, end;
		it = m_CellArray.find(id);
		end = m_CellArray.end();
		for(;it != end; it = it->next)
		{
			NavigationCell* nearCell = it->value;
			int nearSide = (i + 2) % 4;
			if (cell->IsSharedSide((NavigationCell::CELL_SIDE)i, nearCell, (NavigationCell::CELL_SIDE)nearSide))
			{
				nearCell->m_Link[nearSide] = cell;
				cell->m_Link[i] = nearCell;
				break;//only link one side
			}
		}
	}
}

void NavigationCellMgr::UnlinkNeighbor(NavigationCell* cell)
{
	for (int i = 0; i < 4; i++)
	{
		NavigationCell* neighbor = cell->m_Link[i];
		if (neighbor)
		{
			for (int j = 0; j < 4; j++)
			{
				if (neighbor->m_Link[j] == cell)
					neighbor->m_Link[j] = NULL;
			}
			neighbor = NULL;
		}
	}
}

void NavigationCellMgr::SplitCell(NavigationCell* cell, NavigationCell::CELL_SIDE side)
{
	if (cell && cell->m_Link[side])
	{
		NavigationCell* nearCell = cell->m_Link[side];
		int nearSide = (side + 2) % 4;
		nearCell->m_Link[nearSide] = NULL;
		cell->m_Link[side] = NULL;
	}
}


//:	Render
//----------------------------------------------------------------------------------------
//
//	Render the mesh geometry to screen. The mesh is assumed to exist in world corrdinates 
//	for the purpose of this demo 
//
//-------------------------------------------------------------------------------------://
#ifdef STATEBLOCK
void NavigationCellMgr::Render()
{
#ifdef NTJ_EDITOR

	//if (m_isShowOptimized)
	//	//m_gen.Render();

	if(!m_CellArray.size())
		return;

	Vector<GFXVertexPC> vertexBuffer;
	CELL_ARRAY::iterator it,end;
	end = m_CellArray.end();
	int count = 0;				// 需渲染的网格计数

	// 缩小遍历范围
	Point2I CamGrid;
    int radius = Con::getIntVariable("navBrushSize");			// 该值决定vertexBuffer长度，过大会显著降低效率
    
    if (radius < 40)
        radius = 40;

    if (radius > 1000)
        radius = 1000;

	NaviGrid::PosToGrid(EditTSCtrl::smCamPos,CamGrid.x,CamGrid.y);
	S32 startX,startY,endX,endY;
	startX = mClamp(CamGrid.x-radius,0,NaviGrid::TableWidthX);
	startY = mClamp(CamGrid.y-radius,0,NaviGrid::TableWidthY);
	endX = mClamp(CamGrid.x+radius,0,NaviGrid::TableWidthX);
	endY = mClamp(CamGrid.y+radius,0,NaviGrid::TableWidthY);

	ColorI iColor(0,0,255,127);
	vertexBuffer.setSize(getMin(m_CellArray.size(),radius * radius * 4) * 6);

	for(int i = startX; i < endX ; i++)
		for(int j = startY; j < endY ; j++)
		{
			it = m_CellArray.at(NaviGrid::GridToID(i,j));
			for(;it != end; it = it->next)
			{
				GFXVertexPC *verts = &(vertexBuffer[count * 6]);
				const NavigationCell* cell = it->value;

				//todo 用视锥把看不见单元剔除掉
				//if ((cell->GetVertex(0) - EditTSCtrl::smCamPos).lenSquared() > 32400/*180 * 180*/)
				//	continue;

				verts[0].point = cell->GetVertex(0) + VectorF(0,0,0.1f);
				verts[1].point = cell->GetVertex(1) + VectorF(0,0,0.1f);
				verts[2].point = cell->GetVertex(2) + VectorF(0,0,0.1f);
				verts[3].point = cell->GetVertex(2) + VectorF(0,0,0.1f);
				verts[4].point = cell->GetVertex(3) + VectorF(0,0,0.1f);
				verts[5].point = cell->GetVertex(0) + VectorF(0,0,0.1f);
				verts[0].color = iColor;
				verts[1].color = iColor;
				verts[2].color = iColor;
				verts[3].color = iColor;
				verts[4].color = iColor;
				verts[5].color = iColor;

				++count;
			}
		}

		GFXVertexBufferHandle<GFXVertexPC> selectionVB(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
		selectionVB.lock(0, vertexBuffer.size());
		// Copy stuff
		dMemcpy((void*)&selectionVB[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
		selectionVB.unlock();

		GFX->setBaseRenderState();

		if (m_isEnableZBuffer)
		{
			AssertFatal(mZTrueSB, "NavigationCellMgr::Render -- mZTrueSB cannot be NULL.");
			mZTrueSB->apply();
		} 
		else
		{
			AssertFatal(mZFalseSB, "NavigationCellMgr::Render -- mZFalseSB cannot be NULL.");
			mZFalseSB->apply();
		}
		AssertFatal(mSetSB, "NavigationCellMgr::Render -- mSetSB cannot be NULL.");
		mSetSB->apply();

		GFX->setVertexBuffer(selectionVB);
		GFX->disableShaders();

		GFX->drawPrimitive( GFXTriangleList, 0, count * 2);


		//边缘线
		GFX->setBaseRenderState();
		AssertFatal(mAlphaBlendFalseSB, "NavigationCellMgr::Render -- mAlphaBlendFalseSB cannot be NULL.");
		mAlphaBlendFalseSB->apply();

		count = 0;
		vertexBuffer.setSize(getMin(m_CellArray.size(),radius * radius * 4) * 8);

		for(int i = startX; i < endX ; i++)
			for(int j = startY; j < endY ; j++)
			{
				it = m_CellArray.at(NaviGrid::GridToID(i,j));
				for(;it != end; it = it->next)
				{
					GFXVertexPC *verts = &(vertexBuffer[count * 8]);
					const NavigationCell* cell = it->value;

					for (int i=0;i<4;++i)
					{
						if (!cell->m_Link[i])
						{
							verts[i*2].color.set(255,0,0,255);
							verts[i*2+1].color.set(255,0,0,255);
						}
						else
						{
							verts[i*2].color.set(150,150,150,127);
							verts[i*2+1].color.set(150,150,150,127);
						}

						//int idx1 = NavigationCellMgr::sLinePoint[i][0];
						//int idx2 = NavigationCellMgr::sLinePoint[i][1];
						//(cell->GetVertex(idx1));
						//(cell->GetVertex(idx2));
						verts[i*2].point = cell->GetVertex(i) + VectorF(0,0,0.1f);
						verts[i*2+1].point = cell->GetVertex((i+1)%4) + VectorF(0,0,0.1f);
					}

					++count;
				}
			}
			GFXVertexBufferHandle<GFXVertexPC> VB_Line(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
			VB_Line.lock(0, vertexBuffer.size());
			// Copy stuff
			dMemcpy((void*)&VB_Line[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
			VB_Line.unlock();

			GFX->setVertexBuffer(VB_Line);
			GFX->drawPrimitive( GFXLineList, 0, count * 4);

			AssertFatal(mZTrueSB, "NavigationCellMgr::Render -- mZTrueSB cannot be NULL.");
			mZTrueSB->apply();

			//for test
			if(m_Path.size() > 0)
			{
				////搜索过的单元
				//vertexBuffer.setSize((int)m_ClosedCell.size()*6);
				//for(int i = 0; i < m_ClosedCell.size(); i++)
				//{
				//	GFXVertexPC *verts = &(vertexBuffer[i * 6]);
				//	ColorI cellColor(0,mClamp(255-i*5,0,255),0,100);
				//	NavigationCell* cell = m_ClosedCell[i];

				//	verts[0].point = cell->GetVertex(0);
				//	verts[1].point = cell->GetVertex(1);
				//	verts[2].point = cell->GetVertex(2);
				//	verts[3].point = cell->GetVertex(2);
				//	verts[4].point = cell->GetVertex(3);
				//	verts[5].point = cell->GetVertex(0);
				//	verts[0].color = cellColor;
				//	verts[1].color = cellColor;
				//	verts[2].color = cellColor;
				//	verts[3].color = cellColor;
				//	verts[4].color = cellColor;
				//	verts[5].color = cellColor;
				//}
				//GFXVertexBufferHandle<GFXVertexPC> VB_ClosedCell(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
				//VB_ClosedCell.lock(0, vertexBuffer.size());
				//dMemcpy((void*)&VB_ClosedCell[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
				//VB_ClosedCell.unlock();
				//GFX->setVertexBuffer(VB_ClosedCell);
				//GFX->drawPrimitive( GFXTriangleList, 0, vertexBuffer.size()/3);

				//寻径寻径路线(路径)
				vertexBuffer.setSize((int)m_Path.size());
				WAYPOINT_LIST::iterator it;
				int i=0;

				for(it = m_Path.begin(); it != m_Path.end(); ++it,++i)
				{
					VectorF pos = *it;

					vertexBuffer[i].point = (pos + VectorF(0,0,0.1f))  + VectorF(0,0,0.1f);;
					vertexBuffer[i].color.set(0,255,0,255);
				}

				GFXVertexBufferHandle<GFXVertexPC> VB_Path(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
				VB_Path.lock(0, vertexBuffer.size());
				dMemcpy((void*)&VB_Path[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
				VB_Path.unlock();

				GFX->setVertexBuffer(VB_Path);
				GFX->drawPrimitive( GFXLineStrip, 0, vertexBuffer.size()-1);
			}
#endif
}

#else
void NavigationCellMgr::Render()
{
#ifdef NTJ_EDITOR

	//if (m_isShowOptimized)
	//	//m_gen.Render();

	if(!m_CellArray.size())
		return;

	Vector<GFXVertexPC> vertexBuffer;
	CELL_ARRAY::iterator it,end; 
	end = m_CellArray.end();
	int count = 0;				// 需渲染的网格计数

	// 缩小遍历范围
	Point2I CamGrid;
    int radius = dAtoi(Con::getVariable("$navBrushSize"));			// 该值决定vertexBuffer长度，过大会显著降低效率
 
    if (radius < 40)
        radius = 40;
 
    if (radius > 1000)
        radius = 1000;

	NaviGrid::PosToGrid(EditTSCtrl::smCamPos,CamGrid.x,CamGrid.y);
	S32 startX,startY,endX,endY;
	startX = mClamp(CamGrid.x-radius,0,NaviGrid::TableWidthX);
	startY = mClamp(CamGrid.y-radius,0,NaviGrid::TableWidthY);
	endX = mClamp(CamGrid.x+radius,0,NaviGrid::TableWidthX);
	endY = mClamp(CamGrid.y+radius,0,NaviGrid::TableWidthY);

	ColorI iColor(0,0,255,127);
	vertexBuffer.setSize(getMin(m_CellArray.size(),radius * radius * 4) * 6);

	for(int i = startX; i < endX ; i++)
	for(int j = startY; j < endY ; j++)
	{
		it = m_CellArray.at(NaviGrid::GridToID(i,j));
		for(;it != end; it = it->next)
		{
			GFXVertexPC *verts = &(vertexBuffer[count * 6]);
			const NavigationCell* cell = it->value;

			//todo 用视锥把看不见单元剔除掉
			//if ((cell->GetVertex(0) - EditTSCtrl::smCamPos).lenSquared() > 32400/*180 * 180*/)
			//	continue;

			verts[0].point = cell->GetVertex(0) + VectorF(0,0,0.1f);
			verts[1].point = cell->GetVertex(1) + VectorF(0,0,0.1f);
			verts[2].point = cell->GetVertex(2) + VectorF(0,0,0.1f);
			verts[3].point = cell->GetVertex(2) + VectorF(0,0,0.1f);
			verts[4].point = cell->GetVertex(3) + VectorF(0,0,0.1f);
			verts[5].point = cell->GetVertex(0) + VectorF(0,0,0.1f);
			verts[0].color = iColor;
			verts[1].color = iColor;
			verts[2].color = iColor;
			verts[3].color = iColor;
			verts[4].color = iColor;
			verts[5].color = iColor;

			++count;
		}
	}

	GFXVertexBufferHandle<GFXVertexPC> selectionVB(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
	selectionVB.lock(0, vertexBuffer.size());
	// Copy stuff
	dMemcpy((void*)&selectionVB[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
	selectionVB.unlock();

	GFX->setBaseRenderState();
	GFX->setZEnable(m_isEnableZBuffer);

	GFX->setAlphaBlendEnable(true);
	GFX->setSrcBlend(GFXBlendSrcAlpha);
	GFX->setDestBlend(GFXBlendDestAlpha);

	GFX->setVertexBuffer(selectionVB);
	GFX->disableShaders();
	GFX->setCullMode(GFXCullCCW);

	GFX->drawPrimitive( GFXTriangleList, 0, count * 2);


	//边缘线
	GFX->setBaseRenderState();
	GFX->setAlphaBlendEnable(false);

	count = 0;
	vertexBuffer.setSize(getMin(m_CellArray.size(),radius * radius * 4) * 8);

	for(int i = startX; i < endX ; i++)
	for(int j = startY; j < endY ; j++)
	{
		it = m_CellArray.at(NaviGrid::GridToID(i,j));
		for(;it != end; it = it->next)
		{
			GFXVertexPC *verts = &(vertexBuffer[count * 8]);
			const NavigationCell* cell = it->value;

			for (int i=0;i<4;++i)
			{
				if (!cell->m_Link[i])
				{
					verts[i*2].color.set(255,0,0,255);
					verts[i*2+1].color.set(255,0,0,255);
				}
				else
				{
					verts[i*2].color.set(150,150,150,127);
					verts[i*2+1].color.set(150,150,150,127);
				}

				//int idx1 = NavigationCellMgr::sLinePoint[i][0];
				//int idx2 = NavigationCellMgr::sLinePoint[i][1];
				//(cell->GetVertex(idx1));
				//(cell->GetVertex(idx2));
				verts[i*2].point = cell->GetVertex(i) + VectorF(0,0,0.1f);
				verts[i*2+1].point = cell->GetVertex((i+1)%4) + VectorF(0,0,0.1f);
			}

			++count;
		}
	}
	GFXVertexBufferHandle<GFXVertexPC> VB_Line(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
	VB_Line.lock(0, vertexBuffer.size());
	// Copy stuff
	dMemcpy((void*)&VB_Line[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
	VB_Line.unlock();

	GFX->setVertexBuffer(VB_Line);
	GFX->drawPrimitive( GFXLineList, 0, count * 4);

	GFX->setZEnable(true);

	//for test
	if(m_Path.size() > 0)
	{
		////搜索过的单元
		//vertexBuffer.setSize((int)m_ClosedCell.size()*6);
		//for(int i = 0; i < m_ClosedCell.size(); i++)
		//{
		//	GFXVertexPC *verts = &(vertexBuffer[i * 6]);
		//	ColorI cellColor(0,mClamp(255-i*5,0,255),0,100);
		//	NavigationCell* cell = m_ClosedCell[i];

		//	verts[0].point = cell->GetVertex(0);
		//	verts[1].point = cell->GetVertex(1);
		//	verts[2].point = cell->GetVertex(2);
		//	verts[3].point = cell->GetVertex(2);
		//	verts[4].point = cell->GetVertex(3);
		//	verts[5].point = cell->GetVertex(0);
		//	verts[0].color = cellColor;
		//	verts[1].color = cellColor;
		//	verts[2].color = cellColor;
		//	verts[3].color = cellColor;
		//	verts[4].color = cellColor;
		//	verts[5].color = cellColor;
		//}
		//GFXVertexBufferHandle<GFXVertexPC> VB_ClosedCell(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
		//VB_ClosedCell.lock(0, vertexBuffer.size());
		//dMemcpy((void*)&VB_ClosedCell[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
		//VB_ClosedCell.unlock();
		//GFX->setVertexBuffer(VB_ClosedCell);
		//GFX->drawPrimitive( GFXTriangleList, 0, vertexBuffer.size()/3);

		//寻径寻径路线(路径)
		vertexBuffer.setSize((int)m_Path.size());
		WAYPOINT_LIST::iterator it;
		int i=0;

		for(it = m_Path.begin(); it != m_Path.end(); ++it,++i)
		{
            VectorF pos = *it;

			vertexBuffer[i].point = (pos + VectorF(0,0,0.1f))  + VectorF(0,0,0.1f);;
			vertexBuffer[i].color.set(0,255,0,255);
		}

		GFXVertexBufferHandle<GFXVertexPC> VB_Path(GFX, vertexBuffer.size(), GFXBufferTypeStatic);
		VB_Path.lock(0, vertexBuffer.size());
		dMemcpy((void*)&VB_Path[0], (void*)&vertexBuffer[0], sizeof(GFXVertexPC) * vertexBuffer.size());
		VB_Path.unlock();

		GFX->setVertexBuffer(VB_Path);
		GFX->drawPrimitive( GFXLineStrip, 0, vertexBuffer.size()-1);
	}
#endif
}

#endif


NavigationCell* NavigationCellMgr::FindCell(const VectorF& point)
{
	return FindCell(NaviGrid::GenerateID(point));
}

NavigationCell* NavigationCellMgr::FindCell(int id)
{
	CELL_ARRAY::iterator it, end;
	it = m_CellArray.find(id);
	end = m_CellArray.end();

	for(;it != end; it = it->next)
	{
		NavigationCell* cell = it->value;
		//VectorF cen = (cell->GetVertex(0) + cell->GetVertex(1) + cell->GetVertex(2) + cell->GetVertex(3)) / 4;
		//if (mFabs(cen.z - point.z) < NaviGridHeightToler)
		return cell;
	}

	return NULL;
}

struct SaveCellInfo
{
	SaveCellInfo(void) : isMark(false) {}

	bool isMark;
	int  heightIdx[4];
};

bool NavigationCellMgr::Save(const char* fileName)
{
	// 打开文件用于写入地形数据
	FileStream file;

    std::string tmpFileName = fileName;
    tmpFileName += ".tmp";

	if (!file.open(tmpFileName.c_str(), FileStream::Write))
		return false;

	// 写入版本号
	int id  = NaviMeshID;
	int ver = NaviMeshVer;
	file.write(id);
	file.write(ver);

	SaveCellInfo* pCellInfo = new SaveCellInfo[singleTableSize];
	
	//标志存在的CELL
	int row,col;
	CELL_ARRAY::iterator it, end;
	end = m_CellArray.end();

	stdext::hash_map<float,int> heightHashMap;
	std::vector<float>           heightMap;
	VectorF corner;

	for(int i = 0; i < m_CellArray.capacity(); i++)
	{
		it = m_CellArray.at(i);

		for(;it != end; it = it->next)
		{
			NavigationCell* pCell = it->value;

			if (0 == pCell)
				continue;

			NaviGrid::IdToGrid(pCell->GetId(),col,row);

			//做下标志
			SaveCellInfo& cellInfo = pCellInfo[row * (int)singleTableWidth + col];
			cellInfo.isMark = true;

			//缓冲高度
			for (int iCorn = 0; iCorn < 4; ++iCorn)
			{
				corner = pCell->GetVertex(iCorn);

				stdext::hash_map<float,int>::iterator iter = heightHashMap.find(corner.z);

				if (iter == heightHashMap.end())
				{
					cellInfo.heightIdx[iCorn] = heightMap.size();
					heightHashMap.insert(std::make_pair(corner.z,(int)heightMap.size()));
					heightMap.push_back(corner.z);
				}
				else
				{
					cellInfo.heightIdx[iCorn] = iter->second;
				}
			}
		}
	}

	//写入高度缓冲
	file.write(heightMap.size());

	for (size_t i = 0; i < heightMap.size(); ++i)
	{
		file.write(heightMap[i]);
	}

	//写入CELL的MARK
	unsigned char* buf =  new unsigned char[singleTableSize];
	memset(buf,0,singleTableSize);

	BitStream stream((void*)buf,singleTableSize);

	for (int row = 0; row < singleTableWidth; ++row)
	{
		for (int col = 0; col < singleTableWidth; ++col)
		{
			stream.writeFlag(pCellInfo[row * (int)singleTableWidth + col].isMark);
		}
	}
	
	U32 length = stream.getPosition();
	file.write(length);
	file.write(length,stream.getBuffer());

	delete []buf;

	//行程编码
	int lastHeight = -0xffff;
	unsigned int sameHeightCount = 0;

	//写入CELL4个点的索引
	for (int row = 0; row < singleTableWidth; ++row)
	{
		for (int col = 0; col < singleTableWidth; ++col)
		{
			SaveCellInfo& cellInfo = pCellInfo[row * (int)singleTableWidth + col];

			if (cellInfo.isMark)
			{
				if (lastHeight == -0xffff)
				{
					lastHeight = cellInfo.heightIdx[0];
				}

				for (int iCorn = 0; iCorn < 4; ++iCorn)
				{
					if (lastHeight != cellInfo.heightIdx[iCorn])
					{
						file.write(sameHeightCount);

						if (heightMap.size() > 65535)
						{
							file.write(lastHeight);
						}
						else
						{
							file.write((unsigned short)lastHeight);
						}

						sameHeightCount = 1;
						lastHeight      = cellInfo.heightIdx[iCorn];
					}
					else
					{
						sameHeightCount++;
					}
				}
			}
		}
	}
	
	if (sameHeightCount > 0)
	{
		file.write(sameHeightCount);

		if (heightMap.size() > 65535)
		{
			file.write(lastHeight);
		}
		else
		{
			file.write((unsigned short)lastHeight);
		}
	}
	
	delete []pCellInfo;

	file.close();
	m_isCellChanged = false;

    ::CopyFileA(tmpFileName.c_str(),fileName,false);
    ::DeleteFileA(tmpFileName.c_str());
	return true;
}

bool NavigationCellMgr::Open(const char* fileName)
{
#ifdef NTJ_EDITOR
	if (0 == fileName)
		return false;
	
	FileStream stream;

	if (!stream.open(fileName, FileStream::Read))
		return false;

	g_NavigationMesh.Clear();

	// 写入版本号
	int id, ver;
	stream.read(&id);
	stream.read(&ver);

	if (id != NaviMeshID)
	{
		stream.close();
		return false;
	}

	if (ver != NaviMeshVer)
	{
		Con::errorf("invalid navigation file [%s] readed,version:%d",fileName,ver);
		return false;
	}

	//高度缓冲
	std::vector<float> heightMap;
	size_t size;
	float  height;

	stream.read(&size);

	for (size_t i = 0; i < size; ++i)
	{
		stream.read(&height);
		heightMap.push_back(height);
	}

	unsigned char* buf        = new unsigned char[singleTableSize];
	bool*          pMarkTable = new bool[singleTableSize];
	memset(buf,0,singleTableSize);

	U32 markSize;
	stream.read(&markSize);
	stream.read(markSize,buf);
	BitStream bitStream((void*)buf,markSize);

	for (int row = 0; row < singleTableWidth; ++row)
	{
		for (int col = 0; col < singleTableWidth; ++col)
		{
			pMarkTable[row * (int)singleTableWidth + col] = bitStream.readFlag();
		}
	}

	delete []buf;

	int          lastHeight;
	int			 heightCount = 0;

	VectorF      point[4];
	Point2I      offset[4] = { Point2I(0,0), Point2I(0,1), Point2I(1,1), Point2I(1,0) };

	//读取CELL的高度
	for (int row = 0; row < singleTableWidth; ++row)
	{
		for (int col = 0; col < singleTableWidth; ++col)
		{
			if (pMarkTable[row * (int)singleTableWidth + col])
			{	
				point[0] = point[1] = point[2] = point[3] = NaviGrid::GeneratePosition(col,row);

				for (int iCorn = 0; iCorn < 4; ++iCorn)
				{
					if (0 == heightCount)
					{
						stream.read(&heightCount);

						if (heightMap.size() > 65535)
						{
							stream.read(&lastHeight);
						}
						else
						{
							unsigned short tmpHeight;
							stream.read(&tmpHeight);
							lastHeight = tmpHeight;
						}
					}

					if (heightCount <= 0 || lastHeight >= heightMap.size())
					{
						delete []pMarkTable;
						stream.close();
						return false;
					}

					point[iCorn].z = heightMap[lastHeight];
					point[iCorn].x += offset[iCorn].x * NaviGridSize;
					point[iCorn].y += offset[iCorn].y * NaviGridSize;
					heightCount--;
				}

				AddCell(point[0],point[1],point[2],point[3]);
			}
		}
	}
	
	delete []pMarkTable;
	stream.close();

	//m_gen.Build(this,this);
	m_isCellChanged = false;

#endif

	return true;
}

bool NavigationCellMgr::FindPath(VectorF& start, VectorF& end)
{
#ifdef NTJ_EDITOR
	char filename[256];
	dStrcpy(filename, sizeof(filename),Con::getVariable("$Server::MissionFile"));

	//保存导航网格
	char *ext = dStrrchr(filename, '.');
	*ext = '\0';
	dStrcat(filename, sizeof(filename), ".ng");

	if (m_isCellChanged)
		Save(filename);

	CNavigationMeshEx path;

	if (!path.Open(filename,true))
		return false;
    
    while(!path.Finish())
    {
        Sleep(1000);
    }

	if (!path.FindPath(start,end))
		return false;

	m_Path = path.GetPath();
	m_Path.push_front(start);

	return true;
#else
	return false;
#endif
}

void NavigationCellMgr::ShowOptimizeMesh(void)
{
#ifdef NTJ_EDITOR
	m_isShowOptimized = !m_isShowOptimized;

	if (m_isShowOptimized)
	{
		//m_gen.Build(this,this);
	}
#endif
}

void NavigationCellMgr::EnableZBuffer(void)
{
	m_isEnableZBuffer = !m_isEnableZBuffer;
}


void NavigationCellMgr::resetStateBlock()
{
	//mSetSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, true);
	GFX->setRenderState(GFXRSSrcBlend, GFXBlendSrcAlpha);
	GFX->setRenderState(GFXRSDestBlend, GFXBlendDestAlpha);
	GFX->setRenderState(GFXRSCullMode, GFXCullCCW);
	GFX->endStateBlock(mSetSB);

	//mZTrueSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, true);
	GFX->endStateBlock(mZTrueSB);

	//mZFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSZEnable, false);
	GFX->endStateBlock(mZFalseSB);

	//mZFalseSB
	GFX->beginStateBlock();
	GFX->setRenderState(GFXRSAlphaBlendEnable, false);
	GFX->endStateBlock(mZFalseSB);
}


void NavigationCellMgr::releaseStateBlock()
{
	if (mSetSB)
	{
		mSetSB->release();
	}

	if (mZTrueSB)
	{
		mZTrueSB->release();
	}

	if (mZFalseSB)
	{
		mZFalseSB->release();
	}

	if (mAlphaBlendFalseSB)
	{
		mAlphaBlendFalseSB->release();
	}
}

void NavigationCellMgr::init()
{
	if (mSetSB == NULL)
	{
		mSetSB = new GFXD3D9StateBlock;
		mSetSB->registerResourceWithDevice(GFX);
		mSetSB->mZombify = &releaseStateBlock;
		mSetSB->mResurrect = &resetStateBlock;

		mZTrueSB = new GFXD3D9StateBlock;
		mZFalseSB = new GFXD3D9StateBlock;
		mAlphaBlendFalseSB = new GFXD3D9StateBlock;

		resetStateBlock();
	}


}

void NavigationCellMgr::shutdown()
{
	SAFE_DELETE(mSetSB);
	SAFE_DELETE(mZTrueSB);
	SAFE_DELETE(mZFalseSB);
	SAFE_DELETE(mAlphaBlendFalseSB);
}

