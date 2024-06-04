#include <gtest/gtest.h>
#include "../src/Playlist.h"
#define SAMPLE_RATE 44100

TEST(Playlist, ShouldInitPlaylistBeEmpty) {
    // given
    Playlist playlist;

    // then
    ASSERT_FALSE(playlist.current().has_value());
}

TEST(Playlist, AddSongToPlaylist) {
    // given
    auto song = std::make_shared<Song>(nullptr, SAMPLE_RATE);
    Playlist playlist;

    // when
    playlist.addSong(song);
    playlist.next();

    // then
    ASSERT_EQ(playlist.current(), song);
}

TEST(Playlist, NavigateThroughPlaylist) {
    // given
    auto song1 = std::make_shared<Song>(nullptr, SAMPLE_RATE);
    auto song2 = std::make_shared<Song>(nullptr, SAMPLE_RATE*2);
    Playlist playlist;

    // when
    playlist.addSong(song1);
    playlist.addSong(song2);
    playlist.next();
    playlist.next();

    // then
    ASSERT_EQ(playlist.current(), song2);
    ASSERT_EQ(playlist.previous(), song1);
}

TEST(Playlist, NavigateToFirstSong) {
    // given
    auto song1 = std::make_shared<Song>(nullptr, SAMPLE_RATE);
    auto song2 = std::make_shared<Song>(nullptr, SAMPLE_RATE*2);
    auto song3 = std::make_shared<Song>(nullptr, SAMPLE_RATE*3);
    Playlist playlist;

    playlist.addSong(song1);
    playlist.addSong(song2);
    playlist.addSong(song3);
    playlist.next();
    playlist.next();
    playlist.next();

    // when
    playlist.first();

    // then
    ASSERT_EQ(playlist.current(), song1);
}