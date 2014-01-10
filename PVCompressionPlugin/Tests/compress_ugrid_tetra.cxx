#include <vtkXMLUnstructuredGridReader.h>
#include <sys/time.h>
#include "vtkUGridSMeshCompress.h"
#include "testdata.h"

using std::cout;
int main(int argc, char* argv[])
{
	std::string outfname="/dev/stdout";
	int nedges = 1;

	for(int i=1;i<argc;++i)
	{
		if( std::string(argv[i]) == "-c" )
		{
			++i;
			nedges = atoi(argv[i]);
		}
		else if( std::string(argv[i]) == "-o" )
		{
			++i; outfname = argv[i];
		}
	}

	std::cout<<"output = "<<outfname<<"\n";
	std::cout<<"edges = "<<nedges<<"\n";

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(UGRID_FILE);
	reader->Update();
	vtkDataObject* data = reader->GetOutputDataObject(0);

	struct timeval T1; gettimeofday(&T1,NULL);

	vtkUGridSMeshCompress( data , nedges, outfname.c_str() );

	struct timeval T2; gettimeofday(&T2,NULL);
	cout<<"execution time : "<<(T2.tv_sec-T1.tv_sec)*1000.0 + (T2.tv_usec-T1.tv_usec)*0.001<<" ms\n";

	return 0;
}
