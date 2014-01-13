#ifndef __PVCompressionPlugin_ugriddesc_h
#define __PVCompressionPlugin_ugriddesc_h

#include <string>
#include <vector>
#include "onika/sysio.h"

namespace onika { namespace vtk {

	using vtkIdType = long long;

	static constexpr int vtk_VTK_VOID 	 = 0;
	static constexpr int vtk_VTK_BIT             = 1;
	static constexpr int vtk_VTK_CHAR            = 2;
	static constexpr int vtk_VTK_SIGNED_CHAR    = 15;
	static constexpr int vtk_VTK_UNSIGNED_CHAR   = 3;
	static constexpr int vtk_VTK_SHORT           = 4;
	static constexpr int vtk_VTK_UNSIGNED_SHORT  = 5;
	static constexpr int vtk_VTK_INT             = 6;
	static constexpr int vtk_VTK_UNSIGNED_INT    = 7;
	static constexpr int vtk_VTK_LONG            = 8;
	static constexpr int vtk_VTK_UNSIGNED_LONG   = 9;
	static constexpr int vtk_VTK_FLOAT          = 10;
	static constexpr int vtk_VTK_DOUBLE         = 11;
	static constexpr int vtk_VTK_ID_TYPE        = 12;
	static constexpr int vtk_VTK_LONG_LONG          = 16;
	static constexpr int vtk_VTK_UNSIGNED_LONG_LONG = 17;

	template<class T> struct TypeEnum { static constexpr int value = vtk_VTK_VOID; };
#define ONIKA_VTK_TYPE_MACRO(T,E) template<> struct TypeEnum<T> { static constexpr int value = vtk_VTK_##E; }
	ONIKA_VTK_TYPE_MACRO(unsigned char,UNSIGNED_CHAR);
	ONIKA_VTK_TYPE_MACRO(signed char,SIGNED_CHAR);
	ONIKA_VTK_TYPE_MACRO(short,SHORT);
	ONIKA_VTK_TYPE_MACRO(unsigned short,UNSIGNED_SHORT);
	ONIKA_VTK_TYPE_MACRO(int,INT);
	ONIKA_VTK_TYPE_MACRO(unsigned int,UNSIGNED_INT);
	ONIKA_VTK_TYPE_MACRO(long,LONG);
	ONIKA_VTK_TYPE_MACRO(unsigned long,UNSIGNED_LONG);
	ONIKA_VTK_TYPE_MACRO(float,FLOAT);
	ONIKA_VTK_TYPE_MACRO(double,DOUBLE);
	//ONIKA_VTK_TYPE_MACRO(vtkIdType,ID_TYPE);
	ONIKA_VTK_TYPE_MACRO(long long,LONG_LONG);
	ONIKA_VTK_TYPE_MACRO(unsigned long long,UNSIGNED_LONG_LONG);
#undef ONIKA_VTK_TYPE_MACRO
	static inline const char* typeString(int value)
	{
		switch(value)
		{
#define ONIKA_VTK_TYPE_MACRO(T,E) case vtk_VTK_##E: return #T;
	ONIKA_VTK_TYPE_MACRO(void,VOID);
	ONIKA_VTK_TYPE_MACRO(unsigned char,UNSIGNED_CHAR);
	ONIKA_VTK_TYPE_MACRO(signed char,SIGNED_CHAR);
	ONIKA_VTK_TYPE_MACRO(short,SHORT);
	ONIKA_VTK_TYPE_MACRO(unsigned short,UNSIGNED_SHORT);
	ONIKA_VTK_TYPE_MACRO(int,INT);
	ONIKA_VTK_TYPE_MACRO(unsigned int,UNSIGNED_INT);
	ONIKA_VTK_TYPE_MACRO(long,LONG);
	ONIKA_VTK_TYPE_MACRO(unsigned long,UNSIGNED_LONG);
	ONIKA_VTK_TYPE_MACRO(float,FLOAT);
	ONIKA_VTK_TYPE_MACRO(double,DOUBLE);
	//ONIKA_VTK_TYPE_MACRO(vtkIdType,ID_TYPE);
	ONIKA_VTK_TYPE_MACRO(long long,LONG_LONG);
	ONIKA_VTK_TYPE_MACRO(unsigned long long,UNSIGNED_LONG_LONG);
#undef ONIKA_VTK_TYPE_MACRO
		default: return "<type error>";
		}
	}

	struct vtkArrayDescription
	{
		void* ptr;
		int type, components;
		std::string name;
		inline vtkArrayDescription() : ptr(0), type(vtk_VTK_VOID), components(0) {}
		template<class StreamT>
		inline StreamT& print(StreamT& out)
		{
			out<<"ptr="<<ptr<<",type="<<typeString(type)<<",nc="<<components<<",name='"<<name<<"'";
			return out;
		}
	};

	template<class T>
	static T* safeCastPtr(const vtkArrayDescription& ad)
	{
		if( TypeEnum<T>::value != ad.type )
		{
			onika::sys::err() << "Array '"<<ad.name<<"' has type "<<ad.type<<"("<<typeString(ad.type)
			<<") but type "<<typeString(TypeEnum<T>::value)<<" expected\n";
			onika::sys::err().flush();
			return 0;
		}
		else return static_cast<T*>( ad.ptr );
	}

	struct vtkUGridDescription
	{
		std::vector<vtkArrayDescription> cellArrays;
		std::vector<vtkArrayDescription>  vertexArrays; // vertex position must be stored first
		vtkArrayDescription mesh;
		vtkIdType meshSize, nCells, nVertices;
		inline vtkUGridDescription() : meshSize(0), nCells(0), nVertices(0) {}

		template<class StreamT>
		inline StreamT& print(StreamT& out)
		{
			out<<"mesh: "; mesh.print(out); out<<" (size="<<meshSize<<")\n";
			out<<cellArrays.size()<<" cell arrays, "<<nCells<<" cells\n";
			for(int i=0;i<cellArrays.size();i++) { out<<i<<": "; cellArrays[i].print(out); out<<"\n"; }
			out<<vertexArrays.size()<<" vertex arrays, "<<nVertices<<" vertices\n";
			for(int i=0;i<vertexArrays.size();i++) { out<<i<<": "; vertexArrays[i].print(out); out<<"\n"; }
			return out;
		}
	};

} }

#endif

