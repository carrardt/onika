#include <vtkXMLUnstructuredGridReader.h>
#include <sys/time.h>
#include "vtkUGridSMeshCompress.h"

using std::cout;
int main(int argc, char* argv[])
{
	std::string outfname="/dev/stdout";
	std::string infname="/dev/stdin";
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
		else if( std::string(argv[i]) == "-i" )
		{
			++i; infname = argv[i];
		}
	}

	std::cout<<"input = "<<infname<<"\n";
	std::cout<<"output = "<<outfname<<"\n";
	std::cout<<"edges = "<<nedges<<"\n";

	vtkXMLUnstructuredGridReader* reader = vtkXMLUnstructuredGridReader::New();
	reader->SetFileName(infname.c_str());
	reader->Update();
	vtkDataObject* data = reader->GetOutputDataObject(0);

	struct timeval T1; gettimeofday(&T1,NULL);

	vtkUGridSMeshCompress( data , nedges, outfname.c_str() );

	struct timeval T2; gettimeofday(&T2,NULL);
	cout<<"execution time : "<<(T2.tv_sec-T1.tv_sec)*1000.0 + (T2.tv_usec-T1.tv_usec)*0.001<<" ms\n";

	return 0;
}
