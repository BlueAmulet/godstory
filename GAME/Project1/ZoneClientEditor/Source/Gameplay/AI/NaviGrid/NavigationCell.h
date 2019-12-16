#pragma once

//#include "System/SysManager.h"
#include "math/mPoint.h"
#include "math/mPlane.h"
#include "sceneGraph/sceneObject.h"
#include <string>
#include "core/fileStream.h"
#include "NavigationHeap.h"

//--------------------------------------------
//        TOP
//		B------C
//      |      |
//  LEFT|      |RIGHT
//      |      |
//		A------D
//       BOTTOM
//--------------------------------------------

class NavigationCell
{
	friend class NavigationCellMgr;
	friend class AutoGeneralNavigationGridAction;
	friend class RemoveTreeNavigationGridAction;
	friend class NaviGridAutoGenerate;
public:

	// ----- ENUMERATIONS & CONSTANTS -----

	enum CELL_VERT
	{
		VERT_A = 0,
		VERT_B,
		VERT_C,
		VERT_D,
	};

	enum CELL_SIDE
	{
		SIDE_LEFT,
		SIDE_TOP,
		SIDE_RIGHT,
		SIDE_BOTTOM,
	};

	enum CELL_SIDE_MASK
	{
		SIDE_L = 1 << 0,
		SIDE_T = 1 << 1,
		SIDE_R = 1 << 2,
		SIDE_B = 1 << 3,
	};

	enum PATH_RESULT
	{
		NO_RELATIONSHIP,		// the path does not cross this cell
		ENDING_CELL,			// the path ends in this cell	
		EXITING_CELL,			// the path exits this cell through side X
	};

	// ----- CREATORS ---------------------

	inline NavigationCell():m_nId(0), m_Open(false){}
	inline ~NavigationCell(){ };

	// ----- MUTATORS ---------------------

	void			Initialize			(const VectorF& a, const VectorF& b, const VectorF& c, const VectorF& d);
	void			Initialize			(int a, int b, int c, int d);
	void			Initialize			(int a, int b, int c, int d,unsigned int id);

	NavigationCell* GetLink				(int Side)const { return m_Link[Side]; }
	void			SetLink				(CELL_SIDE Side, NavigationCell* Caller) { m_Link[Side] = Caller; }

	VectorF			GetVertex			(int corn) const;
	int				GetVertexIndex		(int corn) const;

	// ---------HELPER FUNCTIONS---------------
	bool			IsSharedSide		(CELL_SIDE side1, NavigationCell* cell2, CELL_SIDE side2);
	
	unsigned int	GetId(void) const {return m_nId;}
private:
	unsigned int		m_nId;		
	int					m_Vertex[4];	// 4¸ö¶¥µãË÷Òý
	NavigationCell*		m_Link[4];
	bool				m_Open;			// are we currently listed as an Open cell to revisit and test?
};