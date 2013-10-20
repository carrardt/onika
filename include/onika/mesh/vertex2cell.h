#ifndef __onika_mesh_vertex2cell_h
#define __onika_mesh_vertex2cell_h

#include <algorithm>

#include "onika/debug/dbgassert.h"
#include "onika/it_type.h"
#include "onika/container/algorithm.h"

/*
 	vertex2cell indeed stands for 2 way associativity between cells and vertices
*/

namespace onika { namespace mesh {

	/* given a random access integer container, stores and maintain vertex to cell associations
	*/
	template<typename V2CList, typename IdType>
	struct V2CIterator
	{
		inline V2CIterator(const V2CIterator& it) : v2c(it.v2c), p(it.p) {}
		inline V2CIterator(V2CList& _m, IdType _p, IdType _c=0) : v2c(_m)
		{
			p = _p;
			while( _c!=0 && !atEnd() ) { ++(*this); --_c; }
		}

		inline auto value() -> decltype( (*(V2CList*)0)[0] )
		{
			return v2c[p];
		}
		inline auto operator * () -> decltype( (*(V2CList*)0)[0] )
		{
			return value();
		}

		inline bool operator == (const V2CIterator& v2c)
		{
			return ( atEnd() && v2c.atEnd() ) || (p == v2c.p) ;
		}

		inline bool toNextP()
		{
			if( !atEnd() )
			{
				if( v2c[p] < 0 ) p = -v2c[p] -1;
				else { ++p; }
			}
			else { return false; }
		}

		inline void forward()
		{
			while( !atEnd() && v2c[p] < 0 ) // jump to next sequence
			{
				p = -v2c[p] -1;
			}
		}
		inline bool toNextValue()
		{
			if( !atEnd() )
			{
				if( value()>=0 ) ++p;
				forward();
			}
			return !atEnd();
		}

		inline V2CIterator& operator ++ () // iterates overs valid (>=0) values
		{
			toNextValue();
			return *this;
		}

		inline bool availableSpace()
		{
			return ( v2c[p] == (-p-2) );
		}
		inline bool atEnd()
		{
			return ( v2c[p] < 0 ) && ( (-v2c[p]-1) >= v2c.size() );
		}
		inline bool toNextFree()
		{
			while( !atEnd() && !availableSpace() )
			{
				if( v2c[p]>=0 ) { ++p; }
				else { p = -v2c[p]-1; }
			}
			return !atEnd();
		}
		inline bool insertNext(IdType x)
		{
			if( toNextFree() ) { *(*this) = x; return true; }
			else { return false; }
		}
		inline bool toNextOccurance(IdType x)
		{
			bool found = false;
			while( !atEnd() && !found )
			{
				found = ( (*(*this)) == x );
				if( !found) { ++(*this); }
			}
			return found;
		}
		inline bool erase()
		{
			if( atEnd() ) return false;
			if( *(*this)>=0 || availableSpace() )
			{
				*(*this) = -p-2;
				return true;
			}
			else return false;
		}
		inline bool eraseNextOccurance(IdType x)
		{
			if( toNextOccurance(x) ) { return erase(); }
			else return false;
		}

		V2CList& v2c;
		IdType p;
	};

	template<typename V2CList, typename IdType>
	inline V2CIterator<V2CList,IdType> v2c_iterator(V2CList& _v2c, IdType _p, IdType _c=0)
	{ return V2CIterator<V2CList,IdType>(_v2c,_p,_c); }


/*
	2 way cell-vertex associativity implemented from cell-to-vertex-only associativity.
*/
template <class _C2V, class _V2CList, class _V2CIndexVector>
struct ReverseC2V : public _C2V
{
	typedef _C2V C2V;
	typedef typename C2V::ContainerType C2VData;
	typedef typename C2V::IdType IdType;
	typedef _V2CList V2CList;
	typedef _V2CIndexVector V2CIndexVector;
	typedef V2CIterator<V2CList,IdType> v2c_iterator_type;

	// =================================================================================
	// ============================ Constructor ========================================
	// =================================================================================
	inline ReverseC2V( C2VData& c2vdata, IdType nVerts )
		: C2V(c2vdata)
	{
		buildV2C(nVerts);
	}


	// =================================================================================
	// ============================ basic read access ==================================
	// =================================================================================
	static constexpr IdType MAX_ADJACENT_CELLS = 64;
	inline IdType getNumberOfVertices() const { return v2cIndex.size(); }


