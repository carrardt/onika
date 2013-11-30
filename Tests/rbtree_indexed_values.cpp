#include "onika/algorithm/rbtree.h"
#include "onika/algorithm/basics.h" // indexed_less

#include <iostream>
#include <vector>
#include <functional>   // std::less
#include <math.h>

typedef onika::algorithm::IndexedLess< std::vector<double> > Compare;

// simple binary tree
typedef onika::algorithm::TupleVectorBTree< std::vector< std::tuple<int,size_t,size_t,size_t> > > TreeBase;
typedef onika::algorithm::BTree< TreeBase, Compare, onika::algorithm::NodeRefT > Tree;
typedef typename Tree::NodeRef NodeRef;

void print(const std::vector<double>& values, NodeRef node, int indent=0)
{
	if( node.self == NodeRef::NullPtr ) return;
	print(values,node.getRight(),indent+4);
	for(int i=0;i<indent;i++){std::cout<<' ';}
	std::cout<<node.getValue()<<" @"<<node.self<<" ,V="<<values[node.getValue()]<<"\n";
	print(values,node.getLeft(),indent+4);
}

void printsorted(const std::vector<double>& values,NodeRef node)
{
	if( node.self == NodeRef::NullPtr ) return;
	printsorted(values,node.getLeft());
	std::cout<<node.getValue()<<"@"<<node.self<<",V="<<values[node.getValue()]<<" ";
	printsorted(values,node.getRight());
}

// positional binary tree
typedef onika::algorithm::TupleVectorPTree< std::vector< std::tuple<int,size_t,size_t,size_t> > > PTreeBase;
typedef onika::algorithm::BTree< PTreeBase, Compare, onika::algorithm::PNodeRefT > PTree;
typedef typename PTree::NodeRef PNodeRef;

void print(const std::vector<double>& values, PNodeRef node, int indent=0)
{
	if( node.self == PNodeRef::NullPtr ) return;
	print(values,node.getRight(),indent+4);
	for(int i=0;i<indent;i++){std::cout<<' ';}
	std::cout<<node.getValue()<<" @"<<node.self<<", P="<<node.getPosition()<<", V="<<values[node.getValue()]<<"\n";
	print(values,node.getLeft(),indent+4);
}

void printsorted(const std::vector<double>& values,PNodeRef node)
{
	if( node.self == PNodeRef::NullPtr ) return;
	printsorted(values,node.getLeft());
	std::cout<<node.getValue()<<" @"<<node.self<<", P="<<node.getPosition()<<", V="<<values[node.getValue()]<<"\n";
	printsorted(values,node.getRight());
}

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand48(seed);

	constexpr unsigned int N = 1000;

	std::vector<double> values(N);
	for(int i=0;i<N;i++)
	{
		values[i]=drand48();
	}

	auto iless = onika::algorithm::indexed_less( values );
	std::cout<<"----------- Simple BTree Test --------------------\n";
	Tree tr( iless );
	for(int i=0;i<N;i++)
	{
		tr.insert(i);
	}
	std::cout<<"sorted list :\n";
	printsorted(values,tr.getRoot());
	unsigned int dmin=-1, dmax=0;
	onika::algorithm::probe_btree_depth( tr.getRoot(), dmin, dmax );
	std::cout<<"Log2(N) = "<<log2(N) <<"\n";
	std::cout<<"Min depth = "<<dmin<<"\n";
	std::cout<<"Max depth = "<<dmax<<"\n";
	std::cout<<"inbalance = "<<(dmax-dmin)/log2(N)<<"\n";

	std::cout<<"Positional BTree Test\n";
	PTree pt( iless );
	for(int i=0;i<N;i++)
	{
		pt.insert(i);
	}
	std::cout<<"--- sorted list ---\n";
	printsorted(values,pt.getRoot());

	std::cout<<"--- tree stats ---\n";
	dmin=-1; dmax=0;
	onika::algorithm::probe_btree_depth( pt.getRoot(), dmin, dmax );
	std::cout<<"Log2(N) = "<<log2(N) <<"\n";
	std::cout<<"Min depth = "<<dmin<<"\n";
	std::cout<<"Max depth = "<<dmax<<"\n";
	std::cout<<"inbalance = "<<(dmax-dmin)/log2(N)<<"\n";
	return 0;
}
 // end of file
