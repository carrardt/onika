#include "onika/kdtree.h"
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