	// =================================================================================
	// ============================ getVertexAdjacentVertices ==========================
	// =================================================================================
	template<typename Iterator>
	inline typename it_type<Iterator>::T getVertexAdjacentVertices(IdType centerVertexId, Iterator clusterBegin, IdType& nVerts )
	{
		typename it_type<Iterator>::T clusterEnd = clusterBegin;
		nVerts = 0;
		for(v2c_iterator_type it=vertexAdjacentCellBegin(centerVertexId);!it.atEnd();++it)
		{
			IdType idloc = *it;;
			IdType cellId = this->getCellFromVertexIdLocation(idloc);
			IdType inCellIndex = idloc - this->getCellVertexIdLocation(cellId,0); // index of central vertex in cell's vertex list
			IdType ncellverts = this->getCellNumberOfVertices(cellId); 
			for(IdType cv=0;cv<ncellverts;cv++) if(cv!=inCellIndex) // for each vertex not being the central one
			{
				IdType vertexId = this->getCellVertexId(cellId,cv);
				if( std::find(clusterBegin,clusterEnd,vertexId)==clusterEnd )
				{
					*(clusterEnd++) = vertexId;
					++ nVerts;
				}
			}
		}
		return clusterEnd;
	}

	// =================================================================================
	// ============================ getVertexAdjacentCells =============================
	// =================================================================================
	template<typename Iterator>
	inline typename it_type<Iterator>::T getVertexAdjacentCells(IdType centerVertexId, Iterator clusterBegin, IdType& nConnexCells )
	{
		typename it_type<Iterator>::T clusterEnd = clusterBegin;
		nConnexCells = 0;
		for(v2c_iterator_type it=vertexAdjacentCellBegin(centerVertexId);!it.atEnd();++it)
		{
			IdType idloc = *it;
			*(clusterEnd++) = this->getCellFromVertexIdLocation(idloc);
			++nConnexCells;
		}
		return clusterEnd;
	}

	// =================================================================================
	// ============================ getEdgeAdjacentCells ===============================
	// =================================================================================
	template<typename Iterator>
	inline typename it_type<Iterator>::T getEdgeAdjacentCells( IdType a, IdType b, Iterator out_begin,IdType& n)
	{
		IdType cellSetA[MAX_ADJACENT_CELLS];
		IdType cellSetB[MAX_ADJACENT_CELLS];
		IdType na=0, nb=0;
		this->getVertexAdjacentCells(a,cellSetA,na);
		this->getVertexAdjacentCells(b,cellSetB,nb);
		std::sort(cellSetA,cellSetA+na);
		std::sort(cellSetB,cellSetB+nb);
		IdType result[MAX_ADJACENT_CELLS];
		n = std::set_intersection(cellSetA,cellSetA+na,cellSetB,cellSetB+nb,result) - result;
		return std::copy(result,result+n,out_begin);
	}

	// =================================================================================
	// ============================ getEdgeAdjacentVertices ============================
	// =================================================================================
	template<typename Iterator>
	inline typename it_type<Iterator>::T getEdgeAdjacentVertices( IdType a, IdType b, Iterator orbitBegin, IdType& nVerts)
	{
		typename it_type<Iterator>::T orbitEnd = orbitBegin;
		IdType cellIds[MAX_ADJACENT_CELLS],ncells=0;
		this->getEdgeAdjacentCells(a,b,cellIds,ncells);
		nVerts = 0;
		for(IdType i=0;i<ncells;i++)
		{
			IdType cellId = cellIds[i];
			IdType ncellverts = this->getCellNumberOfVertices(cellId); 
			for(IdType cv=0;cv<ncellverts;cv++) 
			{
				IdType v = this->getCellVertexId(cellId,cv);
				if( v!=a && v!=b && std::find(orbitBegin,orbitEnd,v)==orbitEnd )
				{
					*(orbitEnd++) = v;
					++nVerts;
				}
			}
		
		}
		return orbitEnd;
	}

	// =================================================================================
	// ============================ swapVertices =======================================
	// =================================================================================
	inline void swapVertices(IdType a, IdType b)
	{
		for(v2c_iterator_type it=vertexAdjacentCellBegin(a);!it.atEnd();++it)
		{
			IdType vloc = (*it);
			IdType c = this->getCellFromVertexIdLocation(vloc);
			IdType v = vloc - this->getCellVertexIdLocation(c,0);
			this->setCellVertexId(c,v,b);
		}
		for(v2c_iterator_type it=vertexAdjacentCellBegin(b);!it.atEnd();++it)
		{
			IdType vloc = *it;
			IdType c = this->getCellFromVertexIdLocation(vloc);
			IdType v = vloc - this->getCellVertexIdLocation(c,0);
			this->setCellVertexId(c,v,a);
		}
		std::swap(v2cIndex[a],v2cIndex[b]);
	}

