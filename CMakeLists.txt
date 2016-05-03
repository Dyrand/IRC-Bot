cmake_minimum_required (VERSION 2.8)
project(IRC-Bot)

include_directories("${PROJECT_BINARY_DIR}")

SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

# Define sources and executable
set(EXECUTABLE_NAME "bot")

#Find Boost
find_package(Boost REQUIRED COMPONENTS filesystem system programoptions)
include_directories(${Boost_INCLUDE_DIR})


add_executable(${EXECUTABLE_NAME} serverMessageStruct.cpp TcpSocket.cpp ircMimic.cpp Bot.cpp main.cpp)

target_link_libraries(${EXECUTABLE_NAME} ${Boost_LIBRARIES})

# Detect and add SFML
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system network)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()
