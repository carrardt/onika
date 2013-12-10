#include "vtkDataArrayWrapper.h"
#include <vtkDoubleArray.h>
#include <iostream>

template<class... T>
inline std::ostream& operator << ( std::ostream& out, const std::tuple<T...>& t )
{
	onika::tuple::print( out, t );
	return out;
}
template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ConstElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}
template<class T>
inline std::ostream& operator << ( std::ostream& out, onika::container::ElementAccessorT<T> t )
{
	out << t.get() ;
	return out;
}

using onika::container::wrap_vtkarray;
using onika::container::wrap_vtkarray_tuple;
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

	auto myContainer = wrap_vtkarray_tuple<3>( doubleArray );
	cout<<myContainer.size()<<" values: ";
	for( auto x : myContainer ) { cout<<x<<" "; }
	cout<<"\n\n";

	myContainer[5] = std::make_tuple(1.5,2.5,3.5);
	for(int i=0;i<10;i++)
	{
		double t[3];
		doubleArray->GetTuple(i,t);
		cout<<'('<<t[0]<<','<<t[1]<<','<<t[2]<<") ";
	}
	cout<<"\n\n";

	return 0;
}
