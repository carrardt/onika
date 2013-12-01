#ifndef __onika_algorithm_rbtree_h
#define __onika_algorithm_rbtree_h

#include <tuple>
#include <vector>
#include <functional>   // std::less
#include "onika/language.h"
#include "onika/debug/dbgmessage.h"

namespace onika { namespace algorithm {

// TODO: add tree_traits
template<class _ContainerType, bool valid =
		std::is_same< typename std::tuple_element<1,typename _ContainerType::value_type>::type ,
				      typename std::tuple_element<2,typename _ContainerType::value_type>::type >::value >
struct TupleVectorBTree
{
};

template<class _ContainerType>
struct TupleVectorBTree<_ContainerType,true>
{
	typedef _ContainerType ContainerType;
	typedef typename ContainerType::value_type Node;
	typedef typename std::tuple_element<0,Node>::type ValueType;
	typedef typename std::tuple_element<1,Node>::type LeftPtrType;
	typedef typename std::tuple_element<2,Node>::type RightPtrType;
	typedef LeftPtrType NodePtr; // position of node description in container

	static constexpr unsigned int TupleSize = 3;
	static constexpr NodePtr NullPtr = (static_cast<NodePtr>(0) - 1) >> 1;

	inline TupleVectorBTree() : freelist(NullPtr) {}

	inline ValueType    getValue(NodePtr i) { return std::get<0>(data[i]) ; }
	inline LeftPtrType  getLeft (NodePtr i) { return std::get<1>(data[i]) ; }
	inline RightPtrType getRight(NodePtr i) { return std::get<2>(data[i]) ; }

	inline void setValue(NodePtr i, ValueType x)    { std::get<0>(data[i]) = x; }
	inline void setLeft (NodePtr i, NodePtr x)  { std::get<1>(data[i]) = x; }
	inline void setRight(NodePtr i, NodePtr x) { std::get<2>(data[i]) = x; }

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
	NodePtr freelist;
};

// a tree storage that holds an additional information on every node
template<class _ContainerType>
struct TupleVectorTaggedBTree : public TupleVectorBTree<_ContainerType>
{
	typedef typename TupleVectorBTree<_ContainerType>::NodePtr NodePtr; // position of node description in container
	typedef typename std::tuple_element<3,typename _ContainerType::value_type>::type TagType;
	static constexpr unsigned int TupleSize = 4;

	inline TagType getTag(NodePtr i) { return std::get<3>(this->data[i]) ; }
	inline void setTag(NodePtr i,const TagType& x) { std::get<3>(this->data[i]) = x; }
};

template<class Tree>
struct NodeRefT
{
	typedef typename Tree::NodePtr NodePtr;
	typedef typename Tree::ValueType ValueType;
	static constexpr NodePtr NullPtr = Tree::NullPtr;

	NodePtr grandparent, parent, self;
	Tree& tree;

	inline NodeRefT(Tree& t, NodePtr s, NodePtr p, NodePtr gp) : tree(t), self(s), parent(p), grandparent(gp) {}
	inline NodeRefT(Tree& t) : tree(t), self( tree.addNode() ), parent(NullPtr), grandparent(NullPtr) {}

	inline NodeRefT getLeft () const { return NodeRefT( tree , tree.getLeft(self), self, parent ); }
	inline NodeRefT getRight() const { return NodeRefT( tree , tree.getRight(self), self, parent ); }
	inline ValueType getValue() const { return tree.getValue(self); }

	inline void setLeft (NodeRefT x) 	 { tree.setLeft(self,x.self); }
	inline void setRight(NodeRefT x) 	 { tree.setRight(self,x.self); }
	inline void setValue(const ValueType& x) { tree.setValue(self,x); }

	inline NodeRefT insert(const ValueType& x)
	{
		debug::dbgmessage() << "insert("<<this->self<<","<<x<<")\n";
		if( this->self == NullPtr )
		{
//			debug::dbgmessage() << "found a place for insertion\n";
			this->self = this->tree.addNode();
			this->setValue( x );
//			debug::dbgmessage() << "Insert "<<x<<" @"<<this->self<<"\n";
		}
		else if( this->tree.compare( x, this->getValue() ) )
		{
//			debug::dbgmessage() << "left of @"<<this->self<<" (x="<<x<<",cur="<<this->getValue()<<") toward @"<<this->getLeft().self<<"\n";
			this->setLeft( this->getLeft().insert(x) );
		}
		else
		{
//			debug::dbgmessage() << "right of @"<<this->self<<" (x="<<x<<",cur="<<this->getValue()<<") toward @"<<this->getRight().self<<"\n";
			this->setRight( this->getRight().insert(x) );
		}
		return *this;
	}
};

// node accessor for tagged BTrees where tag is an integer holding total number of nodes in it's left subtree
template<class Tree>
struct PNodeRefT : public NodeRefT<Tree>
{
	typedef typename Tree::NodePtr NodePtr;
	typedef typename Tree::ValueType ValueType;
	typedef typename Tree::TagType TagType;
	static constexpr NodePtr NullPtr = Tree::NullPtr;

	inline PNodeRefT(Tree& t, NodePtr s, NodePtr p, NodePtr gp, TagType tc=0)
		: NodeRefT<Tree>(t,s,p,gp)
		, traversalCount(tc)
		{}

	inline PNodeRefT(Tree& t, TagType tc=0)
		: NodeRefT<Tree>(t,this->tree.addNode(),NullPtr,NullPtr)
		, traversalCount(tc)
		{}

	inline PNodeRefT getLeft () const
	{
		return PNodeRefT( this->tree , this->tree.getLeft(this->self), this->self, this->parent, this->traversalCount );
	}

