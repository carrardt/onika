#include <vtkUnstructuredGrid.h>
#include <vtkCellArray.h>

#include "jitti.h"
#include "vtkUGridDescription.h"

#include <map>
#include <string>
#include <sstream>

using namespace onika::vtk;

#ifndef UGRID_SMESH_COMPRESS_JIT
#error No path to JIT source file
#endif

static std::map< std::string , jitti::Module* > g_modules;

bool vtkUGridSMeshCompress(vtkDataObject* data, int nedges, const char* outfname)
{
	std::cout<<"jit file = "<<UGRID_SMESH_COMPRESS_JIT<<"\n";

	if( data == 0 ) return false;
//	std::cout<<"OutputDataObject:\n";
//	data->PrintSelf(std::cout,vtkIndent(0));

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast( data );
	if( ugrid == 0 ) return false;
	if( ! allCellsAreTetras(ugrid) ) return false;
	std::cout<<"simplicial mesh ok : "<<ugrid->GetNumberOfPoints()<<" points, "<<ugrid->GetNumberOfCells()<<" cells\n";

	vtkUGridDescription ugrid_desc;
	init_ugrid_description( ugrid_desc , ugrid );
	std::cout<<"ugrid initialized\n";

	std::ostringstream oss;
	printUGridSignature( ugrid, oss );
	std::string sig = oss.str();

	jitti::Module* m = 0;
	auto it = g_modules.find( sig );
	if( it != g_modules.end() )
	{
		m = it->second;
	}
	else
	{
		std::string opt("-DUGRID_DESC=" + sig);
		std::cout<<"compiling module ("<<sig<<")\n";
		m = jitti::Compiler::createModuleFromFile(UGRID_SMESH_COMPRESS_JIT,opt.c_str());
		auto module_init = m->getFunction("initialize");
		std::cout<<"initializing module ...\n";
		module_init();
		g_modules[sig] = m;
	}
	std::cout<<"module found @"<<m<<"\n";

	auto compress = m->getFunction( "ugridsmeshcompress" );

	compress( &ugrid_desc, nedges, outfname ) ;
	std::cout<<"done\n";

	return 1;
}

