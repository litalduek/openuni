#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define FILE_EXTENSION_LENGTH 3
#define HEXA_OUTPUT_COL_SIZE 4

int checkFileOpen(FILE* file, char* fileName);
int exportFiles(char* fileName);

#endif