#include "vtkDataArrayWrapper.h"
#include <vtkDoubleArray.h>
#include <iostream>

using onika::container::wrap_vtkarray;
using std::cout;

int main()
{
	vtkDoubleArray* doubleArray = vtkDoubleArray::New();
	doubleArray->SetNumberOfComponents(3);
	doubleArray->SetNumberOfTuples(10);
	doubleArray->FillComponent(0,0.1);
	doubleArray->FillComponent(1,0.2);
	doubleArray->FillComponent(2,0.3);
	doubleArray->WritePointer(0,10);

	auto myContainer = wrap_vtkarray( doubleArray );
	cout<<myContainer.size()<<" values: ";
	for( auto x : myContainer ) { cout<<x<<" "; }
	cout<<"\n";

	return 0;
}
