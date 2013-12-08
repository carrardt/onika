#ifndef __pvcompressionplugin_vtkmeshwrapper_h
#define __pvcompressionplugin_vtkmeshwrapper_h


#include <vtkDataArray.h>
#include <vtkUnstructuredGrid.h>

#include "onika/container/arraywrapper.h"
#include "onika/language.h"

template<class T>
inline auto wrap_array( vtkDataArrayTemplate<T>* dataArray )
ONIKA_AUTO_RET( onika::container::array_wrapper( dataArray.GetPointer(0), dataArray.GetNumberOfTuples() ) )


#endif
