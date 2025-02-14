#include "headers/playing.h"
#include "headers/audio.h"
void clear_buffer(ALuint *buffers,short *temp_buffer){
            free(temp_buffer);
            free(buffers);
}
void reading_wav(const char *filename,WAVHeader *wheader,SNDFILE **file_ptr, SF_INFO *sfinfo){
    *sfinfo = (SF_INFO){0};
    SNDFILE* file = sf_open(filename, SFM_READ, sfinfo);
    if(!file){
        fprintf(stderr,"error in opening of file ");
    }
    *file_ptr = file;
    printf("Frames: %ld\n", sfinfo->frames);
    printf("Sample rate: %d\n", sfinfo->samplerate);
    printf("Channels: %d\n", sfinfo->channels);
    wheader->numChannels = sfinfo->channels;
    wheader->sampleRate = sfinfo->samplerate;
    sf_count_t offset = sf_seek(file, 0, SEEK_SET);
    if(offset == - 1){
        fprintf(stderr,"error in getting pointer \n");
        sf_close(file);
    }

}
void conf_of_al(ALuint *source, WAVHeader *wheader,int *correct,SNDFILE* file) {
    ALenum format;
    if (wheader->numChannels == 1) {
        format = AL_FORMAT_MONO16;
    } else if (wheader->numChannels == 2) {
        format = AL_FORMAT_STEREO16;
    } else {
        fprintf(stderr, "Unsupported number of channels: %d\n", wheader->numChannels);
        *correct = 1;
        return;
    }

    int buffer_size = wheader->sampleRate * wheader->numChannels * sizeof(short);
    short *temp_buffer = malloc(buffer_size);
    if (!temp_buffer) {
        fprintf(stderr, "Failed to allocate temporary buffer\n");
        *correct = 1;
        return;
    }
    printf("Buffer size: %d bytes\n", buffer_size);

    ALuint *buffers = NULL;
    int num_buffers = 0;
    while (1) {
        sf_count_t frames_read = sf_readf_short(file, temp_buffer, buffer_size / (wheader->numChannels * sizeof(short)));
        if (frames_read == 0) {
            if (sf_error(file)) {
               fprintf(stderr, "Error reading from file: %s\n", sf_strerror(file));
                *correct = 1;
                clear_buffer(buffers, temp_buffer);
                return;
            }
            break;
        }

        ALuint buffer;
        alGenBuffers(1, &buffer);
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            fprintf(stderr, "Failed to generate buffer: %s\n", alGetString(error));
            *correct = 1;
            clear_buffer(buffers, temp_buffer);
            return;
        }

        alBufferData(buffer, format, temp_buffer,frames_read * wheader->numChannels * sizeof(short), wheader->sampleRate);
        error = alGetError();
        if (error != AL_NO_ERROR) {
            fprintf(stderr, "Failed to load buffer data: %s\n   error code %d", alGetString(error),error);
            *correct = 1;
            clear_buffer(buffers, temp_buffer);
            return;
        }

        num_buffers++;
        ALuint *new_buffers = realloc(buffers, num_buffers * sizeof(ALuint));
        if (new_buffers == NULL) {
            fprintf(stderr, "Failed to reallocate buffer array\n");
            *correct = 1;
            clear_buffer(buffers, temp_buffer);
            return;
        }
        buffers = new_buffers;
        buffers[num_buffers - 1] = buffer;

        alSourceQueueBuffers(*source, 1, &buffer);
        error = alGetError();
        if (error != AL_NO_ERROR) {
            fprintf(stderr, "Failed to queue buffer: %s\n", alGetString(error));
            *correct = 1;
            clear_buffer(buffers, temp_buffer);
            return;
        }
    }

    printf("Loaded %d buffers\n", num_buffers);
    free(temp_buffer);
}
void playIt(ALuint *source, double *time_played,int *correct,float volume) {
    alSourcef(*source, AL_GAIN, volume);
   alSourcePlay(*source);
    if (alGetError() != AL_NO_ERROR) {
        fprintf(stderr, "Error starting playback\n");
        *correct = 1;
        return;
    }
    ALint state;
    ALint total_buffers;
    ALint buffers_processed;
    alGetSourcei(*source, AL_BUFFERS_QUEUED,&total_buffers);
    printf("Starting audio playback\n");

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    do {
        alGetSourcei(*source, AL_SOURCE_STATE, &state);
        alGetSourcei(*source, AL_BUFFERS_PROCESSED, &buffers_processed);
        if (alGetError() != AL_NO_ERROR && buffers_processed < total_buffers) {
            fprintf(stderr, "Error getting source state\n");
        *correct = 1;
            break;
        }        printf("Playing... \n");
        nanosleep(&(struct timespec){0,500000000}, NULL);
    } while (buffers_processed < total_buffers);

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
void PlayingSong(const char *filename,int *correct){
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
    WAVHeader wheader;
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
    SNDFILE *file = NULL;
    SF_INFO sfinfo = {0};
    reading_wav(filename, &wheader, &file, &sfinfo);
    ALuint source;
    alGenSources(1, &source);
    conf_of_al(&source, &wheader, correct, file);
        if(*correct == 1){
        sf_close(file);
        return;
        }
    sf_close(file);
    if(!alIsSource(source)){
        fprintf(stderr,"audio is not playable \n" );
        }else {
        double time_played = 0;
            playIt(&source,&time_played,correct,volume);
            printf("audio was played for this time %f \n",time_played);
        }
    CleanUp(&source,context, device);

}
int CheckTheFile(const char *waves_file){
    FILE *wa_file = fopen(waves_file,"rb");
    if(!wa_file){
            fprintf(stderr,"file doesnt exist");
            return 1;
        fclose(wa_file);
       }
    fclose(wa_file);
    return 0;
}
/*add function stop and continue
 add function skip 5s
*/
