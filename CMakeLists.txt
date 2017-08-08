cmake_minimum_required(VERSION 3.0)

project(dronecontrol)

set(SRCS
    main.cpp
    )

include_directories(../PiSBUS)
link_directories(../PiSBUS/bld)

add_executable(dronecontrol ${SRCS})

target_link_libraries(dronecontrol libsbus.a)
