cmake_minimum_required( VERSION 3.27.0-rc4 )
project( Dead_Shrimple_RCON )

add_subdirectory( libs/plibsys EXCLUDE_FROM_ALL )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY )

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    src/RCON.cpp
)


