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

template<class _ContainerType>
struct TupleVectorPTree : public TupleVectorBTree<_ContainerType>
{
	typedef size_t NodePtr; // position of node description in container
	typedef typename std::tuple_element<3,typename _ContainerType::value_type>::type CountType;
	static constexpr unsigned int TupleSize = 4;

	inline CountType getCount(NodePtr i) { return std::get<3>(this->data[i]) ; }
	inline void setCount(NodePtr i, CountType x) { std::get<3>(this->data[i]) = x; }
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
		if( this->self == NullPtr )
		{
			this->self = this->tree.addNode();
			this->setValue( x );
		}
		else if( this->tree.compare( x, this->getValue() ) )
		{
			this->setLeft( this->getLeft().insert(x) );
		}
		else
		{
			this->setRight( this->getRight().insert(x) );
		}
		return *this;
	}
};

template<class Tree>
struct PNodeRefT : public NodeRefT<Tree>
{
	typedef typename Tree::NodePtr NodePtr;
	typedef typename Tree::ValueType ValueType;
	typedef typename Tree::CountType CountType;
	static constexpr NodePtr NullPtr = Tree::NullPtr;

	inline PNodeRefT(Tree& t, NodePtr s, NodePtr p, NodePtr gp, CountType tc=0)
		: NodeRefT<Tree>(t,s,p,gp)
		, traversalCount(tc)
		{}

	inline PNodeRefT(Tree& t, CountType tc=0)
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

	inline CountType getCount() const
	{
		return this->tree.getCount(this->self);
	}

	inline void setCount(CountType n) const
	{
		return this->tree.setCount(this->self, n);
	}

	inline CountType getPosition() const
	{
		return traversalCount + getCount();
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
			this->setCount( 0 );
		}
		else if( this->tree.compare( x, this->getValue() ) )
		{
			this->setLeft( this->getLeft().insert(x) );
			// we have inserted one node somwhere inside the left sub-tree, thus left count inrease by 1
			this->setCount( this->getCount() + 1 );
		}
		else
		{
			this->setRight( this->getRight().insert(x) );
		}
		return *this;
	}

	// count of all nodes on the left of the left-most child of this node's subtree
	CountType traversalCount; 	
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



} } // end of namespace



//==============================================
//============= UNIT TEST ======================
//==============================================
#ifdef onika_algorithm_rbtree_TEST

#include <iostream>
#include <vector>
#include <functional>   // std::less

// simple binary tree
typedef onika::algorithm::TupleVectorBTree< std::vector< std::tuple<double,size_t,size_t> > > SimpleTreeBase;
typedef onika::algorithm::BTree< SimpleTreeBase, std::less<double> > SimpleTree;
typedef typename SimpleTree::NodeRef NodeRef;

// positional binary tree
typedef onika::algorithm::TupleVectorPTree< std::vector< std::tuple<double,size_t,size_t,size_t> > > PTreeBase;
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
#endif // en of unit test

#endif // end of file
