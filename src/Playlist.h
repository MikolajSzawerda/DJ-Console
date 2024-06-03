//
// Created by dkasz on 13.04.24.
//

#ifndef DJ_CONSOLE_PLAYLIST_H
#define DJ_CONSOLE_PLAYLIST_H

#include "Song.h"

class Playlist {
   public:
    Playlist();
    void addSong(const std::shared_ptr<Song>& song);
    std::optional<std::shared_ptr<Song>> current();
    std::optional<std::shared_ptr<Song>> next();
    std::optional<std::shared_ptr<Song>> previous();
    std::optional<std::shared_ptr<Song>> first();

   private:
    std::vector<std::shared_ptr<Song>> playlist;
    int currentSongIdx;
};

#endif  // DJ_CONSOLE_PLAYLIST_H
