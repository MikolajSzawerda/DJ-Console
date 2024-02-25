#include "SineWaveGenerator.h"
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_utils/juce_audio_utils.h>

class SineWaveApplication : public juce::JUCEApplication {
public:
  SineWaveApplication() {}

  const juce::String getApplicationName() override {
    return "Sine Wave Application";
  }

  const juce::String getApplicationVersion() override { return "1.0"; }

  void initialise(const juce::String &) override {
    sineWaveGenerator.prepareToPlay(512, 44100);
    audioSourcePlayer.setSource(&sineWaveGenerator);
    auto error = deviceManager.initialise(0, 2, nullptr, true);
    jassert(error.isEmpty());

    deviceManager.addAudioCallback(&audioSourcePlayer);
    deviceManager.initialiseWithDefaultDevices(0, 2);
  }

  void shutdown() override {
    deviceManager.removeAudioCallback(&audioSourcePlayer);
    audioSourcePlayer.setSource(nullptr);
  }

private:
  juce::AudioDeviceManager deviceManager;
  juce::AudioSourcePlayer audioSourcePlayer;
  SineWaveGenerator sineWaveGenerator;
};

START_JUCE_APPLICATION(SineWaveApplication)