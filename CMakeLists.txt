cmake_minimum_required( VERSION 3.15 )
project( Dead_Shrimple_RCON )



add_subdirectory( libs/plibsys )
add_subdirectory( libs/CLI11 )


add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    src/RCON.cpp
)

target_link_libraries( ${PROJECT_NAME} plibsysstatic )
target_include_directories( ${PROJECT_NAME} PRIVATE inc libs/plibsys libs/plibsys/src )

target_link_libraries( ${PROJECT_NAME} CLI11::CLI11 )
