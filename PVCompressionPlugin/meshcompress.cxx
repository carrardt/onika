#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <tuple>

#include "onika/tuple.h"
#include "onika/mathfunc.h"
#include "onika/container/iterator.h"
#include "onika/container/tuplevec.h"

#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/mesh/meshalgorithm.h" // cellminedgelength

#include "onika/vtk/readvtkascii.h"
#include "onika/codec/asciistream.h"

#include "onika/language.h"  // ONIKA_AUTO_RET macro

#include "onika/algorithm/rbtree.h"

// VTK dependencies
#include <vtkUnstructuredGrid.h>



typedef std::vector< std::tuple<
		std::tuple<double,double,double>	// vertex position
		, double				// vertex scalar
		> > MyVertexContainer;

typedef std::vector<int> MyCellContainer;
typedef std::vector<double> MyCellValueContainer;

struct Mesh
{
	typedef struct { double x[3]; } VertexPos;

	inline Mesh() : nverts(0) {}

	inline void addVertex(const VertexPos& p)
	{
		vertices.push_back( std::make_tuple( std::make_tuple(p.x[0],p.x[1],p.x[2]) , (double)nverts ) );
		nverts++;
	}
	
	template<class VertIdIterator>
	inline void addCell(int np, VertIdIterator it)
	{
		if( np != 4 ) return;
		for(int i=0;i<np; ++i, ++it) cells.push_back( *it );
	}

	inline size_t getNumberOfCells() const { return cells.size()/4; }
	inline size_t getNumberOfVertices() const { return vertices.size(); }

	template<class Iterator>
	void addCellScalars(const std::string& name, Iterator first, Iterator last)
	{
		onika::debug::dbgmessage() << "Add cell scalars "<<name<<"\n";
		cellScalars.clear();
		for(Iterator it=first; it!=last; ++it)
		{
			cellScalars.push_back( *it );
		}
	}

	template<class Iterator>
	void addVertexScalars(const std::string& name, Iterator first, Iterator last)
	{
		onika::debug::dbgmessage() << "Add vertex scalars "<<name<<"\n";
		size_t i=0;
		for(Iterator it=first; it!=last; ++it, ++i )
		{
			std::get<1>( vertices[i] ) = *it;
		}
		onika::debug::dbgassert( i == vertices.size() );
	}

	MyCellContainer cells;
	MyVertexContainer vertices;
	MyCellValueContainer cellScalars;
	int nverts;
};

// define how connectivity is mapped to your data storage
typedef onika::mesh::smesh_c2v_basic_traits< std::vector<int>, 3 > MyC2VBasicTraits;
typedef onika::mesh::c2v_traits< MyC2VBasicTraits > MyC2VTraits;
typedef onika::mesh::C2VWrapper<MyC2VTraits> MyC2VWrapper;
typedef onika::mesh::ReverseC2V<MyC2VWrapper, std::vector<int>, std::vector<unsigned int> > V2C;

// edge accessor
typedef onika::mesh::smesh_c2e_basic_traits<MyC2VBasicTraits> MyC2EBasicTraits;

// specialization of vertexDistance method for my own vertx container
namespace onika { namespace mesh {
template<class IdType>
inline auto vertexDistance( const MyVertexContainer& vertices, IdType a, IdType b )
ONIKA_AUTO_RET(   onika::math::distance( std::get<0>(vertices[a]), std::get<0>(vertices[b]) )   )
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

	Mesh mesh;
	// build data structure from grid

	// build reverse connectivity
	V2C v2c( mesh.cells, mesh.nverts ); 
	//onika::debug::dbgassert( v2c.checkConsistency(mesh.nverts) );
	
	int nCells = v2c.getNumberOfCells();
	std::cout<<v2c.getNumberOfVertices()<<" vertices, "<< v2c.getNumberOfCells()<<" cells, mem="<<v2c.getMemoryBytes()<<"\n";

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

// on le garde en commentaire pour exemple de quantification
#if 0
	Mesh basemesh;
	basemesh.nullVertexPos = nullPos;
	basemesh.nullVertexId = -1;
	onika::vtk::readVtkAsciiMesh(ifile,basemesh);
	onika::debug::dbgassert( basemesh.checkConsistency() );

	std::cerr<<"mesh : "<<basemesh.getNumberOfVertices()<<" points, "<<basemesh.getNumberOfCells()<<" cells\n";

	MeshValues meshScalars(basemesh);
	onika::vtk::readVtkAsciiScalars(ifile,meshScalars);
	onika::debug::dbgassert( meshScalars.checkConsistency() );
	std::cerr<<"cell scalars : ";
	for(int i=0;i<meshScalars.cellScalars.size();i++)
	{
		std::cerr<<meshScalars.cellScalarNames[i]<<" ";
	}
	std::cerr<<"\nvertex scalars : ";
	for(int i=0;i<meshScalars.vertexScalars.size();i++)
	{
		std::cerr<<meshScalars.vertexScalarNames[i]<<" ";
	}
	std::cerr<<"\n";

