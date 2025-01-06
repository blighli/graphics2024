#REQUIRED 指定cmake最低版本 (FetchContent 需要 3.14+)
cmake_minimum_required(VERSION 3.16)

#UNIQUE 指定编译程序
    #默认编译triangle项目，Option：window, rectangle
if ("${EXECUTE_NAME}" STREQUAL "")
    set(EXECUTE_NAME triangle)
endif()


#REQUIRED 指定项目名及项目版本
project(${EXECUTE_NAME} VERSION 1.0)

#OPTION 指定C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#REQUIRED 添加cmake目录到CMAKE_MODULE_PATH
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#REQUIRED include方法：载入并运行来自于指定文件或模块CMAKE_MODULE_PATH下的glfw.cmake代码
include(glfw)

#REQUIRED 添加可执行文件
    #文件名变量化声明，GLOB表示 SRC_FILES 为一个文件列表
file(GLOB SRC_FILES 
    src/glad.c 
    src/${EXECUTE_NAME}.cpp
    )
add_executable(${PROJECT_NAME} ${SRC_FILES})
 
#REQUIRED 添加链接库
target_link_libraries(
        ${PROJECT_NAME} PUBLIC
        glfw
)

#REQUIRED 添加include路径
target_include_directories(
        ${PROJECT_NAME} PUBLIC
        include
        ${PROJECT_BINARY_DIR}
)