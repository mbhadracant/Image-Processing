cmake_minimum_required(VERSION 3.0)
project(window)


#Compiler/linker flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

include_directories(include)

set(WINDOW_SRC src/window.cpp)

add_executable(window ${WINDOW_SRC})

find_package(wxWidgets REQUIRED net gl core base)
include(${wxWidgets_USE_FILE})
target_link_libraries(window ${wxWidgets_LIBRARIES})
