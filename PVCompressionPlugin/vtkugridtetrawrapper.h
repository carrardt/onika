#ifndef __pvcompressionplugin_vtkugridtetrawrapper_h
#define __pvcompressionplugin_vtkugridtetrawrapper_h

#include <vtkUnstructuredGrid.h>
#include <vtkCellArray.h>
#include <vtkIdTypeArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkDataArrayTemplate.h>

#include <type_traits>

#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "vtkarraywrapper.h"

namespace onika { namespace vtk {

	template<unsigned int Dim>
	inline bool allCellsAreSimplicies(vtkUnstructuredGrid* ugrid)
	{
		vtkIdType n = ugrid->GetNumberOfCells();
		vtkIdType* cells = ugrid->GetCells()->GetData()->GetPointer(0);
		for(vtkIdType i=0;i<n;i++)
		{
			vtkIdType nverts = *(cells++);
			if( nverts != (Dim+1) ) return false;
			cells += nverts;
		}
		return true;
	}

	inline bool allCellsAreTetras(vtkUnstructuredGrid* ugrid)
	{
		return allCellsAreSimplicies<3>(ugrid);
	}

	template<class T, int NC, bool CellData>
	struct DataSetAttribute
	{
		std::string name;
		inline DataSetAttribute(const std::string& n) : name(n) {}
		inline auto wrap(vtkUnstructuredGrid * ds) const ONIKA_AUTO_RET(
		  select_vtkarray_wrapper(
			vtkDataArrayTemplate<T>::FastDownCast(
			  (CellData?vtkDataSetAttributes::SafeDownCast(ds->GetCellData()):vtkDataSetAttributes::SafeDownCast(ds->GetPointData()))
			  ->GetArray(name.c_str()) ) ,
			ONIKA_CONST(NC)
			)
		  )
	};

	template<class T, int NC=3>
	struct UGridPoints
	{
		inline auto wrap(vtkUnstructuredGrid* grid) const ONIKA_AUTO_RET(
			select_vtkarray_wrapper(
				vtkDataArrayTemplate<T>::FastDownCast(grid->GetPoints()->GetData()) ,
				ONIKA_CONST(NC)
				)
			)
	};

	struct UGridCells
	{
		inline auto wrap(vtkUnstructuredGrid* grid) const ONIKA_AUTO_RET( wrap_vtkarray( grid->GetCells()->GetData() ) )
	};

	template<class Cells, class PointArrays , class CellArrays>
	struct UGridSMesh // don't reference array wrappers, copy them
	{
		Cells m_cell2vertex; // connectivity
		CellArrays m_cellValues; // cell scalars
		PointArrays m_vertices; // point position and scalars
		inline UGridSMesh(const Cells& cells, const Points& points, const CellVaues& cellValues)
		: m_cell2vertex(cells), m_vertices(points), m_cellValues(cellValues) {}
	};

	template<class Cells, class Points, class CellValues>
	static inline auto wrap_ugrid_tetra(const Cells& cells, const Points& points, const CellValues& cellValues)
	ONIKA_AUTO_RET( UGridSMesh<Cells,Points,CellValues>(cells,points,cellValues) )

	template<class... Wrappers>
	static inline auto zip_array_wrappers( vtkUnstructuredGrid* grid, const Wrappers&... wrappers )
	ONIKA_AUTO_RET( zip_vectors_cpy( wrappers.wrap(grid)... ) )

	template<class Cells, class Points, class CellValues>
	static inline auto wrap_vtk_ugrid_smesh(const Cells& cells, const Points& points, const CellVaues& cellValues)
	ONIKA_AUTO_RET( UGridSMesh<Cells,Points,CellValues>(cells,points,cellValues) )

} }

#endif
