#include "onika/algorithm/rbtree.h"
#include "onika/algorithm/basics.h" // indexed_less

#include <iostream>
#include <vector>
#include <functional>   // std::less


typedef onika::algorithm::IndexedLess< std::vector<double> > Compare;

// positional binary tree
typedef onika::algorithm::TupleVectorPTree< std::vector< std::tuple<int,size_t,size_t,size_t> > > PTreeBase;
typedef onika::algorithm::BTree< PTreeBase, Compare, onika::algorithm::PNodeRefT > PTree;
typedef typename PTree::NodeRef PNodeRef;

void print(PNodeRef node, int indent=0)
{
	if( node.self == PNodeRef::NullPtr ) return;
	print(node.getRight(),indent+4);
	for(int i=0;i<indent;i++){std::cout<<' ';}
	std::cout<<node.getValue()<<" @"<<node.self<<", P="<<node.getPosition()<<"\n";
	print(node.getLeft(),indent+4);
}

void printsorted(PNodeRef node)
{
	if( node.self == PNodeRef::NullPtr ) return;
	printsorted(node.getLeft());
	std::cout<<node.getValue()<<" @"<<node.self<<", P="<<node.getPosition()<<"\n";
	printsorted(node.getRight());
}

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand48(seed);

	std::vector<double> values(100);
	for(int i=0;i<100;i++)
	{
		values[i]=drand48();
	}

	auto iless = onika::algorithm::indexed_less( values );
	std::cout<<"Positional BTree Test\n";
	PTree pt( iless );
	for(int i=0;i<100;i++)
	{
		pt.insert(i);
	}
	std::cout<<"--- sorted list ---\n";
	printsorted(pt.getRoot());

	return 0;
}
 // end of file
