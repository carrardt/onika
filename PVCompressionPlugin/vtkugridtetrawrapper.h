#ifndef __pvcompressionplugin_vtkugridtetrawrapper_h
#define __pvcompressionplugin_vtkugridtetrawrapper_h

#include <vtkUnstructuredGrid.h>
#include <vtkCellArray.h>
#include <vtkIdTypeArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkDataArrayTemplate.h>

#include <type_traits>

#include "onika/mesh/cell2vertex.h"
#include "onika/mesh/vertex2cell.h"
#include "onika/mesh/cell2edge.h"
#include "onika/mesh/simplicialmesh.h"
#include "vtkarraywrapper.h"
#include "onika/language.h"
#include "onika/tuple.h"

namespace onika { namespace vtk {

	template<int Dim>
	inline bool allCellsAreSimplicies(vtkUnstructuredGrid* ugrid)
	{
		vtkIdType n = ugrid->GetNumberOfCells();
		vtkIdType* cells = ugrid->GetCells()->GetData()->GetPointer(0);
		for(vtkIdType i=0;i<n;i++)
		{
			vtkIdType nverts = *(cells++);
			if( nverts != (Dim+1) ) return false;
			cells += nverts;
		}
		return true;
	}

	inline bool allCellsAreTetras(vtkUnstructuredGrid* ugrid)
	{
		return allCellsAreSimplicies<3>(ugrid);
	}

	// connectivity wrapper. translates cell description to Onika's Simplicial Mesh View
	template<class Container,int DIM>
	inline auto wrap_ugrid_smesh_c2v( Container& cells, std::integral_constant<int,DIM> )
	ONIKA_AUTO_RET( onika::mesh::C2VWrapper< onika::mesh::c2v_traits< onika::mesh::smesh_c2v_basic_traits<Container,DIM,1> > >(cells) )

	// TODO: improve ReverseC2V construtor to admit additional containers taken with &&
	template<class C2V, class Integer>
	inline auto make_v2c( C2V& c2v, Integer nVerts )
	ONIKA_AUTO_RET( onika::mesh::ReverseC2V<C2V,std::vector<int>,std::vector<unsigned int> >(c2v.c2v,nVerts) )

	// Wraps a vtkDataSetAttribute (CellData or PointData)'s array to an onika container view
	template<class T, int NC, bool CellData,int AI>
	struct DataSetAttribute
	{
		inline auto wrap(vtkUnstructuredGrid * ds) const ONIKA_AUTO_RET(
		  select_vtkarray_wrapper(
			vtkDataArrayTemplate<T>::FastDownCast(
			  (CellData?vtkDataSetAttributes::SafeDownCast(ds->GetCellData()):vtkDataSetAttributes::SafeDownCast(ds->GetPointData()))
			  ->GetArray(AI) ) ,
			ONIKA_CONST(NC)
			)
		  )
	};

	// Wraps point coordinates to an onika container view
	template<class T, int NC=3>
	struct UGridPoints
	{
		inline auto wrap(vtkUnstructuredGrid* grid) const ONIKA_AUTO_RET(
			select_vtkarray_wrapper(
				vtkDataArrayTemplate<T>::FastDownCast(grid->GetPoints()->GetData()) ,
				ONIKA_CONST(NC)
				)
			)
	};

	// Wraps cell connectivity array to an onika container view
	struct UGridCells
	{
		inline auto wrap(vtkUnstructuredGrid* g) const ONIKA_AUTO_RET(
			container::array_wrapper( g->GetCells()->GetData()->GetPointer(1), g->GetCells()->GetData()->GetNumberOfTuples() )
		 	)
	};

	template<
		class VT, int VS, class CT, class CS, class PT, class PS,
		bool Valid=((CT::count==CS::count)&&(PT::count==PS::count))
		>
	struct UGridWrapper {};

	template<class VT, int VS, class CT, class CS, class PT, class PS>
	struct UGridWrapper<VT,VS,CT,CS,PT,PS,true>
	{
		vtkUnstructuredGrid* ugrid;
		static constexpr int NCA = CT::count;
		static constexpr int NPA = PT::count;

		inline UGridWrapper(vtkDataObject* obj)
		{
			ugrid = vtkUnstructuredGrid::SafeDownCast(obj);
			onika::debug::dbgassert(ugrid != 0);
			onika::debug::dbgassert(allCellsAreTetras(ugrid));
		}

	private:
		template<class... T, int ... S, int... I>
		inline auto vertices_aux( tuple::types<T...> , tuple::indices<S...> , tuple::indices<I...> )
		ONIKA_AUTO_RET( zip_vectors_cpy( UGridPoints<VT,VS>().wrap(ugrid), (DataSetAttribute<T,S,false,I>().wrap(ugrid))... ) )
		template<class... T, int ... S, int... I>
		inline auto cellValues_aux( tuple::types<T...> , tuple::indices<S...> , tuple::indices<I...> )
		ONIKA_AUTO_RET( zip_vectors_cpy( (DataSetAttribute<T,S,true,I>().wrap(ugrid))... ) )
	public:
		inline auto cellValues() ONIKA_AUTO_RET( cellValues_aux(CT(),CS(),tuple::make_indices<NCA>()) )
		inline auto vertices() ONIKA_AUTO_RET( vertices_aux(PT(),PS(),tuple::make_indices<NPA>()) )
		inline auto cells() ONIKA_AUTO_RET( UGridCells().wrap(ugrid) )
	};


} }

#endif
