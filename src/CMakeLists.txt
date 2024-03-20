target_sources(${PROJECT_NAME} PRIVATE DJConsole.cpp ui/windows/MainWindow.h ui/components/MainComponent.h)

target_link_libraries(${PROJECT_NAME} PRIVATE
        juce::juce_audio_utils
        juce::juce_gui_extra)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)