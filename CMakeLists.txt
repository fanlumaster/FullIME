cmake_minimum_required(VERSION 3.25.0)
project(fullcpp VERSION 0.1.0)

# Set C++ compiler flags to use Unicode
# 实际上，我们也可以在代码里面直接添加 UNICODE 的宏。
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE /DUNICODE")

# Don't include unneccessary things in <Windows.h>
add_definitions(-DWIN32_LEAN_AND_MEAN)

# include_directories(".")
include_directories("${CMAKE_SOURCE_DIR}/src")
include_directories("${CMAKE_SOURCE_DIR}/libs")

# 指定 sqlite3 dll 和 lib 文件路径
link_directories("${CMAKE_SOURCE_DIR}/libs/sqlite")

add_executable(fullcpp WIN32
    "main.cpp"
    "./src/ui/cand_ui.cpp"
    "./src/hook/ime_hook.cpp"
    "./src/sqlite/sqlite_wrapper.cpp"
    "./flypytblsqlite.cpp"
    "./src/utils/caret_helper.cpp"
    "./src/hook/key_handle_func_lib.cpp"
    "./uiaccess.cpp"
    "./Resource.h"
    "./NotificationIcon.rc"
)

# 设置 lib，其实这个也可以在 cpp 文件中使用 #pragma comment(lib, "d2d1") 来声明
set(LIBS d2d1 sqlite3)

# TODO: 设置 asInvoker 权限即可，不必设置管理员执行权限。平时编译调试的时候不需要，只有最终需要编译 release 的时候需要打开注释
# set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/level='asInvoker' /uiAccess='true'" )

target_link_libraries(fullcpp ${LIBS})
target_sources(fullcpp PRIVATE "DeclareDPIAware.manifest")
