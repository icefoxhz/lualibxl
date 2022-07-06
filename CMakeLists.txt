cmake_minimum_required(VERSION 3.22)
project(lualibxlCore)

set(CMAKE_CXX_STANDARD 11)

# 关闭警告
add_definitions(-w)


set(INC_DIR
#        "E:/tools/windows/c++/thirdLib/lua-5.4.4/src"
        "E:/tools/windows/lua/LuaJIT-2.1.0-beta3/src"
        "E:/tools/windows/c++/thirdLib/libxl-4.0.4.0/include_cpp"
        )
#
set(LINK_DIR
        "E:/tools/windows/c++/thirdLib/libxl-4.0.4.0/lib64"
        )

include_directories(${INC_DIR})
link_directories(${LINK_DIR})

add_library(${CMAKE_PROJECT_NAME} SHARED libraryMulti.cpp StringUtil.hpp)
#
target_link_libraries(${CMAKE_PROJECT_NAME}
        libxl.lib
        )
