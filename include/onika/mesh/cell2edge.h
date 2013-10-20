#ifndef __onika_mesh_cell2edge_h
#define __onika_mesh_cell2edge_h

#include <tuple>

namespace onika { namespace mesh {

struct example_c2e_basic_traits
{
	typedef int* ContainerType;
	typedef int IdType;

	// =================================================================================
	// ============================ read only access ===================================
	// =================================================================================
	static constexpr IdType getMaxCellEdges(const ContainerType& c) { return 10; }
	static inline IdType getNumberOfCells(const ContainerType& c) { return 10; }
        static inline IdType getTotalNumberOfCellEdges(const ContainerType& c) { return 100; }
	static inline IdType getCellNumberOfEdges(const ContainerType& c, IdType i) { return 8; }
	static inline std::tuple<IdType,IdType> getCellEdge(const ContainerType& c, IdType cell, IdType e)
	{
		return std::make_tuple(3,5);
	}
};


} }


// ==========================================================
// =================== Unit Test ============================
// ==========================================================
#ifdef onika_mesh_cell2edge_TEST
int main()
{
        return 0;
}

#endif // end of unit test

#endif // end of file

