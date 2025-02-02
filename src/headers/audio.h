#pragma once
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#define NUM_BUFFERS 4
#include <string.h>
#include <stdlib.h>
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


