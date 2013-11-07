#ifndef __aabb_h
#define __aabb_h

// AABB
template <int _NDim, typename _CoordType>
struct AxisAlignedBoundingBox
{
        enum { NDim = _NDim };
        typedef _CoordType CoordType;

	inline
	AxisAlignedBoundingBox<NDim,CoordType> split( int axis, double pcoord, bool side ) const
	{
		AxisAlignedBoundingBox<NDim,CoordType> aabb( *this );
		CoordType middle = aabb.min[axis] + ( aabb.max[axis] - aabb.min[axis] ) * pcoord;
		if( side ) aabb.min[axis] = middle;
		else aabb.max[axis] = middle;
		return aabb;
	}

	CoordType min[NDim], max[NDim];
};

#endif // __aabb_h


#endif

