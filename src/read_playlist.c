#include "headers/read_platlist.h"
#include <sqlite3.h>
#include <stdio.h>
const char *reading_song_form_db(sqlite3 *db,char *name){
    if(db== NULL){
        fprintf(stderr, "database isnt establish");
        return NULL;
        }
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM songs WHERE name = ?";
    int rc = sqlite3_prepare_v2(db,sql,-1,&stmt,NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "something gone wrong in preparation %s \n",sqlite3_errmsg(db));
        return NULL;
        }
    if(name != NULL){
        sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
        }
    char *song_path = NULL;
    if((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char *path = sqlite3_column_text(stmt,0);
        song_path = strdup((char *)path);

    }else if (rc == SQLITE_DONE){
            fprintf(stderr, "No song this name found: %s \n",name);
    }else{
        fprintf(stderr, "Error executing query: %s \n",sqlite3_errmsg(db));

    }
    sqlite3_finalize(stmt);
 return song_path;
}
void free_song_list(SongNode *head){
    while(head != NULL){
    SongNode *temp = head;
    head = head->next;
    free(temp->path);
    free(temp->name);
    free(temp);
    }
}
SongNode *reading_in_order(sqlite3 *db, int id) {
    if (db == NULL) {
        fprintf(stderr, "Database isn't established\n");
        return NULL;
    }

    sqlite3_stmt *stmt;
    const char *sql =
        "SELECT songs.path, songs.name, playlist_songs.position "
        "FROM playlist_songs "
        "INNER JOIN songs ON playlist_songs.song_id = songs.id "
        "WHERE playlist_songs.playlist_id = ? "
        "ORDER BY playlist_songs.position";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Preparation failed: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    rc = sqlite3_bind_int(stmt, 1, id);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Binding failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }

    SongNode *head = NULL;
    SongNode *tail = NULL;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        SongNode *newSong = malloc(sizeof(SongNode));
        if (newSong == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free_song_list(head);
            sqlite3_finalize(stmt);
            return NULL;
        }

        newSong->path = strdup((const char *)sqlite3_column_text(stmt, 0));
        newSong->name = strdup((const char *)sqlite3_column_text(stmt, 1));

        if (newSong->path == NULL || newSong->name == NULL) {
            fprintf(stderr, "String duplication failed\n");
            free(newSong->path);
            free(newSong->name);
            free(newSong);
            free_song_list(head);
            sqlite3_finalize(stmt);
            return NULL;
        }

        newSong->next = NULL;
        newSong->prev = NULL;

        if (head == NULL) {
            head = tail = newSong;
        } else {
            tail->next = newSong;
            newSong->prev = tail;
            tail = newSong;
        }

        printf("Added song: %s (%s)\n", newSong->name, newSong->path);
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Query execution failed: %s\n", sqlite3_errmsg(db));
        free_song_list(head);
        head = NULL;
    }

    sqlite3_finalize(stmt);
    return head;
}int finding_playlist(sqlite3 *db,const char *name_playlist){

   if(db == NULL){

    fprintf(stderr,"databses isnt establish");
    return -1;
    }
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id FROM playlists WHERE trim(name) = trim(?)";
    int rc = sqlite3_prepare_v2(db, sql,-1,&stmt,NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "something gone wrong in preparation in finding_playlist %s %s \n",sqlite3_errmsg(db),name_playlist);
        return -1;
        }
    rc = sqlite3_bind_text(stmt,1,name_playlist,-1, SQLITE_STATIC);
    if(rc != SQLITE_OK){
       fprintf(stderr, "Binding error in finding_playlist: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }
    int playlist_id = -1;
    if((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        playlist_id = sqlite3_column_int(stmt, 0);
        printf("Found the playlist id %d \n",playlist_id);
        }else if (rc == SQLITE_DONE){
            fprintf(stderr, "No playlist  this name found: '%s' \n",name_playlist);
    }else{
        fprintf(stderr, "Error executing query: %s \n",sqlite3_errmsg(db));
        return playlist_id = -2;
    }
    sqlite3_finalize(stmt);
    return playlist_id;
}
SongNode *moving_in_playlist(SongNode *head,int direction){
    if(direction == -1){
        if(head && head->prev){
            return head->prev;

        }else{
            return head;

        }
    }else if (direction == 1) {
        if(head && head->next){
           return head->next;

        }else {
         return head;
        }
    }else{
    fprintf(stderr,"something sus in moving_in_playlist \n");
        return NULL;
    }
}
