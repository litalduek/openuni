#include "parser.h"
#include "file_management.h"

/*Gets file returning 1 if file opens or 0 if isn't. */
int checkFileOpen(FILE* file, char* fileName){
    if(file == NULL){
        printf("Couldn't open file %s\n",fileName);
        return 0;
    }
    return 1;
}

/*The method gets the input file name and exports the machine code in hexa decimals. */
/*The method is passing over four loops which holds a different type of commands. */
/*Operations, directives, entries and externals exports to .ob, .ent, .ext files. */
int exportFiles(char* fileName){
    int i, j, ind, col;
    char *hex[HEXA_OUTPUT_COL_SIZE], *file = (char*)malloc((strlen(fileName)+FILE_EXTENSION_LENGTH+1) * sizeof(char));
    char temp1[9] = {0}, temp2[9] = {0}, temp3[9] = {0}, temp4[9] = {0};
    FILE *outputFile;
    Array *dataHexaArray;
    Operation* operationList;
    Directive* directiveList;
    initArray((Array*)&dataHexaArray,0);

    for (i = 0; i < HEXA_OUTPUT_COL_SIZE; i++){ /*Allocates array to store 4 hexadecimals numbers */
        hex[i] = (char*)malloc(2*sizeof(char*));
    }

    sprintf(file, "%s.ob", fileName);
    outputFile = fopen(file, "w");
    if(!checkFileOpen(outputFile,fileName)){
        return 0;
    }
    fprintf(outputFile,"  %d %d  \n", g_icf-100, g_dcf);

    if(g_operations > 0){ 
        operationList = operationHead;

        while(operationList){ /*Operation machine code length is constant */
            for (i = 0; i < 8; i ++){
                temp1[i] = operationList->machineCode[i+24]; /*Bits 24 to 31 */
                temp2[i] = operationList->machineCode[i+16]; /*Bits 16 to 23 */
                temp3[i] = operationList->machineCode[i+8]; /*Bits 8 to 15 */
                temp4[i] = operationList->machineCode[i]; /*Bits 0 to 7 */
            }

            sprintf(hex[0],"%02X", (int)strtol(temp1,NULL,2)); /*Hexadecimal stored in the first coloumn */
            sprintf(hex[1],"%02X", (int)strtol(temp2,NULL,2)); /*Hexadecimal stored in the second coloumn*/
            sprintf(hex[2],"%02X", (int)strtol(temp3,NULL,2)); /*Hexadecimal stored in the third coloumn*/
            sprintf(hex[3],"%02X", (int)strtol(temp4,NULL,2)); /*Hexadecimal stored in the fourth coloumn*/
            fprintf(outputFile,"%04d %s %s %s %s\n",operationList->ic, hex[0],hex[1],hex[2],hex[3]); /*Print row to output object file */

            operationList = operationList->next; 
        }
    }
    
    if(g_directives > 0){
        directiveList = directiveHead;

            while(directiveList != NULL){  /*Directive machine code length is dynamic */
                for (i = 0; i < 8; i++){ 
                    if(strlen(directiveList->machineCode) >= 32){ /*If directive length is equal or less than 32 bits */
                        temp1[i] = directiveList->machineCode[i+24]; /*Store bits 24 to 31 in temp1 */
                        temp2[i] = directiveList->machineCode[i+16]; /*Store bits 16 to 23 in temp2 */
                        if(i == 7){ /*If 7 iterations occured, temp1 and temp2 length is 8 bits and inserted to dynamic array */
                            insertArray(dataHexaArray,(int)strtol(temp1,NULL,2));    
                            insertArray(dataHexaArray,(int)strtol(temp2,NULL,2));    
                        }
                    }
                    if(strlen(directiveList->machineCode) >= 16){ /*If directive length is equal or less than to 16 bits */
                        temp3[i] = directiveList->machineCode[i+8]; /*Store bits 8 to 15 in temp3 */
                        if(i == 7){ /*If 7 iterations occured, temp3 length is 8 bits and inserted to dynamic array */
                            insertArray(dataHexaArray,(int)strtol(temp3,NULL,2));    
                        }
                    }
                    if(strlen(directiveList->machineCode) >= 8){ /*If directive length is equal or less than to 8 bits */
                        temp4[i] = directiveList->machineCode[i]; /*Store bits 0 to 7 in temp4 */
                        if(i == 7){  /*If 7 iterations occured, temp4 length is 8 bits and inserted to dynamic array */
                            insertArray(dataHexaArray,(int)strtol(temp4,NULL,2));   
                        }
                    }
                }
                directiveList = directiveList->next;
                }

            directiveList = directiveHead;
            ind = directiveList->dc;
            j = 0;

            for (i = 0; i < dataHexaArray->used; i++){ /*Iterating over the dynamic directive hexa array and print row to output object file */
                sprintf(hex[j++],"%02X", dataHexaArray->array[i]);
                if(j==4){
                    fprintf(outputFile,"%04d %s %s %s %s\n",ind, hex[0],hex[1],hex[2],hex[3]);
                    j = 0;
                    ind += 4;
                }
            }
            
            col = dataHexaArray->used %4; /*Remainder of dynamic array length, will be the last row in output file*/
            if(col == 1){ /*If remainder is 1, only one coloumn will have hexa decimal value in the last row of output file etc. */
                fprintf(outputFile,"%04d %s\n",ind, hex[0]);
            }    
            else if(col == 2){
                fprintf(outputFile,"%04d %s %s\n",ind, hex[0], hex[1]);
            }
            else if(col == 3){
                fprintf(outputFile,"%04d %s %s %s\n",ind, hex[0], hex[1], hex[2]);
            }
            else{
                fprintf(outputFile,"%04d %s %s %s %s\n",ind, hex[0],hex[1],hex[2],hex[3]);       
            }
    }

    freeArray(dataHexaArray);
    fclose(outputFile);

    if(g_externals > 0){ 
        Extern* externList = externalHead;
        sprintf(file, "%s.ext", fileName);
        outputFile = fopen(file, "w");
        if(!checkFileOpen(outputFile,fileName)){
            return 0;
        }
        while(externList->next != NULL){
            fprintf(outputFile," %s %04d\n", externList->labelName ,externList->value); /*Print row to output extern file */
            externList = externList->next;
        }
    fclose(outputFile);
    }


    if(g_entries > 0){ 
        Label* label = labelHead;
        sprintf(file, "%s.ent", fileName);
        outputFile = fopen(file, "w");
        if(!checkFileOpen(outputFile,fileName)){
            return 0;
        }
        while(label->next != NULL){
            if(!strcmp(label->attribute,ENTRY_LABEL_ATTRIBUTE)){
                fprintf(outputFile," %s %04d\n", label->name ,label->value); /*Print row to output entry file */
            }
            label = label->next;
        }
    fclose(outputFile);
    }

    freeAlloc();
    free(file);

    return 1;
}
