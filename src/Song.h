//
// Created by dkasz on 13.04.24.
//

#ifndef DJ_CONSOLE_SONG_H
#define DJ_CONSOLE_SONG_H

#include <juce_audio_utils/juce_audio_utils.h>


class Song {
public:
    Song(juce::AudioFormatReaderSource* source, double sampleRate)
        : source(source), sampleRate(sampleRate) {

    }

    void load(juce::AudioTransportSource& transportSource) {
        transportSource.setSource(source.get(), 0, nullptr, sampleRate);
    }

private:
    std::unique_ptr<juce::AudioFormatReaderSource> source;
    double sampleRate;
};


#endif //DJ_CONSOLE_SONG_H
