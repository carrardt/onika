#ifndef __onika_algorithm_rbtree_h
#define __onika_algorithm_rbtree_h

#include <tuple>
#include <vector>
#include <functional>   // std::less
#include "onika/language.h"

namespace onika { namespace algorithm {

// TODO: add tree_traits
template<class _ContainerType>
struct TupleVectorBTree
{
	typedef _ContainerType ContainerType;
	typedef size_t NodePtr; // position of node description in container
	typedef typename ContainerType::value_type Node;
	typedef typename std::tuple_element<0,Node>::type ValueType;
	typedef typename std::tuple_element<1,Node>::type LeftPtrType;
	typedef typename std::tuple_element<2,Node>::type RightPtrType;

	static constexpr NodePtr NullPtr = (static_cast<NodePtr>(0) - 1) >> 1;

	inline TupleVectorBTree() : root(NullPtr), freelist(NullPtr) {}

	inline ValueType    getValue(NodePtr i) { return std::get<0>(data[i]) ; }
	inline LeftPtrType  getLeft (NodePtr i) { return std::get<1>(data[i]) ; }
	inline RightPtrType getRight(NodePtr i) { return std::get<2>(data[i]) ; }

	inline void setValue(NodePtr i, ValueType x)    { std::get<0>(data[i]) = x; }
	inline void setLeft (NodePtr i, LeftPtrType x)  { std::get<1>(data[i]) = x; }
	inline void setRight(NodePtr i, RightPtrType x) { std::get<2>(data[i]) = x; }

	// returns new freeNoseList
	inline NodePtr addNode()
	{
		NodePtr newNode;
		if( freelist != NullPtr )
		{
			newNode = freelist;
			freelist = getRight(freelist);
		}
		else
		{
			newNode = data.size();
			data.resize( data.size() + 1 );
		}
		this->setLeft(newNode,NullPtr);
		this->setRight(newNode,NullPtr);
		return newNode;
	}

	inline void removeNode(NodePtr i)
	{
		if( i == (data.size()-1) )
		{
			data.pop_back();
		}
		else
		{
			setRight(i,freelist);
			freelist = i;
		}
	}

	ContainerType data;
	NodePtr root;
	NodePtr freelist;

};

template<class _ContainerType>
struct TupleVectorPTree : public TupleVectorTree<_ContainerType>
{
	typedef typename std::tuple_element<3,typename _ContainerType::value_type>::type CountType;
	inline CountType getCount(NodePtr i) { return std::get<3>(data[i]) ; }
	inline void setCount(NodePtr i, CountType x) { std::get<3>(data[i]) = x; }
};

template<class Tree>
struct _NodeRef<Tree>
{
	typedef typename Tree::NodePtr NodePtr;
	typedef typename Tree::ValueType ValueType;
	static constexpr NodePtr NullPtr = Tree::NullPtr;

	NodePtr grandparent, parent, self;
	Tree& tree;

	inline _NodeRef(Tree& t, NodePtr s, NodePtr p=NullPtr, NodePtr gp=NullPtr) : tree(t), self(s), parent(p), grandparent(gp) {}
	inline _NodeRef(Tree& t) : tree(t), self( tree.addNode() ), parent(NullPtr), grandparent(NullPtr) {}

	inline _NodeRef  getLeft () const { return _NodeRef( tree , tree.getLeft(self), self, parent ); }
	inline _NodeRef  getRight() const { return _NodeRef( tree , tree.getRight(self), self, parent ); }
	inline ValueType getValue() const { return tree.getValue(self); }

	inline void setLeft (_NodeRef x) 	 { tree.setLeft(self,x.self); }
	inline void setRight(_NodeRef x) 	 { tree.setRight(self,x.self); }
	inline void setValue(const ValueType& x) { tree.setValue(self,x); }

