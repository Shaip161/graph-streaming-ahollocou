cmake_minimum_required (VERSION 3.10)
PROJECT(DYCOLA CXX)
SET(CMAKE_EXPORT_COMPILE_COMMANDS 1)
SET(CMAKE_CXX_FLAGS "-g -std=c++11 ")

INCLUDE_DIRECTORIES(./include)
project (STREAMING CXX)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/flatbuffers
        ${CMAKE_CURRENT_BINARY_DIR}/flatbuffers-build
        EXCLUDE_FROM_ALL)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/VieClus)

# Create a "VieClus" STATIC library from the submodule’s OBJECT libs
add_library(VieClus STATIC
    $<TARGET_OBJECTS:libpadygrcl>
    $<TARGET_OBJECTS:libeval>
    $<TARGET_OBJECTS:libkaffpa>
    $<TARGET_OBJECTS:libclustering>
)

# Link any necessary system libraries (MPI, OpenMP, etc.)
target_link_libraries(VieClus
    PUBLIC
    ${MPI_CXX_LIBRARIES}
    ${OpenMP_CXX_LIBRARIES}
)

# If needed, add includes (MPI, for instance)
target_include_directories(VieClus
    PUBLIC
    ${MPI_CXX_INCLUDE_PATH}
    # ... other includes ...
)

# Include FlatBuffers headers
include_directories(extern/flatbuffers/include)

FILE(GLOB COMMON_SOURCE_FILES "source/*.cpp")
add_executable(streamcom "source/streamcom/main.cpp" ${COMMON_SOURCE_FILES})
target_link_libraries(streamcom PRIVATE flatbuffers)
target_link_libraries(streamcom PRIVATE VieClus)


