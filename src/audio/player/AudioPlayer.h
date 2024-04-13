//
// Created by mszawerd on 11.04.24.
//

#ifndef DJ_CONSOLE_AUDIOPLAYER_H
#define DJ_CONSOLE_AUDIOPLAYER_H

#include <juce_audio_utils/juce_audio_utils.h>

#include <utility>

class AudioPlayer : public juce::ChangeListener {
public:
    AudioPlayer() {
        formatManager.registerBasicFormats(); // Register formats like WAV, MP3
        transportSource.addChangeListener(this);
    }

    void releaseResources() { transportSource.releaseResources(); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
        transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void loadFile(const juce::File &audioFile) {
        auto *reader = formatManager.createReaderFor(audioFile);
        if (reader != nullptr) {
            std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader, true));
            transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
            readerSource = std::move(newSource);
        }
    }

    void play() {
        if (!transportSource.isPlaying()) {
            transportSource.start();
        }
    }

    void stop() {
        if (transportSource.isPlaying()) {
            transportSource.stop();
        }
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
        if (readerSource == nullptr) {
            bufferToFill.clearActiveBufferRegion();
            return;
        }

        transportSource.getNextAudioBlock(bufferToFill);

        if (transportSource.hasStreamFinished() && loopCurrentSong)
        {
            transportSource.setPosition(0);
            play();
        }
    }

    void changeListenerCallback(juce::ChangeBroadcaster *source) override {
        // Handle transport source changes, if needed
    }

    void setGain(float newGain) {
        transportSource.setGain(newGain);
    }

    void setLooping(bool shouldLoop) {
        loopCurrentSong = shouldLoop;
    }

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    bool loopCurrentSong = false;
};

#endif // DJ_CONSOLE_AUDIOPLAYER_H
