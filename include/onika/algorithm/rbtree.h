#ifndef __onika_algorithm_rbtree_h
#define __onika_algorithm_rbtree_h


namespace onika { namespace algorithm {

template<class _IntegerVector>
struct rbtree_intvec_traits
{
	typedef _IntegerVector Container;
	typedef typename IntegerVector::value_type PointerType;
	
	static constexpr PointerType NullPtr = static_cast<PointerType>(0) - 1;
	
	static inline PointerType getLeft(const Container& tree, PointerType node)
	{
		return NullPtr;
	}
	
	static inline PointerType getRight(const Container& tree, PointerType node)
	{
		return NullPtr;
	}

	static inline void setLeft(Container& tree, PointerType node, PointerType leftChild)
	{
	}

	static inline void setRight(const Container& tree, PointerType node)
	{
	}
};


template<class _traits>
struct RbTree
{
	typedef _traits traits;
	typedef traits::Container Container;
	typedef traits::PointerType PointerType;
	
	
	
	Container& tree;
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
