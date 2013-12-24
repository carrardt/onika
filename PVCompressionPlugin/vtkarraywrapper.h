#ifndef __pvcompressionplugin_vtkarraywrapper_h
#define __pvcompressionplugin_vtkarraywrapper_h

#include <vtkDataArrayTemplate.h>

#include "onika/container/arraywrapper.h"
#include "onika/container/tuplevec.h"
#include "onika/language.h"
#include <type_traits>

namespace onika { namespace vtk {

template<class T>
inline auto wrap_vtkarray( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( container::array_wrapper( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

template<unsigned int N,class T>
inline auto wrap_vtkarray_tuple_rev( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( container::array_wrapper( reinterpret_cast<onika::tuple::uniform_tuple<T,N>*>(dataArray->GetPointer(0)), dataArray->GetNumberOfTuples() ) )

template<unsigned int N,class T>
inline auto wrap_vtkarray_tuple( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( container::flat_tuple_array_wrapper<N>( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

template<class T, unsigned int N=1>
struct ArrayWrapperSelector
{
	static inline auto wrap(vtkDataArrayTemplate<T>* array) ONIKA_AUTO_RET( wrap_vtkarray_tuple_rev<N,T>(array) )
};
template<class T>
struct ArrayWrapperSelector<T,1>
{
	static inline auto wrap(vtkDataArrayTemplate<T>* array) ONIKA_AUTO_RET( wrap_vtkarray(array) )
};

template<class T,int NC>
inline auto select_vtkarray_wrapper( vtkDataArrayTemplate<T>* dataArray, std::integral_constant<int,NC> )
ONIKA_AUTO_RET( ArrayWrapperSelector<T,NC>::wrap( dataArray ) )

} }

#endif
