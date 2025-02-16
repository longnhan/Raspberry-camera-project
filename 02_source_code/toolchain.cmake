# Set the target system to Linux
set(CMAKE_SYSTEM_NAME Linux)

# Set the target architecture for Raspberry Pi 4 (ARMv7)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Set the cross compiler path (update this if needed)
set(TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/tools/cross-pi-gcc-10.2.0-32)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PATH}/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PATH}/bin/arm-linux-gnueabihf-g++)

# Set the sysroot to the local sysroot directory
set(CMAKE_SYSROOT ${CMAKE_SOURCE_DIR}/tools/sysroot CACHE PATH "Sysroot for cross-compilation")

# set(CMAKE_SYSROOT ${TOOLCHAIN_PATH}/arm-linux-gnueabihf/libc)

# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--sysroot=${CMAKE_SYSROOT} -Wl,-rpath,${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf -L${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf")

# Ensure compiler uses the correct sysroot
set(CMAKE_C_FLAGS "--sysroot=${CMAKE_SYSROOT} ${CMAKE_C_FLAGS}")
set(CMAKE_CXX_FLAGS "--sysroot=${CMAKE_SYSROOT} ${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--sysroot=${CMAKE_SYSROOT} ${CMAKE_EXE_LINKER_FLAGS}")


# Make sure CMake finds the correct libraries
set(CMAKE_FIND_ROOT_PATH ${CMAKE_SYSROOT} ${CMAKE_SYSROOT}/usr)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Set the path for library search
set(CMAKE_LIBRARY_PATH 
    ${CMAKE_SYSROOT}/lib 
    ${CMAKE_SYSROOT}/usr/lib 
    ${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf
)
