cmake_minimum_required(VERSION 3.26)
project(dj_console VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -pedantic -Werror)

add_subdirectory(libs/JUCE)
juce_add_gui_app(${PROJECT_NAME})

find_package(PkgConfig REQUIRED)
pkg_check_modules(AUBIO REQUIRED aubio)

target_compile_definitions(${PROJECT_NAME} PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)
target_link_libraries(${PROJECT_NAME} PRIVATE aubio)
add_subdirectory(src)
add_subdirectory(test)
enable_testing()

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

find_package(Doxygen REQUIRED)

set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
message("Doxygen build started")

add_custom_target(doc_doxygen
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)


