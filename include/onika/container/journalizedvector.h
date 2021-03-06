#ifndef __onika_container_journalizedvector_h
#define __onika_container_journalizedvector_h

#include "onika/container/iterator.h"
#include "onika/container/sequence.h"
#include "onika/debug/dbgassert.h"
#include "onika/debug/dbgmessage.h"
#include "onika/language.h"

namespace onika { namespace container {

/*
 * Requirements :
 * 	1. base container has random access (iterator)
 *	2. index container can store large enough integers
 *	3. base container is not modified during JournalizedVector's life cycle
 * 
 *
 *
*/

template<typename _BaseContainer,
	 typename _WriteBuffer,
	 typename _IndexContainer>

struct JournalizedVector
{
	typedef _BaseContainer BaseContainer;
	typedef typename BaseContainer::value_type ElementType;
	typedef _IndexContainer IndexContainer;
	typedef _WriteBuffer WriteBuffer;
	typedef typename IndexContainer::value_type IndexType;

	// generic container API
	ONIKA_CONTAINER_ACCESS_DEF(JournalizedVector,ElementType);
	ONIKA_CONTAINER_PUSH_BACK_DEF(JournalizedVector,ElementType);
	typedef ONIKA_MAKE_SIGNED(value_type) signed_value_type;

	inline JournalizedVector(const BaseContainer& v) : m_vec(v), m_count(v.size())
	{
		SequenceIterator<IndexType> start = { 0 };
		SequenceIterator<IndexType> end = { m_count };
		m_indices.assign( start, end );
	}

	// equivalent to write c[i] = c[i]
	// index for ith element points to a new slot in the write buffer initialized with the same value
	inline void touch(IndexType i)
	{
		debug::dbgassert( i>=0 && i<m_indices.size() );
		IndexType j = m_indices[i];
		if( j < m_count )
		{
			m_indices[i] = m_count + m_buffer.size();
			m_buffer.push_back( m_vec[j] );
		}
	}

	// accessors
	inline void set(IndexType i, const ElementType& x)
	{
		debug::dbgassert( i>=0 && i<m_indices.size() );
		IndexType j = m_indices[i];
		if( j >= m_count ) // element is in write buffer
		{
			j -= m_count;
			m_buffer[j] = x;
		}
		else // element is in base container (read only), we move it to write buffer
		{
			m_indices[i] = m_count + m_buffer.size();
			m_buffer.push_back( x );
		}
	}

	inline ElementType get(IndexType i) const
	{
		return get_indexed(m_indices[i]);
	}
	inline ElementType get_indexed(IndexType j) const
	{
		if( j >= m_count ) // element is in write buffer
		{
			j -= m_count;
			return m_buffer[j];
		}
		else // element is in base container
		{
			return m_vec[j];
		}
	}

	//=========================================
	//============= size adjustments ==========
	//=========================================
	inline void clear()
	{
		m_indices.clear();
		m_buffer.clear();
	}

	inline size_t size() const
	{
		return m_indices.size();
	}

	inline void resize(size_t n)
	{
		if( n > size() )
		{
			m_buffer.resize( m_buffer.size() + n - size() );
		}
		size_t s = size();
		m_indices.resize(n);
		for(size_t i=s;i<n;++i) { m_indices[i]=i; }
	}

	inline void resize(size_t n, const value_type& x )
	{
		size_t s = size();
		size_t b = m_buffer.size();
		if( n > s )
		{
			m_buffer.resize( b + n-s , x );
		}
		m_indices.resize(n);
		for(size_t i=s;i<n;++i) { m_indices[i] = m_count+b + i-s; }
	}

	inline void erase(const iterator& it)
	{
		erase(it.i);
	}

	inline void erase(size_t i)
	{
		size_t last = size()-1;
		if( i != last )
		{
			std::swap(m_indices[i],m_indices[last]);
		}
		pop_back();
	}

	/*!
	 * compress write buffer.
	 * complexity = O( N.i + n.v ) : N=total number of indices, n=write buffer size, i=index rw op, v=value rw op
	 * extra memory = O(n)  
	 */
	inline void strip()
	{
		IndexType s = m_buffer.size();
		IndexType ts = m_indices.size();
		IndexType nil = m_count + s; // non negative junk value
		m_indices.resize( ts + s , nil ); // allocate extra memory needed
#define         FLAGS(i) m_indices[ts+(i)]		
		for(IndexType i=0;i<ts;++i)
		{
			if( m_indices[i] >= m_count )
			{
				FLAGS(m_indices[i]-m_count) = i;
			}
		}
		IndexType n = 0;
		for(IndexType i=0;i<s;++i)
		{
			IndexType ei = FLAGS(i);
			if( ei != nil )
			{
				m_indices[ ei ] = m_count+n;
				m_buffer[n] = m_buffer[i];
				++n;
			}
		}
		m_indices.resize(ts); // restore to its normal size
		m_buffer.resize(n); 
	}
#undef  FLAGS