	inline _NodeRef insert(const ValueType& x)
	{
		if( self == NullPtr )
		{
			self = tree.addNode();
			setValue( x );
		}
		else if( tree.compare( x, getValue() ) )
		{
			setLeft( getLeft().insert(x) );
		}
		else
		{
			setRight( getRight().insert(x) );
		}
		return *this;
	}
};

#if 0
// counter stored at each node is equal to the total number of nodes of it's left child's subtree 
template<class RbTree, class count_traits >
struct _PNodeRef : public _NodeRef<RbTree>
{
	typedef typename RbTree::ValueType ValueType;
	typedef typename pos_traits::value_type CountType;
	inline CountType getLeftCount() { return count_traits::get(this->tree.data,this->self); }
	inline void setLeftCount(CountType n) { count_traits::set(this->tree.data,this->self,n); }

	inline _PNodeRef(RbTree& t, NodePtr s, _PNodeRef p, _PNodeRef gp ) : _NodeRef<RbTree>(t,s,p.self,gp.self)
	{
		index = getLeftCount();
		if( this->parent != NullPtr )
		{
			if(  )
			index += 1 + count_traits::get(this->tree.data,this->parent);
		}
	}
	inline _PNodeRef(RbTree& t) : _NodeRef<RbTree>(t), index(0) { setLeftCount(0); }

	inline _PNodeRef  getLeft () const { return _NodeRef( tree , tree.getLeft(self), self, parent ); }
	inline _PNodeRef  getRight() const { return _NodeRef( tree , tree.getRight(self), self, parent ); }

	inline _PNodeRef insert(const ValueType& x)
	{
		if( self == NullPtr )
		{
			this->self = tree.addNode();
			this->setValue( x );
			this->setLeftCount( 0 );
		}
		else if( tree.compare( x, getValue() ) )
		{
			this->setLeftCount( this->getLeftCount() + 1 );
			this->setLeft( this->getLeft().insert(x) );
		}
		else
		{
			this->setRight( this->getRight().insert(x) );
		}
		return *this;
	}
	
	CountType index;
};
#endif

template<class BTreeBase, class _BTreeNodeRef=_NodeRef<BTree> class _ValueCompare>
struct BTree : public BTreeBase
{
	typedef _ValueCompare ValueCompare;
	typedef typename BTreeBase::NodePtr NodePtr;
	typedef typename BTreeBase::ValueType ValueType;
	typedef _NodeRef<BTree> NodeRef;
	static constexpr NodePtr NullPtr = BTreeBase::NullPtr;

	inline BTree( const ValueCompare& comp = ValueCompare() ) : compare(comp) {}

	inline NodeRef getRoot()
	{
		return NodeRef(*this,this->root);
	}
	inline void setRoot(NodeRef n)
	{
		this->root = n.self;
	}

	inline void insert(const ValueType& x)
	{
		setRoot( getRoot().insert(x) );	
	}

	ValueCompare compare;
};



} } // end of namespace



//==============================================
//============= UNIT TEST ======================
//==============================================
#ifdef onika_algorithm_rbtree_TEST

#include <iostream>
#include <vector>
#include <functional>   // std::less

typedef onika::algorithm::TupleVectorBTree< std::vector< std::tuple<double,int,int> > > SimpleTreeBase;
typedef onika::algorithm::BTree< SimpleTreeBase, std::less<double> > SimpleTree;
typedef typename SimpleTree::NodeRef NodeRef;

void print(NodeRef node, int indent=0)
{
	for(int i=0;i<indent;i++) std::cout<<' ';
	if( node == NodeRef::NullPtr ) 
	{
		std::cout<<"null\n";
		return;
	}
	std::cout<<node.getValue()<<" @"<<node.self<<"\n";
	print(node.getLeft(),indent+4);
	print(node.getRight(),indent+4);
}

/*
void print(PNodeRef node, int indent=0)
{
	for(int i=0;i<indent;i++) std::cout<<' ';
	if( node == NodeRef::NullPtr ) 
	{
		std::cout<<"null\n";
		return;
	}
	std::cout<<node.getValue()<<" @"<<node.self<<", P="<<node.getPosition()<<"\n";
	print(node.getLeft(),indent+4);
	print(node.getRight(),indent+4);
}
*/

int main()
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand48(seed);

	SimpleTree t;
	double value;
	
	for(int i=0;i<10;i++)
	{
		t.insert( drand48() );
	}

	print(t.getRoot());

	return 0;
}
#endif // en of unit test

#endif // end of file
