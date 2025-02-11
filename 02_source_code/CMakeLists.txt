# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Rasp_Camera_App VERSION 1.0)

# Enable C++17 support
set(CMAKE_CXX_STANDARD 17)

# Add flag standard c++17
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Set toolchain for cross-compilation (C++ compiler)
# set(CMAKE_CXX_COMPILER ${CMAKE_SOURCE_DIR}/tools/cross-pi-gcc-10.2.0-32/bin/arm-linux-gnueabihf-g++)

# Add the directories where headers and libraries are located
include_directories(
    ${CMAKE_SOURCE_DIR}/tools/sysroot/usr/include
    ${CMAKE_SOURCE_DIR}/tools/sysroot/usr/include/libcamera

)

# link_directories(${CMAKE_SOURCE_DIR}/tools/sysroot/usr/lib)
link_directories(${CMAKE_SOURCE_DIR}/tools/sysroot/usr/lib/arm-linux-gnueabihf)


set(ENV{PKG_CONFIG_PATH} ${CMAKE_SOURCE_DIR}/tools/sysroot/usr/lib/arm-linux-gnueabihf/pkgconfig)
set(CMAKE_EXE_LINKER_FLAGS " -static -Wl,--whole-archive -lpthread -Wl,--no-whole-archive -lc -ldl")

add_definitions(-DENABLE_DWARF=OFF)

# Find the libcamera package or manually link the libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBCAMERA REQUIRED libcamera)

message(STATUS "Found libcamera: ${LIBCAMERA_LIBRARIES}")

# Add subdirectories for the app and middleware modules
add_subdirectory(app)
add_subdirectory(middleware)

# Create the executable for the app (main source file is in app/main.cpp)
add_executable(Rasp_Camera_App 
    ${CMAKE_SOURCE_DIR}/app/main.cpp
)

# Link the app with necessary libraries (camera, gpio, logging, etc.)

target_link_libraries(Rasp_Camera_App PRIVATE 
    # camera-base
    camera        # camera library
    gpio          # gpio library
    gui           # gui library
    logging       # logging library
    storage       # storage library
    app
    pthread
)
