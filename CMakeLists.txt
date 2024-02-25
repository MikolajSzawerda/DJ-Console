cmake_minimum_required(VERSION 3.26)
project(NoiseGen VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(JUCE)

juce_add_gui_app(noise_gen)
target_sources(noise_gen
        PRIVATE
        NoiseGenerator.h NoiseGenerator.cpp)
target_link_libraries(noise_gen
        PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
)
juce_generate_juce_header(noise_gen)

target_compile_definitions(noise_gen
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0)
