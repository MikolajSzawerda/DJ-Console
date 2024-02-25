//
// Created by mszawerd on 25.02.24.
//

#ifndef DJ_POC_SINEWAVEGENERATOR_H
#define DJ_POC_SINEWAVEGENERATOR_H

#include <juce_audio_basics/juce_audio_basics.h>
#include <cmath>

class SineWaveGenerator : public juce::AudioSource {
public:
    SineWaveGenerator() {}

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
        currentSampleRate = sampleRate;
        currentAngle = 0.0;
        angleDelta = 0.0;
        setFrequency(440.0); // Default to A4
    }

    void releaseResources() override {}

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
        auto *leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto *rightChannel = bufferToFill.buffer->getNumChannels() > 1 ? bufferToFill.buffer->getWritePointer(1,
                                                                                                              bufferToFill.startSample)
                                                                       : nullptr;

        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample) {
            auto currentSample = (float) std::sin(currentAngle);
            currentAngle += angleDelta;
            leftChannel[sample] = currentSample;

            if (rightChannel != nullptr)
                rightChannel[sample] = currentSample;
        }
    }

    void setFrequency(double frequency) {
        auto cyclesPerSample = frequency / currentSampleRate;
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

private:
    double currentSampleRate, currentAngle, angleDelta;
};

#endif //DJ_POC_SINEWAVEGENERATOR_H
