#ifndef __PVCompressionPlugin_vtkUGridDescipriton_h
#define __PVCompressionPlugin_vtkUGridDescipriton_h

#include <vtkUnstructuredGrid.h>
#include <vtkDataArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include "ugriddesc.h"

namespace onika { namespace vtk {

	template<class StreamT>
	static inline void printAttributeSignature(vtkDataSetAttributes* dsa, vtkDataArray* optArray, StreamT& out )
	{
		out << "types<";
		int n = dsa->GetNumberOfArrays();
		if(optArray!=0) { out << optArray->GetDataTypeAsString(); }
		for(int i=0;i<n;i++)
		{
			if( i>=1 || optArray!=0 ) out << ',';
			out << dsa->GetArray(i)->GetDataTypeAsString();
		}
		out << ">,integers<";
		if(optArray!=0) { out << optArray->GetNumberOfComponents(); }
		for(int i=0;i<n;i++)
		{
			if( i>=1 || optArray!=0 ) out << ',';
			out << dsa->GetArray(i)->GetNumberOfComponents();
		}
		out << '>';
	}

	template<class StreamT>
	static inline void printUGridSignature( vtkUnstructuredGrid* data, StreamT& out )
	{
		printAttributeSignature(data->GetCellData(),data->GetPoints()->GetData(),out);
		out<<',';
		printAttributeSignature(data->GetPointData(),0,out);
	}

	void init_ugrid_description(vtkUGridDescription& gd, vtkUnstructuredGrid* ugrid);
} }

#endif

