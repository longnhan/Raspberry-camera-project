# middleware/camera/CMakeLists.txt

# List source files for the camera module
set(CAMERA_SOURCES
    camera_control.cpp
    camera_log.cpp
    camera_control.h
)

# Create a static library for the Camera module
add_library(camera_control STATIC ${CAMERA_SOURCES})

# Include directories for the Camera module
target_include_directories(camera_control PUBLIC ${CMAKE_SOURCE_DIR}/middleware/camera)

# Link the app library with the gpio library
target_link_libraries(camera_control PUBLIC logging)
