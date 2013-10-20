#ifndef __kdtreehalfsplit_h
#define __kdtreehalfsplit_h

template<int _NDim>
KdTreeHalfSplitNodeVector
{
        enum
	{
		NDim = _NDim,
		NSymbols = NDim+1, // we need to know if we're a leaf
		BitsPerSymbol = NextPowOfTwoLog<BitsPerSymbol>::value
	};

	inline const KdTreeNode getNode( size_t i ) const
	{
		return KdTreeNode( data.get(i) , 0.5 );
	}

	inline void splitNode( size_t i , int axis, double t)
	{
		// t is ignored, always 0.5
		data.set(i,axis); // axis wax tagged as a leaf, and is now an intermediate node
		addNode( NDim, 0.0 ); // add 2 leaves
		addNode( NDim, 0.0 );
	}

	inline size_t addNode( int axis, double t )
	{
		data.push_back(axis);
		return data.size()-1;
	}

private:
	NBitsVector<BitsPerSymbol> data; 
};

#endif // __kdtreehalfsplit_h

