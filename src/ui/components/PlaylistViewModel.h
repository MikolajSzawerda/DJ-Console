//
// Created by dkasz on 14.04.24.
//

#ifndef DJ_CONSOLE_PLAYLISTVIEWMODEL_H
#define DJ_CONSOLE_PLAYLISTVIEWMODEL_H

#include <juce_audio_utils/juce_audio_utils.h>

class PlaylistViewModel : public juce::ListBoxModel {
public:
    int getNumRows() override {
        return (int) songNames.size();
    }

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override {
        g.setColour(juce::Colours::white);
        g.drawText(songNames[rowNumber], 0, 0, width, height,juce::Justification::centredLeft, true);
    }

    void addSong(const std::string& songName) {
        songNames.push_back(songName);
    }

private:
    std::vector<std::string> songNames;
};

#endif //DJ_CONSOLE_PLAYLISTVIEWMODEL_H