	// =================================================================================
	// ============================ moveVertexTo =======================================
	// =================================================================================
	// === move vertex a to position b, overwriting b
	inline void moveVertexTo(IdType a, IdType b)
	{
		for(v2c_iterator_type it=vertexAdjacentCellBegin(a);!it.atEnd();++it)
		{
			IdType vloc = (*it);
			IdType c = this->getCellFromVertexIdLocation(vloc);
			IdType v = vloc - this->getCellVertexIdLocation(c,0);
			this->setCellVertexId(c,v,b);
		}
		v2cIndex[b] = v2cIndex[a];
	}

	// =================================================================================
	// ============================ eraseVertex ========================================
	// =================================================================================
	inline void eraseVertex(IdType i)
	{
		v2cIndex[i] = v2cList.size()+1;
	}

	// =================================================================================
	// ============================ swapCells ==========================================
	// =================================================================================
	inline void swapCells(IdType a, IdType b)
	{
		if( b < a ) { std::swap(a,b); }
		if( a == b ) return;
		removeReferencesToCell(a);
		removeReferencesToCell(b);
		this->C2V::swapCells(a,b);
		addReferencesToCell(a);
		addReferencesToCell(b);
	}

	// =================================================================================
	// ============================ moveCellTo =======================================
	// =================================================================================
	// === move cell a to position b, overwriting b
	inline void moveCellTo(IdType a, IdType b)
	{
		removeReferencesToCell(a);
		this->C2V::moveCellTo(a,b);
		addReferencesToCell(b);
	}

	// =================================================================================
	// ============================ removeLastVertex ===================================
	// =================================================================================
	// === move cell a to position b, overwriting b
	inline void removeLastVertex()
	{
		v2cIndex.pop_back();
	}

	// =================================================================================
	// ============================ checkConsistency ===================================
	// =================================================================================
	inline bool checkConsistency() // const
	{
#ifdef _DEBUG
#define CHECK_PREDICATE(p) debug::dbgassert(p)
#else
#define CHECK_PREDICATE(p) if(!(p)) return false
#endif
		IdType nverts = this->getNumberOfVertices();
		CHECK_PREDICATE(C2V::checkConsistency(nverts));

		IdType nCells = this->getNumberOfCells();
		IdType v2cListSize = v2cList.size();
		IdType nCellVertices = this->getTotalNumberOfCellVertices();
		CHECK_PREDICATE( v2cList.size() >= nCellVertices );
		CHECK_PREDICATE( v2cIndex.size() == nverts );
		for(IdType v=0;v<nverts;v++)
		{
			for(v2c_iterator_type it=vertexAdjacentCellBegin(v);!it.atEnd();++it)
			{
				IdType vloc = *it; 
				CHECK_PREDICATE( vloc>=0 && vloc<nCellVertices );
				IdType ac = this->getCellFromVertexIdLocation(vloc);
				IdType cv = vloc - this->getCellVertexIdLocation(ac,0);
				CHECK_PREDICATE( this->getCellVertexId(ac,cv) == v );
			}
		}
		// check that all vertices back reference their containing cells
		for(IdType c=0;c<nCells;c++)
		{
			IdType nAdjVertices = this->getCellNumberOfVertices(c);
			for(IdType cv=0;cv<nAdjVertices;cv++)
			{
				IdType v = this->getCellVertexId(c,cv);
				bool foundContainingCell = false;
				for(v2c_iterator_type it=vertexAdjacentCellBegin(v);!it.atEnd();++it)
				{
					IdType vloc = *it;
					IdType cellId = this->getCellFromVertexIdLocation(vloc);
					foundContainingCell |= ( cellId == c );
				}
				CHECK_PREDICATE( foundContainingCell );
			}
		}
#undef CHECK_PREDICATE
		return true;
	}

	// =================================================================================
	// ============================ getMemoryBytes =====================================
	// =================================================================================
	inline size_t getMemoryBytes() const
	{
		return C2V::getMemoryBytes()
		     + container::memory_bytes(v2cIndex)
		     + container::memory_bytes(v2cList)
		     + 2 * sizeof(IdType);
	}

