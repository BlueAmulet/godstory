#ifndef NAVIGATIONHEAP_H
#define NAVIGATIONHEAP_H

#include <vector>
#include <list>
#include <algorithm>

/************************************************************************/
/* 对CELL按权重进行堆排序
/************************************************************************/

template<typename _NavigationCell>
class NavigationNode
{
public:
	NavigationNode(): cell(0), cost(0),id(0) {}
	~NavigationNode() {}

	_NavigationCell* cell; // pointer to the cell in question
	unsigned int id;
	float cost;        // (g + h) in A* represents the cost of traveling through this cell

	inline bool operator < (const NavigationNode& b )
	{
		// To compare two nodes, we compare the cost or `f' value, which is the
		// sum of the g and h values defined by A*.
		return (cost < (b.cost));
	}

	inline bool operator > (const NavigationNode& b )
	{
		// To compare two nodes, we compare the cost or `f' value, which is the
		// sum of the g and h values defined by A*.
		return (cost > (b.cost));
	}

	inline bool operator == (const NavigationNode& b )
	{
		// Two nodes are equal if their components are equal
		return ((cell == b.cell) && (cost == b.cost));
	}
};

//
// The STL requires some external operators as well
//
template<typename _NavigationCell>
inline bool operator < ( const NavigationNode<_NavigationCell>& a, const NavigationNode<_NavigationCell>& b )
{
	return (a.cost < b.cost);
}

template<typename _NavigationCell>
inline bool operator > ( const NavigationNode<_NavigationCell>& a, const NavigationNode<_NavigationCell>& b )
{
	return (a.cost > b.cost);
}

template<typename _NavigationCell>
inline bool operator == ( const NavigationNode<_NavigationCell>& a, const NavigationNode<_NavigationCell>& b )
{
	return ((a.cell == b.cell) && (a.cost == b.cost));
}


/*	NavigationHeap
------------------------------------------------------------------------------------------
	
	A NavigationHeap is a priority-ordered list facilitated by the STL heap functions.
	This class is also used to hold the current path finding session ID and the desired 
	goal point for NavigationCells to query. Thanks to Amit J. Patel for detailing the use
	of STL heaps in this way. It's much faster than a linked list or multimap approach.
	
------------------------------------------------------------------------------------------
*/

template<typename _NavigationCell>
class NavigationHeap
{
public:

	// ----- ENUMERATIONS & CONSTANTS -----
	// greater<NavigationNode> is an STL thing to create a 'comparison' object out of
	// the greater-than operator, and call it comp.
	typedef std::vector<NavigationNode<_NavigationCell> > Container;
	std::greater<NavigationNode<_NavigationCell> > comp;


	// ----- CREATORS ---------------------

	/*NavigationHeap() {}
	~NavigationHeap() {}*/

	// ----- OPERATORS --------------------

	// ----- MUTATORS ---------------------
	void Setup(int SessionID, const VectorF& Goal)
	{
		m_Goal = Goal;
		m_SessionID = SessionID;
		m_Nodes.clear();
	}

	void AddCell(_NavigationCell* pCell)
	{
		NavigationNode<_NavigationCell> NewNode;

		NewNode.cell = pCell;
		NewNode.cost = pCell->PathfindingCost();

		m_Nodes.push_back(NewNode);
		std::push_heap( m_Nodes.begin(), m_Nodes.end(), comp );
	}

	void AddCell(_NavigationCell* pCell,unsigned int id,float cost)
	{
		NavigationNode<_NavigationCell> NewNode;

		NewNode.cell    = pCell;
		NewNode.cost    = cost;
		NewNode.id      = id;
		
		m_Nodes.push_back(NewNode);
		std::push_heap( m_Nodes.begin(), m_Nodes.end(), comp );
	}

	void AdjustCell(_NavigationCell* pCell)
	{
		Container::iterator iter = FindNodeInterator(pCell);

		if (iter!=m_Nodes.end())
		{
			// update the node data
			(*iter).cell = pCell;
			(*iter).cost = pCell->PathfindingCost();

			// reorder the heap
			std::push_heap( m_Nodes.begin(), iter+1, comp );
		}
	}

	void AdjustCell(_NavigationCell* pCell,float cost)
	{
		Container::iterator iter = FindNodeInterator(pCell);

		if (iter!=m_Nodes.end())
		{
			// update the node data
			(*iter).cell = pCell;
			(*iter).cost = cost;

			// reorder the heap
			std::push_heap( m_Nodes.begin(), iter+1, comp );
		}
	}

	// ----- ACCESSORS --------------------

	bool NotEmpty()const
	{
		return(m_Nodes.size() ? true:false);
	}

	void GetTop(NavigationNode<_NavigationCell>& n)
	{
		n = m_Nodes.front();
		std::pop_heap( m_Nodes.begin(), m_Nodes.end(), comp );
		m_Nodes.pop_back();
	}

	int SessionID()const
	{
		return(m_SessionID);
	}

	const VectorF& Goal()const
	{
		return(m_Goal);
	}

private:

	// ----- DATA -------------------------
	Container m_Nodes;
	int m_SessionID;
	VectorF m_Goal;

	// ----- HELPER FUNCTIONS ------------
	typename Container::iterator FindNodeInterator(_NavigationCell* pCell)
	{
		for( Container::iterator i = m_Nodes.begin(); i != m_Nodes.end(); ++i )
		{
			if( (*i).cell == pCell )
				return i;
		}
		return m_Nodes.end();
	}

	// ----- UNIMPLEMENTED FUNCTIONS ------

	/*NavigationHeap( const NavigationHeap& Src);
	NavigationHeap& operator=( const NavigationHeap& Src);*/
};

#endif  // end of file      ( NavigationHeap.h )

