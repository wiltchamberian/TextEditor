cmake_minimum_required(VERSION 3.0)

project(MyProject)

# 添加源文件
set(SOURCES
    main.cpp
    EditorCore.cpp
    CmdExecutor.cpp
    CmdBuffer.cpp
    TextBuffer.cpp
)

# 添加头文件路径
include_directories(${CMAKE_SOURCE_DIR})

# 生成可执行文件
add_executable(my_executable ${SOURCES})