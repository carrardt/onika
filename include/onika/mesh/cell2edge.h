#ifndef __onika_mesh_cell2edge_h
#define __onika_mesh_cell2edge_h

#include <tuple>
#include "onika/language.h"

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

template<class traits>
struct C2EWrapper
{
	using ContainerType = typename traits::ContainerType;
	using IdType = typename traits::IdType;
	const ContainerType& data; /* const ref, we only have read-only method*/

	inline C2EWrapper(const ContainerType& c) : data(c) {}

	// =================================================================================
	// ============================ read only access ===================================
	// =================================================================================
	inline auto getMaxCellEdges() const ONIKA_AUTO_RET( traits::getMaxCellEdges(data) )
	inline auto getNumberOfCells() const ONIKA_AUTO_RET( traits::getNumberOfCells(data) )
    inline auto getTotalNumberOfCellEdges() const ONIKA_AUTO_RET( traits::getTotalNumberOfCellEdges(data) )
	inline auto getCellNumberOfEdges(IdType i) const ONIKA_AUTO_RET( traits::getCellNumberOfEdges(data,i) )
	inline auto getCellEdge(IdType cell, IdType e) const ONIKA_AUTO_RET( traits::getCellEdge(data,cell,e) )
};

} }


// ==========================================================
// =================== Unit Test ============================
// ==========================================================
#endif // end of file

