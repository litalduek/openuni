#include "data.h"
extern int g_err_code;
extern char* g_err_desc;
extern char* g_error_table[COL_SIZE_ERROR_TABLE][ROW_SIZE_ERROR_TABLE];

/*Receives string */
/*Removes spaces from line for easier validation and returns */
char* removeSpaces(char* str){
    int i, x;
    for (i = x = 0 ; str[i]; ++i)
    {
        if(!isspace(str[i])){
            str[x] = str[i];
            x += 1;                
        }
    }
    str[x] = '\0';
    
    return str;
}

/*Receives a string */
/*Ends the string with '\0' character in the end of the string and returns*/
char* endStr(char* str){
    if(isspace(str[strlen(str)-1])){
        str[strlen(str)-1] = '\0';
    }
    else{
        str[strlen(str)] = '\0';
    }
    return str;
}

/*Receives string */
/*Removes extra spaces (between two words one space at most) for easier validation and replacing tabs with space*/
/*Returns the cleaned string */
char* removeGarbge(char* str){
    int i, x;
    if(strchr(str, ';') != NULL){
      str[0] = ';';
      str[1] = '\0';
    }
    else{
        for (i = x = 0 ; str[i]; ++i)
        {
            if(str[i] == '\t'){
              str[i] = ' ';
            }
            if((!isspace(str[i])) || (i>0 && !isspace(str[i-1]))){
                str[x] = str[i];
                x += 1;                
            }
        }
        if(str[x-1] == '\n'){
            str[x-1] = '\0';
        }
        else{
            str[x] = '\0';
        }
    }
    return str;
}

/*Receives error code, line num and extra data */
/*Prints error by error code table */
void printError(int errorCode, int lineNum, char* extraData){
    int code, i;
    char* errorDescription;
    errorDescription = (char*)malloc(sizeof(char)*100);
    for (i = 0; i < strlen((char*)g_error_table[0])-1; i++)
    {
        code = atoi(g_error_table[i][0]);
        if(errorCode == code){   
            errorDescription = g_error_table[i][1];         
            break;
        }
    }
    if(strcmp(g_err_desc,"")){
        printf("line[%d]: %s %s\n", lineNum,errorDescription, g_err_desc);
        g_err_desc = "";
    }
    else{
        printf("line[%d]: %s %s\n", lineNum,errorDescription,extraData);
    }
}
