/*
 * genugriddesc.cxx
 *
 *  Created on: 2 janv. 2014
 *      Author: thierry
 */
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkDataArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>

#include <string>
#include <sstream>

inline std::string vtk_array_desc(vtkDataArray* array)
{
	std::string s = array->GetDataTypeAsString();
	s += ',';
	std::ostringstream oss;
	oss<<array->GetNumberOfComponents();
	s += oss.str();
	return s;
}

inline std::string vtk_dsa_desc(vtkDataSetAttributes* dsa)
{
	std::string s="types<";
	int n = dsa->GetNumberOfArrays();
	for(int i=0;i<n;i++)
	{
		s += dsa->GetArray(i)->GetDataTypeAsString();
		if(i<(n-1)) s += ',';
	}
	s += ">,integers<";
	for(int i=0;i<n;i++)
	{
		std::ostringstream oss;
		oss<<dsa->GetArray(i)->GetNumberOfComponents();
		s += oss.str();
		if(i<(n-1)) s += ',';
	}
	s += '>';
	return s;
}

int main(int argc, char* argv[])
{
	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(argv[1]);
	reader->Update();
	vtkUnstructuredGrid* data = vtkUnstructuredGrid::SafeDownCast(reader->GetOutputDataObject(0));
	if( data == 0 ) return 1;

	cout<<vtk_array_desc(data->GetPoints()->GetData())<<',';
	cout<<vtk_dsa_desc(data->GetCellData())<<',';
	cout<<vtk_dsa_desc(data->GetPointData())<<'\n';
	return 0;
}
