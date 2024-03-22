cmake_minimum_required(VERSION 3.26)
project(dj_console VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(libs/JUCE)



juce_add_gui_app(${PROJECT_NAME})

add_subdirectory(src)
