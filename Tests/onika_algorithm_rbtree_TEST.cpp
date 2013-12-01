#include "onika/algorithm/rbtree.h"

#include <iostream>
#include <vector>
#include <functional>   // std::less

// simple binary tree
typedef onika::algorithm::TupleVectorBTree< std::vector< std::tuple<double,size_t,size_t> > > SimpleTreeBase;
typedef onika::algorithm::BTree< SimpleTreeBase, std::less<double> > SimpleTree;
typedef typename SimpleTree::NodeRef NodeRef;

// positional binary tree
typedef onika::algorithm::TupleVectorTaggedBTree< std::vector< std::tuple<double,size_t,size_t,size_t> > > PTreeBase;
typedef onika::algorithm::BTree< PTreeBase, std::less<double>, onika::algorithm::PNodeRefT > PTree;
typedef typename PTree::NodeRef PNodeRef;


void print(NodeRef node, int indent=0)
{
	if( node.self == NodeRef::NullPtr ) return;
	print(node.getRight(),indent+4);
	for(int i=0;i<indent;i++){std::cout<<' ';}
	std::cout<<node.getValue()<<" @"<<node.self<<"\n";
	print(node.getLeft(),indent+4);
}

void printsorted(NodeRef node)
{
	if( node.self == NodeRef::NullPtr ) return;
	printsorted(node.getLeft());
	std::cout<<node.getValue()<<" @"<<node.self<<"\n";
	printsorted(node.getRight());
}

void print(PNodeRef node, int indent=0)
{
	if( node.self == NodeRef::NullPtr ) return;
	print(node.getRight(),indent+4);
	for(int i=0;i<indent;i++){std::cout<<' ';}
	std::cout<<node.getValue()<<" @"<<node.self<<", P="<<node.getPosition()<<"\n";
	print(node.getLeft(),indent+4);
}

void printsorted(PNodeRef node)
{
	if( node.self == NodeRef::NullPtr ) return;
	printsorted(node.getLeft());
	std::cout<<node.getValue()<<" @"<<node.self<<", P="<<node.getPosition()<<"\n";
	printsorted(node.getRight());
}

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";

	std::cout<<"Simple BTree Test\n";
	srand48(seed);
	SimpleTree t;
	for(int i=0;i<10;i++)
	{
		double value = drand48();
		t.insert( value );
	}
	print(t.getRoot());
	std::cout<<"Sorted list\n";
	printsorted(t.getRoot());


	std::cout<<"Positional BTree Test\n";
	srand48(seed);
	PTree pt;
	for(int i=0;i<10;i++)
	{
		double value = drand48();
		std::cout<<"--- insert "<<value<<" ---\n";
		pt.insert( value );
		print(pt.getRoot());
	}
	std::cout<<"--- sorted list ---\n";
	printsorted(pt.getRoot());


	return 0;
}
 // end of file
