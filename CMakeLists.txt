cmake_minimum_required(VERSION 3.26)
project(dj_console VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/JUCE)
juce_add_gui_app(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)
add_subdirectory(src)
add_subdirectory(test)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
if (CLANG_FORMAT_EXE)
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")
    add_custom_target(run-clang-format
            COMMAND find ${CMAKE_SOURCE_DIR}/src -name '*.cpp' -o -name '*.h' | xargs ${CLANG_FORMAT_EXE} -i
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
else ()
    message(WARNING "clang-format not found!")
endif ()