        // =================================================================================
        // ============================ getAdjacentCellBegin ===============================
        // =================================================================================	
       	inline v2c_iterator_type vertexAdjacentCellBegin(IdType v)
	{
		v2c_iterator_type it(v2cList,v2cIndex[v]);
		it.forward(); // advance to the first relevant value
		return it;
	}

        // =================================================================================
        // ============================ getAdjacentCellEnd =================================
        // ================================================================================= 
       	inline v2c_iterator_type vertexAdjacentCellEnd(IdType v)
	{
		return v2c_iterator_type(v2cList,v2cIndex[v],-1);
	}

        // =================================================================================
        // ============================ addVertexAdjacentCell ==============================
        // ================================================================================= 
	inline bool addVertexAdjacentCell(IdType v, IdType vertexIdLocation)
	{
		return v2c_iterator_type(v2cList,v2cIndex[v]).insertNext(vertexIdLocation);
	}

	inline bool removeVertexAdjacentCell(IdType v, IdType vertexIdLocation)
	{
		return v2c_iterator_type(v2cList,v2cIndex[v]).eraseNextOccurance(vertexIdLocation);
	}

	// =================================================================================
        // ============================ vertexAdjacentCellCount ============================
        // =================================================================================	
       	inline IdType vertexAdjacentCellCount(IdType v)
	{
		v2c_iterator_type it(v2cList,v2cIndex[v]);
		it.forward(); // advance to the first relevant value
		IdType count = 0;
		while( !it.atEnd() ) { ++count; ++it; }
		return count;
	}

	// =================================================================================
        // ============================ vertexAdjacentCellFreeSpace ========================
        // =================================================================================	
       	inline IdType vertexAdjacentCellFreeSpace(IdType v)
	{
		v2c_iterator_type it(v2cList,v2cIndex[v]);
		IdType count = 0;
		it.toNextFree();
		while( !it.atEnd() )
		{
			if(it.availableSpace()) { ++count; }
			it.toNextP();
		}
		return count;
	}

	// =================================================================================
        // ============================ vertexAdjacentCellClear ============================
        // =================================================================================	
       	inline void vertexAdjacentCellClear(IdType v)
	{
		v2c_iterator_type it = vertexAdjacentCellBegin(v);
		while( !it.atEnd() )
		{
			it.erase();
			++it;
		}
	}

	// =================================================================================
        // ============================ vertexAdjacentCellChain ============================
        // =================================================================================	
       	inline void vertexAdjacentCellChain(IdType a, IdType b)
	{
		* vertexAdjacentCellEnd(a) = -v2cIndex[b]-1;
	}

        // =================================================================================
        // ============================ removeReferencesToCell =============================
        // =================================================================================
	inline void removeReferencesToCell(IdType cellId)
	{
		IdType nRem=0;
		IdType nverts = this->getCellNumberOfVertices(cellId);
		for(IdType i=0;i<nverts;i++)
		{
			IdType vloc = this->getCellVertexIdLocation(cellId,i);
			IdType v = this->getCellVertexId(cellId,i);
			if( removeVertexAdjacentCell(v,vloc) ) { ++nRem; }
		}
		debug::dbgassert(nRem==nverts);
	}

	// =================================================================================
	// ============================ addReferencesToCell ================================
	// =================================================================================
	inline void addReferencesToCell(IdType cellId)
	{
		IdType nverts = this->getCellNumberOfVertices(cellId);
		for(IdType i=0;i<nverts;i++)
		{
			IdType vloc = this->getCellVertexIdLocation(cellId,i);
			IdType v = this->getCellVertexId(cellId,i);
			addVertexAdjacentCell(v,vloc);
		}
	}

