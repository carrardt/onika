#ifndef __pvcompressionplugin_vtkdataarraywrapper_h
#define __pvcompressionplugin_vtkdataarraywrapper_h

#include <vtkDataArrayTemplate.h>

#include "onika/container/arraywrapper.h"
#include "onika/container/tuplevec.h"
#include "onika/language.h"

namespace onika { namespace container {

template<class T>
inline auto wrap_vtkarray( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( array_wrapper( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

template<unsigned int N,class T>
inline auto wrap_vtkarray_tuple( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( onika::container::flat_tuple_array_wrapper<N>( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

} }

#endif
