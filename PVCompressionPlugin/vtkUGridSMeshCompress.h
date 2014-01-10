#ifndef __vtkUGridSMeshCompress_h
#define __vtkUGridSMeshCompress_h

class vtkDataObject;
bool vtkUGridSMeshCompress(vtkDataObject* data, int nedges, const char* outfname);

#endif
