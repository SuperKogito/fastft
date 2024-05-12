cmake_minimum_required(VERSION 3.10)
project(fastft)

# Add your source files
set(SOURCES
    src/pad.c
    src/window.c
    src/signal.c
    src/spectral.c
    src/trafo_stft.c
    src/trafo_istft.c
)

# Create a library target
add_library(fastft STATIC ${SOURCES})

# Include directories
target_include_directories(fastft PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link against FFTW and OpenBLAS libraries
target_link_libraries(fastft PUBLIC
    m 
    fftw3f     # Linking FFTW Library
    sndfile     # Linking Soundfile Library
)

# Enable testing
enable_testing()

# Add the test directory
add_subdirectory(test)
