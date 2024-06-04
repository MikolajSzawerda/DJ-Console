//
// Created by mszawerd on 11.04.24.
//

#ifndef DJ_CONSOLE_AUDIOPLAYER_H
#define DJ_CONSOLE_AUDIOPLAYER_H

#include <juce_audio_utils/juce_audio_utils.h>

#include <utility>

#include "../../Playlist.h"
#include "../../Song.h"

#define CROSSFADE_UPDATE_RATE_HZ 30

class AudioPlayer : public juce::ChangeListener, public juce::ActionBroadcaster, private juce::Timer {
   public:
    AudioPlayer() {
        formatManager.registerBasicFormats();  // Register formats like WAV, MP3
        currentAudioSource().addChangeListener(this);
        playlist = std::make_unique<Playlist>();
    }

    void releaseResources() {
        currentAudioSource().releaseResources();
        crossfadeAudioSource().releaseResources();
        mixerSource.releaseResources();
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
        delayBufferSize = static_cast<int>(sampleRate * delayTime);
        delayBuffer.resize(delayBufferSize);
        std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);

        currentAudioSource().prepareToPlay(samplesPerBlockExpected, sampleRate);
        crossfadeAudioSource().prepareToPlay(samplesPerBlockExpected, sampleRate);

        currentAudioSource().setGain(1.0f);
        crossfadeAudioSource().setGain(0.0f);

        mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

        mixerSource.addInputSource(&currentAudioSource(), false);
        mixerSource.addInputSource(&crossfadeAudioSource(), false);
    }

    void loadFile(const juce::File &audioFile) {
        auto *reader = formatManager.createReaderFor(audioFile);
        if (reader != nullptr) {
            auto song = std::make_shared<Song>(new juce::AudioFormatReaderSource(reader, true), reader->sampleRate);
            playlist->addSong(song);

            if (!hasAnySourceLoaded) {
                playlist->next().value()->load(currentAudioSource());
                hasAnySourceLoaded = true;
            }
        }
    }

    void play() {
        if (!currentAudioSource().isPlaying()) {
            if(isCrossfading){
                startTimerHz(CROSSFADE_UPDATE_RATE_HZ);
                crossfadeAudioSource().start();
            }
            currentAudioSource().start();
        }
    }

    void stop() {
        if (currentAudioSource().isPlaying()) {
            if(isCrossfading){
                stopTimer();
                crossfadeAudioSource().stop();
            }
            currentAudioSource().stop();
        }
    }

    void loadAndPlayNextSong() {
        if(isCrossfading){
            crossfadeAudioSource().stop();
        }
        auto nextSong = playlist->next();
        loadAndPlaySong(nextSong);

        if (!nextSong.has_value()) {
            sendActionMessage("playlist_end");
            auto firstSong = playlist->first();
            if (firstSong.has_value()) {
                firstSong->get()->load(currentAudioSource());
            }
        } else {
            sendActionMessage("next_song");
        }
    }

    void loadAndPlayPreviousSong() {
        if(isCrossfading){
            crossfadeAudioSource().stop();
        }

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

        if(shouldStartCrossFade()){
            startCrossfade();
        }

        mixerSource.getNextAudioBlock(bufferToFill);
        applyPanning(bufferToFill);
        if (isDelayEffectActivated) {
            applyDelayEffect(bufferToFill);
        }


        if (currentAudioSource().hasStreamFinished()) {
            if (shouldLoopSong) {
                loadAndPlaySong(playlist->current());
                return;
            }
            if(!playlist->peekNext().has_value()){
                handlePlaylistFinished();
            }
        }
    }

    void handlePlaylistFinished(){
        sendActionMessage("playlist_end");
        auto firstSong = playlist->first();
        if (firstSong.has_value()) {
            firstSong->get()->load(currentAudioSource());
        }
    }

    bool shouldStartCrossFade(){
        if(shouldLoopSong){
            return false;
        }
        if(playlist->songsCount()<2 || playlist->songsLeft() <= 1){
            return false;
        }
        if(currentAudioSource().isPlaying() && !isCrossfading){
            auto position = currentAudioSource().getCurrentPosition();
            auto length = currentAudioSource().getLengthInSeconds();
            return position >= length - crossfadeDurationSeconds;
        }
        return false;
    }



    void changeListenerCallback(juce::ChangeBroadcaster *source __attribute__((unused))) override {
        // Handle transport source changes, if needed
    }

    void setGain(float newGain) {
        gain = newGain;
        currentAudioSource().setGain(newGain);
    }

    void setLooping(bool shouldLoop) { shouldLoopSong = shouldLoop; }

    void setDelayEffect(bool delay) {
        isDelayEffectActivated = delay;

        if (!delay) {
            delayBufferIndex = 0;
            std::fill(delayBuffer.begin(), delayBuffer.end(), 0.0f);
        }
    }

    void mute() {
        if (!isMuted) {
            currentAudioSource().setGain(0);
            isMuted = !isMuted;
        }
    }

    void unmute() {
        if (isMuted) {
            currentAudioSource().setGain(gain);
            isMuted = !isMuted;
        }
    }

    void setPan(float newPan)
    {
        pan = newPan;
    }

