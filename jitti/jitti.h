#include <cstdint>

namespace llvm
{
	class Module;
	class ExecutionEngine;
}

namespace jitti
{
	template<class... ArgsT> struct PushArgs;
	struct FunctionPriv;

	class Function
	{
	public:
		inline Function() : priv(0) { }
		inline Function( Function&& f) : priv(f.priv) { f.priv=0; }
		inline Function( const Function& f ) : priv(f.priv) {}
		inline Function( FunctionPriv* p) : priv(p) { }
		inline Function& operator = ( Function&& f) { priv=f.priv; f.priv=0; }

		void resetCallArgs();
		void pushArgPtr(void * ptr);
		void pushArgInt(const int & x); 
		void pushArgUInt(const unsigned int & x);
		void pushArgLong(const long long int & x);
		void pushArgULong(const unsigned long long int & x);
		void pushArgFloat(const float & x); 
		void pushArgDouble(const double & x);
		void call();
		void* getReturnValueAsPtr();
		uint64_t getReturnValueAsULong();
		float getReturnValueAsFloat();
		double getReturnValueAsDouble();

		template<class... ArgT>
		uint64_t operator () (const ArgT&... args)
		{
			resetCallArgs();
			PushArgs<ArgT...>::push_args(*this,args...);
			call();
			return getReturnValueAsULong();
		}
		
	private:
		FunctionPriv* priv;
	};

	template<class... ArgsT> struct PushArgs {};
	template<class ArgT0, class... ArgsT> struct PushArgs<ArgT0,ArgsT...>
	{
		template<class T> static inline void push_arg( Function& f, T * ptr ) {	f.pushArgPtr( (void*)(ptr) ); }
		template<class T> static inline void push_arg( Function& f, T & ref ) {	f.pushArgPtr( (void*)(&ref) ); }
		static inline void push_arg(Function& f, const int& x) { f.pushArgInt( x ); }
		static inline void push_arg(Function& f, const unsigned int& x) { f.pushArgUInt( x ); }
		static inline void push_arg(Function& f, const long long int& x) { f.pushArgLong( x ); }
		static inline void push_arg(Function& f, const unsigned long long int& x) { f.pushArgULong( x ); }
		static inline void push_arg(Function& f, const float& x) { f.pushArgFloat( x ); }
		static inline void push_arg(Function& f, const double& x) { f.pushArgDouble( x ); }
		static inline void push_args(Function& f, const ArgT0& a0, const ArgsT&... ai)
		{
			push_arg(f,a0);
			PushArgs<ArgsT...>::push_args(f,ai...);
		}
	};
	template<> struct PushArgs<>
	{
		static inline void push_args(Function& f) { }
	};

	struct ModulePriv;

	class Module
	{
	public:
		inline Module() : priv(0) {}
		Module( const Module& m );
		inline Module( Module&& m ) : priv(m.priv) { m.priv=0; }
		Module( llvm::Module* mod , char** args = 0 );
		~Module();
		inline Module& operator = ( Module&& m ) { priv=m.priv; m.priv=0; }
		Function getFunction(const char* name);
	private:
		ModulePriv* priv;
	};

	class Compiler
	{
	public:
		static Module createModuleFromFile(const char* filePath,const char* opt_args="");
	};
}

