# middleware/storage/CMakeLists.txt

# List source files for the storage module
set(STORAGE_SOURCES
    image_storage.cpp
    storage_log.cpp
)

# Create a static library for the Storage module
add_library(storage STATIC ${STORAGE_SOURCES})

# Include directories for the Storage module
target_include_directories(storage PUBLIC ${CMAKE_SOURCE_DIR}/middleware/storage)
