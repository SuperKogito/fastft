cmake_minimum_required(VERSION 3.10)
project(fastft_stft)

# Set C++ standard to C++14
set(CMAKE_CXX_STANDARD 14)

# Add your source files here
set(SOURCES
    main.cpp
)

# Add subdirectory for fastft library
add_subdirectory(external/fastft)

# Create executable target
add_executable(fastft_stft ${SOURCES})

# Include directories
target_include_directories(fastft_stft PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/external/fastft/include
)

# Link against required libraries
target_link_libraries(fastft_stft 
    fftw3
    fastft
    sndfile
)
