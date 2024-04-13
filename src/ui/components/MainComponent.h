#ifndef DJ_CONSOLE_MAINCOMPONENT_H
#define DJ_CONSOLE_MAINCOMPONENT_H
#include <juce_audio_utils/juce_audio_utils.h>
#include "Track.h"
#include <utility>


#define TRACKS_NUM 5

class MainComponent : public juce::AudioAppComponent {
public:
  std::vector<std::unique_ptr<Track>> initTracks() {
    std::vector<std::unique_ptr<Track>> tempTracks;
    for(int i=0; i<TRACKS_NUM;i++){
      tempTracks.push_back(std::make_unique<Track>("", "Track " + std::to_string(i)));
    }
    return tempTracks;
  }
  MainComponent() :tracks(initTracks()){
    setSize(600, 400);
    for(auto& t: tracks){
      addAndMakeVisible(t.get());
    }
    setAudioChannels(0, 2);
  }
  ~MainComponent(){
  }

private:

  void resized() override {
      tracks.at(0)->setBounds(getLocalBounds().reduced(20));
      juce::FlexBox flexBox;
      flexBox.flexDirection = juce::FlexBox::Direction::row;
      flexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
      flexBox.alignContent = juce::FlexBox::AlignContent::stretch;
      flexBox.alignItems = juce::FlexBox::AlignItems::stretch;

      for (auto& track : tracks)
      {
        flexBox.items.add(juce::FlexItem(*track).withFlex(1));
      }

      flexBox.performLayout(getLocalBounds().toFloat());
  }

  void
  getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) override {
      juce::AudioBuffer<float> tempBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.buffer->getNumSamples());
      bufferToFill.clearActiveBufferRegion();

      for (auto& track : tracks) {
        tempBuffer.clear();
        juce::AudioSourceChannelInfo tempInfo(&tempBuffer, bufferToFill.startSample, bufferToFill.numSamples);
        track->getNextAudioBlock(tempInfo);

        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
          bufferToFill.buffer->addFrom(channel, bufferToFill.startSample, tempBuffer, channel, 0, bufferToFill.numSamples);
        }
      }
  }

  void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override {
      for(auto& t: tracks){
        t->prepareToPlay(samplesPerBlockExpected, sampleRate);
      }
  }

  void releaseResources() override {
      std::for_each(tracks.begin(), tracks.end(),
                    [](const std::unique_ptr<Track>& t) {t->releaseResources();});
  }

  std::vector<std::unique_ptr<Track>> tracks;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

#endif
