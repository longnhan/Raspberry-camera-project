cmake_minimum_required(VERSION 3.10)

project(Rasp_Camera_App VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

# Ensure correct paths for cross-compilation
set(ENV{PKG_CONFIG_PATH} "${CMAKE_SOURCE_DIR}/tools/sysroot/usr/lib/arm-linux-gnueabihf/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SOURCE_DIR}/tools/sysroot")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera libcamera-base)

# Debug output
message(STATUS "Found libcamera libraries: ${LIBCAMERA_LIBRARIES}")
message(STATUS "Found libcamera include directories: ${LIBCAMERA_INCLUDE_DIRS}")
message(STATUS "Found libcamera library directories: ${LIBCAMERA_LIBRARY_DIRS}")

# Use pkg-config include directories instead of hardcoded ones
include_directories(${LIBCAMERA_INCLUDE_DIRS})

# Use pkg-config library directories
link_directories(${LIBCAMERA_LIBRARY_DIRS})

# Add subdirectories
add_subdirectory(app)
add_subdirectory(middleware)

# Define the executable
add_executable(Rasp_Camera_App ${CMAKE_SOURCE_DIR}/app/main.cpp)

# Link libraries properly
target_link_libraries(Rasp_Camera_App PRIVATE 
    ${LIBCAMERA_LIBRARIES}  # Ensure libcamera is linked
    app
    camera
    gpio
    gui
    logging
    storage
    pthread
)

# Linker flags
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lc -ldl")
