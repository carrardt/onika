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

struct UGRidSMeshCompressionModule
{
	jitti::Module module;
	jitti::Function function;

	inline UGRidSMeshCompressionModule()
		{ }

	inline UGRidSMeshCompressionModule( UGRidSMeshCompressionModule&& cm )
		: module( std::move(cm.module) )
		, function( std::move(cm.function) )
		{ }
	
	inline UGRidSMeshCompressionModule( jitti::Module&& m, jitti::Function&& f )
		: module(m)
		, function(f)
		{ }

	inline  UGRidSMeshCompressionModule& operator = ( UGRidSMeshCompressionModule&& cm )
	{
		module = std::move( cm.module ) ;
		function = std::move( cm.function ) ;
	}
	
	static jitti::Function& getFunction( const std::string& sig )
	{
		auto it = m_modules.find( sig );
		if( it != m_modules.end() )
		{
			return it->second.function;
		}

		std::string opt = "-DUGRID_DESC=" + sig ;
		auto m = jitti::Compiler::createModuleFromFile(UGRID_SMESH_COMPRESS_JIT,opt.c_str());
		auto f = m.getFunction("ugridsmeshcompress");
		m_modules[sig] = UGRidSMeshCompressionModule( std::move(m) ,std::move(f) ) ;
		return m_modules[sig].function; 
	}

	static std::map< std::string , UGRidSMeshCompressionModule > m_modules;

private:
	UGRidSMeshCompressionModule& operator = ( const UGRidSMeshCompressionModule& cm );
	//UGRidSMeshCompressionModule( const UGRidSMeshCompressionModule& cm );
};

std::map< std::string , UGRidSMeshCompressionModule > UGRidSMeshCompressionModule::m_modules;

bool vtkUGridSMeshCompress(vtkDataObject* data, int nedges, const char* outfname)
{
	std::cout<<"jit file = "<<UGRID_SMESH_COMPRESS_JIT<<"\n";

	if( data == 0 ) return false;
//	std::cout<<"OutputDataObject:\n";
//	data->PrintSelf(std::cout,vtkIndent(0));

	vtkUnstructuredGrid* ugrid = vtkUnstructuredGrid::SafeDownCast( data );
	if( ugrid == 0 ) return false;
	if( ! allCellsAreTetras(ugrid) ) return false;

	vtkUGridDescription ugrid_desc;
	init_ugrid_description( ugrid_desc , ugrid );
	std::cout<<"ugrid initialized\n";

	std::ostringstream oss;
	printUGridSignature( ugrid, oss );
	std::cout<<"signature = "<<oss.str()<<"\n";

	auto compress = UGRidSMeshCompressionModule::getFunction( oss.str() );	

	compress( &ugrid_desc, nedges, outfname ) ;
	//std::cout<<"result = "<<r<<"\n";

	return 1;
}

