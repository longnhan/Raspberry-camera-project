# Set the target system to Linux
set(CMAKE_SYSTEM_NAME Linux)

# Set the target architecture for Raspberry Pi 4 (ARMv7)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set the cross compiler path (change this to your toolchain path)
set(TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/tools/gcc-linaro-7.5.0-2019.12-x86_64_arm-linux-gnueabihf)
# set(TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/tools/cross-pi-gcc-10.2.0-32/bin/arm-linux-gnueabihf-g++)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/arm-linux-gnueabihf-g++)

# Set the sysroot to the local sysroot directory
set(CMAKE_SYSROOT ${CMAKE_SOURCE_DIR}/tools/sysroot)

# Set CMake to find libraries in the sysroot
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/tools/sysroot)

# Force CMake to prefer libraries and headers from the sysroot
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Optionally set compiler flags
set(CMAKE_C_FLAGS --sysroot=${CMAKE_SOURCE_DIR}/tools/sysroot)
set(CMAKE_CXX_FLAGS --sysroot=${CMAKE_SOURCE_DIR}/tools/sysroot)

# Set the path for library search
set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/tools/sysroot/lib:${CMAKE_SOURCE_DIR}/tools/sysroot/usr/lib)
