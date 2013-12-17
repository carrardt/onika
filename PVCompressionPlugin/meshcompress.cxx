#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/debug/dbgassert.h"
#include "onika/container/iterator.h"
#include "onika/container/tuplevec.h"
#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h" // cellminedgelength
#include "onika/tuple.h"
#include "onika/mathfunc.h"
#include "onika/language.h"  // ONIKA_AUTO_RET macro
#include "onika/algorithm/rbtree.h"

#include "vtkugridtetrawrapper.h"
#include "vtkarraywrapper.h"

#include <vtkUnstructuredGrid.h>

#include <tuple>
#include <string>
#include <iostream>
#include <fstream>


// VTK dependencies
#include <vtkUnstructuredGrid.h>

// TODO: progressively get rid of the need to buil types prior to use them.
// everything would be much more readable and easy to use with in-function, auto declared variables
// initialized with template functions.


// Vertex-Cell 2-way connectivity
typedef onika::container::ArrayWrapper<vtkIdType> MyCellContainer;
typedef onika::mesh::smesh_c2v_basic_traits<MyCellContainer,3,1> MyC2VBasicTraits;
typedef onika::mesh::c2v_traits<MyC2VBasicTraits> MyC2VTraits;
typedef onika::mesh::C2VWrapper<MyC2VTraits> MyC2VWrapper;
typedef onika::mesh::ReverseC2V<MyC2VWrapper, std::vector<int>, std::vector<unsigned int> > V2C;

// Edge view
typedef onika::mesh::smesh_c2e_basic_traits<MyC2VBasicTraits> MyC2EBasicTraits;

// Vertex Adpater
// Vertices will be types as tuples of the form : ( (Vx,Vy,Vz) , Scalar )
typedef onika::container::ArrayWrapper<double,3> VertSingleComp;
typedef onika::container::TupleVecCpy<VertSingleComp,VertSingleComp,VertSingleComp> VertexCoordArray;
typedef onika::container::ArrayWrapper<double> VertexScalarArray;
typedef onika::container::TupleVec<VertexCoordArray,VertexScalarArray> MyVertexContainer;

// Cell Values Adapter
typedef onika::container::ArrayWrapper<double> MyCellValueContainer;


// specialization of vertexDistance method for my own vertex container
// FIXME: is overloading of a common function the best way to define distance between vertices ?
namespace onika { namespace mesh {

// Vertices are tuples where first element is a 3-tuple storing vertex coordinates
template<class IdType>
inline auto vertexDistance( const MyVertexContainer& vertices, IdType a, IdType b )
ONIKA_AUTO_RET( onika::math::distance( std::get<0>(vertices[a]), std::get<0>(vertices[b]) )   )

} }

// operator to compare cells according to their shortest edge's length
typedef onika::mesh::CellMinEdgeLengthCompare<MyVertexContainer,MyCellContainer,MyC2EBasicTraits> MyCellShortestEdge;

// positional binary tree to maintain cells sorted by contraction priority
typedef onika::algorithm::TupleVectorBTree< std::vector< std::tuple<unsigned int,int,int> > > CellPriorityTreeBase;
typedef onika::algorithm::BTree< CellPriorityTreeBase, MyCellShortestEdge, onika::algorithm::NodeRefT > CellPriorityTree;
typedef typename CellPriorityTree::NodeRef CellPriorityNode;

// ostream operator for tuples
template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}

using namespace onika::vtk;

bool onikaEncodeMesh(vtkUnstructuredGrid* input, vtkUnstructuredGrid* output, int nedges, const std::string& outputFileName)
{
	std::ofstream ofile(outputFileName);
	if(!ofile)
	{
		std::cerr<<"Couldn't open '"<<outputFileName<<"' for writing\n";
		return false;
	}

	std::cout<<"Output = "<<outputFileName<<"\n";
	std::cout<<"Edges = "<<nedges<<"\n";

	if( input == 0 ) return false;
	if( ! allCellsAreTetras(input) )
	{
		cout<<"Not a simplicial mesh\n";
		return false;
	}

	// build 2-way cell-vertex connectivity adpater
	vtkIdType* cellsPtr = input->GetCells()->GetData()->GetPointer(0);
	size_t cellsSize = input->GetCells()->GetData()->GetNumberOfTuples();
	MyCellContainer cells = { cellsPtr, cellsSize, cellsSize };
	vtkIdType nVerts = input->GetNumberOfPoints();
	V2C v2c( cells, nVerts );
	onika::debug::dbgassert( v2c.checkConsistency() );
	
	int nCells = v2c.getNumberOfCells();
	std::cout<<v2c.getNumberOfVertices()<<" vertices, "<< v2c.getNumberOfCells()<<" cells, mem="<<v2c.getMemoryBytes()<<"\n";

	// build vertices adapter


#if 0
	// build the comparison operator that sorts cells according to their shortest edge length
	MyCellShortestEdge cellPriorityCompare(mesh.vertices,mesh.cells);
	CellPriorityTree cellPriorityTree(cellPriorityCompare);

	std::cout<< "edge (54,32) length = "<< onika::mesh::vertexDistance(mesh.vertices,54,32) <<"\n";
	for(int i=0;i<nCells;i++)
	{
		int nEdges = MyC2EBasicTraits::getCellNumberOfEdges(mesh.cells,i);
		std::cout<<"Cell "<<i<<" :\n";
		for(int e=0;e<nEdges;e++)
		{
			auto edge = MyC2EBasicTraits::getCellEdge(mesh.cells,i,e);
			std::cout<<"\tEdge "<< e<<": "<< edge <<", length="<< onika::mesh::edgeLength(mesh.vertices,edge)<<"\n";
		}
	}

	std::cout<<"Sorting cells ... \n";
	for(int i=0;i<nCells;i++)
	{
		cellPriorityTree.insert( i );
	}
	unsigned int dmin=-1, dmax=0;
	onika::algorithm::probe_btree_depth( cellPriorityTree.getRoot(), dmin, dmax );
	std::cout<<"Tree depth in ["<<dmin<<","<<dmax<<"]\n";
#endif

    std::cerr<<"\nDONE\n";
    return true;
}


