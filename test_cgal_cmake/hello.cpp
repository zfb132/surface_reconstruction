#define CGAL_USE_BASIC_VIEWER
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/draw_surface_mesh.h>

typedef CGAL::Simple_cartesian<double> K;
typedef CGAL::Surface_mesh<K::Point_3> Mesh;
typedef Mesh::Vertex_index vertex_descriptor;
typedef Mesh::Face_index face_descriptor;

int main() 
{
    Mesh m;

    vertex_descriptor a = m.add_vertex(K::Point_3(0,0,0));
    vertex_descriptor b = m.add_vertex(K::Point_3(1,0,0));
    vertex_descriptor c = m.add_vertex(K::Point_3(1,1,0));
    vertex_descriptor d = m.add_vertex(K::Point_3(0,1,0));
    vertex_descriptor e = m.add_vertex(K::Point_3(0,0,1));
    vertex_descriptor f = m.add_vertex(K::Point_3(1,0,1));
    vertex_descriptor g = m.add_vertex(K::Point_3(1,1,1));
    vertex_descriptor h = m.add_vertex(K::Point_3(0,1,1));

    m.add_face(a,b,c,d);
    m.add_face(e,f,b,a);
    m.add_face(c,b,f,g);
    m.add_face(c,g,h,d);
    m.add_face(e,a,d,h);
    m.add_face(e,h,g,f);

	CGAL::draw(m);

    return 0;


}


/*
// 显示本地磁盘的3D模型文件
#define CGAL_USE_BASIC_VIEWER
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/IO/Polyhedron_iostream.h>
#include <CGAL/draw_polyhedron.h>
#include <fstream>
typedef CGAL::Exact_predicates_inexact_constructions_kernel  Kernel;
typedef CGAL::Polyhedron_3<Kernel>                       Polyhedron;
int main(int argc, char* argv[])
{
  Polyhedron P;
  std::ifstream in1("data/polygon_mesh.off");
  in1 >> P;
  CGAL::draw(P);
  return EXIT_SUCCESS;
}
*/