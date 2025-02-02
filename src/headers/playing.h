#pragma once
#include <stdio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <string.h>

void PlayingSong(FILE *waves_file, int *correct);
FILE* CheckTheFile(char *waves_file);
