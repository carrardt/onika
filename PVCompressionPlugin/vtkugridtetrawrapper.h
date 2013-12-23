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
	struct CellDataArray
	{
		std::string name;
		inline CellDataArray(const std::string& n) : name(n) {}
		inline auto wrap_array(vtkDataSet* grid)
		ONIKA_AUTO_RET( wrap_vtkarray_selector<NC,T>( vtkDataArrayTemplate<T>::SafeDownCast(grid->GetCellData()->GetArray(name.c_str())) ) )
	};

	template<class T, unsigned int NC=1>
	struct PointDataArray
	{
		std::string name;
		inline PointDataArray(const std::string& n) : name(n) {}
		inline auto wrap_array(vtkDataSet* grid)
		ONIKA_AUTO_RET( wrap_vtkarray_selector<NC,T>( vtkDataArrayTemplate<T>::SafeDownCast(grid->GetPointData()->GetArray(name.c_str())) ) )
	};

	template<class T, unsigned int NC=3>
	struct UGridPoints
	{
		inline auto wrap_array(vtkDataSet* grid)
		ONIKA_AUTO_RET( wrap_vtkarray_selector<NC,T>( vtkDataArrayTemplate<T>::SafeDownCast(grid->GetPoints()) ) )
	};

} }

#endif
