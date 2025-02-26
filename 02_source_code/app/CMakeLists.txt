# Add source files for the app
set(APP_SOURCES
    button_handler.cpp
    camera_app.cpp
    screen.cpp
    button_handler.h
    camera_app.h
    screen.h
)
# Add subdirectories for the utils modules
add_subdirectory(utils)

# Create a static library for the app module
add_library(app STATIC ${APP_SOURCES})

# Include directories for the app module
target_include_directories(app PUBLIC ${CMAKE_SOURCE_DIR}/app)

# Link the app library with the gpio library
target_link_libraries(app PUBLIC gpio utils camera_control)