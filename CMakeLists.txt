cmake_minimum_required (VERSION 3.23.3)
project(Logger VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 23)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
 
set(TESTS_ROOT ${PROJECT_SOURCE_DIR}/tests)
set(SRC_ROOT ${PROJECT_SOURCE_DIR}/src)
set(SRC
    ${SRC_ROOT}/Log.cpp

)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options("-O3")
    add_compile_options("-mwindows")
endif()
set(CMAKE_EXE_LINKER_FLAGS "-static")


add_executable (LoggerTest ${TESTS_ROOT}/Source.cpp)
target_include_directories(LoggerTest PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/"
)