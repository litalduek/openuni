/*This program translates assembly language into machine code.*/
/*The program pass over the assembly code */
/*The program then eliminates all types of unnecesary parts like extra spaces and comments */
/*First Pass parses operations and directives with operands that are already known to the compiler */
/*The Second Pass program complete the machine code and looking for labels that declared after the operation or directive */
/*Parsing the commands into a binary and exports to files in hexadecimal format. */
#include "data.h"
#include "parser.h"
#include "file_management.h"

/*Global variables*/
int g_icf; /*Icf value */
int g_dcf; /*Dcf value */
int g_operations; /*Number of operations */
int g_directives; /*Number of directives */
int g_externals; /*Number of externals */
int g_entries; /*Number of entries */
int g_err_code; /*Error counter */

/*Global linked list*/
Label* labelHead;
Directive* directiveHead;
Extern* externalHead;
Operation* operationHead;

/*Additional error description */
char* g_err_desc; 

/*Global static tables*/
char* g_error_table[COL_SIZE_ERROR_TABLE][ROW_SIZE_ERROR_TABLE]  = {{"-999","Empty label list"},{"-998","Label not found"},{"-3","Operation code not found"},
{"-4","unrecognize directive"},{"-5","Missing number"},{"-6","String is empty"},{"-7","Wrong operation spelling"},
{"-8","Invalid comma operation"},{"-9","Unrecognize operation"},{"-10","Missing comma operation"},{"-11","Missing register operation"},
{"-12","Illegal immediate"},{"-13","Illegal registers number operation"},{"-14","Empty label"},{"-15","Illegal register"},
{"-16","unrecognize command"},{"-17","Label defined external"},{"-18","Label already defined not as an external attribute"},
{"-19","Label already defined"},{"-20","Label too long"},{"-21","Keyword cannot be defined as a label name"},
{"-22","Label is empty"},{"-23","Operation group not found"},{"-24","Line too long"},{"-25","Quatation mark is missing"},{"-26","Illegal label"},
{"-27","Label can't start with a digit"},{"-28","Missing immediate"}};
char* g_operation_table[COL_SIZE_OPERATION_TABLE][ROW_SIZE_OPERATION_TABLE] = {{"000000","00001","R","add"},{"000000","00010","R","sub"},{"000000","00011","R","and"},{"000000","00100","R","or"},
{"000000","00101","R","nor"},{"000001","00001","R","move"},{"000001","00010","R","mvhi"},{"000001","00011","R","mvlo"},{"001100","","I","addi"},
{"001101","","I","subi"},{"001100","","I","andi"},{"001101","","I","ori"},{"001110","","I","nori"},{"001111","","I","bne"},{"010000","","I","beq"},
{"010001","","I","blt"},{"010010","","I","bgt"},{"010011","","I","lb"},{"010100","","I","sb"},{"010101","","I","lw"},{"010110","","I","sw"},
{"010111","","I","lh"},{"011000","","I","sh"},{"011110","","J","jmp"},{"011111","","J","la"},{"100000","","J","call"},{"111111","","J","stop"}};

int main(int argc, char *argv[]){
    if(argc<2){
        printf("Error passing file arguments %s", *argv);
    }
    else{
        for (argv++; *argv; argv++){
            FILE *inputFile;
            char *inputFileName = (char*) malloc((strlen(*argv)+1) * sizeof(char));
            strcpy(inputFileName,*argv);
            inputFile = fopen(strcat(inputFileName,".as"),"r");

            if(!checkFileOpen(inputFile, inputFileName)){
                continue;
            }
            else{
                g_icf = 0, g_dcf = 0, g_err_code = 0, g_operations = 0, g_directives = 0, g_entries = 0, g_externals = 0;
                g_err_desc = (char*)malloc(ERROR_DESCRIPTION_MAX_LENGTH * sizeof(char));
                labelHead = NULL, directiveHead = NULL, externalHead =NULL, operationHead = NULL;
                printf("Starting read file %s\n", inputFileName);
                printf("Starting first pass\n");
                firstPass(inputFile);
                if(g_err_code != 0){ 
                    printf("%d errors recongnized, skipping %s file \n",g_err_code, inputFileName);
                    continue; 
                }
                else{
                    printf("First pass completed !\n");
                    printf("Starting second pass\n");
                    rewind(inputFile);
                    secondPass(inputFile);
                    if(g_err_code != 0){ 
                        printf("%d errors recongnized, skipping %s file \n",g_err_code, inputFileName);
                        continue; 
                    }
                    else{
                        printf("Second pass completed !\n");
                        printf("Exporting files ... \n");
                        if(!exportFiles(*argv)){
                            printf("Error occurred while creating files \n");
                        }
                        printf("Files created successfully \n");
                    }
                }
                fclose(inputFile);
            }
        }
    }

    return 0;
}