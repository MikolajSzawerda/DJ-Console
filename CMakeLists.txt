cmake_minimum_required(VERSION 3.26)
project(dj_poc)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(JUCE)
add_executable(dj_poc main.cpp
        SineWaveGenerator.h)

target_link_libraries(dj_poc PRIVATE
        juce::juce_core
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_utils
)

# Set JUCE-specific definitions and options
target_compile_definitions(dj_poc PRIVATE
        JUCE_WEB_BROWSER=0 # Disable the JUCE web browser component
        JUCE_USE_CURL=0    # Disable JUCE's use of CURL
)
