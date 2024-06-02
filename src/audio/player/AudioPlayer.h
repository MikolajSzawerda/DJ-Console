//
// Created by mszawerd on 11.04.24.
//

#ifndef DJ_CONSOLE_AUDIOPLAYER_H
#define DJ_CONSOLE_AUDIOPLAYER_H

#include <juce_audio_utils/juce_audio_utils.h>

#include <utility>

#include "../../Playlist.h"
#include "../../Song.h"

class AudioPlayer : public juce::ChangeListener, public juce::ActionBroadcaster {
   public:
    AudioPlayer() {
        formatManager.registerBasicFormats();  // Register formats like WAV, MP3
        audioSource.addChangeListener(this);
        playlist = std::make_unique<Playlist>(Playlist());
        hasAnySourceLoaded = false;
        shouldLoopSong = false;
    }

    void releaseResources() { audioSource.releaseResources(); }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
        audioSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    }

    void loadFile(const juce::File &audioFile) {
        auto *reader = formatManager.createReaderFor(audioFile);
        if (reader != nullptr) {
            auto song = std::make_shared<Song>(new juce::AudioFormatReaderSource(reader, true), reader->sampleRate);
            playlist->addSong(song);

            if (!hasAnySourceLoaded) {
                playlist->next().value()->load(audioSource);
                hasAnySourceLoaded = true;
            }
        }
    }

    void play() {
        if (!audioSource.isPlaying()) {
            audioSource.start();
        }
    }

    void stop() {
        if (audioSource.isPlaying()) {
            audioSource.stop();
        }
    }

    void loadAndPlayNextSong() {
        auto nextSong = playlist->next();
        loadAndPlaySong(nextSong);

        if (!nextSong.has_value()) {
            sendActionMessage("playlist_end");
        } else {
            sendActionMessage("next_song");
        }
    }

    void loadAndPlayPreviousSong() {
        auto prevSong = playlist->previous();
        loadAndPlaySong(prevSong);

        if (prevSong.has_value()) {
            sendActionMessage("prev_song");
        }
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo &bufferToFill) {
        if (!hasAnySourceLoaded) {
            bufferToFill.clearActiveBufferRegion();
            return;
        }

        audioSource.getNextAudioBlock(bufferToFill);

        if (audioSource.hasStreamFinished()) {
            if (shouldLoopSong) {
                loadAndPlaySong(playlist->current());
            } else {
                loadAndPlayNextSong();
            }
        }
    }

    void changeListenerCallback(juce::ChangeBroadcaster *source __attribute__((unused))) override {
        // Handle transport source changes, if needed
    }

    void setGain(float newGain) {
        gain = newGain;
        audioSource.setGain(newGain);
    }

    void setLooping(bool shouldLoop) { shouldLoopSong = shouldLoop; }

    void mute() {
        if (!isMuted) {
            audioSource.setGain(0);
            isMuted = !isMuted;
        }
    }

    void unmute() {
        if (isMuted) {
            audioSource.setGain(gain);
            isMuted = !isMuted;
        }
    }

   private:
    void loadAndPlaySong(std::optional<std::shared_ptr<Song>> song) {
        if (song.has_value()) {
            song.value()->load(audioSource);
            play();
        }
    }

    bool hasAnySourceLoaded;
    bool shouldLoopSong;
    bool isMuted;
    float gain = 1.0;

    std::unique_ptr<Playlist> playlist;

    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource audioSource;
};

#endif  // DJ_CONSOLE_AUDIOPLAYER_H
