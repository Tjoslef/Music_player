#pragma once
#include <string.h>
#include <sqlite3.h>
#include "/home/tjoslef/skola/music_player/src/lib/dotenv.h"
#define BUFFER_SIZE 256
#define BUFFER_LEN 1024
int insertPlaylist(const char *song, sqlite3 *db, const char *name,int id_playlist);
char *getName(const char *song);
sqlite3 *OpenDB(int *rc);
int createPlaylist(sqlite3 *db, const char *name_of_playlist,int *id_playlist);
int finding_playlist(sqlite3 *db,const char *name_playlist);
