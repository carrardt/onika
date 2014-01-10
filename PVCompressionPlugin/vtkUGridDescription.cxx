#include <vtkUnstructuredGrid.h>
#include <vtkDataArray.h>
#include <vtkCellData.h>
#include <vtkCellArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <assert.h>

#include "ugriddesc.h"

namespace onika { namespace vtk {

	void init_array_description(vtkArrayDescription& ad, vtkDataArray* array)
	{
		assert(array!=0);
		const char* s = array->GetName();
		ad.name = (s==0) ? "" : s;
		ad.type = array->GetDataType();
		ad.components = array->GetNumberOfComponents();
		ad.ptr = array->GetVoidPointer(0);
	}

	void init_ugrid_description(vtkUGridDescription& gd, vtkUnstructuredGrid* ugrid)
	{
		assert(ugrid!=0);
		assert(ugrid->GetCells()!=0);
		assert(ugrid->GetCells()->GetData()!=0);
		assert(ugrid->GetPointData()!=0);
		assert(ugrid->GetCellData()!=0);

		// mesh connectivity
		gd.meshSize = ugrid->GetCells()->GetData()->GetNumberOfTuples();
		init_array_description( gd.mesh , ugrid->GetCells()->GetData() );

		// vertices
		gd.nVertices = ugrid->GetNumberOfPoints();
		int npa = ugrid->GetPointData()->GetNumberOfArrays();
		gd.vertexArrays.resize(npa+1);
		init_array_description( gd.vertexArrays[0] , ugrid->GetPoints()->GetData() );
		for(int i=0;i<npa;i++)
		{
			init_array_description( gd.vertexArrays[i+1] , ugrid->GetPointData()->GetArray(i) ); 
		}

		// cell centered values
		gd.nCells = ugrid->GetNumberOfCells();
		int nca = ugrid->GetCellData()->GetNumberOfArrays();
		gd.cellArrays.resize(nca);
		for(int i=0;i<nca;i++)
		{
			init_array_description( gd.cellArrays[i] , ugrid->GetCellData()->GetArray(i) );
		}
	}

} }

