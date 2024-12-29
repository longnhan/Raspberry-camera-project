# app/CMakeLists.txt

# Add source files for the app
set(APP_SOURCES
    button_handler.cpp
    camera_app.cpp
    screen.cpp
    main.cpp
)

# Create the executable for the app
add_executable(CameraApp ${APP_SOURCES})

# Include directories for the app module
include_directories(${CMAKE_SOURCE_DIR}/app)

# Link with the middleware libraries
target_link_libraries(CameraApp
    camera        # camera library
    gpio          # gpio library
    gui           # gui library
    logging       # logging library
    storage       # storage library
)
