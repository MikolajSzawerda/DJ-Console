
#ifndef DJ_CONSOLE_TRACK_H
#define DJ_CONSOLE_TRACK_H

#include <juce_audio_utils/juce_audio_utils.h>

#include <iostream>

#include "../../audio/player/AudioPlayer.h"
#include "PlaylistViewModel.h"

#define BUTTON_HEIGHT 30

class Track : public juce::GroupComponent, public juce::ActionListener {
   public:
    Track(const juce::String &componentName, const juce::String &labelText) : GroupComponent(componentName, labelText) {
        addOpenButton();
        addPlayButton();
        addVolumeSlider();
        addLoopButton();
        addMuteButton();
        addPrevButton();
        addNextButton();
        addLabel();
        addPlaylistView();

        player.addActionListener(this);
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) { player.getNextAudioBlock(bufferToFill); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
        player.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void releaseResources() { player.releaseResources(); }

    void actionListenerCallback(const juce::String &message) override {
        if (message == "playlist_end") {
            changeState(Stopped);
        }
    }

   private:
    void addOpenButton() {
        addAndMakeVisible(&openButton);
        openButton.setButtonText("Open...");
        openButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
        openButton.onClick = [this] { openButtonClicked(); };
    }
    void addPlayButton() {
        playButton.setButtonText("Play");
        playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
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
        loopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
        loopButton.onClick = [this] { loopButtonClicked(); };

        addAndMakeVisible(&loopButton);
    }

    void addMuteButton() {
        muteButton.setButtonText("Mute");
        muteButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
        muteButton.onClick = [this] { muteButtonClicked(); };

        addAndMakeVisible(&muteButton);
    }

    void addPrevButton() {
        prevButton.setButtonText("<");
        prevButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
        prevButton.onClick = [this] { player.loadAndPlayPreviousSong(); };

        addAndMakeVisible(&prevButton);
    }

    void addNextButton() {
        nextButton.setButtonText(">");
        nextButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blue);
        nextButton.onClick = [this] { player.loadAndPlayNextSong(); };

        addAndMakeVisible(&nextButton);
    }

    void loopButtonClicked() {
        isAudioLooping = !isAudioLooping;
        player.setLooping(isAudioLooping);

        auto colour = isAudioLooping ? juce::Colours::red : juce::Colours::blue;
        loopButton.setColour(juce::TextButton::buttonColourId, colour);
    }

    void muteButtonClicked() {
        if (isAudioMuted) {
            player.unmute();
        } else {
            player.mute();
        }

        isAudioMuted = !isAudioMuted;

        auto colour = isAudioMuted ? juce::Colours::red : juce::Colours::blue;
        muteButton.setColour(juce::TextButton::buttonColourId, colour);
    }

    void addLabel() {
        label.setText("<filename>", juce::dontSendNotification);
        label.setColour(juce::Label::backgroundColourId, juce::Colour(30, 30, 30));
        label.setColour(juce::Label::outlineColourId, juce::Colours::yellow);

        addAndMakeVisible(&label);
    }

    void addPlaylistView() {
        playlistView.setModel(&playlistViewModel);
        addAndMakeVisible(playlistView);
    }

    void trackVolumeChanged() { player.setGain((float)volumeSlider.getValue() * 4); }

    enum AudioState { Stopped, Playing };

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

        int marginLoopMute = 10;
        auto buttonLoopMuteWidth = (area.getWidth() - marginLoopMute) / 2;
        auto buttonLoopMuteArea = area.withHeight(BUTTON_HEIGHT);

        loopButton.setBounds(buttonLoopMuteArea.removeFromLeft(buttonLoopMuteWidth));
        buttonLoopMuteArea.removeFromLeft(marginLoopMute);
        muteButton.setBounds(buttonLoopMuteArea.removeFromLeft(buttonLoopMuteWidth));

        area.removeFromTop(BUTTON_HEIGHT + 10);

        int marginPrevNext = 10;
        auto buttonWidth = (area.getWidth() - marginPrevNext) / 2;
        auto buttonArea = area.withHeight(BUTTON_HEIGHT);

        prevButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
        buttonArea.removeFromLeft(marginPrevNext);
        nextButton.setBounds(buttonArea.removeFromLeft(buttonWidth));

        area.removeFromTop(BUTTON_HEIGHT + 10);

        playlistView.setBounds(area.removeFromTop(300));
    }

    void openButtonClicked() {
        changeState(Stopped);
        chooser = std::make_unique<juce::FileChooser>("Select a Wave file to play...", juce::File{}, "*.wav");
        auto chooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        chooser->launchAsync(chooserFlags, [this](const juce::FileChooser &fc) {
            auto file = fc.getResult();

            if (file != juce::File{}) {
                label.setText(file.getFileName(), juce::dontSendNotification);
                player.loadFile(file);
                playlistViewModel.addSong(file.getFileName().toStdString());
                playlistView.updateContent();
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
    juce::TextButton muteButton;
    juce::Slider volumeSlider;
    juce::ListBox playlistView;
    std::unique_ptr<juce::FileChooser> chooser;

    PlaylistViewModel playlistViewModel;
    AudioState state;
    AudioPlayer player;

    bool isAudioLooping = false;
    bool isAudioMuted = false;
};

#endif  // DJ_CONSOLE_TRACK_H
