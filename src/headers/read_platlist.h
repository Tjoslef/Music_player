#pragma once
#include "playlist.h"
#include "audio.h"
#include <string.h>
#include <sqlite3.h>
typedef struct SongNode {
    char *name;
    char *path;
    struct SongNode *next;
    struct SongNode *prev;
} SongNode;
const char *reading_song_form_db(sqlite3 *db,char *name);
SongNode *reading_in_order(sqlite3 *db,int id);
int finding_playlist(sqlite3 *db,const char *name_playlist);

