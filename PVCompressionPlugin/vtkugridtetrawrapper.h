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


	template<class CellsT, class PointArraysT , class CellArraysT>
	struct UGridSMeshWrapper // don't reference array wrappers, copy them
	{
		CellsT m_cell2vertex; // connectivity
		CellArraysT m_cellValues; // cell scalars
		PointArraysT m_vertices; // point position and scalars
		inline UGridSMeshWrapper(const CellsT& cells, const PointArraysT& points, const CellArraysT& cellValues)
		: m_cell2vertex(cells), m_vertices(points), m_cellValues(cellValues) {}
	};

	template<class Cells, class Points, class CellValues>
	static inline auto wrap_ugrid_tetra(const Cells& cells, const Points& points, const CellValues& cellValues)
	ONIKA_AUTO_RET( UGridSMeshWrapper<Cells,Points,CellValues>(cells,points,cellValues) )

	template<class... Wrappers>
	static inline auto zip_array_wrappers( vtkUnstructuredGrid* grid, const Wrappers&... wrappers )
	ONIKA_AUTO_RET( zip_vectors_cpy( wrappers.wrap(grid)... ) )

} }

#endif
