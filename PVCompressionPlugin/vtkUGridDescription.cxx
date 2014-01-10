#include <vtkUnstructuredGrid.h>
#include <vtkDataArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include "ugriddesc.h"

namespace onika { namespace vtk {

	void init_array_description(vtkArrayDescription& ad, vtkDataArray* array)
	{
		strncpy( ad.name , array->GetName() , 255 );
		ad.type = array->GetDataType();
		ad.components = array->GetNumberOfcomponents();
		ad.ptr = array->GetVoidPointer(0);
	}

	void init_ugrid_description(vtkUGridDescription& gd, vtkUnstructuredGrid* ugrid)
	{
		// mesh connectivity
		gd.meshSize = ugrid->GetCells()->GetData()->GetNumberOfTuples();
		init_array_description( gd.mesh , ugrid->GetCells()->GetData() );

		// vertices
		gd.nVertices = ugrid->GetNumberOfPoints();
		init_array_description( gd.vertexArrays[0] , ugrid->GetPoints()->GetData() );
		int npa = ugrid->GetPointData()->GetNumberOfArrays();
		if( npa >= 255 )
		{
			std::cerr<<"Maximum number of point arrays reached, limiting to 254\n";
			npa = 254;
		}
		for(int i=0;i<npa;i++)
		{
			init_array_description( gd.vertexArrays[i+1] , ugrid->GetPointData()->GetArray(i) ); 
		}
		gd.nVertexArrays = npa + 1;
		
		// cell centered values
		gd.nCells = ugrid->GetNumberOfCells();
		gd.nCellArrays = ugrid->GetCellData()->GetNumberOfArrays();
		if( gd.nCellArrays > 255 )
		{
			std::cerr<<"Maximum number of point arrays reached, limiting to 255\n";
			gd.nCellArrays = 255;
		}
		for(int i=0;i<gd.nCellArrays;i++)
		{
			init_array_description( gd.vertexArrays[i] , ugrid->GetCellData()->GetArray(i) ); 
		}
	}

} }

