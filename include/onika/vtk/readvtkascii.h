#ifndef __onika_readvtkascii_h
#define __onika_readvtkascii_h

#include <string>
#include <vector>
#include "onika/debug/dbgassert.h"

namespace onika { namespace vtk {

template <typename Stream, typename Mesh>
inline bool readVtkAsciiMesh(Stream& file, Mesh& mesh)
{
	if( !file )
	{
		//std::cout<<"FAILED"<<std::endl;
		return false;
	}

	std::string token;
	do { file>>token; } while(token!="POINTS");
	int nPoints=0;
	file>>nPoints;
	file>>token;
	//std::cout<<nPoints<<" points("<<token<<")"<<std::endl;
	for(int i=0;i<nPoints;i++)
	{
		typename Mesh::VertexPos p;
		file>>p.x[0];
		file>>p.x[1];
		file>>p.x[2];
		mesh.addVertex(p);
	}
	do { file>>token; } while(token!="CELLS");
	int nCells=0;
	file>>nCells;
	file>>token;
	//std::cout<<nCells<<" cells ("<<token<<" values)"<<std::endl;
	for(int i=0;i<nCells;i++)
	{
		int np=0; file>>np;
		int v[np];
		for(int j=0;j<np;j++) { file>>v[j]; }
		mesh.addCell(np,v);
	}
//	cout<<"mesh : "<<mesh.getNumberOfVertices()<<" points, "<<mesh.getNumberOfCells()<<" cells"<<endl<<endl;
	return true;
}

template <typename Stream, typename Mesh>
inline bool readVtkAsciiScalars(Stream& file, Mesh& mesh)
{
	if( !file )
	{
		std::cout<<"FAILED"<<std::endl;
		return false;
	}

	std::string token;
	bool eof = false;
	while( !eof )
	{
		do { token.clear(); file>>token; }
		while( token!="" && token!="CELL_DATA" && token!="POINT_DATA" );
		//std::cout<<"found "<<token<<"\n";
		if( token=="CELL_DATA" || token=="POINT_DATA" )
		{
			bool cellData = (token=="CELL_DATA");
			int nValues=0;
			file>>nValues;
			file>>token; 
			std::string name;
			file>>name;
			file>>token>>token>>token;
			//std::cout<<nValues<<" values (name='"<<name<<"')"<<std::endl;
			std::vector<double> values(nValues);
			for(typename std::vector<double>::iterator it=values.begin(); it!=values.end(); ++it)
			{
				file >> (*it);
			}
			//for_each(values.begin(),values.end(), [file](double& x) {file>>x;} );
			if( cellData )
			{
				onika::debug::dbgassert(values.size()==mesh.getNumberOfCells());
				mesh.addCellScalars(name,values.begin(),values.end());
			}
			else
			{
				onika::debug::dbgassert(values.size()==mesh.getNumberOfVertices());
				mesh.addVertexScalars(name,values.begin(),values.end());
			}
		}
		else { eof = true; }
	}
	return true;
}


} } // end of namespace onika::vtk

#endif

