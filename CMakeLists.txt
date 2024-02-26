cmake_minimum_required(VERSION 3.10)
project(cstft)

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Add your source files and headers
set(SOURCES
    main.c
    src/plot.c 
    src/io.c 
    src/pad.c 
    src/spectral.c
)

set(HEADERS
    include/plot.h
    include/io.h
    include/pad.h 
    include/spectral.h
)

# Create the executable target
add_executable(cstft ${SOURCES} ${HEADERS})

# Link against other required libraries
target_link_libraries(cstft
    m           # Linking Math Library
    pthread     # Linking PThread Library
    openblas    # Linking OpenBLAS Library
    sndfile     # Linking Soundfile Library
    fftw3f
)

# Include directories
target_include_directories(cstft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Configure the compiler options
target_compile_options(cstft PRIVATE -Wall -Wextra -O0 -g)
