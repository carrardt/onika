#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/debug/dbgassert.h"

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

using namespace onika::vtk;
using onika::tuple::map_indexed;
using std::cout;

struct PrefixTupleElementsOp
{
	template<class T>
	inline auto operator () ( int i, const T& x ) const ONIKA_AUTO_RET( std::make_tuple(i,x) )
};
struct TupleIndicesOp
{
	template<class T>
	inline auto operator () ( int i, const T& x ) const ONIKA_AUTO_RET( i )
};
static const PrefixTupleElementsOp  prefix_elements;
static const TupleIndicesOp tuple_indices;
int main(int argc, char* argv[])
{
	if( argc<2 ) return 1;

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	cout<<"Reader:\n";
	reader->PrintSelf(cout,vtkIndent(0));

	vtkDataObject* data = reader->GetOutputDataObject(0);
	if( data == 0 ) return 1;
	cout<<"OutputDataObject:\n";
	data->PrintSelf(cout,vtkIndent(0));

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast(data);
	if( ugrid == 0 ) return 1;

	if( ! allCellsAreTetras(ugrid) )
	{
		cout<<"Not a simplicial mesh\n";
		return 1;
	}

	auto cells = UGridCells().wrap(ugrid);

	vtkIdType nverts = ugrid->GetNumberOfPoints();
	auto c2v = wrap_ugrid_smesh_c2v( cells, ONIKA_CONST(3) );
	onika::debug::dbgassert( c2v.checkConsistency(nverts) );

	cout<<nverts<<" vertices, "<< c2v.getNumberOfCells()<<" cells, mem="<<onika::container::memory_bytes(cells)<<"\n";

	auto cellValues = zip_array_wrappers(ugrid
			,DataSetAttribute<long,1,true>("GlobalElementId")
			,DataSetAttribute<long,1,true>("PedigreeElementId")
			,DataSetAttribute<int,1,true>("ObjectId")
			);

//	ugrid->GetPointData()->GetArray("Temp")->PrintSelf(cout,vtkIndent(0));
	auto vertices = zip_array_wrappers(ugrid
			, UGridPoints<float,3>()
			, DataSetAttribute<long,1,false>("GlobalNodeId")
			, DataSetAttribute<long,1,false>("PedigreeNodeId")
			, DataSetAttribute<double,1,false>("Temp")
			, DataSetAttribute<double,3,false>("V")
			, DataSetAttribute<double,1,false>("Pres")
			, DataSetAttribute<double,1,false>("AsH3")
			, DataSetAttribute<double,1,false>("GaMe3")
			, DataSetAttribute<double,1,false>("CH4")
			, DataSetAttribute<double,1,false>("H2")
			);
	cout<<ugrid->GetPointData()->GetArray("H2")->GetClassName()<<"\n";

	for(int i=0;i<3;i++)
	{
		cout<<"Vertex "<<i<<"\n";
		cout<<"\t.0 "<<std::get<0>(vertices[i].get()) <<"\n";
		cout<<"\t.1 "<< std::get<1>(vertices[i].get()) <<"\n";
		cout<<"\t.2 "<< std::get<2>(vertices[i].get()) <<"\n";
		cout<<"\t.3 "<< std::get<3>(vertices[i].get()) <<"\n";
		cout<<"\t.4 "<< std::get<4>(vertices[i].get()) <<"\n";
		cout<<"\t.5 "<< std::get<5>(vertices[i].get()) <<"\n";
		cout<<"\t.6 "<< std::get<6>(vertices[i].get()) <<"\n";
		cout<<"\t.7 "<< std::get<7>(vertices[i].get()) <<"\n";
		cout<<"\t.8 "<< std::get<8>(vertices[i].get()) <<"\n";
		cout<<"\t.9 "<< std::get<9>(vertices[i].get()) <<"\n";
		cout<<"\tall:"<<vertices[i].get() <<"\n";
		auto indices =  map_indexed( vertices[i].get() , tuple_indices );
		cout<<"\tindices:"<<indices<<"\n";
		auto t = map_indexed( vertices[i].get() , prefix_elements );
		cout<<"\tall_i:"<< t <<"\n";
	}
//	cout<<"Vertices:";
//	for( auto x : vertices ) { cout<<x<<" "; } cout<<"\n";

	return 0;
}
