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
		out << container::value_enc(vertices,b); // could be replaced by delta_enc(vertices,a,b);
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
		std::tuple<IdType,IdType> edge,
		StreamT& out)
	{
		smeshEdgeCollapseEncode( cvcon, vertices, mesh::vertex_lexical_order(vertices), cells, mesh::cell_lexical_order(cvcon,cells),std::get<0>(edge),std::get<1>(edge),out);
	}


} } // end of namespace


#endif // end of file

