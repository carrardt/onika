#ifndef __onika_compress_edgecompress_h
#define __onika_compress_edgecompress_h

#include "onika/mesh/edgecollapse.h"
#include "onika/mesh/meshalgorithm.h"
#include "onika/codec/types.h"
#include "onika/codec/encoding.h"
#include "onika/debug/dbgmessage.h"

namespace onika { namespace compress {

	// =================================================================================
        // ============================ smeshEdgeCollapseEncode ============================
        // =================================================================================
	template<
		class CVConnectivy,
		class VertexContainer,
		class VertexComp,
		class CellContainer,
		class CellComp,
		class IdType,
		class StreamT>    	
	inline void smeshEdgeCollapseEncode(
		CVConnectivy& cvcon,
		VertexContainer& vertices,
		VertexComp vertexCompare, // purpose is to uniquely order vertices
		CellContainer& cells,
		CellComp cellCompare, // purpose is only to uniquely order cells
		IdType a,
		IdType b,
		StreamT& out)
	{
		constexpr unsigned int MAX_ADJACENT_CELLS = CVConnectivy::MAX_ADJACENT_CELLS;
		constexpr unsigned int NDim = CVConnectivy::traits::NDim;
		IdType adjCells[MAX_ADJACENT_CELLS];
		IdType dispCells[MAX_ADJACENT_CELLS];
		IdType removedCells[MAX_ADJACENT_CELLS];
		IdType nEdgeAdjCells=0,nAdjCells=0,nDisp=0;

		mesh::smeshEdgeCollapseProlog(cvcon,a,b,adjCells,nAdjCells,dispCells,nDisp,removedCells,nEdgeAdjCells);		
		
		debug::dbgmessage() <<"nEdgeAdjCells = "<<nEdgeAdjCells<<"\n";

		IdType removedCoCells[ nEdgeAdjCells ]; // dynamically allocated in stack
		IdType removedCellVertices[ nEdgeAdjCells*(NDim-1) ]; // dynamically allocated in stack
		mesh::smeshEdgeCollapseCoCells(cvcon,a,b,adjCells,nAdjCells,dispCells,nDisp,removedCells,nEdgeAdjCells,removedCoCells,removedCellVertices);

		// 1er appel qui modifie les donnees
		mesh::smeshEdgeCollapseUnsafe(cvcon,a,b,adjCells,nAdjCells,dispCells,nDisp,removedCells,nEdgeAdjCells);
		// ca n'a modifie que la connectivite, pas l'ordre ni le contenu des sommets / mailles


		// ============= encoding begin ==============
		IdType nverts = cvcon.getNumberOfVertices();

		// WARNING !!!
		// FIXME: if a is the last vertex, crash guaranteed !!!
		// solution: must track non removed vertex displacements and apply the cnages to previously encoded vertex indices
		// OR index vertices through a geometrically sorted list of vertices
		out.debug() << "collapse edge (" << a << ',' << b << ")";
		out << codec::bounded_value( a , static_cast<IdType>(0), static_cast<IdType>(nverts-1) );

		out.debug() << "remove vertex #" << b ;
		out << codec::value_enc(vertices,b); // could be replaced by delta_enc(vertices,a,b);
		// encode value(s) attached to removed vertex b
		// peut etre fait avant l'appel a cette fonction

		// supose que l'on ait bien reconstruit le v2c apres le merge
		IdType adjVertices[MAX_ADJACENT_CELLS], nAdjVertices=0;
		cvcon.getVertexAdjacentVertices(a,adjVertices,nAdjVertices);
		std::sort(adjVertices,adjVertices+nAdjVertices, vertexCompare ); 

		IdType* adjCells_begin = adjCells;
		IdType* adjCells_end = adjCells_begin+nAdjCells;
		std::sort(adjCells_begin,adjCells_end,cellCompare);

		IdType* dispCells_begin = dispCells;
		IdType* dispCells_end = dispCells_begin + nDisp;
		std::sort(dispCells_begin,dispCells_end,cellCompare);

		out << codec::subset( adjCells_begin,adjCells_end, dispCells_begin,dispCells_end );
		out << codec::bounded_value( nEdgeAdjCells, static_cast<IdType>(0), static_cast<IdType>(nAdjVertices) );
		// trier les ensembles adjCells et dispCells de facon unique ( sur critere de geometrie)
		// encoder les mailles deplacées comme un sous-ensemble des mailles adjacentes apres fusion.
		// le tri geometrique NE PEUT PAS SE FAIRE avant l'appel a cette fonction car la geometrie telle que je la trouverai
		// dans le maillage a reconstruire n'a la bonne valeur qu'ici (les references au sommet b ont ete remplacees par a)

		IdType* removedCells_begin = removedCells;
		IdType* removedCells_it = removedCells_begin;
		for(IdType ci=0; ci<nEdgeAdjCells;++ci,++removedCells_it)
		{
			// express vertex set (cells vertices not being a nor b) as a set of distinct indices in a spatially sorted set
			IdType indices[NDim-1];
			for(IdType j=0;j<(NDim-1);j++)
			{
				indices[j] = std::find(adjVertices,adjVertices+nAdjVertices,removedCellVertices[ci*(NDim-1)+j]) - adjVertices;
			}
			out.debug() << "remove cell #"<<*removedCells_it;
			out<< codec::bounded_integer_set(0,nAdjVertices,indices,indices+(NDim-1));
		}

		removedCells_it=removedCells_begin;
		for(IdType ci=0; ci<nEdgeAdjCells;++ci,++removedCells_it)
		{
			if( *removedCells_it != removedCoCells[ci] )
			{
				out << codec::delta_enc( cells, removedCoCells[ci] , *removedCells_it );
			}
			else if( nDisp > 0 ) // encode against first displaced cell
			{
				out << codec::delta_enc( cells, *dispCells_begin, *removedCells_it );
			}
			else
			{
				out << codec::value_enc( cells , *removedCells_it );
			}
		}

		// =================== encoding end =================
		
		mesh::smeshEdgeCollapseEpilog(cvcon, vertices, cells, a,b, removedCells,nEdgeAdjCells);
	}

