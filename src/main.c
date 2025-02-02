#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <time.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <libgen.h>
#define NUM_BUFFERS 4
#include "./lib/dotenv.h"
typedef struct {
    char chunkID[4];       // "RIFF"
    uint32_t chunkSize;    // File size - 8 bytes
    char format[4];        // "WAVE"
    char subchunk1ID[4];   // "fmt "
    uint32_t subchunk1Size;// Size of the fmt chunk (16 for PCM)
    uint16_t audioFormat;  // Audio format (1 = PCM)
    uint16_t numChannels;  // Number of channels (1 = mono, 2 = stereo)
    uint32_t sampleRate;   // Sample rate (e.g., 44100 Hz)
    uint32_t byteRate;     // Byte rate
    uint16_t blockAlign;   // Block align
    uint16_t bitsPerSample;// Bits per sample
    char subchunk2ID[4];   // "data"
    size_t num_samples;
    uint32_t data_size;
} WAVHeader;
void readingHeader(FILE *waves_file, WAVHeader *wheader,int *correct) {
    size_t read = fread(wheader, sizeof(WAVHeader), 1, waves_file);
    if (read != 1) {
        perror("wrong read");
        *correct = 1;
        return;
    }

    if (strncmp(wheader->chunkID, "RIFF", 4) != 0 || strncmp(wheader->format, "WAVE", 4) != 0) {
        fprintf(stderr, "wrong file format\n");
        *correct = 1;
        return;
    }

    char chunk_id[4];
    uint32_t chunk_size;
    long data_pos = 36;
    fseek(waves_file, 36, SEEK_SET);

    fread(chunk_id, 1, 4, waves_file);
    if (strncmp(chunk_id, "LIST", 4) == 0) {
        fread(&chunk_size, sizeof(uint32_t), 1, waves_file);
        data_pos += 8 + chunk_size;
        fseek(waves_file, chunk_size, SEEK_CUR);
        fread(chunk_id, 1, 4, waves_file);
    }

    if (strncmp(chunk_id, "data", 4) != 0) {
        fprintf(stderr, "Expected data chunk\n");
        *correct = 1;
        return;
    }

    fread(&chunk_size, sizeof(uint32_t), 1, waves_file);
    wheader->data_size = chunk_size;
    wheader->num_samples = wheader->data_size / ((wheader->bitsPerSample / 8) * wheader->numChannels);

    printf("Sample Rate: %u Hz\n", wheader->sampleRate);
    printf("Channels: %u\n", wheader->numChannels);
    printf("Bits Per Sample: %u\n", wheader->bitsPerSample);
    printf("bytes in file: %u \n", wheader->chunkSize);
    printf("Number of samples: %lu \n", wheader->num_samples);
    printf("data size: %u \n", wheader->data_size);

    data_pos += 8;
    fseek(waves_file, data_pos, SEEK_SET);
    long current_pos = ftell(waves_file);
    printf("Reading from position: %ld\n", current_pos);
}
void conf_of_al(ALuint *source,WAVHeader *wheader,FILE *waves_file,int *correct){
    ALenum format;
    if (wheader->numChannels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (wheader->numChannels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        fprintf(stderr, "Unsupported number of channels\n");
        *correct = 1;
        return;
    }
    int buffer_size = wheader->sampleRate * wheader->numChannels * (wheader->bitsPerSample / 8);
    unsigned char temp_buffer[buffer_size];
    ALuint buffers[buffer_size];
    alGenBuffers(NUM_BUFFERS, buffers);
    for(int i = 0;i < NUM_BUFFERS;i++){
        size_t bytes_read = fread(temp_buffer,1,buffer_size,waves_file);
        if(bytes_read == 0){
            fprintf(stderr, "No more data to read from file\n");
        *correct = 1;
            break;
        }
        alBufferData(buffers[i], format, temp_buffer, bytes_read, wheader->sampleRate);
         if (alGetError() != AL_NO_ERROR) {
            fprintf(stderr, "Error loading buffer data\n");
        *correct = 1;
            return;
        }
        alSourceQueueBuffers(*source, 1, &buffers[i]);
      ALint error = alGetError();
    if (error != AL_NO_ERROR) {
        fprintf(stderr, "Error queueing buffer: %s\n", alGetString(error));
        *correct = 1;
        return;
    }
    }
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        fprintf(stderr, "OpenAL error: %s\n", alGetString(error));
        *correct = 1;
    }

    ALint buffer_attached;
    alGetSourcei(*source, AL_BUFFERS_QUEUED, &buffer_attached);
    if (buffer_attached == 0) {
        fprintf(stderr, "No buffer attached to source\n");
        *correct = 1;
    }
}
void playIt(ALuint *source, double *time_played,float volume,int *correct) {
    alSourcef(*source, AL_GAIN, volume);
    alSourcePlay(*source);
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        printf("Error starting playback: %s\n", alGetString(error));
        *correct = 1;
        return;
    }
    ALint state;
    printf("Starting audio playback\n");

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    do {
        alGetSourcei(*source, AL_SOURCE_STATE, &state);
        if (alGetError() != AL_NO_ERROR) {
            fprintf(stderr, "Error getting source state\n");
        *correct = 1;
            break;
        }        printf("Playing... \n");
        nanosleep(&(struct timespec){0,500000000}, NULL);
    } while (state == AL_PLAYING);

    clock_gettime(CLOCK_MONOTONIC, &end_time);
    *time_played = (end_time.tv_sec - start_time.tv_sec) +
                   (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
}
void CleanUp(ALuint *source,ALCcontext *context,ALCdevice *device){
alSourceStop(*source);
    ALint numProcessed;
    alGetSourcei(*source, AL_BUFFERS_PROCESSED, &numProcessed);

    if (numProcessed > 0) {
        ALuint processedBuffers[numProcessed];
        alSourceUnqueueBuffers(*source, numProcessed, processedBuffers);
        alDeleteBuffers(numProcessed, processedBuffers);
        printf("Cleaned up %d processed buffers\n", numProcessed);
    }
   alDeleteSources(1, source);
    alcMakeContextCurrent(NULL);
   alcDestroyContext(context);
   alcCloseDevice(device);
}
int insertPlaylist(const char *song,sqlite3 *db,const char *name){

    const char *sql = "INSERT INTO songs (name, path) VALUES (?, ?)";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql,-1, &stmt,NULL);
    if(rc != SQLITE_DONE){
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


void PlayingSong(FILE *waves_file,int *correct){
int user_volume;
printf("Please input volume on rate of 1 to 10\n");
scanf("%d",&user_volume);
if(user_volume < 0){
    printf("volume cannot be negative\n");
    *correct = 1;
    return;
    }
if(user_volume > 10){
    printf("volume is out of scale\n");
    *correct = 1;
    return;
    }

float volume = (float)user_volume / 10.0f;
WAVHeader *wheader = malloc(sizeof(WAVHeader));
if(wheader == NULL){
    perror("Malloc of wheader is wrong");
    *correct = 1;
    return;
    }
ALCdevice *device = alcOpenDevice(NULL);
if (!device) {
    fprintf(stderr, "Failed to open audio device\n");
    *correct = 1;
    return;
}
ALCcontext *context = alcCreateContext(device, NULL);
if (!context) {
    fprintf(stderr, "Failed to create audio context\n");
    alcCloseDevice(device);
    *correct = 1;
    return;
}
alcMakeContextCurrent(context);
readingHeader(waves_file, wheader,correct);
if(*correct == 1){
    return;
    }
double time_played = 0;
ALuint source;
alGenSources(1, &source);
conf_of_al(&source, wheader,waves_file,correct);
    if(*correct == 1){
    return;
    }
if(!alIsSource(source)){
    fprintf(stderr,"audio is not playable \n" );
    }else {
        playIt(&source,&time_played,volume,correct);
        printf("audio was played for this time %f \n",time_played);
    }
CleanUp(&source,context, device);
free(wheader);

}
FILE* CheckTheFile(char *waves_file){
    FILE *wa_file = fopen(waves_file,"r");
    if(!wa_file){
            fprintf(stderr,"file doesnt exist");
            return NULL;
       }
    return wa_file;
}
sqlite3 *OpenDB(int *rc){
    env_load(".", false);
    const char* db_path = getenv("DB_PATH");
    if (db_path == NULL) {
        fprintf(stderr, "DB_PATH not found in .env file\n");
        return NULL;
        }
    sqlite3 *db;
    *rc = sqlite3_open("db_path", &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
        }
    return db;
}
int main(){
      char q1;
    printf("Do you want to play song y/n?\n");
    scanf("%c",&q1);
    while ((q1 = getchar()) != '\n' && q1 != EOF) { }
    if(q1 == 'y' || q1 == 'Y'){
        char file[256];
        printf("Give path to file: ");
        scanf("%255s", file);
        FILE *song_check = CheckTheFile(file);
        if(!song_check){
            return 1;
        }
        int correct = 0;
        PlayingSong(song_check, &correct);
        if(correct == 1){
            fprintf(stderr,"Playing went wrong\n");
        }
        fclose(song_check);
    }
    char input;
    printf("Do you want to create a playlist y/n \n");
    scanf(" %c", &input);
    while ((q1 = getchar()) != '\n' && q1 != EOF) { }
    if (input == 'y' || input == 'Y') {
        bool con = true;
       // int rc;
       // sqlite3 *db = OpenDB(&rc);
        while(con){
            char song[256];
            printf("past the file adress of the song \n");
            if(fgets(song, sizeof(song),stdin) != NULL){
                size_t len  =strlen(song);
                if(len > 0 && song[len-1] == '\n'){
                    song[len-1] = '\0';
                }

            }else {
            fprintf(stdout,"wrong path");
                continue;
            }
            while ((q1 = getchar()) != '\n' && q1 != EOF) { }
            FILE *song_check = CheckTheFile(song);
            if(!song_check){
                printf("wrong path");
                continue;
            }

            char *name = getName(song);
            printf("name of song: %c \n",*name);
            break;
        }
           /* if(insertPlaylist(song,db,name) != 0){
                fprintf(stderr, "Failed to insert song into playlist\n");
            }
            free(name);
            char q3;
            printf("do you want to add more songs? y/n");
            scanf("%c",&q3);
            if(q3 == 'n' || q3 == 'N'){
                con = false;
            }
        }
        sqlite3_close(db);
    }else if (input == 'n') {
        return 0;
    }else {
        printf("wrong input \n");
    }
    */


    return 0;

}
}
