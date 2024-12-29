# gpio/CMakeLists.txt

# List source files for the GPIO module
set(GPIO_SOURCES
    gpio_button.cpp
    gpio_init.cpp
    gpio_log.cpp
)

# Create a library or object files for the GPIO module
add_library(gpio STATIC ${GPIO_SOURCES})

# Include directories for the GPIO module
target_include_directories(gpio PUBLIC ${CMAKE_SOURCE_DIR}/middleware/gpio)

# Link with other dependencies if needed (e.g., wiringPi or libgpiod)
# Uncomment if you are using wiringPi or other GPIO libraries
# find_package(WiringPi REQUIRED)

# Link to external libraries (e.g., wiringPi or libgpiod)
# target_link_libraries(gpio PRIVATE WiringPi)
