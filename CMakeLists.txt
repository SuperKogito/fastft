cmake_minimum_required(VERSION 3.10)
project(cstft)

# Set the MinGW compiler
set(CMAKE_C_COMPILER "gcc")

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Add your source files and headers
set(SOURCES
        main.c
        src/plot.c
        src/io.c
        src/pad.c
        src/spectral.c

        src/cJSON.c
        src/four_stems_spleeter.c
        src/gemm.c
        src/spleeter_network.c
        src/utils.c
        )

set(HEADERS
        include/plot.h
        include/io.h
        include/pad.h
        include/spectral.h

        include/cJSON.h
        include/four_stems_spleeter.h
        include/gemm.h
        include/spleeter_network.h
        include/utils.h
        )

# Create the executable target
add_executable(cstft ${SOURCES} ${HEADERS})

# Link against other required libraries
target_link_libraries(cstft
        m           # Linking Math Library
        -fopenmp
        -pthread    # Linking PThread Library
        -lopenblas  # Linking OpenBLAS Library
        -lsndfile   # Linking Soundfile Library
        -lfftw3f
        -Wl,-Bstatic -Bdynamic -static-libgcc -static-libstdc++
        )

if (UNIX)
    message("UNIX system detected.")
    # Include directories
    target_include_directories(cstft PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

endif()

if (WIN32)
    message("WIN32 system detected.")
    # Adjust the path separator for Windows
    target_include_directories(cstft PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}\\include"
                                             "D:\\toolchains\\w64devkit\\include")
    target_link_directories(cstft PRIVATE "D:\\toolchains\\w64devkit\\lib")
endif()

# Configure the compiler options
target_compile_options(cstft PRIVATE -Wall -Wextra -O0 -g)
