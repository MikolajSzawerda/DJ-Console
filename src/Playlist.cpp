//
// Created by dkasz on 14.04.24.
//

#include "Playlist.h"


Playlist::Playlist() {
    currentSongIdx = -1;
}

void Playlist::addSong(std::shared_ptr<Song> song) {
    playlist.push_back(song);
}

std::optional<std::shared_ptr<Song>> Playlist::current() {
    if (currentSongIdx == -1) {
        return std::nullopt;
    }

    return playlist[currentSongIdx];
};


std::optional<std::shared_ptr<Song>> Playlist::next() {
    if (playlist.empty()) {
        return std::nullopt;
    }

    if (currentSongIdx + 1 >= playlist.size()) {
        return std::nullopt;
    }

    return playlist[++currentSongIdx];
};

std::optional<std::shared_ptr<Song>> Playlist::previous() {
    if (currentSongIdx <= 0 || playlist.empty()) {
        return std::nullopt;
    }

    return playlist[--currentSongIdx];
}
