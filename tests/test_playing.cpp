#include "CppUTest/TestHarness.h"
#include <CppUTest/UtestMacros.h>
#include "CppUTestExt/MockSupport.h"

extern "C" {
#include "/home/tjoslef/skola/music_player/src/headers/audio.h"
#include "/home/tjoslef/skola/music_player/src/headers/playing.h"
#include "/home/tjoslef/skola/music_player/src/headers/playlist.h"
#include "/home/tjoslef/skola/music_player/src/headers/read_platlist.h"
#include "/home/tjoslef/skola/music_player/src/headers/main.h"
}

TEST_GROUP(Music_player_test_group) {
    void setup() {
    }
    void teardown() {
        mock().clear();
    }
};

TEST(Music_player_test_group, Test_user_input) {
    mock().expectOneCall("fgets").andReturnValue("y\n");
    const char *prompt = "Test prompt";
    char result = get_user_input(prompt);
    CHECK_EQUAL('y', result);
    mock().checkExpectations();
}

TEST(Music_player_test_group, Test_checking_file) {
    const char* validPath = "/home/tjoslef/skola/music_player/tests/crang.wav";
    const char* invalidPath = "/path/to/invalid/file.wav";

    FILE* validFile = tmpfile();
    mock().expectOneCall("fopen").withParameter("filename", validPath).andReturnValue(validFile);
    mock().expectOneCall("fopen").withParameter("filename", invalidPath).andReturnValue(NULL);

    CHECK(CheckTheFile(validPath) != NULL);
    CHECK(CheckTheFile(invalidPath) == NULL);

    fclose(validFile);
    mock().checkExpectations();
}

TEST(Music_player_test_group, Test_creation_playlist) {
    sqlite3* db = (sqlite3*)1;
    const char* playlistName = "Test_Playlist";
    int id_playlist = -1;

    mock().expectOneCall("sqlite3_prepare_v2").andReturnValue(SQLITE_OK);
    mock().expectOneCall("sqlite3_bind_text").andReturnValue(SQLITE_OK);
    mock().expectOneCall("sqlite3_step").andReturnValue(SQLITE_DONE);
    mock().expectOneCall("sqlite3_finalize").andReturnValue(SQLITE_OK);

    int result = createPlaylist(db, playlistName, &id_playlist);

    CHECK_EQUAL(0, result);
    CHECK(id_playlist != -1);
    mock().checkExpectations();
}

TEST(Music_player_test_group, Test_find_playlist) {
    sqlite3* db = (sqlite3*)1;
    const char *validPlaylist = "Test_Playlist";
    const char *invalidPlaylist = "wrong_playlist";

    mock().expectOneCall("finding_playlist")
        .withParameter("db", db)
        .withParameter("playlist_name", validPlaylist)
        .andReturnValue(1);

    mock().expectOneCall("finding_playlist")
        .withParameter("db", db)
        .withParameter("playlist_name", invalidPlaylist)
        .andReturnValue(-1);

    CHECK_EQUAL(1, finding_playlist(db, validPlaylist));
    CHECK_EQUAL(-1, finding_playlist(db, invalidPlaylist));

    mock().checkExpectations();
}
