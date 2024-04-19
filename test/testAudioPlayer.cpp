#include <gtest/gtest.h>
#include "../src/Playlist.h"
#include "../src/audio/player/AudioPlayer.h"
#define SAMPLE_RATE 44100


TEST(AudioPlayer, LoadingFile){
    juce::ScopedJuceInitialiser_GUI guard;

    // given
    juce::File file(STUB_FILE);
    AudioPlayer player;
    juce::AudioBuffer<float> audioBuffer(1, 32);
    audioBuffer.clear();
    juce::AudioSourceChannelInfo buffer(audioBuffer);

    // when
    player.loadFile(file);
    player.prepareToPlay(32, SAMPLE_RATE);

    // then
    ASSERT_NO_THROW({player.getNextAudioBlock(buffer);});
}