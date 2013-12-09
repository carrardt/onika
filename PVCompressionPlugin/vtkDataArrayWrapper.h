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

template<class T>
inline auto wrap_vtkarray_tuple2( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( onika::container::flatten_tuple2_array_wrapper( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

template<class T>
inline auto wrap_vtkarray_tuple3( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( flatten_tuple3_array_wrapper( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

} }

#endif
