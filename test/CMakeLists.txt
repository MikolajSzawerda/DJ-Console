Include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0)  #v1.14
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    # Note: We add this directory to prevent installing Google Test
    # into the user's system and only use it for this project
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    add_subdirectory("${googletest_SOURCE_DIR}" "${googletest_BINARY_DIR}")
endif()

enable_testing()
include(GoogleTest)


add_executable(
        dj_console_tests
        test_Playlist.cpp
        ../src/Playlist.h
        ../src/Playlist.cpp
        ../src/Song.h
)
target_link_libraries(dj_console_tests
        PRIVATE
        GTest::gtest_main
        juce_audio_utils
)

target_compile_definitions(dj_console_tests
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)
gtest_discover_tests(dj_console_tests)
