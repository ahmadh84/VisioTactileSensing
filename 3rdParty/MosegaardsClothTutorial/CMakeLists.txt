cmake_minimum_required(VERSION 2.6)
project(MosegaardsClothTutorial)

# Guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed. You may need to remove CMakeCache.txt.")
endif()

# Compilation options                             
add_definitions(-g)
add_definitions(-o3)

# Include OpenGL, GLUT and GLU
find_package (OpenGL REQUIRED)
find_package (GLUT REQUIRED)
find_package (GLU REQUIRED)
include_directories(${GLUT_INCLUDE_DIR} ${GLU_INCLUDE_DIR})
link_libraries(${GLUT_LIBRARY} ${GLU_LIBRARY} glui)

# Include Eigen3
include_directories(/usr/local/include/eigen-3.0.5)

# Build the script
file(GLOB srcs "*.cpp")
file(GLOB hdrs "*.h")
add_executable(MosegaardsClothTutorial ${srcs} ${hdrs})
