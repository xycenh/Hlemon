cmake_minimum_required(VERSION 3.31.4)

project(lemon)
include_directories(include)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(lemon 
               src/lemon.cpp 
               src/battery.cpp 
               src/functions.cpp 
               src/lemonOutput.cpp 
               src/brightness.cpp
               src/workspaces.cpp
	       src/clock.cpp
             )
target_link_libraries(lemon udev)


# Installation rules
install(TARGETS lemon DESTINATION /usr/bin)
install(DIRECTORY include/ DESTINATION /usr/include/lemon)
# install(FILES config/lemon.conf DESTINATION /etc/lemon)
