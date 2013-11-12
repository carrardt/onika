#include "onika/poweroftwo.h"

#include <iostream>
using namespace std;
using namespace onika;
int main()
{
	cout<<"NextPowOfTwoLog(0) = "<< NextPowOfTwoLog<0>::value <<endl;
	cout<<"NextPowOfTwoLog(1) = "<< NextPowOfTwoLog<1>::value <<endl;
	cout<<"NextPowOfTwoLog(2) = "<< NextPowOfTwoLog<2>::value <<endl;
	cout<<"NextPowOfTwoLog(3) = "<< NextPowOfTwoLog<3>::value <<endl;
	cout<<"NextPowOfTwoLog(4) = "<< NextPowOfTwoLog<4>::value <<endl;
	cout<<"NextPowOfTwoLog(7) = "<< NextPowOfTwoLog<7>::value <<endl;
	cout<<"NextPowOfTwoLog(33) = "<< NextPowOfTwoLog<33>::value <<endl;
	cout<<"NextPowOfTwoLog(5177) = "<< NextPowOfTwoLog<5177>::value <<endl;
	int x=0;
	cin>>x;
	cout<<"nextPowerOfTwoLog("<<x<<") = "<<nextpo2log(x)<<endl;
	return 0;
}


