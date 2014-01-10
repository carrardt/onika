#ifndef __pvcompressionplugin_vtkugridtetrawrapper_h
#define __pvcompressionplugin_vtkugridtetrawrapper_h

#include <type_traits>

#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "onika/language.h"
#include "onika/tuple.h"
#include "ugriddesc.h"

namespace onika { namespace vtk {

	template<class T, int N=1>
	struct ArrayWrapperSelector
	{
		static inline auto wrap(T* array, vtkIdType n)
		ONIKA_AUTO_RET(
			container::flat_tuple_array_wrapper<N>( array, n )
			//container::array_wrapper( reinterpret_cast<onika::tuple::uniform_tuple<T,N>*>(array), n )
		)
	};
	template<class T>
	struct ArrayWrapperSelector<T,1>
	{
		static inline auto wrap(T* array, vtkIdType n)
		ONIKA_AUTO_RET(
			container::array_wrapper( array , n )
		)
	};

	template<class T,int NC>
	inline auto make_array_wrapper( T* ptr, std::integral_constant<int,NC>, vtkIdType n )
	ONIKA_AUTO_RET( ArrayWrapperSelector<T,NC>::wrap( ptr , n ) )

	// Wraps a vtkDataSetAttribute (CellData or PointData)'s array to an onika container view
	template<class T, int NC, bool CellData,int AI>
	struct DataSetAttribute
	{
		static inline auto wrap( vtkUGridDescription& d ) const
		ONIKA_AUTO_RET(
		  make_array_wrapper( safeCastPtr<T>( CellData ? d.cellArrays[AI] : d.vertexArrays[AI] ) ,
				      ONIKA_CONST(NC) ,
				      CellData ? d.nCells : d.nVertices )
		  )
	};

	struct UGridCells
	{
		static inline auto wrap( vtkUGridDescription& d ) const
		ONIKA_AUTO_RET( container::array_wrapper( safeCastPtr<vtkIdType>( d.mesh ) , d.meshSize ) )
	};

	template< class CT, class CS, class PT, class PS,
		  bool Valid=((CT::count==CS::count)&&(PT::count==PS::count)) >
	struct UGridWrapper {};

	template<class CT, class CS, class PT, class PS>
	struct UGridWrapper<CT,CS,PT,PS,true>
	{
		const vtkUGridDescription& ugrid;
		static constexpr int NCA = CT::count;
		static constexpr int NPA = PT::count;

		inline UGridWrapper(const vtkUGridDescription& d ) : ugrid( d ) {}

	private:
		template<class... T, int ... S, int... I>
		inline auto vertices_aux( tuple::types<T...> , tuple::indices<S...> , tuple::indices<I...> )
		ONIKA_AUTO_RET( zip_vectors_cpy( (DataSetAttribute<T,S,false,I>().wrap(ugrid))... ) )

		template<class... T, int ... S, int... I>
		inline auto cellValues_aux( tuple::types<T...> , tuple::indices<S...> , tuple::indices<I...> )
		ONIKA_AUTO_RET( zip_vectors_cpy( (DataSetAttribute<T,S,true,I>::wrap(ugrid))... ) )

	public:
		inline auto cellValues() ONIKA_AUTO_RET( this->cellValues_aux(CT(),CS(),tuple::make_indices<NCA>()) )
		inline auto vertices() ONIKA_AUTO_RET( this->vertices_aux(PT(),PS(),tuple::make_indices<NPA>()) )
		inline auto cells() ONIKA_AUTO_RET( UGridCells::wrap(ugrid) )
	};

	// connectivity wrapper. translates cell description to Onika's Simplicial Mesh View
	template<class Container,int DIM>
	inline auto wrap_ugrid_smesh_c2v( Container& cells, std::integral_constant<int,DIM> )
	ONIKA_AUTO_RET( onika::mesh::C2VWrapper< onika::mesh::c2v_traits< onika::mesh::smesh_c2v_basic_traits<Container,DIM,1,0> > >(cells) )

	// TODO: improve ReverseC2V construtor to admit additional containers taken with &&
	// TODO: move to onika/mesh/vertex2cell.h
	template<class C2V, class Integer>
	inline auto make_v2c( C2V& c2v, Integer nVerts )
	ONIKA_AUTO_RET( onika::mesh::ReverseC2V<C2V,std::vector<int>,std::vector<unsigned int> >(c2v.c2v,nVerts) )

} }

#endif
