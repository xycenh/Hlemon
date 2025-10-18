cmake_minimum_required(VERSION 3.10)
project(lemon CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories for project headers
include_directories(include)

# Find required packages
find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)
find_package(X11 REQUIRED COMPONENTS Xkbfile) # Add Xkbfile component

# Add executable with source files
add_executable(lemon
    src/lemon.cpp
    src/battery.cpp
    src/functions.cpp
    src/lemonOutput.cpp
    src/brightness.cpp
    src/workspaces.cpp
    src/clock.cpp
    src/volume.cpp
    src/mic.cpp
    src/layout.cpp
)

# Include directories for system libraries
include_directories(
    ${ALSA_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR} # Use X11's include path instead of hardcoding /usr/include
)

# Link libraries
target_link_libraries(lemon
    udev
    ${ALSA_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
    xcb
    xcb-ewmh
    xcb-icccm
    xcb-randr
    X11
    X11-xcb
    ${X11_Xkbfile_LIB} # Add xkbfile for XkbRF_GetNamesProp
)

# Install rules
install(TARGETS lemon DESTINATION /usr/bin)
install(DIRECTORY include/ DESTINATION /usr/include/lemon)
