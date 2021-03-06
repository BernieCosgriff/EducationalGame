#Change this if you need to target a specific CMake version
cmake_minimum_required(VERSION 3.1.0)


# Enable debug symbols by default
# must be done before project() statement
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)

project(loque)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall")

include_directories("${PROJECT_BINARY_DIR}")

# Define sources and executable
set(EXECUTABLE_NAME "server")

set( source
         src/main.cpp)

add_executable(${EXECUTABLE_NAME} ${source})

# Detect and add SFML and MYSQL
set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules" )
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED network)
find_package(MySQLClient REQUIRED)
find_package(MySQLConnectorCPP REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
target_link_libraries(${EXECUTABLE_NAME} Threads::Threads)
include_directories(${SFML_INCLUDE_DIR}
#                   ${MYSQL_INCLUDE_DIR}
                    ${MYSQLCONNECTORCPP_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES}
                                         ${MYSQL_LIBRARIES}    
                                         ${MYSQLCONNECTORCPP_LIBRARY})


# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
