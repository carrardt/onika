#include <iostream>
#include <fstream>

// this solves the mangling problem with c++ functions
extern "C"
{
	extern int justdoit(char* message);
}

template<class... T> void voidret( const T&... ) {}

template<class... T>
int myfunction( std::ostream& cout, const T&... args )
{
	voidret( (cout<<args) ... );
	return sizeof...(T);
}

int justdoit(char* message)
{
	std::ofstream cout("/dev/stdout");
	cout<<message<<"\n";
	return myfunction(cout,"Hello",' ',"World",' ',4,std::string("\n"));
}
