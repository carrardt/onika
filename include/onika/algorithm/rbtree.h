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
	typedef typename std::tuple_element<3,NodeValue>::type ValueType; // position of node description in container
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
	inline NodePtr addNode(bool color=Black, NodePtr left=NullPtr, NodePtr right=NullPtr)
	{
		NodePtr newNode;
		if( freeNodes != NullPtr )
		{
			newNode = freeNodes;
			freeNodes = getRight(tree,freeNodes);
			tree[newNode] = Node(color,left,right);
		}
		else
		{
			newNode = tree.size();
			tree.push_back( Node(color,left,right) );
		}
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

template<class RbTreeBase, class _ValueCompare, bool has_position=RbTreeBase::has_position>
struct RbTree : public RbTreeBase
{
	typedef _ValueCompare ValueCompare;
	typedef RbTreeBase::NodePtr NodePtr;
	typedef RbTreeBase::NodeValue NodeValue;
	static constexpr NodePtr NullPtr = RbTreeBase::NullPtr;

	struct NodeRef
	{
		NodePtr grandparent, parent, self;
		RbTree& tree;
		inline NodeRef(RbTree& t, NodePtr s, NodePtr p=NullPtr, NodePtr gp=NullPtr) : tree(t), self(s), parent(p), grandparent(gp) {}

		inline bool getColor() { return tree.getColor(self); }
		inline NodeRef getLeft() { return NodeRef( tree , tree.getLeft(self), self, parent ); }
		inline NodeRef getRight() { return NodeRef( tree , tree.getRight(self), self, parent ); }

		inline void setColor(bool x) { return tree.setColor(self,x); }
		inline void setLeft(NodeRef x) { return tree.setLeft(self,x.self); }
		inline void setRight(NodeRef x) { return tree.setRight(self,x.self); }
	};

	inline RbTree( const ValueCompare& comp ) : compare(comp) {}

	inline NodeRef getRoot()
	{
		return NodeRef(tree,root);
	}

	ValueCompare compare;
};

} } // end of namespace



//==============================================
//============= UNIT TEST ======================
//==============================================
#ifdef onika_algorithm_rbtree_TEST
int main()
{
	return 0;
}
#endif // en of unit test

#endif // end of file
