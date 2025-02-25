# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Rasp_Camera_App VERSION 1.0)

# Enable C++17 support
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Use pkg-config to find libcamera
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${LIBCAMERA_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
link_directories(${LIBCAMERA_LIBRARY_DIRS})
add_definitions(${LIBCAMERA_CFLAGS_OTHER})

# Add subdirectories for the app and middleware modules
add_subdirectory(app)
add_subdirectory(middleware)

# Create the executable for the app (main source file is in app/main.cpp)
add_executable(Rasp_Camera_App 
    ${CMAKE_SOURCE_DIR}/app/main.cpp
)

# Link the app with necessary libraries
target_link_libraries(Rasp_Camera_App
    camera_control          # Your middleware library
    gpio                    # GPIO library
    gui                     # GUI library
    logging                 # Logging library
    storage                 # Storage library
    app
    pthread
    ${LIBCAMERA_LIBRARIES}  # Expands to: -lcamera -lcamera-base
    ${OpenCV_LIBS}          # Link OpenCV libraries
)
