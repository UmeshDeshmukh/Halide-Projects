cmake_minimum_required(VERSION 3.2)
project(Halide_project)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    /g/Halide_Language/Halide_Projects/My_projects/lesson_01_modified.cpp
    
)

include_directories(/mnt/g/Halide_Language/Halide-13.0.2-x86-64-linux/include/
                    /mnt/g/Halide_Language/Halide-13.0.2-x86-64-linux/share/Halide/tools/)
add_library(msg_lib lib_file.cpp)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} msg_lib)