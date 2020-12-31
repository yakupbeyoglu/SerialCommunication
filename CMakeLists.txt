cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0054 NEW)

set(PROJECT_NAME "SerialCommunication")
project(${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Bin)
add_compile_options(-std=c++14)

set( MAIN
    Source/Main.cpp)

if (WIN32)
    set (OSFILE
         Source/SerialCom/Windows.cpp
         )
else ()
    set (OSFILE
        Source/SerialCom/Linux.cpp
        )
endif()

add_executable(SerialCommunication ${MAIN}
                                   Source/SerialCom/Serial.h
                                   ${OSFILE}
)
