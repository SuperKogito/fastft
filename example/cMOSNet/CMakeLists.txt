cmake_minimum_required(VERSION 3.10)
project(cMOSNet C)

# Set C standard to C99
set(CMAKE_C_STANDARD 99)

# Enable optimization flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native")

# Find and enable OpenMP
find_package(OpenMP REQUIRED)
if (OpenMP_C_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
endif()

# Add ONNX Runtime
set(onnx_dir ${CMAKE_SOURCE_DIR}/external/onnxruntime)
set(onnx_lib_name libonnxruntime.so)

find_library(onnx_lib NAMES ${onnx_lib_name} PATHS ${onnx_dir}/lib NO_DEFAULT_PATH)
message(STATUS "[onnx] ONNX dir     : ${onnx_dir}")
message(STATUS "[onnx] ONNX libname : ${onnx_lib_name}")
message(STATUS "[onnx] ONNX libpath : ${onnx_lib}")

add_library(onnx INTERFACE)
target_link_libraries(onnx INTERFACE ${onnx_lib})
target_include_directories(onnx INTERFACE ${onnx_dir}/include)

# Add your source files here
set(SOURCES
    main.c
)

# Add subdirectory for fastft library
# add_subdirectory(external/fastft)

# Create executable target
add_executable(cMOSNet ${SOURCES})

# Include directories
target_include_directories(cMOSNet PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/fastft/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/onnxruntime/include
)

# Link against required libraries
target_link_libraries(cMOSNet 
    onnx
    fastft
    sndfile
    OpenMP::OpenMP_C
)

# Ensure the ONNX Runtime library is linked
target_link_directories(cMOSNet PRIVATE ${onnx_dir}/lib)
target_link_libraries(cMOSNet ${onnx_lib})
