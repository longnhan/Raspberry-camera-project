# middleware/CMakeLists.txt

# Add subdirectories for the middleware modules
add_subdirectory(gui)
add_subdirectory(camera)
add_subdirectory(gpio)
add_subdirectory(storage)
add_subdirectory(logging)

# Include directories for middleware module
include_directories(${CMAKE_SOURCE_DIR}/middleware)
