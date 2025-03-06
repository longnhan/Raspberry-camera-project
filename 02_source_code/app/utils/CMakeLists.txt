# app/utils/CMakeLists.txt

# Add source files for UTILS module
set(UTILS_SOURCES
    delay.h
    diagnostics.h
    diagnostics.cpp
    settings.h
    settings.cpp
)

# Create the UTILS librarys
add_library(utils STATIC ${UTILS_SOURCES})

# Include directories for the UTILS module
target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/app/utils)
target_link_libraries(utils PUBLIC logging)