	std::cerr<<"Original mesh size = "<<meshScalars.getMemoryBytes()<<"\n";

	// ============== quantization ================
	std::cerr<<"Quantizing vertices :";
	QMesh qmesh;
	for(int d=0;d<3;d++)
	{
		qmesh.vertices.data[d].copyFrom( basemesh.vertices.data[d].begin(),basemesh.vertices.data[d].end() );
		std::cerr<<" "<<qmesh.vertices.data[d].quantizer.getNBits();
	}

	std::cerr<<"\nQuantizing vertex indices : ";
	size_t nVerts = basemesh.getNumberOfVertices();
	size_t nCellVerts = basemesh.getTotalNumberOfCellVertices();
	int64_t maxId = 0;
	for( auto it = basemesh.cells.begin(); it!= basemesh.cells.end(); ++it )
	{
		int64_t id = *it;
		onika::debug::dbgassert( id >= 0 && id < nVerts );
		if( id > maxId ) maxId = id;	
	}
	int idBits = onika::nextpo2log(maxId+2);
	std::cerr<<idBits<<" bits\n";
	qmesh.cells.clear(idBits);
	qmesh.cells.resize(nCellVerts);
	std::copy( basemesh.cells.begin(), basemesh.cells.end(), qmesh.cells.begin() );
	qmesh.nullVertexId = onika::mask1lsb<uint64_t>(idBits);

	std::cerr<<"Quantizing cell scalars :";
	QMeshValues qmeshvalues(qmesh);
	qmeshvalues.setNumberOfCellScalars( meshScalars.getNumberOfCellScalars() );
	for(int i=0;i<meshScalars.getNumberOfCellScalars();i++)
	{
		qmeshvalues.cellScalarNames[i] = meshScalars.getCellScalarName(i);
		qmeshvalues.cellScalars[i].copyFrom( meshScalars.getCellScalarsBegin(i), meshScalars.getCellScalarsEnd(i) ); 
		std::cerr<<" "<<qmeshvalues.cellScalarNames[i]<<"/"<<qmeshvalues.cellScalars[i].quantizer.getNBits();
	}
	std::cerr<<"\n";
	std::cerr<<"Quantizing vertex scalars :";
	qmeshvalues.setNumberOfVertexScalars( meshScalars.getNumberOfVertexScalars() );
	for(int i=0;i<meshScalars.getNumberOfVertexScalars();i++)
	{
		qmeshvalues.vertexScalarNames[i] = meshScalars.getVertexScalarName(i);
		qmeshvalues.vertexScalars[i].copyFrom( meshScalars.getVertexScalarsBegin(i), meshScalars.getVertexScalarsEnd(i) ); 
		std::cerr<<" "<<qmeshvalues.vertexScalarNames[i]<<"/"<<qmeshvalues.vertexScalars[i].quantizer.getNBits();
	}
	std::cerr<<"\n";
	onika::debug::dbgassert( qmeshvalues.checkConsistency() );

	std::cerr<<"Quantized mesh size = "<<qmeshvalues.getMemoryBytes()<<"\n";

	// avec connectivitée inverse V2C
	std::cerr<<"Build Vertex 2 Cell connectivity\n";
	QMeshV2C mesh(qmeshvalues,false);
	size_t v2cListBits = onika::nextpo2log(nCellVerts+3) + 1;
	mesh.v2cList.clear(v2cListBits);
	size_t v2cIndexBits = onika::nextpo2log(nCellVerts+2);
	mesh.v2cIndex.clear(v2cIndexBits);
	mesh.buildV2C();
	onika::debug::dbgassert( mesh.checkConsistency() );

	int ncells = mesh.getNumberOfCells();

	std::cerr<<"compression overhead = "<<mesh.getMemoryBytes()<<" ( "<<mesh.getMemoryBytes()*100.0/meshScalars.getMemoryBytes()<<"% )\n";

	// build output stream
	onika::codec::OutputBitStream out(ofile,std::cerr);
 
//	out << mesh.metaData();
	out.debug() << "--- end of meta data ---";

	for(int i=0;i<nedges;i++)
	{
		int minCell = std::min_element( mesh.cellIdBegin(), mesh.cellIdEnd(), shortest_edge_order(mesh) );
		typename QMesh::VertexIdPair edge = mesh.cellShortestEdge(minCell);
		mesh.edgeCollapse(edge.v1,edge.v2,out);
		onika::debug::dbgassert(mesh.checkConsistency());
	}

	onika::debug::dbgassert( mesh.checkConsistency() );
#endif

        std::cerr<<"\nDONE\n";
        return 0;
}


