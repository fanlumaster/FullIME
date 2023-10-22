cmake_minimum_required(VERSION 3.25.0)
project(fullcpp VERSION 0.1.0)

# 下面这个选项并不能解决 unicode 编译器警告的问题
# add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
# add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Set C++ compiler flags to use Unicode
# 这个可以解决 L 前缀的 unicode 字符串的问题，但是由于 VSCode 的原因，似乎有时候这个警告会删一下
# 而，实际上，我们也可以在代码里面直接添加 UNICODE 的宏。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE /DUNICODE")

# Use unicode versions of functions
# add_definitions(-DUNICODE=1 -D_UNICODE=1)

# Don't include unneccessary things in <Windows.h>
add_definitions(-DWIN32_LEAN_AND_MEAN)

# include_directories(".")
include_directories("${CMAKE_SOURCE_DIR}/src")

# 指定 sqlite dll 和 lib 文件路径
link_directories("${CMAKE_SOURCE_DIR}/libs/sqlite")

add_executable(fullcpp WIN32 "main.cpp" "./src/ui/cand_ui.cpp" "./src/hook/ime_hook.cpp" "./src/sqlite/sqlite_wrapper.cpp" "./flypytblsqlite.cpp" "./src/utils/caret_helper.cpp" "./src/hook/key_handle_func_lib.cpp" "./uiaccess.cpp" "./Resource.h" "./NotificationIcon.rc" "DeclareDPIAware.manifest")

# add_executable(fullcpp WIN32 "main.cpp" "./src/ui/cand_ui.cpp" "./src/hook/ime_hook.cpp" "./src/sqlite/sqlite_wrapper.cpp" "./flypytblsqlite.cpp" "./src/utils/caret_helper.cpp" "./src/hook/key_handle_func_lib.cpp" "./UIAccess.manifest")

# target_link_libraries(fullcpp "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.22621.0\\um\\x64\\d2d1.lib")
# set(LIBS d2d1 d3d9 d3d11 d3dcompiler)
# 设置 lib，其实这个也可以在 cpp 文件中使用 pragma comment 来声明
set(LIBS d2d1 sqlite3)
target_link_libraries(fullcpp ${LIBS})
