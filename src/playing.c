#include "headers/playing.h"
#include "headers/audio.h"
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

/*add function stop and continue
 add function skip 5s
*/
