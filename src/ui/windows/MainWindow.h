#ifndef DJ_POC_MAINWINDOW_H
#define DJ_POC_MAINWINDOW_H
#include <juce_gui_basics/juce_gui_basics.h>
#include "../components/MainComponent.h"

class MainWindow : public juce::DocumentWindow
{
public:
  MainWindow(juce::String name)  : DocumentWindow (name,
                                                 juce::Colours::black,
                                                 DocumentWindow::allButtons)
  {
    setUsingNativeTitleBar (true);
    setContentOwned (new MainComponent, true);

    setResizable (true, false);
    setResizeLimits (300, 250, 10000, 10000);
    centreWithSize (getWidth(), getHeight());
    setVisible (true);
  }

  void closeButtonPressed() override
  {
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
  }

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
};
#endif
