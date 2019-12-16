#include "NavigationCell.h"
//#include "navigationheap.h"
#include "NavigationCellMgr.h"


VectorF NavigationCell::GetVertex(int corn) const
{
	AssertFatal(corn >= 0 && corn <= VERT_D, "Invalidate index of NavigationCell");
	return NavigationCellMgr::s_GridPointBase.GetPoint(m_Vertex[corn]);
}

int NavigationCell::GetVertexIndex(int corn) const
{
	AssertFatal(corn >= 0 && corn <= VERT_D, "Invalidate index of NavigationCell");
	return m_Vertex[corn];
}

bool NavigationCell::IsSharedSide(CELL_SIDE side1, NavigationCell* cell2, CELL_SIDE side2)
{
	return ( (this->m_Vertex[NavigationCellMgr::sLinePoint[side1][0]] == cell2->m_Vertex[NavigationCellMgr::sLinePoint[side2][0]]) &&
		(this->m_Vertex[NavigationCellMgr::sLinePoint[side1][1]] == cell2->m_Vertex[NavigationCellMgr::sLinePoint[side2][1]]) );
}

void NavigationCell::Initialize(const VectorF& a, const VectorF& b, const VectorF& c, const VectorF& d)
{
	m_Vertex[VERT_A] = NavigationCellMgr::s_GridPointBase.AddPoint(a);
	m_Vertex[VERT_B] = NavigationCellMgr::s_GridPointBase.AddPoint(b);
	m_Vertex[VERT_C] = NavigationCellMgr::s_GridPointBase.AddPoint(c);
	m_Vertex[VERT_D] = NavigationCellMgr::s_GridPointBase.AddPoint(d);

	// object must be re-linked
	m_Link[SIDE_LEFT]	= NULL;
	m_Link[SIDE_TOP]	= NULL;
	m_Link[SIDE_RIGHT]	= NULL;
	m_Link[SIDE_BOTTOM] = NULL;

	m_nId = NaviGrid::GenerateID((a + b + c + d) / 4.0f);
}

void NavigationCell::Initialize(int a, int b, int c, int d)
{
	m_Vertex[VERT_A] = a;
	m_Vertex[VERT_B] = b;
	m_Vertex[VERT_C] = c;
	m_Vertex[VERT_D] = d;

	// object must be re-linked
	m_Link[SIDE_LEFT]	= NULL;
	m_Link[SIDE_TOP]	= NULL;
	m_Link[SIDE_RIGHT]	= NULL;
	m_Link[SIDE_BOTTOM] = NULL;

	m_nId = NaviGrid::GenerateID((GetVertex(0) + GetVertex(1) + GetVertex(2) + GetVertex(3)) / 4.0f);
}

void NavigationCell::Initialize(int a, int b, int c, int d,unsigned int id)
{
	m_Vertex[VERT_A] = a;
	m_Vertex[VERT_B] = b;
	m_Vertex[VERT_C] = c;
	m_Vertex[VERT_D] = d;

	// object must be re-linked
	m_Link[SIDE_LEFT]	= NULL;
	m_Link[SIDE_TOP]	= NULL;
	m_Link[SIDE_RIGHT]	= NULL;
	m_Link[SIDE_BOTTOM] = NULL;

	m_nId = id;
}