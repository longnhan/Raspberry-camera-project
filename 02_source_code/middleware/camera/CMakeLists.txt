# middleware/camera/CMakeLists.txt

# List source files for the camera module
set(CAMERA_SOURCES
    camera_control.cpp
    camera_log.cpp
)

# Create a static library for the Camera module
add_library(camera STATIC ${CAMERA_SOURCES})

# Include directories for the Camera module
target_include_directories(camera PUBLIC ${CMAKE_SOURCE_DIR}/middleware/camera)
