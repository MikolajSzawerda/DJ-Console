//
// Created by dkasz on 14.04.24.
//

#ifndef DJ_CONSOLE_PLAYLISTVIEWMODEL_H
#define DJ_CONSOLE_PLAYLISTVIEWMODEL_H

#include <juce_audio_utils/juce_audio_utils.h>

class PlaylistViewModel : public juce::ListBoxModel {
   public:
    int getNumRows() override { return (int)songNames.size(); }

    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected __attribute__((unused))) override {
        if (rowNumber == currentSong) {
            g.setColour(juce::Colours::red);
        } else {
            g.setColour(juce::Colours::white);
        }
        g.drawText(songNames[rowNumber], 0, 0, width, height, juce::Justification::centredLeft, true);
    }

    void addSong(const std::string& songName) { songNames.push_back(songName); }

    void setCurrentSong(int currentSongIdx) {
        if (currentSongIdx >= (int) songNames.size()) {
            throw std::invalid_argument("Invalid song index");
        }

        currentSong = currentSongIdx;
    }

   private:
    std::vector<std::string> songNames;
    int currentSong = 0;
};

#endif  // DJ_CONSOLE_PLAYLISTVIEWMODEL_H
