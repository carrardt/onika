#include <fstream>

// this solves the mangling problem with c++ functions
extern "C"
{
	extern int justdoit(const char*,int);
}

template<class... T> void voidret( const T&... ) {}

template<class... T>
int myfunction( std::ostream& cout, const T&... args )
{
	voidret( (cout<<args) ... );
	return sizeof...(T);
}

//#define MYDEFINE "<not redefined>"

int justdoit(const char* message,int x)
{
	std::ofstream cout("/dev/stdout");
	return myfunction(cout,message,' ',"World",' ',x,' ',MYDEFINE,std::string("\n"));
}
