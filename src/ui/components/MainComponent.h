#ifndef DJ_CONSOLE_MAINCOMPONENT_H
#define DJ_CONSOLE_MAINCOMPONENT_H
#include <juce_audio_utils/juce_audio_utils.h>

class MainComponent : public juce::AudioAppComponent,
                      public juce::ChangeListener{
public:
  MainComponent(): state(Stopped){
    setSize (600, 400);

    addAndMakeVisible(slider);
    addAndMakeVisible(label);
    addAndMakeVisible(&openButton);
    addAndMakeVisible(&startButton);
    addAndMakeVisible(&stopButton);

    openButton.setButtonText ("Open...");
    openButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    openButton.onClick = [this] { openButtonClicked(); };
    startButton.setButtonText ("Start");
    startButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    startButton.onClick = [this] { playButtonClicked(); };
    stopButton.setButtonText ("Stop");
    stopButton.setColour (juce::TextButton::buttonColourId, juce::Colours::green);
    stopButton.onClick = [this] { stopButtonClicked(); };

    slider.setRange(0, 20.0);
    slider.setTextValueSuffix(" Hz");

    label.attachToComponent(&slider, true);
    label.setText ("Frequency", juce::dontSendNotification);


  }


private:
  enum TransportState
  {
    Stopped,
    Starting,
    Playing,
    Stopping
  };

  void changeState (TransportState newState)
  {
    if (state != newState)
    {
      state = newState;

      switch (state)
      {
      case Stopped:                           // [3]
        stopButton.setEnabled (false);
        startButton.setEnabled (true);
        transportSource.setPosition (0.0);
        break;

      case Starting:                          // [4]
        startButton.setEnabled (false);
        transportSource.start();
        break;

      case Playing:                           // [5]
        stopButton.setEnabled (true);
        break;

      case Stopping:                          // [6]
        transportSource.stop();
        break;
      }
    }
  }

  void changeListenerCallback(juce::ChangeBroadcaster *source) override {if (source == &transportSource)
    {
      if (transportSource.isPlaying())
        changeState (Playing);
      else
        changeState (Stopped);
    }}
  void resized() override {
    auto sliderLeft = 120;
    slider.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    openButton.setBounds (10, 50, getWidth() - 20, 20);
    startButton.setBounds (10, 80, getWidth() - 20, 20);
    stopButton.setBounds (10, 110, getWidth() - 20, 20);
  }

  void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override
  {
    if (readerSource.get() == nullptr)
    {
      bufferToFill.clearActiveBufferRegion();
      return;
    }

    transportSource.getNextAudioBlock (bufferToFill);
  }

  void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override
  {
    transportSource.prepareToPlay (samplesPerBlockExpected, sampleRate);
  }

  void releaseResources() override
  {
    transportSource.releaseResources();
  }

  void openButtonClicked()
  {
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file to play...",
                                                  juce::File{},
                                                  "*.wav");                     // [7]
    auto chooserFlags = juce::FileBrowserComponent::openMode
                        | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)     // [8]
                         {
                           auto file = fc.getResult();

                           if (file != juce::File{})                                                // [9]
                           {
                             auto* reader = formatManager.createReaderFor (file);                 // [10]

                             if (reader != nullptr)
                             {
                               auto newSource = std::make_unique<juce::AudioFormatReaderSource> (reader, true);   // [11]
                               transportSource.setSource (newSource.get(), 0, nullptr, reader->sampleRate);       // [12]
                               startButton.setEnabled (true);                                                      // [13]
                               readerSource.reset (newSource.release());                                          // [14]
                             }
                           }
                         });
  }

  void playButtonClicked()
  {
    changeState (Starting);
  }

  void stopButtonClicked()
  {
    changeState (Stopping);
  }

  juce::Slider slider;
  juce::Label label;
  juce::TextButton openButton;
  juce::TextButton startButton;
  juce::TextButton stopButton;

  std::unique_ptr<juce::FileChooser> chooser;
  juce::AudioFormatManager formatManager;
  std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
  juce::AudioTransportSource transportSource;
  TransportState state;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

#endif
