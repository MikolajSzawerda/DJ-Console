
#ifndef DJ_CONSOLE_TRACK_H
#define DJ_CONSOLE_TRACK_H

#include "../../audio/player/AudioPlayer.h"
#include <juce_audio_utils/juce_audio_utils.h>
#include <iostream>

#define BUTTON_HEIGHT 30

class Track : public juce::GroupComponent {
public:
    Track(const juce::String &componentName, const juce::String &labelText) : GroupComponent(componentName, labelText) {
        addOpenButton();
        addPlayButton();
        addVolumeSlider();
        addLoopButton();
        addPrevButton();
        addNextButton();
        addLabel();
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
        player.getNextAudioBlock(bufferToFill);
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
        player.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void releaseResources() { player.releaseResources(); }

private:
    void addOpenButton() {
        addAndMakeVisible(&openButton);
        openButton.setButtonText("Open...");
        openButton.setColour(juce::TextButton::buttonColourId,
                             juce::Colours::green);
        openButton.onClick = [this] { openButtonClicked(); };

    }
    void addPlayButton() {
        playButton.setButtonText("Play");
        playButton.setColour(juce::TextButton::buttonColourId,
                             juce::Colours::green);
        playButton.onClick = [this] { playButtonClicked(); };

        addAndMakeVisible(&playButton);
    }

     void addVolumeSlider() {
         volumeSlider.setRange(0.0, 2.0);
         volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
         volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
         volumeSlider.setValue(1.0);
         volumeSlider.onValueChange = [this]() { trackVolumeChanged(); };

         addAndMakeVisible(volumeSlider);
     }

    void addLoopButton() {
        loopButton.setButtonText("Loop");
        loopButton.setColour(juce::TextButton::buttonColourId,
                             juce::Colours::blue);
        loopButton.onClick = [this] { loopButtonClicked(); };

        addAndMakeVisible(&loopButton);
    }

    void addPrevButton() {
        prevButton.setButtonText("<");
        prevButton.setColour(juce::TextButton::buttonColourId,
                             juce::Colours::blue);
        prevButton.onClick = [this] { player.loadAndPlayPreviousSong(); };

        addAndMakeVisible(&prevButton);
    }

    void addNextButton() {
        nextButton.setButtonText(">");
        nextButton.setColour(juce::TextButton::buttonColourId,
                             juce::Colours::blue);
        nextButton.onClick = [this] { player.loadAndPlayNextSong(); };

        addAndMakeVisible(&nextButton);
    }

    void loopButtonClicked() {
        isAudioLooping = !isAudioLooping;
        player.setLooping(isAudioLooping);

        if (isAudioLooping) {
            loopButton.setColour(juce::TextButton::buttonColourId,
                                 juce::Colours::red);
        } else {
            loopButton.setColour(juce::TextButton::buttonColourId,
                                 juce::Colours::blue);
        }
    }

     void addLabel() {
         label.setText("<filename>", juce::dontSendNotification);
         label.setColour(juce::Label::backgroundColourId, juce::Colour(30, 30, 30));
         label.setColour(juce::Label::outlineColourId, juce::Colours::yellow);

         addAndMakeVisible(&label);
     }

    void trackVolumeChanged() {
        player.setGain((float) volumeSlider.getValue() * 4);
    }

    enum AudioState {
        Stopped, Playing
    };

    void changeState(AudioState newState) {
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

        volumeSlider.setBounds(area.removeFromTop(BUTTON_HEIGHT));
        area.removeFromTop(10);

        loopButton.setBounds(area.removeFromTop(BUTTON_HEIGHT));
        area.removeFromTop(10);

        int margin = 10;
        auto buttonWidth = (area.getWidth() - margin) / 2;
        auto buttonArea = area.withHeight(BUTTON_HEIGHT);

        prevButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
        buttonArea.removeFromLeft(margin);
        nextButton.setBounds(buttonArea.removeFromLeft(buttonWidth));

        area.removeFromTop(10);
    }

    void openButtonClicked() {
        changeState(Stopped);
        chooser = std::make_unique<juce::FileChooser>(
                "Select a Wave file to play...", juce::File{}, "*.wav");
        auto chooserFlags = juce::FileBrowserComponent::openMode |
                            juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(
                chooserFlags, [this](const juce::FileChooser &fc) {
                    auto file = fc.getResult();

                    if (file != juce::File{}) {
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
    juce::TextButton loopButton;
    juce::TextButton prevButton;
    juce::TextButton nextButton;
    juce::Slider volumeSlider;
    std::unique_ptr<juce::FileChooser> chooser;
    AudioState state;
    AudioPlayer player;
    bool isAudioLooping = false;
};

#endif // DJ_CONSOLE_TRACK_H
