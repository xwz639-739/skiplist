# 根目录 CMakeLists.txt
cmake_minimum_required(VERSION 3.5)
project(SkippingListProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加子目录
add_subdirectory(src/lib)
add_subdirectory(test)