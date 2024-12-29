# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(CameraApp)

# Enable C++17
set(CMAKE_CXX_STANDARD 17)

# Add subdirectories for the app and middleware modules
add_subdirectory(app)
add_subdirectory(middleware)

# Link the app with necessary libraries (camera, gpio, logging, etc.)
target_link_libraries(CameraApp
    camera        # camera library
    gpio          # gpio library
    gui           # gui library
    logging       # logging library
    storage       # storage library
)

