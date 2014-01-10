#ifndef __PVCompressionPlugin_ugriddesc_h
#define __PVCompressionPlugin_ugriddesc_h

namespace onika { namespace vtk {

	using vtkIdType = long long;

	static constexpr int VTK_VOID 	     = 0;
	static constexpr int VTK_BIT             = 1;
	static constexpr int VTK_CHAR            = 2;
	static constexpr int VTK_SIGNED_CHAR    = 15;
	static constexpr int VTK_UNSIGNED_CHAR   = 3;
	static constexpr int VTK_SHORT           = 4;
	static constexpr int VTK_UNSIGNED_SHORT  = 5;
	static constexpr int VTK_INT             = 6;
	static constexpr int VTK_UNSIGNED_INT    = 7;
	static constexpr int VTK_LONG            = 8;
	static constexpr int VTK_UNSIGNED_LONG   = 9;
	static constexpr int VTK_FLOAT          = 10;
	static constexpr int VTK_DOUBLE         = 11;
	static constexpr int VTK_ID_TYPE        = 12;
	static constexpr int VTK_LONG_LONG          = 16;
	static constexpr int VTK_UNSIGNED_LONG_LONG = 17;

	template<class T> struct TypeEnum { static constexpr int value = VTK_VOID; };
	template<> struct TypeEnum<unsigned char> { static constexpr int value = VTK_UNSIGNED_CHAR; };
	template<> struct TypeEnum<signed char> { static constexpr int value = VTK_SIGNED_CHAR; };
	template<> struct TypeEnum<short> { static constexpr int value = VTK_SHORT; };
	template<> struct TypeEnum<unsigned short> { static constexpr int value = VTK_UNSIGNED_SHORT; };
	template<> struct TypeEnum<int> { static constexpr int value = VTK_INT; };
	template<> struct TypeEnum<unsigned int> { static constexpr int value = VTK_UNSIGNED_INT; };
	template<> struct TypeEnum<long> { static constexpr int value = VTK_LONG; };
	template<> struct TypeEnum<unsigned long> { static constexpr int value = VTK_UNSIGNED_LONG; };
	template<> struct TypeEnum<float> { static constexpr int value = VTK_FLOAT; };
	template<> struct TypeEnum<double> { static constexpr int value = VTK_DOUBLE; };
	template<> struct TypeEnum<vtkIdType> { static constexpr int value = VTK_ID_TYPE; };
	template<> struct TypeEnum<long long> { static constexpr int value = VTK_LONG_LONG; };
	template<> struct TypeEnum<unsigned long long> { static constexpr int value = VTK_UNSIGNED_LONG_LONG; };

	struct vtkArrayDescription
	{
		void* ptr;
		int type, components;
		char name[256];
	};

	template<class T>
	static T* safeCastPtr(const vtkArrayDescription& ad)
	{
		if( TypeEnum<T>::value != ad.type ) { return 0; }
		else return static_cast<T*>( ad.ptr );
	}

	struct vtkUGridDescription
	{
		vtkArrayDescription cellArrays[256];
		vtkArrayDescription vertexArrays[256]; // vertex position must be stored first
		vtkArrayDescription mesh;
		vtkIdType meshSize, nCells, nVertices;
		int nVertexArrays, nCellArrays;
		inline vtkUGridDescription() : meshSize(0), nCells(0), nVertices(0), nVertexArrays(0), nCellArrays(0) {}

		template<class StreamT>
		static inline void printTemplateSignature(StreamT& out)
		{
		}
	};

} }

#endif

