cmake_minimum_required(VERSION 3.0)

set(APP_NAME dronecontrol)

project(${APP_NAME})

set(SRCS
    main.cpp
    )

include_directories(../PiSBUS)
link_directories(../PiSBUS/bld)

add_executable(${APP_NAME} ${SRCS})

target_link_libraries(${APP_NAME} libsbus.a)

add_custom_target(run
                  COMMAND ./${APP_NAME} /dev/ttyS0
                  DEPENDS ${APP_NAME}
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                  COMMENT "Running ${APP_NAME}")
