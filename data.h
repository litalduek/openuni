#ifndef DATA_H
#define DATA_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define COL_SIZE_ERROR_TABLE 28
#define ROW_SIZE_ERROR_TABLE 2
#define ERROR_DESCRIPTION_MAX_LENGTH 10

/*Global variables */
int g_err_code;
char* g_err_desc;
char* g_error_table[COL_SIZE_ERROR_TABLE][ROW_SIZE_ERROR_TABLE];

char* removeSpaces(char* str);
char* removeGarbge(char* str);
char* endStr(char* str);
void printError(int errorCode, int lineNum, char* extraData);

#endif
