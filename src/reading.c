#include "./headers/audio.h"
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