	// member data
	const BaseContainer& m_vec;
	IndexType m_count;
	IndexContainer m_indices;
	WriteBuffer m_buffer;
};


} } // end of namespace onika::container


//==========================================
//=== container algorithm specialization ===
//==========================================
#include "onika/container/algorithm.h"
namespace onika { namespace container {

	template<typename _B,typename _W,typename _I> inline auto 
	min_value(const JournalizedVector<_B,_W,_I>& c) 
	ONIKA_AUTO_RET( std::min( min_value(c.m_vec) , min_value(c.m_buffer) ) )

	template<typename _B,typename _W,typename _I> inline auto 
	max_value(const JournalizedVector<_B,_W,_I>& c) 
	ONIKA_AUTO_RET( std::max( max_value(c.m_vec) , max_value(c.m_buffer) ) )

	template<typename _B,typename _W,typename _I> inline auto 
	memory_bytes(const JournalizedVector<_B,_W,_I>& c) -> size_t
	{
		return 	  sizeof(typename JournalizedVector<_B,_W,_I>::BaseContainer *)
			+ sizeof(typename JournalizedVector<_B,_W,_I>::IndexType)
			+ memory_bytes( c.m_indices )
			+ memory_bytes( c.m_buffer ) ;
	}

} }


//==========================================
//=== STL algorithm specialization       ===
//==========================================
#include <algorithm>
namespace std
{
	// std::swap
	template<typename B,typename W,typename I>
	inline void swap(
		onika::container::ElementAccessorT< onika::container::JournalizedVector<B,W,I> > accA,
		onika::container::ElementAccessorT< onika::container::JournalizedVector<B,W,I> > accB )
	{
		onika::container::JournalizedVector<B,W,I> & jvec = accA.v;
		onika::debug::dbgassert( ( & jvec ) == ( & accB.v ) );

		onika::debug::dbgmessage()<<"JVec swap\n";
		std::swap( jvec.m_indices[accA.i] , jvec.m_indices[accB.i] );
	}

	template<typename B,typename W,typename I>
	inline void iter_swap(
		onika::container::IteratorT< onika::container::JournalizedVector<B,W,I> > itA,
		onika::container::IteratorT< onika::container::JournalizedVector<B,W,I> > itB )
	{
		onika::container::JournalizedVector<B,W,I> & jvec = itA.vec;
		onika::debug::dbgassert( ( & jvec ) == ( & itB.vec ) );

		onika::debug::dbgmessage()<<"JVec iter_swap\n";
		std::swap( jvec.m_indices[itA.i] , jvec.m_indices[itB.i] );
	}

#define ONIKA_JVEC_STL_SORT_ADAPTOR(_F) \
	template<typename B,typename W,typename I> \
	inline void _F( \
		onika::container::IteratorT< onika::container::JournalizedVector<B,W,I> > itA, \
		onika::container::IteratorT< onika::container::JournalizedVector<B,W,I> > itB ) \
	{ \
		onika::container::JournalizedVector<B,W,I> & jvec = itA.vec; \
		onika::debug::dbgassert( ( & jvec ) == ( & itB.vec ) ); \
		size_t ai = itA.i; \
		size_t bi = itB.i; \
		std::_F(jvec.m_indices.begin()+ai , \
			jvec.m_indices.begin()+bi , \
			[jvec](size_t a, size_t b) -> bool { return jvec.get_indexed(a) < jvec.get_indexed(b); } ); \
	} \
	\
	template<typename B,typename W,typename I,typename CompF> \
	inline void _F( \
		onika::container::IteratorT< onika::container::JournalizedVector<B,W,I> > itA, \
		onika::container::IteratorT< onika::container::JournalizedVector<B,W,I> > itB, \
		CompF comp  ) \
	{ \
		onika::container::JournalizedVector<B,W,I> & jvec = itA.vec; \
		onika::debug::dbgassert( ( & jvec ) == ( & itB.vec ) ); \
		size_t ai = itA.i; \
		size_t bi = itB.i; \
		std::_F(jvec.m_indices.begin()+ai , \
			jvec.m_indices.begin()+bi , \
			[jvec,comp](size_t a, size_t b) -> bool { return comp( jvec.get_indexed(a) , jvec.get_indexed(b) ); } ); \
	}

	ONIKA_JVEC_STL_SORT_ADAPTOR(sort)
	ONIKA_JVEC_STL_SORT_ADAPTOR(push_heap)
	ONIKA_JVEC_STL_SORT_ADAPTOR(pop_heap)
	ONIKA_JVEC_STL_SORT_ADAPTOR(make_heap)
	ONIKA_JVEC_STL_SORT_ADAPTOR(sort_heap)

#undef ONIKA_JVEC_STL_SORT_ADAPTOR

} // end of namespace std

#undef DECL_AUTO_RET


// ==========================================================
// ======================== UNIT TEST =======================
// ==========================================================
#endif // end of journalizedvector.h

