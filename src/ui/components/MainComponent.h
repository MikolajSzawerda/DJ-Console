#ifndef DJ_CONSOLE_MAINCOMPONENT_H
#define DJ_CONSOLE_MAINCOMPONENT_H
#include <juce_gui_basics/juce_gui_basics.h>

class MainComponent : public juce::Component{
public:
  MainComponent(){
    setSize (600, 400);

    addAndMakeVisible(slider);
    slider.setRange(0, 20.0);
    slider.setTextValueSuffix(" Hz");

    addAndMakeVisible(label);
    label.attachToComponent(&slider, true);
    label.setText ("Frequency", juce::dontSendNotification);
  }
  void resized() override { auto sliderLeft = 120;
    slider.setBounds (sliderLeft, 20, getWidth() - sliderLeft - 10, 20); }

private:


private:
  juce::Slider slider;
  juce::Label label;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};

#endif
