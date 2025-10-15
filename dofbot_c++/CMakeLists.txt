# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(DofbotControl CXX)

# Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create the Arm_Lib library from its source files
add_library(arm_lib
    Arm_Lib.cpp
    Arm_Lib.h
)

# Shared CLI helper to parse --port and --init-delay
add_library(cli_args
    cli_args.cpp
    cli_args.h
)

# Tell CMake that the headers are public
target_include_directories(arm_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(cli_args PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# We need pthreads for std::thread
find_package(Threads REQUIRED)

set(DEMO_TARGETS
    beep
    ctrl_all_servo
    ctrl_servo
    dance
    left_right
    read_servo
)

foreach(target_name IN LISTS DEMO_TARGETS)
    add_executable(${target_name}
        ${target_name}.cpp
    )
    target_link_libraries(${target_name}
        PRIVATE
            arm_lib
            cli_args
            Threads::Threads
    )
endforeach()
