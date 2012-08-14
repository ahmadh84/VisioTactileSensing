cmake_minimum_required(VERSION 2.6)
project(VisioTactileSensing)

# Guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed. You may need to remove CMakeCache.txt.")
endif()

###############################################################################
# Thirdparty libraries
message(STATUS "Configuring thirdparty libraries")

ADD_SUBDIRECTORY (3rdParty/MosegaardsClothTutorial)