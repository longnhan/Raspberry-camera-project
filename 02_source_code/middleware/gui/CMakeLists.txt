# middleware/gui/CMakeLists.txt

# List source files for the GUI module
set(GUI_SOURCES
    gui_display.cpp
    gui_gallery.cpp
    gui_settings.cpp
)

# Create a static library for the GUI module
add_library(gui STATIC ${GUI_SOURCES})

# Include directories for the GUI module
target_include_directories(gui PUBLIC ${CMAKE_SOURCE_DIR}/middleware/gui)
