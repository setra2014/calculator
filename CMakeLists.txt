cmake_minimum_required(VERSION 3.0)

project(calc)

set(CMAKE_CXX_STANDART 17)

file(GLOB CPPS "*.cpp")

add_executable(${PROJECT_NAME} ${CPPS})
