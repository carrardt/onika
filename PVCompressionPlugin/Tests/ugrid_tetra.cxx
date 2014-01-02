#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h"
#include "onika/container/sequence.h"
#include "onika/debug/dbgassert.h"
#include "onika/language.h"

#include <vtkUnstructuredGrid.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkFloatArray.h>
#include <vtkDataArrayTemplate.h>

template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}
template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ConstElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}
template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}


using std::cout;
using namespace onika::vtk;
using onika::mesh::edge_length_op;
using onika::mesh::cell_shortest_edge_less;
using onika::mesh::make_smesh_c2e;
using onika::mesh::ordered_cell_set;
using onika::tuple::indices;
using onika::tuple::make_indices;
using onika::tuple::types;

template<
	class VT, int VS, class CT, class CS, class PT, class PS,
	bool Valid=((CT::count==CS::count)&&(PT::count==PS::count))
	>
struct UGridWrapper {};

template<class VT, int VS, class CT, class CS, class PT, class PS>
struct UGridWrapper<VT,VS,CT,CS,PT,PS,true>
{
	vtkUnstructuredGrid* ugrid;
	static constexpr int NCA = CT::count;
	static constexpr int NPA = PT::count;

	inline UGridWrapper(vtkDataObject* obj)
	{
		ugrid = vtkUnstructuredGrid::SafeDownCast(obj);
		onika::debug::dbgassert(ugrid != 0);
		onika::debug::dbgassert(allCellsAreTetras(ugrid));
	}

	template<class... T, int ... S, int... I>
	inline auto cellValues_aux( types<T...> arrayTypes, indices<S...> arrayTupleSizes, indices<I...> arrayIndices )
	ONIKA_AUTO_RET( zip_vectors_cpy( (DataSetAttribute<T,S,true,I>().wrap(ugrid))... ) )

	inline auto cellValues() ONIKA_AUTO_RET( cellValues_aux(CT(),CS(),make_indices<NCA>()) )

	template<class... T, int ... S, int... I>
	inline auto vertices_aux( types<T...> arrayTypes, indices<S...> arrayTupleSizes, indices<I...> arrayIndices )
	ONIKA_AUTO_RET( zip_vectors_cpy( UGridPoints<VT,VS>().wrap(ugrid), (DataSetAttribute<T,S,false,I>().wrap(ugrid))... ) )

	inline auto vertices() ONIKA_AUTO_RET( vertices_aux(PT(),PS(),make_indices<NPA>()) )
};

#define UGRID_DESC float,3,types<long,long,int>,indices<1,1,1>,types<long,long,double,double,double,double,double,double,double>,indices<1,1,1,3,1,1,1,1,1>

int main(int argc, char* argv[])
{
	if( argc<2 ) return 1;

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(argv[1]);
	reader->Update();
//	cout<<"Reader:\n";
//	reader->PrintSelf(cout,vtkIndent(0));

	vtkDataObject* data = reader->GetOutputDataObject(0);
	if( data == 0 ) return 1;
	cout<<"OutputDataObject:\n";
	data->PrintSelf(cout,vtkIndent(0));

	UGridWrapper<UGRID_DESC> wrapper( data );

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(data);
	if( ugrid == 0 ) return 1;

	if( ! allCellsAreTetras(ugrid) )
	{
		cout<<"Not a simplicial mesh\n";
		return 1;
	}

	// wrap cell values
	auto cellValues = wrapper.cellValues();
	auto vertices = wrapper.vertices();

	auto cells = UGridCells().wrap(ugrid);
	vtkIdType nverts = ugrid->GetNumberOfPoints();
	auto c2v = wrap_ugrid_smesh_c2v( cells, ONIKA_CONST(3) );
	auto v2c = make_v2c( c2v , nverts );
	onika::debug::dbgassert( v2c.checkConsistency() );
	int nCells = c2v.getNumberOfCells();
	cout<<nverts<<" vertices, "<<nCells <<" cells, mem="<<onika::container::memory_bytes(cells)<<"\n";

	// build edge length metric and shortest edge based cell ordering
	// when vertices have a complex type i.e. tuple of values, each of which can be a tuple,
	// edge_length_op takes the first element and compute the distance based on the first element only.
	// this is why it is important to place vertex coordinates first in the vertex definition
	auto edgeLength = edge_length_op(vertices);
	auto c2e = make_smesh_c2e(c2v);
	auto shortestEdgeOrder = cell_shortest_edge_less( c2e, edgeLength);
	auto orderedCells = ordered_cell_set(nCells, shortestEdgeOrder);
	auto edge = std::tuple<vtkIdType,vtkIdType>( 461, 467 );
	cout<<"edge "<<edge<<" length = "<<edgeLength(edge)<<"\n";

	auto maxEdgeLen = edgeLength(edge);

	for(auto i : orderedCells)
	{
		int ne = c2e.getCellNumberOfEdges(i);
//		cout<<"Cell "<<i<<" has "<<ne<<" edges";
		for(int e=0;e<ne;e++)
		{
			auto edge = c2e.getCellEdge(i,e);
			auto len = edgeLength(edge);
			if( len > maxEdgeLen ) maxEdgeLen = len;
			//cout<<"\t"<<edge<<" : len="<<edgeLength(edge)<<"\n";
		}
	}
	cout<< "Cell with shortest edge is "<< ( * orderedCells.begin() )<< "\n";
	cout<< "longest edge length = "<<maxEdgeLen<< "\n";

	return 0;
}