	template<
		class CVConnectivy,
		class VertexContainer,
		class CellContainer,
		class IdType,
		class StreamT>    	
	inline void smeshEdgeCollapseEncode(
		CVConnectivy& cvcon,
		VertexContainer& vertices,
		CellContainer& cells,
		IdType a,
		IdType b,
		StreamT& out)
	{
		smeshEdgeCollapseEncode( cvcon, vertices, mesh::vertex_lexical_order(vertices), cells, mesh::cell_lexical_order(cvcon,cells),a,b,out);
	}
	


} } // end of namespace


//==============================================
//============= UNIT TEST ======================
//==============================================
#ifdef onika_compress_edgecompress_TEST

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <tuple>

#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/vtk/readvtkascii.h"
#include "onika/codec/asciistream.h"
#include "onika/tuple.h"

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

	std::vector<int> cells;
	int nverts;
	std::vector< std::tuple< std::tuple<double,double,double> , double > > vertices;
	std::vector< double > cellScalars;
};

// define how connectivity is mapped to your data storage
typedef onika::mesh::smesh_c2v_basic_traits< std::vector<int>, 3 > MyC2VBasicTraits;
typedef onika::mesh::c2v_traits< MyC2VBasicTraits > MyC2VTraits;
typedef onika::mesh::C2VWrapper<MyC2VTraits> MyC2VWrapper;
typedef onika::mesh::ReverseC2V<MyC2VWrapper, std::vector<int>, std::vector<unsigned int> > V2C;

int main(int argc, char* argv[])
{
	int seed = 33;
	seed = (std::ptrdiff_t)(&seed);
	std::cout<<"seeding with "<<seed<<"\n";
	srand( seed );

	std::string fname="data/tetraMesh.vtk";
	bool autorun = false;

	for(int i=1;i<argc;++i)
	{
		if( std::string(argv[i]) == "-a" )
		{
			autorun = true;
		}
		if( std::string(argv[i]) == "-i" )
		{
			++i; fname = argv[i];
		}
	}
	
	std::cout<<"input = "<<fname<<"\n";

	std::ifstream ifile(fname.c_str());
	if( !ifile )
	{
		std::cerr<<"Failed to open "<<fname<<"\n";
		return 1;
	}

	Mesh mesh;
	onika::vtk::readVtkAsciiMesh(ifile,mesh);
	onika::vtk::readVtkAsciiScalars(ifile,mesh);
	
	V2C v2c( mesh.cells, mesh.nverts ); 
	//onika::debug::dbgassert( v2c.checkConsistency(mesh.nverts) );
	
	std::cout<<v2c.getNumberOfVertices()<<" vertices, "<< v2c.getNumberOfCells()<<" cells, mem="<<v2c.getMemoryBytes()<<"\n";

	int v=0;
	if( autorun ) { v = (int)( drand48()*(mesh.nverts-1) ); std::cout<<"Vertex A = "<<v<<"\n"; }
	else {
		std::cout<<"\nVertex A ? "; std::cout.flush();
		std::cin>>v;
	}
	
	int adjacentCells[64], nAdj=0;
	v2c.getVertexAdjacentCells( v, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	int adjacentVertices[64], nAdjV=0;
	v2c.getVertexAdjacentVertices( v, adjacentVertices,  nAdjV );
	std::cout<<nAdjV<<" adjacent vertices : "; 
	for(int i=0;i<nAdjV;i++) { std::cout<<adjacentVertices[i]<<' '; } std::cout<<"\n";

	int vb=0;
	if( autorun ) { vb = (int)( drand48()*(nAdjV-1) ); vb = adjacentVertices[vb]; std::cout<<"Vertex B = "<<vb<<"\n"; }
	else {
		std::cout<<"\nVertex B ? "; std::cout.flush();
		std::cin>>vb;
	}
	v2c.getVertexAdjacentCells( vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	v2c.getVertexAdjacentVertices( vb, adjacentVertices,  nAdjV );
	std::cout<<nAdjV<<" adjacent vertices : "; 
	for(int i=0;i<nAdjV;i++) { std::cout<<adjacentVertices[i]<<' '; } std::cout<<"\n";

	v2c.getEdgeAdjacentCells( v, vb, adjacentCells,  nAdj );
	std::cout<<nAdj<<" edge adjacent cells : "; 
	for(int i=0;i<nAdj;i++) { std::cout<<adjacentCells[i]<<' '; } std::cout<<"\n";

	onika::debug::dbgassert( v2c.checkConsistency() );

	std::cout<<"collapse edge "<<v<<"-"<<vb<<"\n";

	onika::codec::AsciiStream out(std::cout);
	onika::compress::smeshEdgeCollapseEncode( v2c, mesh.vertices, mesh.cellScalars, v, vb, out );

	onika::debug::dbgassert( v2c.checkConsistency() );

        std::cout<<std::endl<<"PASSED"<<std::endl;
	return 0;
}


#endif // end of unit test

#endif // end of file
