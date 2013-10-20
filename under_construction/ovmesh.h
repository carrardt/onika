#ifndef __onika_ovmesh_h
#define __onika_ovmesh2c_h

#include <vector>
#include <cstdint>
#include <algorithm>

#include "debug/dbgassert.h"
#include "debug/dbgmessage.h"


namespace onika
{

template<typename _BaseMesh, typename _CompareOp>
struct OrderedVerticesMesh
{
	typedef _CompareOp CompareOp;
	typedef typename _BaseMesh::IdType IdType;
	typedef typename _BaseMesh::IdTypeVector CellIdVector;
	typedef typename _BaseMesh::VertexPos VertexPos;
	typedef _BaseMesh BaseMesh;

	enum {
		NDim = _BaseMesh::NDim
	};

	inline OrderedVerticesMesh(BaseMesh& bm, CompareOp& cmp) : base(bm), vertex_compare(cmp)
	{
		std::make_heap( base.verticesBegin(), base.verticesEnd(), vertex_compare );
	}

	// =================================================================================
	// ============================ compatibility ======================================
	// =================================================================================
	inline bool cellHasVertex(IdType cellId, IdType v) const { return base.cellHasVertex(cellId,v); }
	inline IdType getNumberOfVertices() const { return base.getNumberOfVertices(); }
	inline IdType getNumberOfCells() const { return base.getNumberOfCells(); }
	inline IdType getTotalNumberOfCellVertices() const { return base.getTotalNumberOfCellVertices(); }
	inline IdType getCellNumberOfVertices(IdType i) const { return base.getCellNumberOfVertices(i); }
	inline IdType getCellVertexIdLocation(IdType cell, IdType cellvert) const { return base.getCellVertexIdLocation(cell,cellvert); } 
	inline IdType getCellFromVertexIdLocation(IdType i) const { return base.getCellFromVertexIdLocation(i); }
	inline IdType getCellVertexId(IdType cell,IdType i) const { return base.getCellVertexId(cell,i); }
	inline VertexPos getVertexPos(IdType i) const { return base.getVertexPos(i); }
	inline void setCellVertexId(IdType cell,IdType i,IdType j) { base.setCellVertexId(cell,i,j); }
	inline typename VertexPos::LengthType cellMinEdgeLength(IdType c) const { return base.cellMinEdgeLength(c); }
	inline void swapCells(IdType a, IdType b) { base.swapCells(a,b); }

	// attention, après ca, seuls les N-1 éléments sont un tas
	inline void moveBackVertex(IdType v)
	{
		IdType last = base.getNumberOfVertices()-1;
		base.swapVertex(v,last);
		std::pop_heap();
	}

        // =================================================================================
        // ============================ mergeLastVertex ====================================
        // =================================================================================
        template<typename Iterator,typename Stream=codec::NullStream>
        inline void mergeLastVertex(IdType a, Iterator cellsBegin,Iterator cellsEnd, Stream& out)
        {
        }

	// =================================================================================
	// ============================ members ============================================
	// =================================================================================
	BaseMesh& base;
	CompareOp& vertex_compare;
};


}; // namespace onika

#endif


// =================================================================================
// ============================ UNIT TEST ==========================================
// =================================================================================

#ifdef meshv2c_TEST

#include <string>
#include <iostream>

#include "vec.h"
#include "simplicialmesh.h"
#include "vtk/readvtkascii.h"
#include "codec/asciistream.h"
#include <fstream>

typedef onika::Vec<3,double> vec3;
typedef onika::SimplicialMesh< 3, vec3, std::vector<vec3>, int, std::vector<int> > BaseMesh;
typedef onika::MeshV2C<BaseMesh> Mesh;

using namespace std;

void testMeshAccess(Mesh & mesh, int a, int b)
{
	int Va[Mesh::MAX_ADJACENT_CELLS], VaSize=0;
	mesh.getVertexAdjacentVertices(a,Va,VaSize);
	cout<<"Adjacent vertices of V"<<a<<" = {"; for(int i=0;i<VaSize;i++) cout<<" "<<Va[i]; cout<<" } Size="<<VaSize<<endl;

	int Qa[Mesh::MAX_ADJACENT_CELLS], QaSize=0;
	mesh.getVertexAdjacentCells(a,Qa,QaSize);
	cout<<"Adjacent cells of V"<<a<<" = {";
	for(int i=0;i<QaSize;i++) cout<<" "<<Qa[i]<<(mesh.cellHasVertex(Qa[i],a)?"":"x");
	cout<<" } Size="<<QaSize<<endl;

	int Vb[Mesh::MAX_ADJACENT_CELLS], VbSize=0;
	mesh.getVertexAdjacentVertices(b,Vb,VbSize);
	cout<<"Adjacent vertices of V"<<b<<" = {"; for(int i=0;i<VbSize;i++) cout<<" "<<Vb[i]; cout<<" } Size="<<VbSize<<endl;

	int Qb[Mesh::MAX_ADJACENT_CELLS], QbSize=0;
	mesh.getVertexAdjacentCells(b,Qb,QbSize);
	cout<<"Adjacent cells of V"<<b<<" = {";
	for(int i=0;i<QbSize;i++) cout<<" "<<Qb[i]<<(mesh.cellHasVertex(Qb[i],b)?"":"x");
	cout<<" } Size="<<QbSize<<endl;

	int Vab[Mesh::MAX_ADJACENT_CELLS], VabSize=0;
	mesh.getEdgeAdjacentVertices(a,b,Vab,VabSize);
	cout<<"Adjacent vertices of ("<<a<<","<<b<<") = {"; for(int i=0;i<VabSize;i++) cout<<" "<<Vab[i]; cout<<" } Size="<<VabSize<<endl;

	int Qab[Mesh::MAX_ADJACENT_CELLS], QabSize=0;
	mesh.getEdgeAdjacentCells(a,b,Qab,QabSize);
	cout<<"Adjacent cells of ("<<a<<","<<b<<") = {"; for(int i=0;i<QabSize;i++) cout<<" "<<Qab[i]; cout<<" } Size="<<QabSize<<endl;
}

int main()
{
	string fname;
	cout<<"File ? "; cout.flush();
	cin>>fname;

	BaseMesh basemesh;
	onika::vtk::readvtkascii(fname,basemesh);

	// avec connectivitée inverse V2C
	Mesh mesh(basemesh);
	onika::debug::dbgassert( mesh.checkConsistency() );

	cout<<"Vertex a ? "; cout.flush();
	int a=0;
	cin>>a;
	cout<<"Vertex b ? "; cout.flush();
	int b=0;
	cin>>b;

	cout<<endl;
	testMeshAccess(mesh,a,b);

	cout<<endl<<"---------------- collapse (a,b) ---------------------"<<endl<<endl;
	onika::codec::AsciiStream out(cout);
	mesh.edgeCollapse(a,b,out);
	testMeshAccess(mesh,a,b);

	onika::debug::dbgassert( mesh.checkConsistency() );
        cout<<endl<<"PASSED"<<endl;
        return 0;
}

#endif

