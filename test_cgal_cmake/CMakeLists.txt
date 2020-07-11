cmake_minimum_required(VERSION 2.8)
project(myfirstcgal)

find_package(CGAL COMPONENTS Qt5 Core)
find_package(Qt5 QUIET COMPONENTS Xml Script OpenGL Svg)

include(${CGAL_USE_FILE})

add_executable(demo_exec hello.cpp)

target_link_libraries(demo_exec PRIVATE
CGAL::CGAL CGAL::CGAL_Qt5 Qt5::Gui)