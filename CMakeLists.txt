cmake_minimum_required(VERSION 3.10)
project(lemon)

# Enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(include)

# Find required libraries
find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

# Add source files
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
)

# Include ALSA directories
include_directories(${ALSA_INCLUDE_DIRS})

# Link required libraries
target_link_libraries(lemon
    udev
    ${ALSA_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

# Installation rules
install(TARGETS lemon DESTINATION /usr/bin)
install(DIRECTORY include/ DESTINATION /usr/include/lemon)
# install(FILES config/lemon.conf DESTINATION /etc/lemon)
