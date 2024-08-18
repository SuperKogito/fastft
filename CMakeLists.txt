cmake_minimum_required(VERSION 3.14)  # FetchContent

set(FASTFT_NAME "fastft")
project(${FASTFT_NAME})

# Include the helper script for FetchDependency and PlatformDependencies
include(cmake/FetchDependency.cmake)
include(cmake/FetchFFTW.cmake)
include(cmake/GetPlatformLibraries.cmake)

# Source files
set(SOURCES
    src/fastft_padding.c
    src/fastft_window.c
    src/fastft_signal.c
    src/fastft_spectral.c
    src/fastft_stft.c
    src/fastft_istft.c
)

# Create a library target
add_library(${FASTFT_NAME} STATIC ${SOURCES})

# Find or automatically download dependencies
FetchDependency(${FASTFT_NAME} libsndfile  https://github.com/libsndfile/libsndfile.git 1.2.2)
FetchFFTW(${FASTFT_NAME})

# Include search directories
target_include_directories(${FASTFT_NAME} PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
	${fftw_SOURCE_DIR}
	${libsndfile_SOURCE_DIR}/include
)

# Platform libraries
GetPlatformLibraries(${FASTFT_NAME} PLATFORM_LIBS)

# Library search directories
target_link_directories(${FASTFT_NAME} PUBLIC
    ${fftw_BINARY_DIR}
    $<BUILD_INTERFACE:${libsndfile_BINARY_DIR}>
)

# Link against the required libraries
target_link_libraries(${FASTFT_NAME} PUBLIC
    ${PLATFORM_LIBS}
    fftw3f      # Linking FFTW Library
    sndfile     # Linking Soundfile Library
)

# Enable testing
enable_testing()

# Add the test directory
add_subdirectory(test)