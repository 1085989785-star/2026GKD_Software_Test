cmake_minimum_required(VERSION 3.10)
project(2026GKD_SOFTWARE_TEST)

# 设置C++标准
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

# 使用file命令收集源文件
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/*.c"
)

# 或者显式列出所有文件
set(SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/task.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/task_manager.cpp"
)

# 包含头文件目录
include_directories(include)

# 创建可执行文件
add_executable(${PROJECT_NAME} ${SOURCES})

# 设置目标属性（更现代的方式）
target_include_directories(${PROJECT_NAME} PRIVATE include)

# 编译器选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# 检查源文件是否存在
foreach(source_file ${SOURCES})
    if(NOT EXISTS ${source_file})
        message(WARNING "Source file not found: ${source_file}")
    endif()
endforeach()