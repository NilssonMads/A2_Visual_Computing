cmake_minimum_required(VERSION 3.10)
project(VideoProcessing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

# Find required packages
find_package(OpenCV REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glad CONFIG REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external
    ${OpenCV_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
)

# Source files
set(SOURCES
    main.cpp
    common/Shader.cpp
    common/Camera.cpp
    common/Scene.cpp
    common/Object.cpp
    common/TextureShader.cpp
    common/Quad.cpp
    common/Texture.cpp
)

# Create executable
add_executable(VideoProcessing ${SOURCES})

# Link libraries
target_link_libraries(VideoProcessing
    PRIVATE
    ${OpenCV_LIBS}
    ${OPENGL_LIBRARIES}
    glfw
    glad::glad
)

# For Windows, link additional libraries
if(WIN32)
    target_link_libraries(VideoProcessing PRIVATE opengl32)
endif()

# Copy shaders to build directory
file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR})

# On Windows, copy OpenCV DLLs to output directory (for runtime)
if(WIN32 AND OpenCV_FOUND)
    # Copy DLLs for Debug configuration
    foreach(LIB ${OpenCV_LIBS})
        get_target_property(LIB_PATH ${LIB} LOCATION_DEBUG)
        if(LIB_PATH)
            string(REPLACE ".lib" ".dll" DLL_PATH ${LIB_PATH})
            if(EXISTS ${DLL_PATH})
                file(COPY ${DLL_PATH} DESTINATION ${CMAKE_BINARY_DIR}/Debug)
            endif()
        endif()
    endforeach()
    
    # Copy DLLs for Release configuration
    foreach(LIB ${OpenCV_LIBS})
        get_target_property(LIB_PATH ${LIB} LOCATION_RELEASE)
        if(LIB_PATH)
            string(REPLACE ".lib" ".dll" DLL_PATH ${LIB_PATH})
            if(EXISTS ${DLL_PATH})
                file(COPY ${DLL_PATH} DESTINATION ${CMAKE_BINARY_DIR}/Release)
            endif()
        endif()
    endforeach()
endif()

# Print configuration info
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")