	// =================================================================================
	// ============================ buildV2C ===========================================
	// =================================================================================
	inline void buildV2C(IdType nverts)
	{
		IdType ncellverts = 0;
		IdType nCells = this->getNumberOfCells();
		V2CIndexVector v2cCount;
		v2cCount.resize(nverts);
		for(IdType i=0;i<nverts;i++)
		{
			v2cCount[i] = 0;
		}
		
		for(IdType c=0;c<nCells;c++)
		{
			IdType nv = this->getCellNumberOfVertices(c);
			for(IdType i=0;i<nv;i++)
			{
				++ v2cCount[ this->getCellVertexId(c,i) ];
				++ ncellverts;
			}
		}
		maxAdjacentCells = 0;
		for(IdType i=0;i<nverts;i++)
		{
			debug::dbgassert( v2cCount[i] < MAX_ADJACENT_CELLS );
			if(v2cCount[i]>maxAdjacentCells) { maxAdjacentCells=v2cCount[i]; }
		}
		v2cIndex.resize(nverts);
		IdType v2cSum = 0;
		for(IdType i=0;i<nverts;i++)
		{
			v2cIndex[i] = v2cSum;
			v2cSum += v2cCount[i]+1;
			v2cCount[i] = 0;
		}
		debug::dbgassert( v2cSum == (ncellverts+nverts) );
		v2cList.resize(ncellverts+nverts);
		for(IdType c=0;c<nCells;c++)
		{
			IdType nv = this->getCellNumberOfVertices(c);	
			for(IdType i=0;i<nv;i++)
			{
				IdType v = this->getCellVertexId(c,i);
				v2cList[ v2cIndex[v] + (v2cCount[v]++) ] = this->getCellVertexIdLocation(c,i);
			}
		}
		IdType maxId = v2cList.size();
		for(IdType i=0;i<nverts;i++)
		{
			v2cList[v2cIndex[i]+v2cCount[i]] = -maxId-1;
		}
	}

	V2CIndexVector v2cIndex;
	V2CList v2cList;
	IdType maxAdjacentCells;
};

} } // end of namespace



// ==========================================================
// =================== Unit Test ============================
// ==========================================================

#ifdef onika_mesh_vertex2cell_TEST
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "onika/vtk/readvtkascii.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/simplicialmesh.h"

struct Mesh
{
	typedef struct { double x[3]; } VertexPos;

	inline Mesh() : nverts(0) {}

	inline void addVertex(const VertexPos& p) { nverts++; }
	
	template<class VertIdIterator>
	inline void addCell(int np, VertIdIterator it)
	{
		if( np != 4 ) return;
		for(int i=0;i<np; ++i, ++it) cells.push_back( *it );
	}

	std::vector<int> cells;
	int nverts;
};

typedef onika::mesh::smesh_c2v_basic_traits< std::vector<int>, 3 > MyC2VBasicTraits;
typedef onika::mesh::c2v_traits< MyC2VBasicTraits > MyC2VTraits;
typedef onika::mesh::C2VWrapper<MyC2VTraits> MyC2VWrapper;
typedef onika::mesh::ReverseC2V<MyC2VWrapper, std::vector<int>, std::vector<unsigned int> > V2C;

int main()
{
	std::string fname;
	std::cout<<"File ? "; std::cout.flush();
	std::cin>>fname;
	std::ifstream ifile(fname.c_str());
	if( !ifile )
	{
		std::cerr<<"Failed to open "<<fname<<"\n";
		return 1;
	}

	Mesh mesh;
	onika::vtk::readVtkAsciiMesh(ifile,mesh);
	V2C v2c( mesh.cells, mesh.nverts ); 
	//onika::debug::dbgassert( v2c.checkConsistency(mesh.nverts) );
	
	std::cout<<v2c.getNumberOfVertices()<<" vertices, "<< v2c.getNumberOfCells()<<" cells, mem="<<v2c.getMemoryBytes()<<"\n";

	int ca=0, cb=1;
	std::cout<<"Cell A ? "; std::cout.flush();
	std::cin>>ca;
	for(int i=0;i<v2c.getCellNumberOfVertices(ca);i++) std::cout<<v2c.getCellVertexId(ca,i)<<' ';
	std::cout<<"\nCell B ? "; std::cout.flush();
	std::cin>>cb;
	for(int i=0;i<v2c.getCellNumberOfVertices(cb);i++) std::cout<<v2c.getCellVertexId(cb,i)<<' ';
	std::cout<<"\n";
	v2c.swapCells( ca, cb);

	int v=0;
	std::cout<<"\nVertex A ? "; std::cout.flush();
	std::cin>>v;
	int adjacentCells[64], nAdj=0;
	v2c.getVertexAdjacentCells( v, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";
	int vb=0;
	std::cout<<"\nVertex B ? "; std::cout.flush();
	std::cin>>vb;
	v2c.getVertexAdjacentCells( vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	v2c.getEdgeAdjacentCells( v, vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" edge adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	onika::debug::dbgassert( v2c.checkConsistency() );

        std::cout<<std::endl<<"PASSED"<<std::endl;
        return 0;
}

#endif // end of unit test


#endif // end of file
