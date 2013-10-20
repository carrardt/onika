#ifndef __KdTree_h
#define __KdTree_h

// AABB
template <int _NDim, typename _CoordType>
struct AxisAlignedBoundingBox
{
        enum { NDim = _NDim };
        typedef _CoordType CoordType;
	CoordType x[NDim];
};

template<int nbits>
struct BasicTypeBitContainer
{
	
	unsigned long bits;
}

template<int _MaxDepth=56>
struct BTreeParser
{
	enum SearchMode
	{
		PREFIX,
		INFIX,
		POSTFIX
	};

	enum { DepthHardLimit=254 };
	enum { MaxDepth=((_MaxDepth+7)/8)*8 };
	// assert MaxDepth < DepthHardLimit

	unsigned char stack[MaxDepth/8];
	unsigned char sp;

	inline BTreeParserInterface() : sp(0) {}
	inline unsigned int depth() const
	{
		return sp;
	}

	inline bool getStack(unsigned int d) const
	{
		int i=d/8;
		int j=d%8;
		return ( (stack[i]>>j) & 1 ) ;
	}

	inline void setStack(unsigned int d, bool tf)
	{
		int i=d/8;
		int j=d%8;
		unsigned char mask = (tf?1:0) << j;
		stack[i] &= ~mask;
		stack[i] |= mask;
	}
	inline void getState() { return  }

	template<typename BTreeCursor>
	inline bool toNextNode( SearchMode sm, BTreeCursor& bt )
	{
		bool state = getStack(sp);
		bool direction = getStack(sp+1);

		switch(sm)
		{
			case PREFIX:
			  if( direction == DOWN ) { bt.process(); direction=DOWN; }
			  if( direction == UP )
			  {
				if( state == 0 ) { state=1; direction=DOWN; }
				// else { DIRECTION=UP; } // go on upward
			  }
			  break;
			case INFIX: // inversion of left/right is delegated to bt implementation
				if( state == 0 )
				break;
			case POSTFIX:
				if( state == 0 )
				break;
		}

		// apply traversal step according to direction and state
		if( direction == DOWN )
		{
			if( bt.isLeaf() ) { setStack(sp+1,UP); };
			else
			{	// recusively going down
				setStack(sp,state);
				++sp;
				setStack(sp,0); setStack(sp+1,DOWN);
				return true;
			}
		}
		else // if( direction == UP )
		{
			if( bt.isRoot() ) { return false; }
			else
			{
				--sp;
				setStack(sp+1,UP);
				return true;
			}
		}
	}

};

// KdTree
template<int _NDim, typename _CoordType>
struct KdTreeParserInterface
{
        enum { NDim = _NDim };
        typedef _CoordType CoordType;
	typedef AxisAlignedBoundingBox<_NDim,_CoordType> AABB;

	// start traversal
	virtual void initialize(SearchMode searchMode, Vec<NDim,bool> searchDirection, AABB initBB )=0;

	// go to next
	virtual void next();

	// in-traversal informations
	virtual bool isLeaf()=0;
	virtual int depth()=0;
	virtual AABB bounds()=0;

	// tree modification operators
	// non, car très dépendant du type de KdTree
	// oui, si le KdTree possède une version sans paramètres, qui effectue un découpage automatique
	// par example a partir d'un delegué qui gere la stratégie de decoupage
	virtual void split()=0;
};

template <int _NDim>
struct KdTreeHalfSplitAlternateDirection
{
        enum { NDim = _NDim };
};

#endif

