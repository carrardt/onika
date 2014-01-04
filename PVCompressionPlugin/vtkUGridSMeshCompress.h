#ifndef __vtkUGridSMeshCompress_h
#define __vtkUGridSMeshCompress_h

#include <string>
class vtkDataObject;
bool vtkUGridSMeshCompress(vtkDataObject* data, int nedges, const std::string& outfname);

#endif