	inline PNodeRefT getRight() const
	{
		return PNodeRefT( this->tree , this->tree.getRight(this->self), this->self, this->parent, this->getPosition()+1 );
	}

	inline TagType getLeftCount() const
	{
		return this->tree.getTag(this->self);
	}

	inline void setLeftCount(TagType n) const
	{
		return this->tree.setTag(this->self, n);
	}

	inline TagType getPosition() const
	{
		return traversalCount + getLeftCount();
	}

	inline void setLeft (PNodeRefT x) 	 { this->tree.setLeft(this->self,x.self); }
	inline void setRight(PNodeRefT x) 	 { this->tree.setRight(this->self,x.self); }
	inline void setValue(const ValueType& x) { this->tree.setValue(this->self,x); }

	// returns sub-tree size increase
	inline PNodeRefT insert(const ValueType& x)
	{
		if( this->self == NullPtr )
		{
			this->self = this->tree.addNode();
			this->setValue( x );
			this->setLeftCount( 0 );
		}
		else if( this->tree.compare( x, this->getValue() ) )
		{
			this->setLeft( this->getLeft().insert(x) );
			// we have inserted one node somwhere inside the left sub-tree, thus left count inrease by 1
			this->setLeftCount( this->getLeftCount() + 1 );
		}
		else
		{
			this->setRight( this->getRight().insert(x) );
		}
		return *this;
	}

	// count of all nodes on the left of the left-most child of this node's subtree
	TagType traversalCount;
};

// node accessor for tagged BTrees where tag is an integer holding total number of nodes in it's left subtree
template<class Tree>
struct RBNodeRefT : public NodeRefT<Tree>
{
	typedef typename Tree::NodePtr NodePtr;
	typedef typename Tree::ValueType ValueType;
	typedef typename Tree::TagType TagType;
	static constexpr NodePtr NullPtr = Tree::NullPtr;
	static constexpr bool Black=0;
	static constexpr bool Red=1;

	inline RBNodeRefT(Tree& t, NodePtr s, NodePtr p, NodePtr gp)
		: NodeRefT<Tree>(t,s,p,gp)
		{}

	inline RBNodeRefT(Tree& t)
		: NodeRefT<Tree>(t,this->tree.addNode(),NullPtr,NullPtr)
		{}

	inline RBNodeRefT getLeft () const
	{
		return RBNodeRefT( this->tree , this->tree.getLeft(this->self), this->self, this->parent );
	}

	inline RBNodeRefT getRight() const
	{
		return RBNodeRefT( this->tree , this->tree.getRight(this->self), this->self, this->parent );
	}

	inline TagType getColor() const
	{
		return this->tree.getTag(this->self);
	}

	inline const char* getColorString() const
	{
		return ( this->tree.getTag(this->self) == Black ) ? "Black" : "Red";
	}

	inline void setColor(TagType n) const
	{
		return this->tree.setTag(this->self, n);
	}

	inline void setLeft (RBNodeRefT x) 	 { this->tree.setLeft(this->self,x.self); }
	inline void setRight(RBNodeRefT x) 	 { this->tree.setRight(this->self,x.self); }
	inline void setValue(const ValueType& x) { this->tree.setValue(this->self,x); }

	// returns sub-tree size increase
	inline RBNodeRefT insert(const ValueType& x)
	{
		if( this->self == NullPtr )
		{
			this->self = this->tree.addNode();
			this->setValue( x );
			this->setColor( Black );
		}
		else if( this->tree.compare( x, this->getValue() ) )
		{
			this->setLeft( this->getLeft().insert(x) );
			// we have inserted one node somwhere inside the left sub-tree, thus left count inrease by 1
			this->setColor( Red );
		}
		else
		{
			this->setRight( this->getRight().insert(x) );
		}
		return *this;
	}

};


template<class BTreeBase, class _ValueCompare, template<typename> class _BTreeNodeRef=NodeRefT>
struct BTree : public BTreeBase
{
	typedef _ValueCompare ValueCompare;
	typedef typename BTreeBase::NodePtr NodePtr;
	typedef typename BTreeBase::ValueType ValueType;
	typedef _BTreeNodeRef<BTree> NodeRef;
	static constexpr NodePtr NullPtr = BTreeBase::NullPtr;

	inline BTree( const ValueCompare& comp = ValueCompare() ) : root(NullPtr), compare(comp) {}

	inline NodeRef getRoot()
	{
		return NodeRef(*this,this->root,NullPtr,NullPtr);
	}
	inline void setRoot(NodeRef n)
	{
		this->root = n.self;
	}

	inline void insert(const ValueType& x)
	{
		setRoot( getRoot().insert(x) );	
	}

	NodePtr root;
	ValueCompare compare;
};

template<class NodeRef>
void probe_btree_depth(NodeRef node, unsigned int depth, unsigned int& minDepth, unsigned int &maxDepth)
{
	if( node.self == NodeRef::NullPtr )
	{
		if( depth < minDepth ) minDepth = depth;
		if( depth > maxDepth ) maxDepth = depth;
		return;
	}
	probe_btree_depth( node.getLeft(), depth+1, minDepth, maxDepth );
	probe_btree_depth( node.getRight(), depth+1, minDepth, maxDepth );
}

template<class NodeRef>
void probe_btree_depth(NodeRef node, unsigned int& minDepth, unsigned int &maxDepth)
{
	minDepth = -1;
	maxDepth = 0;
	probe_btree_depth( node, 0, minDepth, maxDepth );
}

} } // end of namespace



//==============================================
//============= UNIT TEST ======================
//==============================================
#endif // end of file
