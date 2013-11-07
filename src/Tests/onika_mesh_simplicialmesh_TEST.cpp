#include "onika/mesh/simplicialmesh.h"
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>

#include "onika/vec.h"
#include "onika/vtk/readvtkascii.h"
#include "onika/mesh/meshalgorithm.h"

typedef onika::Vec<3,double> vec3;

int main()
{
	std::cout<<" 2-simplex number of vertices : "<< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,2>::NVerts << "\n";
	std::cout<<" 2-simplex number of edges : "<< onika::mesh::smesh_c2e_basic_traits< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,2> >::NEdges << "\n";
	std::cout<<" 3-simplex number of vertices : "<< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,3>::NVerts << "\n";
	std::cout<<" 3-simplex number of edges : "<< onika::mesh::smesh_c2e_basic_traits< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,3> >::NEdges << "\n";
	std::cout<<" 4-simplex number of vertices : "<< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,4>::NVerts << "\n";
	std::cout<<" 4-simplex number of edges : "<< onika::mesh::smesh_c2e_basic_traits< onika::mesh::smesh_c2v_basic_traits<std::vector<int>,4> >::NEdges << "\n";

        std::cout<<std::endl<<"PASSED"<<std::endl;
        return 0;
}

 // end of file
