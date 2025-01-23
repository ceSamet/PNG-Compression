cmake_minimum_required(VERSION 3.10)
project(ImageCompressor VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    main.cpp
    ImageCompressor.cpp
    PNGImage.cpp
    PNGStructs.cpp
)

# Add header files
set(HEADERS
    ImageCompressor.h
    PNGImage.h
    PNGStructs.h
)

# Create executable
add_executable(image_compressor ${SOURCES} ${HEADERS})

# Set output directories
set_target_properties(image_compressor
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Add compiler warnings
if(MSVC)
    target_compile_options(image_compressor PRIVATE /W4)
else()
    target_compile_options(image_compressor PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install target
install(TARGETS image_compressor
    RUNTIME DESTINATION bin
) 