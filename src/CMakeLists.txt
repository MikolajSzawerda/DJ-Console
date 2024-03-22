target_sources(${PROJECT_NAME} PRIVATE DJConsole.cpp ui/windows/MainWindow.h ui/components/MainComponent.h)

target_link_libraries(${PROJECT_NAME} PRIVATE
        juce_audio_basics
        juce_audio_devices
        juce_audio_formats
        juce_audio_processors
        juce_audio_utils
        juce_core
        juce_data_structures
        juce_events
        juce_graphics
        juce_gui_basics
        juce_gui_extra
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)