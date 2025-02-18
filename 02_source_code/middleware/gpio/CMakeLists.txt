# gpio/CMakeLists.txt

# Add source files for GPIO module
set(GPIO_SOURCES
    gpio.cpp
    gpio.h
)

# Create the GPIO library
add_library(gpio STATIC ${GPIO_SOURCES})

# Include directories for the GPIO module
target_include_directories(gpio PUBLIC ${CMAKE_SOURCE_DIR}/middleware/gpio 
${CMAKE_SOURCE_DIR}/middleware/logging
)
