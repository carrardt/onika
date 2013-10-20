#ifndef __onika_kdtree_h
#define __onika_kdtree_h

#include "onika/aabb.h"
#include "onika/poweroftwo.h"
#include "onika/debug/dbgassert.h"

#include <stdlib.h> // size_t
#include <ostream> // std::ostream for PrintTreeOp

namespace onika {

template<int _NDim>
struct KdTreeNode
{
        enum
	{
		NDim = _NDim,
	};
 
	inline KdTreeNode() : axis(NDim), pcoord(0.5) {}
	inline KdTreeNode(unsigned int a, double t) : axis( (a>NDim) ? NDim : a ) , pcoord(t) {}

	inline bool isLeaf() const { return (axis == NDim); }
	inline int getSplitAxis() const { return axis; }
	inline double getSplitPCoord() const { return pcoord; }

	// write operations
	inline void setAxis(unsigned int a) { axis = (a>NDim) ? NDim : a ; }
	inline void setLeaf() { axis = NDim; }
	inline void setPCoord(double t) { pcoord = t ; }

	// alternate constructors
	static inline KdTreeNode makeLeaf() { return KdTreeNode(NDim,0.0); }

private:
	unsigned int axis;
	double pcoord;
};

struct BuildIndexTableOp
{
	inline BuildIndexTableOp(size_t* t) : table(t) {}
	template<typename KdTreeType>
	inline void operator () (const KdTreeType& kdt, size_t i, int depth, size_t left=0, size_t right=0)
	{
		table[i]=left;
	}
	size_t* table;
};

// typedef std::vector< KdTreeNode<3> > KdTreeNodeVector
template<int _NDim, typename _KdTreeNodeVector>
struct KdTree
{
        enum
	{
		NDim = _NDim,
		PREFIX = 1,
		INFIX = 2,
		POSTFIX = 4
	};

	typedef _KdTreeNodeVector KdTreeNodeVector;
	typedef KdTreeNode<NDim> KdTreeNodeT;

	inline KdTree()	: indexTable(0)  {}

	// in order split
	inline void addNode( const KdTreeNodeT& kdtn )
	{
		nodes.push_back( kdtn );
	}

	inline const KdTreeNodeT getNode( size_t i ) const
	{
		return nodes[i];
	}

	template<typename NodeOp>
	inline size_t parseTree( NodeOp& nodeFunc, int searchMode=POSTFIX, size_t i=0, int depth=0 )
	{
		size_t nodeIdx = i;
		debug::dbgassert( i < nodes.size() );
		++i;
		if( searchMode & PREFIX ) { nodeFunc( *this, nodeIdx, depth ); }
		bool leaf = nodes[nodeIdx].isLeaf();
		if( !leaf ) { i = parseTree(nodeFunc,searchMode,i,depth+1); }
		size_t l = (i-1) - nodeIdx;
		if( searchMode & INFIX ) { nodeFunc( *this, nodeIdx, depth, l ); }
		if( !leaf ) { i = parseTree(nodeFunc,searchMode,i,depth+1); }
		size_t r = (i-1) - nodeIdx - l;
		if( searchMode & POSTFIX ) { nodeFunc( *this, nodeIdx, depth, l, r ); }
		return i;
	}

	inline void buildTreeIndex()
	{
		if( indexTable != 0 ) return;
		indexTable = new size_t[ nodes.size() ];
		BuildIndexTableOp tableBuild(indexTable);
		parseTree( tableBuild );
	}

	private: 
	// minimal storage : all nodes in prefix search (self,left,right) order
	KdTreeNodeVector nodes;

	// optional acceleration structures
	size_t* indexTable;
};



//================ utility operators ==================

// Count number of non leaf nodes
struct CommonCountersOp
{
	inline CommonCountersOp() : leaves(0), nonleaf(0), maxdepth(-1) {}

	template<typename KdTreeType>
	inline void operator () (const KdTreeType& kdt, size_t idx, int depth, size_t left=0, size_t right=0)
	{
		if( ! kdt.getNode(idx).isLeaf() ) ++nonleaf;
		else ++leaves;
		if( depth > maxdepth ) maxdepth=depth;
	}

	size_t leaves;
	size_t nonleaf;
	int maxdepth;
};

// prints human readable representation of a tree to a stream
struct PrintTreeOp
{
	inline PrintTreeOp(std::ostream& o) : out(o) {}

	template<typename KdTreeType>
	inline void operator () (const KdTreeType& kdt, size_t idx, int depth, size_t left=0, size_t right=0)
	{
		static const char axisName[3]={'X','Y','Z'};
		for(int i=0;i<depth;i++) out<<"  ";
		out<<"#"<<idx<<" ";
		typename KdTreeType::KdTreeNodeT kdtn = kdt.getNode(idx);
		if( kdtn.isLeaf() ) out<<"*";
		else out<<axisName[kdtn.getSplitAxis()]<<"@"<<kdtn.getSplitPCoord();
		out<<std::endl;
	}

	std::ostream& out;
};

} // end of namespace

#endif // __KdTree_h




// ==========================================================
// =================== Unit Test ============================
// ==========================================================

#ifdef onika_kdtree_TEST
#include <iostream>
#include <vector>
using namespace std;

template<int NDim>
inline
void userInputNode(onika::KdTreeNode<NDim>& node)
{
	int axis;
	double pcoord;
	cout<<"Axis PCoord ? "; cout.flush();	
	cin>>axis;
	cin>>pcoord;
	node.setAxis(axis);
	node.setPCoord(pcoord);
}

typedef onika::KdTree< 3 , std::vector< onika::KdTreeNode<3> > > KdTree3D;

int main()
{
	KdTree3D kdtree;

	int toCreate = 1;
	do
	{
		KdTree3D::KdTreeNodeT kdtn;
		userInputNode(kdtn);
		kdtree.addNode( kdtn );
		if( kdtn.isLeaf() ) --toCreate;
		else ++toCreate;
	} while( toCreate > 0 );

	onika::PrintTreeOp print(std::cout);
	kdtree.parseTree( print, KdTree3D::PREFIX );

	onika::CommonCountersOp counter;
	kdtree.parseTree( counter );
	cout<<"intermediate nodes = "<<counter.nonleaf<<endl;
	cout<<"max depth = "<<counter.maxdepth<<endl;

	return 0;
}
#endif


