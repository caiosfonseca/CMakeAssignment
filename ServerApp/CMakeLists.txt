cmake_minimum_required (VERSION 3.0)

project ("ServerApp")

set (ASSIGNMENT_VERSION_MAJOR "0")
set (ASSIGNMENT_VERSION_MINOR "1")
set (ASSIGNMENT_VERSION_PATCH "0")
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set (PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/*.cpp")

include_directories("${PROJECT_INCLUDE_DIR}")

add_executable(ServerApp ${SOURCES})

target_link_libraries(${PROJECT_NAME}
    commonLib
)

install (TARGETS ServerApp DESTINATION bin)