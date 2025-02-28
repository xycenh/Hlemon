cmake_minimum_required(VERSION 3.10)
project(lemon)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

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

include_directories(${ALSA_INCLUDE_DIRS})

target_link_libraries(lemon
    udev
    ${ALSA_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS lemon DESTINATION /usr/bin)
install(DIRECTORY include/ DESTINATION /usr/include/lemon)
