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

	template<class T, unsigned int NC, bool CellData>
	struct DataSetAttribute
	{
		std::string name;
		inline DataSetAttribute(const std::string& n) : name(n) {}
		vtkDataSetAttributes* getDSA(vtkUnstructuredGrid* ) {  }
		inline auto wrap(vtkDataSet * ds) ONIKA_AUTO_RET(
		  select_vtkarray_wrapper(
			vtkDataArrayTemplate<T>::FastDownCast(
			  (CellData?static_cast<vtkDataSetAttributes*>(ds->GetCellData()):static_cast<vtkDataSetAttributes*>(ds->GetPointData()))
			  ->GetArray(name.c_str()) ) ,
			ONIKA_CONST(NC)
			)
		  )
	};

	template<class T, int NC=3>
	struct UGridPoints
	{
		inline auto wrap(vtkPointSet* grid) ONIKA_AUTO_RET(
			select_vtkarray_wrapper(
				vtkDataArrayTemplate<T>::FastDownCast(grid->GetPoints()->GetData()) ,
				ONIKA_CONST(NC)
				)
			)
	};

	struct UGridCells
	{
		inline auto wrap(vtkUnstructuredGrid* grid) ONIKA_AUTO_RET( wrap_vtkarray( grid->GetCells()->GetData() ) )
	};

	template<class Cells, class PointArrays , class CellArrays>
	struct UGridSMesh // don't reference array wrappers, copy them
	{
		Cells cells;
		CellArrays cellValues;
		PointArrays vertexValues;
	};

	template<class Cells, class Points, class CellValues>
	static inline auto wrap_ugrid_tetra(const Cells& cells, const Points& points, const CellValues& cellValues)
	ONIKA_AUTO_RET( UGridSMesh<Cells,Points,CellValues>(cells,points,cellValues) )

	template<class... Wrappers>
	static inline auto zip_array_wrappers( vtkUnstructuredGrid* grid, const Wrappers&... wrappers )
	ONIKA_AUTO_RET( zip_vectors_cpy( wrappers.wrap(grid)... ) )

} }

#endif
