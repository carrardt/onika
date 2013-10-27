#ifndef __onika_algorithm_rbtree_h
#define __onika_algorithm_rbtree_h

#include <tuple>
#include <functional>   // std::less
#include "onika/language.h"

namespace onika { namespace algorithm {

// RbTRee stored as an integer array, with least signficant bit used for color
// tree composed of : [color|left|right|value] ... 
template<class _TupleVector>
struct RbTreeTupleVecBase
{
	typedef _TupleVector Container;
	typedef typename Container::value_type Node; // position of node description in container
	typedef typename std::tuple_element<3,Node>::type ValueType; // position of node description in container
	typedef size_t NodePtr; // position of node description in container

	static constexpr NodePtr NullPtr = (static_cast<NodePtr>(0) - 1) >> 1;
	static constexpr bool Black = false;
	static constexpr bool Red = true;
	static constexpr bool has_position = false;

	inline RbTreeTupleVecBase() : root(NullPtr), freeNodes(NullPtr) {}

	inline NodePtr   getRoot()           const { return root; }
	inline bool      getColor(NodePtr i) const { return std::get<0>( tree[i] ) ; }
	inline NodePtr   getLeft (NodePtr i) const { return std::get<1>( tree[i] ) ; }
	inline NodePtr   getRight(NodePtr i) const { return std::get<2>( tree[i] ) ; }
	inline ValueType getValue(NodePtr i) const { return std::get<3>( tree[i] ) ; }

	inline void setRoot (NodePtr i)            	    { root = i; }
	inline void setColor(NodePtr i, bool    x) 	    { std::get<0>(tree[i]) = x; }
	inline void setLeft (NodePtr i, NodePtr x) 	    { std::get<1>(tree[i]) = x; }
	inline void setRight(NodePtr i, NodePtr x) 	    { std::get<2>(tree[i]) = x; }
	inline void setValue(NodePtr i, const ValueType& x) { std::get<3>(tree[i]) = x; }

	// returns new freeNoseList
	inline NodePtr addNode()
	{
		NodePtr newNode;
		if( freeNodes != NullPtr )
		{
			newNode = freeNodes;
			freeNodes = getRight(freeNodes);
		}
		else
		{
			newNode = tree.size();
			tree.resize( tree.size() + 1 );
		}
		setLeft(newNode,NullPtr);
		setRight(newNode,NullPtr);
		return newNode;
	}

	inline void removeNode(NodePtr i)
	{
		if( i == (tree.size()-1) )
		{
			tree.pop_back();
		}
		else
		{
			setRight(i,freeNodes);
			freeNodes = i;
		}
	}

	Container tree;
	NodePtr root;
	NodePtr freeNodes;
};

template<class _TupleVector>
struct RbTreeTupleVecBaseWithPosition : public RbTreeTupleVecBase<_TupleVector>
{
	typedef RbTreeTupleVecBase<_TupleVector> Super;
	typedef typename Super::Node Node; 
	typedef typename Super::NodePtr NodePtr; 
	typedef typename std::tuple_element<4,Node>::type PositionType; 
	static constexpr bool has_position = true;

	inline PositionType getPosition(NodePtr i) const { return std::get<4>(this->tree[i]) ; }
	inline void setPosition(NodePtr i, PositionType p) { std::get<4>(this->tree[i]) = p ; }
};

template<class RbTree, bool has_position=RbTree::has_position> struct _NodeRef {};

template<class RbTree>
struct _NodeRef<RbTree,false>
{
	typedef typename RbTree::NodePtr NodePtr;
	typedef typename RbTree::ValueType ValueType;
	static constexpr NodePtr NullPtr = RbTree::NullPtr;
	
	NodePtr grandparent, parent, self;
	RbTree& tree;
	
	inline _NodeRef(RbTree& t, NodePtr s, NodePtr p=NullPtr, NodePtr gp=NullPtr) : tree(t), self(s), parent(p), grandparent(gp) {}
	inline _NodeRef(RbTree& t) : tree(t), self( tree.addNode() ), parent(NullPtr), grandparent(NullPtr) {}

	inline bool operator == (NodePtr i) const { return self == i; }

	inline bool 	 getColor() const { return tree.getColor(self); }
	inline _NodeRef  getLeft () const { return _NodeRef( tree , tree.getLeft(self), self, parent ); }
	inline _NodeRef  getRight() const { return _NodeRef( tree , tree.getRight(self), self, parent ); }
	inline ValueType getValue() const { return tree.getValue(self); }

	inline void setColor(bool x) 		 { tree.setColor(self,x); }
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

template<class RbTree>
struct _NodeRef <RbTree,true> : public _NodeRef<RbTree,false>
{
	typedef typename RbTree::PositionType PositionType;
	inline PositionType getPosition() const { return this->tree.getPosition(this->self); }
	inline void setPosition(const PositionType& x) { this->tree.setPosition(this->self,x); }
};

template<class RbTreeBase, class _ValueCompare, bool has_position=RbTreeBase::has_position>
struct RbTree : public RbTreeBase
{
	typedef _ValueCompare ValueCompare;
	typedef typename RbTreeBase::NodePtr NodePtr;
	typedef typename RbTreeBase::ValueType ValueType;
	typedef _NodeRef<RbTree,has_position> NodeRef;
	static constexpr NodePtr NullPtr = RbTreeBase::NullPtr;

	inline RbTree( const ValueCompare& comp = ValueCompare() ) : compare(comp) {}

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

typedef std::vector< std::tuple<bool,int,int,double> > TreeStorage;
typedef onika::algorithm::RbTreeTupleVecBase< TreeStorage > SimpleTreeBase;
typedef onika::algorithm::RbTree< SimpleTreeBase, std::less<double> > SimpleTree;
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

int main()
{
	SimpleTree t;
	double value;
	
	
	std::cin >> value;
	while( value != 999 )
	{
		t.insert( value );
		std::cin >> value;
	}
	print(t.getRoot());

	return 0;
}
#endif // en of unit test

#endif // end of file
