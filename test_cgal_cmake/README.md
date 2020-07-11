## 1. 安装配置cgal在linux系统的开发环境
具体步骤见[cgal-gcc](https://github.com/zfb132/surface_reconstruction/tree/master/test_cgal_gcc)
## 2. 配置qt5显示3D模型
在上一步的基础上，继续在WSL子系统安装qt5所需的包、在windows系统安装用于显示GUI的软件  
### 2.1 linux子系统安装Qt5
使用以下命令安装需要用到的包：  
`sudo apt install cmake make libqt5widgets5 libqt5gui5 libqt5dbus5 libqt5network5 libqt5core5a`
### 2.2 windows系统安装VcXsrv
经测试，Xming软件是不行的，各种配置都无法成功。目前可以用VcXsrv软件来显示最终的3D模型，下载[windows安装包](https://sourceforge.net/projects/vcxsrv/)安装，然后打开里面的xlaunch.exe，Display settings保持默认（multiple windows），Client setup保持默认（start no client），Extra settings一定要取消选择native opengl（如果命令行则保证`LIBGL_ALWAYS_INDIRECT`为空，即`export LIBGL_ALWAYS_INDIRECT=`），然后保存这个配置文件为`VcXsrv.xlaunch`，以后就可以直接双击此文件启动后台了  
如果你的电脑是高分屏，那么可能需要更改兼容性来适配本软件的显示，具体步骤是：右键单击`xlaunch.exe`或者`vcxsrv.exe`，选择`属性-->兼容性-->更改高DPI设置`，在弹出界面的`程序DPI`处保持不选中，`高DPI缩放替代`处选中`替代高DPI缩放行为`，`缩放执行`选择应用程序，然后确定并应用即可保存。另外，如果要将UI缩放为0.5倍大小则可以在控制台`export GDK_SCALE=0.5`，它会把字体也一并放大，字体就会模糊。如果要同时不影响字体则还需要输入`export GDK_DPI_SCALE=0.5`  
### 2.3 使用CMake编译程序
如果要用cgal的C++代码直接调用Qt5并利用VcXsrv显示3D模型，那么配置CMake编译工具是最方便快捷的。具体步骤如下，首先创建项目文件夹`test_cgal_cmake`，再创建代码文件`test_cgal_cmake/hello.cpp`，文件内容如下：  
```cpp
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

    // Be careful for the direction
    m.add_face(a,b,c,d);
    m.add_face(e,f,b,a);
    m.add_face(c,b,f,g);
    m.add_face(c,g,h,d);
    m.add_face(e,a,d,h);
    m.add_face(e,h,g,f);

	CGAL::draw(m);
    return 0;
}
```
然后创建`test_cgal_cmake/CMakeLists.txt`，内容如下：  
```cmake
cmake_minimum_required(VERSION 2.8)
project(myfirstcgal)

find_package(CGAL COMPONENTS Qt5 Core)
find_package(Qt5 QUIET COMPONENTS Xml Script OpenGL Svg)

include(${CGAL_USE_FILE})

add_executable(demo_exec hello.cpp)

target_link_libraries(demo_exec PRIVATE
CGAL::CGAL CGAL::CGAL_Qt5 Qt5::Gui)
```
然后在项目主目录（即`test_cgal_cmake`）下创建`build`目录，执行`cmake`命令在`build`目录下生成`Makefile`，执行`make`命令编译代码得到可执行文件`demo_exec`并运行，此过程需要依次输入以下命令：  
```bash
mkdir build && cd build
# cmake ..表示在上一级目录寻找CMakeLists.txt文件并运行
cmake ..
# make 表示在当前目录寻找Makefile文件并运行
make
# 执行编译成功的程序
./demo_exec
```
如果报错包含如下信息：  
```txt
libQt5Core.so.5: cannot open shared object file
```
这是[WSL的问题](https://github.com/Microsoft/WSL/issues/3023)，解决方法是  
```txt
zfb@wsl:~/$ find /usr/. -name "libQt5Core.so.5"
/usr/./lib/x86_64-linux-gnu/libQt5Core.so.5
zfb@wsl:~/$ sudo strip --remove-section=.note.ABI-tag /usr/./lib/x86_64-linux-gnu/libQt5Core.so.5
zfb@wsl:~/$ 
```
### 2.4 使用VSCode调试CMake项目
使用`Remote-WSL`插件打开在linux子系统中打开项目文件夹`test_cgal_cmake`，然后在linux子系统中安装`CMake Tools`插件，点击`运行-->启动调试`，则会弹出创建配置文件的提示，即在当前项目主目录下生成`.vscode/launch.json`文件，将其内容修改如下：  
```json
{
    "version": "0.2.0",
    "configurations": [
        
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            // 此键修改为如下内容
            "program": "${command:cmake.launchTargetPath}",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": false,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "为 gdb 启用整齐打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```
对于`.vscode/settings.json`文件则不必修改  
下一步，点击状态栏的`CMake Tools`工具的第一个图标`CMake:[Debug]:[Ready]-->[Unspecified]-->Debug`（也可以选择一个编译工具链，这里选择不指定，工具会自动选择一个），然后CMake会自动在build目录创建Debug版本的Makefile文件，显示如下：  
```txt
......
[cmake] -- Configuring done
[cmake] -- Generating done
[cmake] -- Build files have been written to: /home/zfb/surface_reconstruction/test/build
```
然后点击状态栏的`CMake Tools`工具的虫子图标（Debug的标识），软件会自动运行`build`目录下的`Makefile`文件，并自动执行`make`命令，再自动启动生成的可执行文件，自动运行并停在断点处等待调试  
**如果加载的本地磁盘3D文件无法显示（如下代码），尝试用vscode调试此代码，断点设置在return这一行即可，具体原因未知**
```cpp
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
```