set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Define paths relative to CMAKE_SOURCE_DIR
set(TOOLCHAIN_PATH "${CMAKE_SOURCE_DIR}/tools/cross-pi-gcc-10.2.0-32/bin")
# set(SYSROOT "${CMAKE_SOURCE_DIR}/tools/sysroot")

# Set the cross-compiler paths
# set(CMAKE_C_COMPILER "${TOOLCHAIN_PATH}/arm-linux-gnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_PATH}/arm-linux-gnueabihf-g++")

# Set the sysroot
# set(CMAKE_SYSROOT "${SYSROOT}")
# set(CMAKE_FIND_ROOT_PATH "${SYSROOT}")

# Search for libraries and headers inside the sysroot
# set(CMAKE_INCLUDE_PATH "${SYSROOT}/usr/include")
# set(CMAKE_LIBRARY_PATH "${SYSROOT}/usr/lib/arm-linux-gnueabihf")

# Use sysroot for linker and compiler flags
# set(CMAKE_CXX_FLAGS "--sysroot=${SYSROOT}")
# set(CMAKE_EXE_LINKER_FLAGS "--sysroot=${SYSROOT}")
