#include "onika/container/quantizedvector.h"

#include "onika/vtk/readvtkascii.h"
#include "onika/quantize/lossydoublequantizer.h"
#include "onika/vec.h"

#include <iostream>
#include <fstream>
using namespace std;

typedef onika::quantize::LossyDoubleQuantizer Quantizer;
typedef onika::container::QuantizedVector<Quantizer> QRealVector;

struct FakeMesh
{
	typedef onika::Vec<3,double> VertexPos;
	FakeMesh(double l, double h, int nb=52)
	{
		values.data.clear(nb);
		values.quantizer.low=l;
		values.quantizer.high=h;
		values.quantizer.nbits=nb;
	}
	inline void addCell(int n, const int* v) { }
	inline void addVertex(const VertexPos& p)
	{
		values.push_back(p[0]);
		values.push_back(p[1]);
		values.push_back(p[2]);
	}
	QRealVector values;
};

int main(int argc, char* argv[])
{
	bool autoMode = argc > 1 && string("-a") == argv[1];
	std::string fname = "data/tetraMesh.vtk";
	double Min=-10.0, Max=10.0;
	int nbits=52;
	if( !autoMode )
	{
		cout<<"File ? "; cout.flush();
		cin>>fname;
		cout<<"Min Max ? "; cout.flush();
		cin>>Min>>Max;
		cout<<"NBits ? "; cout.flush();
		cin>>nbits;
	}

	ifstream ifile(fname.c_str());
	if( !ifile )
	{
		cerr<<"Failed to open "<<fname<<endl;
		return 1;
	}

	FakeMesh mesh(Min,Max,nbits);
	onika::vtk::readVtkAsciiMesh(ifile,mesh);
	
	return 0;
}
 // end of quantizedvector.h

