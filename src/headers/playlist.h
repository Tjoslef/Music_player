#pragma once
#include <sqlite3.h>
#include "/home/tjoslef/skola/music_player/src/lib/dotenv.h"
int insertPlaylist(const char *song, sqlite3 *db, const char *name);
char *getName(const char *song);
sqlite3 *OpenDB(int *rc);

