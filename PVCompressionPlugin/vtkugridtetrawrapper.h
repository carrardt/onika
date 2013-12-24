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

	template<class T, unsigned int NC=1>
	struct PointDataArray
	{
		std::string name;
		inline PointDataArray(const std::string& n) : name(n) {}
		inline auto wrap(vtkDataSet* grid) ONIKA_AUTO_RET(
			select_vtkarray_wrapper(
				vtkDataArrayTemplate<T>::SafeDownCast(grid->GetPointData()->GetArray(name.c_str())) ,
				ONIKA_CONST(NC)
				)
			)
	};

	template<class T, unsigned int NC=1>
	struct CellDataArray
	{
		std::string name;
		inline CellDataArray(const std::string& n) : name(n) {}
		inline auto wrap(vtkDataSet* grid) ONIKA_AUTO_RET(
			select_vtkarray_wrapper(
				vtkDataArrayTemplate<T>::SafeDownCast(grid->GetCellData()->GetArray(name.c_str())) ,
				ONIKA_CONST(NC)
				)
			)
	};

	template<class T, unsigned int NC=3>
	struct UGridPoints
	{
		inline auto wrap(vtkPointSet* grid) ONIKA_AUTO_RET(
			select_vtkarray_wrapper(
				vtkDataArrayTemplate<T>::SafeDownCast(grid->GetPoints()) ,
				ONIKA_CONST(NC)
				)
			)
	};

	struct UGridCells
	{
		inline auto wrap(vtkUnstructuredGrid* grid) ONIKA_AUTO_RET(
			select_vtkarray_wrapper(
				grid->GetCells()->GetData() ,
				ONIKA_CONST(NC)
				)
			)
	};
	
	template<class Cells, class Points, class CellArrays, class PointArrays >
	struct UGridSMesh
	{
		Cells& cells;
		CellArrays& cellValues;
		Points& vertexCoordinates;
		PointArrays& vertexValues;
	};

} }

#endif
