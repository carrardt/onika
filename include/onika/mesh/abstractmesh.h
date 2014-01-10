#ifndef __onika_mesh_abstractmesh_h
#define __onika_mesh_abstractmesh_h

#include "onika/language.h"

namespace onika { namespace mesh {

template<class CellTypes,class CellConnectivity, class CellValues, class Vertices>
struct AbstractMesh
{
	CellTypes cellTypes;
	CellConnectivity cells; // Cell 2 Vertex
	CellValues cellValues;
	Vertices vertices;
};

} } // namspace onika


#endif // end of file
