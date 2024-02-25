//
// Created by mszawerd on 25.02.24.
//

#ifndef NOISEGEN_NOISEGENERATOR_H
#define NOISEGEN_NOISEGENERATOR_H
#include "JuceHeader.h"
class NoiseGenerator : public juce::AudioAppComponent {
public:
  NoiseGenerator() {
    setSize(800, 600);
    setAudioChannels(0, 2);
  };
  ~NoiseGenerator() override { shutdownAudio(); }
  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
    juce::String message;
    message << "Preparing to play audio...\n";
    message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
    message << " sampleRate = " << sampleRate;
    juce::Logger::getCurrentLogger()->writeToLog(message);
  };
  void releaseResources() override {
    juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources");
  };
  void getNextAudioBlock(const AudioSourceChannelInfo &bufferToFill) override {
    for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels();
         ++channel) {
      auto *buffer = bufferToFill.buffer->getWritePointer(
          channel, bufferToFill.startSample);

      for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        buffer[sample] = random.nextFloat() * 0.25f - 0.125f;
    }
  };

private:
  juce::Random random;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NoiseGenerator)
};

#endif // NOISEGEN_NOISEGENERATOR_H
