#include <string>
#include <vector>
#include <iostream>
#include <fstream>



int main(int argc, char* argv[])
{
	int nedges = 10;
	std::string inputFileName = "/dev/stdin";
	std::string outputFileName = "/dev/stdout";
	for(int i=1;i<argc;++i)
	{
		if( std::string(argv[i]) == "-c" )
		{
			++i; nedges = atoi(argv[i]);
		}
		else if( std::string(argv[i]) == "-i" )
		{
			++i; inputFileName = argv[i];
		}
		else if( std::string(argv[i]) == "-o" )
		{
			++i; outputFileName = argv[i];
		}
		else if( std::string(argv[i]) == "--help" )
		{
			std::cerr<<"Usage: "<<argv[0]<<" [-c <# edges>] [-i <input file>] [-o <output file>]\n";
			return 0;
		}
	}

	std::ifstream ifile(inputFileName.c_str());
	if(!ifile)
	{
		std::cerr<<"Couldn't open '"<<inputFileName<<"' for reading\n";
		return 1;
	}

	std::ofstream ofile(outputFileName);
	if(!ofile)
	{
		std::cerr<<"Couldn't open '"<<outputFileName<<"' for writing\n";
		return 1;
	}
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


