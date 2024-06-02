#include <memory>

#include "ui/windows/MainWindow.h"

class DJConsole : public juce::JUCEApplication {
   public:
    void initialise(const juce::String &commandLineParameters __attribute__((unused))) override {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }
    const juce::String getApplicationName() override { return juce::String("DJConsole"); }
    const juce::String getApplicationVersion() override { return juce::String("0.1.0"); }
    void shutdown() override { mainWindow = nullptr; }

   private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(DJConsole)