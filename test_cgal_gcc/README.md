## 安装cgal并配置linux开发环境
安装cgal开发：`sudo apt install libcgal-dev`  
然后创建cgal的项目文件夹为`test_cgal`，编写代码文件`test_cgal/main.cpp`，内容如下：  
```cpp
// STL includes.
#include <string>
#include <vector>
#include <utility>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>

// CGAL includes.
#include <CGAL/array.h>
#include <CGAL/property_map.h>
#include <CGAL/IO/write_ply_points.h>
#include <CGAL/Simple_cartesian.h>

#include <CGAL/Shape_detection/Region_growing/Region_growing.h> 
#include <CGAL/Shape_detection/Region_growing/Region_growing_on_point_set.h>

// Type declarations.
using Kernel = CGAL::Simple_cartesian<double>;

using FT       = typename Kernel::FT;
using Point_2  = typename Kernel::Point_2;
using Point_3  = typename Kernel::Point_3;
using Vector_2 = typename Kernel::Vector_2;

using Point_with_normal = std::pair<Point_2, Vector_2>;
using Input_range       = std::vector<Point_with_normal>;
using Point_map         = CGAL::First_of_pair_property_map<Point_with_normal>;
using Normal_map        = CGAL::Second_of_pair_property_map<Point_with_normal>;

using Neighbor_query = CGAL::Shape_detection::Point_set::Sphere_neighbor_query<Kernel, Input_range, Point_map>;
using Region_type    = CGAL::Shape_detection::Point_set::Least_squares_line_fit_region<Kernel, Input_range, Point_map, Normal_map>;
using Region_growing = CGAL::Shape_detection::Region_growing<Input_range, Neighbor_query, Region_type>;

using Region  = std::vector<std::size_t>;
using Regions = std::vector<Region>;

using Color            = std::array<unsigned char, 3>;
using Point_with_color = std::pair<Point_3, Color>;
using Pwc_vector       = std::vector<Point_with_color>;
using PLY_Point_map    = CGAL::First_of_pair_property_map<Point_with_color>;
using PLY_Color_map    = CGAL::Second_of_pair_property_map<Point_with_color>;

// Define how a color should be stored.
namespace CGAL {

    template<class F>
    struct Output_rep< ::Color, F > {
        
    const ::Color& c;
    static const bool is_specialized = true;
    
    Output_rep(const ::Color& c) : c(c) { }

    std::ostream& operator()(std::ostream& out) const {      
        if (is_ascii(out)) 
            out << int(c[0]) << " " << int(c[1]) << " " << int(c[2]);
        else 
            out.write(reinterpret_cast<const char*>(&c), sizeof(c));       
        return out;
    }
  };
}

int main(int argc, char *argv[]) {
    std::cout << std::endl << 
        "region_growing_on_point_set_2 example started" 
    << std::endl << std::endl;
    
    std::cout << 
        "Note: if 0 points are loaded, please specify the path to the file data/point_set_2.xyz by hand!" 
    << std::endl << std::endl;

    // Load xyz data either from a local folder or a user-provided file.
    std::ifstream in(argc > 1 ? argv[1] : "data/point_set_2.xyz");
    CGAL::set_ascii_mode(in);

    Input_range input_range;
    FT a, b, c, d, e, f;

    while (in >> a >> b >> c >> d >> e >> f)
        input_range.push_back(std::make_pair(Point_2(a, b), Vector_2(d, e)));
    
    in.close();
    std::cout << "* loaded " << input_range.size() << " points with normals" << std::endl;

    // Default parameter values for the data file point_set_2.xyz.
    const FT          search_sphere_radius = FT(5);
    const FT          max_distance_to_line = FT(45) / FT(10);
    const FT          max_accepted_angle   = FT(45);
    const std::size_t min_region_size      = 5;
  
    // Create instances of the classes Neighbor_query and Region_type.
    Neighbor_query neighbor_query(
        input_range, 
        search_sphere_radius
    );

    Region_type region_type(
        input_range, 
        max_distance_to_line, max_accepted_angle, min_region_size
    );
    
    // Create an instance of the region growing class.
    Region_growing region_growing(
        input_range, neighbor_query, region_type
    );

    // Run the algorithm.
    Regions regions;
    region_growing.detect(std::back_inserter(regions));

    // Print the number of found regions.
    std::cout << "* " << regions.size() << " regions have been found" << std::endl;

    Pwc_vector pwc;
    srand(static_cast<unsigned int>(time(NULL)));
  
    // Iterate through all regions.
    for (const auto& region : regions) {
        // Generate a random color.
        const Color color = CGAL::make_array(
            static_cast<unsigned char>(rand() % 256),
            static_cast<unsigned char>(rand() % 256),
            static_cast<unsigned char>(rand() % 256)
        );
        // Iterate through all region items.
        for (const auto index : region) {
            const auto& key = *(input_range.begin() + index);
            const Point_2& point = get(Point_map(), key);
            pwc.push_back(std::make_pair(Point_3(point.x(), point.y(), 0), color));
        }
    }

    // Save the result to a file in the user-provided path if any.
    if (true) {
        // const std::string path     = argv[2];
        const std::string fullpath = "regions_point_set_2.ply";

        std::ofstream out(fullpath);

        CGAL::set_ascii_mode(out);
        CGAL::write_ply_points_with_properties(
            out, pwc,
            CGAL::make_ply_point_writer(PLY_Point_map()),
            std::make_tuple(
                PLY_Color_map(), 
                CGAL::PLY_property<unsigned char>("red"),
                CGAL::PLY_property<unsigned char>("green"),
                CGAL::PLY_property<unsigned char>("blue")
            )
        );

    std::cout << "* found regions are saved in " << fullpath << std::endl;
    out.close();
    }

    std::cout << std::endl << 
        "region_growing_on_point_set_2 example finished" 
    << std::endl << std::endl;
  
    return EXIT_SUCCESS;
}
```
在VSCode的linux子系统模式打开此文件夹，点击`运行-->打开配置(或启动调试)`则会在当前项目生成`.vscode`文件夹，修改`c_cpp_properties.json`文件的`includePath`，添加一个头文件路径即可
```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/include/**"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c11",
            "cppStandard": "gnu++14",
            "intelliSenseMode": "clang-x64"
        }
    ],
    "version": 4
}
```
然后即可正常调试和运行，生成的3D文件和原始3D文件都可以用其他软件查看  
注意：如果**报错找不到Eigen/Dense库文件**，则使用代码`sudo apt-get install libeigen3-dev`确保eigen3库已经安装，则问题可能是目录层级，执行`sudo ln -s /usr/include/eigen3/Eigen /usr/include/Eigen`创建一个软链接即可解决问题  