#ifndef __pvcompressionplugin_vtkarraywrapper_h
#define __pvcompressionplugin_vtkarraywrapper_h

#include <vtkDataArrayTemplate.h>

#include "onika/container/arraywrapper.h"
#include "onika/container/tuplevec.h"
#include "onika/language.h"

namespace onika { namespace vtk {

template<class T>
inline auto wrap_vtkarray( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( container::array_wrapper( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

template<unsigned int N,class T>
inline auto wrap_vtkarray_tuple( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( container::flat_tuple_array_wrapper<N>( dataArray->GetPointer(0), dataArray->GetNumberOfTuples() ) )

} }

#endif
