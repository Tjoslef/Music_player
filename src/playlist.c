#include "./headers/playlist.h"
#include "./headers/audio.h"
#include <sqlite3.h>
#include <stdio.h>

int insertPlaylist(const char *song, sqlite3 *db, const char *name, int playlist_id) {
    if (db == NULL) {
        fprintf(stderr, "Database connection isn't established\n");
        return 1;
    }
    sqlite3_stmt *stmt;
    int rc;
    // Insert song if it doesn't exist
    const char *sql = "INSERT OR IGNORE INTO songs (name, path) VALUES (?, ?)";
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, song, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE && rc != SQLITE_OK) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Get song_id from the song which we insert in previes function
    const char *sql_song_id = "SELECT id FROM songs WHERE path = ?;";
    rc = sqlite3_prepare_v2(db, sql_song_id, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    sqlite3_bind_text(stmt, 1, song, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Failed to get song_id: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    int song_id = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    // Get next position
    const char *sql_next_position = "SELECT COALESCE(MAX(position), 0) + 1 FROM playlist_songs WHERE playlist_id = ?;";
    rc = sqlite3_prepare_v2(db, sql_next_position, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    sqlite3_bind_int(stmt, 1, playlist_id);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        fprintf(stderr, "Failed to get next position: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    int next_position = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    // Insert into playlist_songs
    const char *sql_insert_playlist_song = "INSERT INTO playlist_songs (playlist_id, song_id, position) VALUES (?, ?, ?);";
    rc = sqlite3_prepare_v2(db, sql_insert_playlist_song, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return rc;
    }
    sqlite3_bind_int(stmt, 1, playlist_id);
    sqlite3_bind_int(stmt, 2, song_id);
    sqlite3_bind_int(stmt, 3, next_position);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Failed to insert into playlist_songs: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    return SQLITE_OK;
}int createPlaylist(sqlite3 *db, const char *name_of_playlist,int *id_playlist) {
    if (db == NULL) {
        fprintf(stderr, "Database connection isn't established\n");
        return SQLITE_ERROR;
    }

    const char *schema_sql =
        "CREATE TABLE IF NOT EXISTS songs ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "path TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS playlists ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS playlist_songs ("
        "playlist_id INTEGER,"
        "song_id INTEGER,"
        "position INTEGER,"
        "FOREIGN KEY (playlist_id) REFERENCES playlists(id),"
        "FOREIGN KEY (song_id) REFERENCES songs(id)"
        ");";

    char *errMsg = NULL;
    int rc = sqlite3_exec(db, schema_sql, NULL, NULL, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return rc;
    }

    const char *insert_sql = "INSERT INTO playlists (name) VALUES (?);";
    sqlite3_stmt *stmt;
    rc = sqlite3_prepare_v2(db, insert_sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    sqlite3_bind_text(stmt, 1, name_of_playlist, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    *id_playlist = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}char *getName(const char *song){
    char *path_cp = strdup(song);
    if(!path_cp){
        perror("failed malloc in getName");
        return NULL;
    }
    char *filename  = strrchr(path_cp,'/');
    if(filename == NULL){
        filename = path_cp;
    }else {
        filename++;
    }
    char *name = strdup(filename);
    free(path_cp);
    if(!name){
        perror("failed malloc in getName");
        return NULL;
    }

    char *dot = strrchr(name, '.');
    if(dot != NULL){
        *dot = '\0';
    }

    return name;
}
sqlite3 *OpenDB(int *rc){
    env_load("/home/tjoslef/skola/music_player/.env", false);
    const char* db_path = getenv("DB_PATH");
    if (db_path == NULL) {
        fprintf(stderr, "DB_PATH not found in .env file\n");
        *rc = SQLITE_ERROR;
        return NULL;
        }
    sqlite3 *db;
    *rc = sqlite3_open(db_path, &db);
    if (*rc != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        *rc = SQLITE_ERROR;
        sqlite3_close(db);
        return NULL;
        }
    return db;
}


