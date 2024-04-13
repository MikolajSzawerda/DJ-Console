
#ifndef DJ_CONSOLE_TRACK_H
#define DJ_CONSOLE_TRACK_H
#include "../../audio/player/AudioPlayer.h"
#include <juce_audio_utils/juce_audio_utils.h>
#define BUTTON_HEIGHT 30

class Track: public juce::GroupComponent
{
public:
  Track(const juce::String &componentName, const juce::String &labelText)
      : GroupComponent(componentName, labelText){
    playButton.setButtonText("HELLO");
    addAndMakeVisible(&playButton);
    addAndMakeVisible(&label);
    addAndMakeVisible(&openButton);
    addAndMakeVisible(&playButton);

    openButton.setButtonText("Open...");
    openButton.setColour(juce::TextButton::buttonColourId,
                         juce::Colours::green);
    openButton.onClick = [this] { openButtonClicked(); };
    playButton.setButtonText("Play");
    playButton.setColour(juce::TextButton::buttonColourId,
                         juce::Colours::green);
    playButton.onClick = [this] { playButtonClicked(); };

    label.setText("<filename>", juce::dontSendNotification);
    label.setColour(juce::Label::backgroundColourId, juce::Colour(30, 30, 30));
    label.setColour(juce::Label::outlineColourId, juce::Colours::yellow);
  }

  void
  getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill)  {
    player.getNextAudioBlock(bufferToFill);
  }

  void prepareToPlay(int samplesPerBlockExpected, double sampleRate)  {
    player.prepareToPlay(samplesPerBlockExpected, sampleRate);
  }

  void releaseResources()  { player.releaseResources(); }

private:
  enum TransportState { Stopped, Playing };


  void changeState(TransportState newState) {
    if (state != newState) {
      state = newState;

      switch (state) {
      case Stopped:
        player.stop();
        playButton.setButtonText("Play");
        break;

      case Playing:
        player.play();
        playButton.setButtonText("Stop");
        break;
      }
    }
  }

  void resized() override {
    auto area = getLocalBounds().reduced(20);

    label.setBounds(area.removeFromTop(BUTTON_HEIGHT));
    area.removeFromTop(10);
    openButton.setBounds(area.removeFromTop(BUTTON_HEIGHT));
    area.removeFromTop(10);
    playButton.setBounds(area.removeFromTop(BUTTON_HEIGHT));
    area.removeFromTop(10);
  }

  void openButtonClicked() {
    changeState(Stopped);
    chooser = std::make_unique<juce::FileChooser>(
        "Select a Wave file to play...", juce::File{},
        "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode |
                        juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync(
        chooserFlags, [this](const juce::FileChooser &fc)
        {
          auto file = fc.getResult();

          if (file != juce::File{})
          {
            label.setText(file.getFileName(), juce::dontSendNotification);
            player.loadFile(file);
          }
        });
  }

  void playButtonClicked() {
    switch (state) {
    case Stopped:
      changeState(Playing);
      break;
    case Playing:
      changeState(Stopped);
      break;
    }
  }


  juce::Label label;
  juce::TextButton openButton;
  juce::TextButton playButton;
  std::unique_ptr<juce::FileChooser> chooser;
  TransportState state;
  AudioPlayer player;

};
#endif // DJ_CONSOLE_TRACK_H
