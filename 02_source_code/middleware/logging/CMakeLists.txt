# middleware/logging/CMakeLists.txt

# List source files for the logging module
set(LOGGING_SOURCES
    log.cpp
    log.h
)

# Create a static library for the Logging module
add_library(logging STATIC ${LOGGING_SOURCES})

# Include directories for the Logging module
target_include_directories(logging PUBLIC ${CMAKE_SOURCE_DIR}/middleware/logging)

