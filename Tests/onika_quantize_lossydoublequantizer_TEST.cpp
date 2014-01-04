#include "onika/quantize/lossydoublequantizer.h"

#include "onika/vtk/readvtkascii.h"
#include "onika/poweroftwo.h"

#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

struct FakeMesh
{
	struct VertexPos
	{
		double x[3];
	};
	inline void addCell(int n, const int* v) { }
	inline void addVertex(const VertexPos& p)
	{
		values.push_back(p.x[0]);
		values.push_back(p.x[1]);
		values.push_back(p.x[2]);
	}
	vector<double> values;
};

int main(int argc, char* argv[])
{
	onika::debug::dbgassert( sizeof(double) == sizeof(uint64_t) );

	bool autoMode = argc > 1 && string("-a") == argv[1];
	std::string fname = "data/tetraMesh.vtk";
	if( !autoMode )
	{
		cout<<"File ? ";
		cout.flush();
		cin>>fname;
	}
	std::ifstream ifile(fname.c_str());
	if( !ifile )
	{
		std::cerr<<"Failed to open "<<fname<<"\n";
		return 1;
	}
	FakeMesh mesh;
	onika::vtk::readVtkAsciiMesh(ifile,mesh);

	vector<uint64_t> buffer(mesh.values.size());

	for(int nbits=8; nbits<=52; nbits++)
	{
		double totalError = 0.0;
		uint64_t n=0;

		onika::quantize::LossyDoubleQuantizer ldq;
		int suggested_bits = ldq.initialize( mesh.values.begin(), mesh.values.end(), 8, nbits );
		ldq.nbits = nbits;

		for(vector<double>::iterator it=mesh.values.begin(); it!=mesh.values.end(); ++it, ++n)
		{
			double x = *it;
			uint64_t q = ldq.encode(x);
			buffer[n] = q;
			double y = ldq.decode(q);
			double err = fabs(y-x);
			totalError += err;
			//printf("%1$G -> %2$llu (0x%2$08llX) -> %3$G : err=%4$G\n",x,q,y,err);
		}
		double avgError = totalError/n;
		double relError = avgError/(ldq.high-ldq.low);
		cout<<nbits<<" Bits : avg error = "<<avgError<<", rel error = "<<relError<<", suggest "<<suggested_bits<<" bits\n";
	}

	return 0;
}

 // end of lossydoublequantizer.h

