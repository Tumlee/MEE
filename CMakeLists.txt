cmake_minimum_required (VERSION 2.6)
project (mee)
set(CMAKE_CXX_FLAGS "-std=c++0x -O3 -s")

#Sources
aux_source_directory("${PROJECT_SOURCE_DIR}" PROJECT_SOURCES)

#Binary
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
#target_link_libraries(${CMAKE_PROJECT_NAME})