private:
    void loadAndPlaySong(std::optional<std::shared_ptr<Song>> song) {
        if (song.has_value()) {
            song.value()->load(currentAudioSource());
            play();
        }
    }

    void applyDelayEffect(const juce::AudioSourceChannelInfo &bufferToFill) {
        auto* leftChannel = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightChannel = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            float delayedSample = delayBuffer[delayBufferIndex];

            delayBuffer[delayBufferIndex] = leftChannel[sample] + (delayedSample * feedback);

            leftChannel[sample] += delayedSample;
            rightChannel[sample] += delayedSample;

            delayBufferIndex = (delayBufferIndex + 1) % delayBufferSize;
        }
    }

    void applyPanning(const juce::AudioSourceChannelInfo &bufferToFill){
        if (bufferToFill.buffer->getNumChannels() == 2)
        {
            float leftGain = 1.0f - pan;
            float rightGain = pan;

            bufferToFill.buffer->applyGain(0, bufferToFill.startSample, bufferToFill.numSamples, leftGain);
            bufferToFill.buffer->applyGain(1, bufferToFill.startSample, bufferToFill.numSamples, rightGain);
        }
    }


    void timerCallback() override
    {
        applyCrossFading();
    }

    void startCrossfade()
    {
        if(!playlist->peekNext().has_value()){
            return;
        }
        playlist->peekNext().value()->load(crossfadeAudioSource());
        crossfadeAudioSource().start();

        isCrossfading = true;
        crossfadePosition = 0.0f;
        crossfadeStep = 1.0f / (CROSSFADE_UPDATE_RATE_HZ * crossfadeDurationSeconds); // crossfadeDuration in seconds
        startTimerHz(CROSSFADE_UPDATE_RATE_HZ);
    }

    void applyCrossFading(){
        if (!isCrossfading) {
            return;
        }
        const float fadeInGain = juce::jlimit(0.0f, 1.0f, crossfadePosition);

        currentAudioSource().setGain(1.0f-fadeInGain);
        crossfadeAudioSource().setGain(fadeInGain);
        crossfadePosition += crossfadeStep;

        if (crossfadePosition >= 1.0f)
        {
            handleEndCrossFade();
        }
    }

    void handleEndCrossFade(){
        crossfadePosition = 0.0f;
        isCrossfading = false;
        stopTimer();
        currentAudioSource().setGain(0.0f);
        crossfadeAudioSource().setGain(1.0f);
        currentAudioSource().stop();
        currentAudioSourceIdx=(currentAudioSourceIdx+1)%2;
        if(playlist->peekNext().has_value()){
            playlist->next();
            sendActionMessage("next_song");
        }
    }

    juce::AudioTransportSource& currentAudioSource(){
        return audioSources.at(currentAudioSourceIdx);
    }

    juce::AudioTransportSource& crossfadeAudioSource(){
        return audioSources.at((currentAudioSourceIdx+1)%2);
    }

    bool hasAnySourceLoaded=false;
    bool shouldLoopSong=false;
    bool isMuted;
    bool isDelayEffectActivated;
    float gain = 1.0;

    float delayTime = 0.5f;
    float feedback = 0.5f;
    std::vector<float> delayBuffer;
    int delayBufferIndex = 0;
    int delayBufferSize;

    std::unique_ptr<Playlist> playlist;

    juce::AudioFormatManager formatManager;
    std::array<juce::AudioTransportSource, 2> audioSources;
    juce::MixerAudioSource mixerSource;
    std::array<juce::AudioTransportSource, 2>::size_type currentAudioSourceIdx = 0;

    bool isCrossfading = false;
    float crossfadePosition = 0.0f;
    float crossfadeStep = 0.0f;
    float crossfadeDurationSeconds = 1.0f;
    float pan=0.0f;

};

#endif  // DJ_CONSOLE_AUDIOPLAYER_H
