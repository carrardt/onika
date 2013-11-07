#include "onika/container/serialvec.h"

#include "onika/vtk/readvtkascii.h"
#include "onika/vec.h"

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

struct FakeMesh
{
	typedef onika::Vec<3,double> VertexPos;
	inline void addCell(int n, const int* v) { }
	inline void addVertex(const VertexPos& p)
	{
		values.push_back(p);
	}
	onika::container::SerialVec<vector<double>,3> values;
};

int main(int argc, char* argv[])
{
	bool autoMode = argc > 1 && string("-a") == argv[1];
	std::string fname = "data/tetraMesh.vtk";
	if( !autoMode )
	{
		cout<<"File ? "; cout.flush();
		cin>>fname;
	}
	ifstream ifile(fname.c_str());
	if( !ifile )
	{
		cerr<<"Failed to open "<<fname<<endl;
		return 1;
	}
	FakeMesh mesh;
	onika::vtk::readVtkAsciiMesh(ifile,mesh);

	return 0;
}

 // end of serialvec.h

