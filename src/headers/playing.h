#pragma once
#include <stdio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <string.h>
#include <sys/types.h>
#include <sndfile.h>
#include <time.h>
#define checkAlError(retval)                                      \
{                                                                 \
    ALenum alError = alGetError();                               \
    if (alError != AL_NO_ERROR) {                               \
        fprintf(stderr, "OpenAL Error: %s (%d)\n", alGetString(alError), alError); \
        return retval;                                             \
    }                                                             \
}
void *PlayingSong(void *arg);
int CheckTheFile(const char *waves_file);
