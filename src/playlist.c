#include "./headers/playlist.h"
#include "./headers/audio.h"

int insertPlaylist(const char *song,sqlite3 *db,const char *name){
    if(db == NULL){
     fprintf(stderr,"database connect isnt establish");
    return 1;

    }
    const char *sql = "INSERT INTO songs (name, path) VALUES (?, ?)";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(db, sql,-1, &stmt,NULL);
    if(rc != SQLITE_OK){
        fprintf(stderr, "SQL error: %s \n", sqlite3_errmsg(db));
        return rc;
    }
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, song, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if(rc != SQLITE_DONE){
        fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return rc;
    }
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}
char *getName(const char *song){
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
    env_load(".", false);
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


