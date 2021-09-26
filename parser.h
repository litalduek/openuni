#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 80

#define COL_SIZE_OPERATION_TABLE 27
#define ROW_SIZE_OPERATION_TABLE 4
#define OPERATION_TABLE_COL_NAME_INDEX 0
#define OPERATION_TABLE_COL_TYPE_INDEX 2
#define OPERATION_NAME_LENGTH 6
#define REGISTERS_NUM 3
#define REGISTER_NUM_GROUP_0 3
#define REGISTER_NUM_GROUP_1 2
#define LEGAL_R_TYPE_REGISTER_NUM 3
#define LEGAL_I_TYPE_REGISTER_NUM 2
#define LEGAL_J_TYPE_REGISTER_NUM 1
#define LEGAL_R_TYPE_COMMA_NUM 2
#define LEGAL_I_TYPE_COMMA_NUM 2
#define LEGAL_J_TYPE_COMMA_NUM 0

#define MAX_LABEL_LENGTH 31
#define DATA_LABEL_ATTRIBUTE "data"
#define EXTERNAL_LABEL_ATTRIBUTE "external"
#define CODE_LABEL_ATTRIBUTE "code"
#define ENTRY_LABEL_ATTRIBUTE "data, entry"

#define MIN_DIRECTIVE_OPERANDS 1
#define EXTERN_SYMBOL_DEFAULT_VALUE 0

#define BYTE_BITS_SIZE 8
#define HALF_WORD_BITS_SIZE 16
#define WORD_BITS_SIZE 32
#define OPCODE_BITS_SIZE 6
#define REGISTER_BITS_SIZE 5
#define IMMED_BITS_SIZE 16
#define REG_BITS_SIZE 1
#define ADDRESS_BITS_SIZE 25
#define OPERATION_BITS_SIZE 32

/*This linked list stores the extern directives */
typedef struct Extern{
    char* labelName;
    int value;
    struct Extern* next;
}Extern;

/*This linked list stores .db .dh .dw .asciz directives */
typedef struct Directive {
    int dc;
    char* machineCode;
    struct Directive* next;
}Directive;

/*This linked list stores operations*/
/*Operation field will hold operation type correspondingly */
typedef struct Operation{
    int ic;
    char type;
    char* machineCode;
    void* operation;
    struct Operation* next;
}Operation;

/*This struct holds operation type 'R' */
/*This struct will be assigned as an operation type 'R' to the operations linked list */
typedef struct Operation_R {
    char* opcode;
    char* rs;
    char* rt;
    char* rd;
    char* funct;
    char* notinuse;
}Operation_R;

/*This struct holds operation type 'I' */
/*This struct will be assigned as an operation type 'I' to the operations linked list */
typedef struct Operation_I {
    char* opcode;
    char* rs;
    char* rt;
    char* immed;
}Operation_I;

/*This struct holds operation type 'J' */
/*This struct will be assigned as an operation type 'J' to the operations linked list */
typedef struct Operation_J {
    char* opcode;
    char* reg;
    char* address;
}Operation_J;

/*This linked list stores labels */
typedef struct Label{
    char* name;
    int value;
    char* attribute;
    struct Label* next;
}Label;

/*This is an implementation to dynamic array */
typedef struct Array{
  int *array;
  size_t used;
  size_t size;
} Array;

/*Enum used for recognizing command */
enum command {DIRECTIVE, OPERATION, COMMENT, EMPTY,UNRECONGNIZE};
/*Enum used for recognizing directive type*/
enum directive {D,ASCIZ,EXTERN,ENTRY,UNRECONGNIZEDIRECTIVE};
/*Enum used for parsers and recognizing states when checking text lines*/
enum validationState {COMMA,REGISTER,NUM,SPACE,IMMED};

/*Global variables */
int g_icf;
int g_dcf;
int g_externals;
int g_entries;
int g_operations;
int g_directives;
Label* labelHead;
Directive* directiveHead;
Extern* externalHead;
Operation* operationHead;

void initArray(Array *a, size_t initialSize) ;
void insertArray(Array *a, int element);
int validateImmed(char* operandStr, int immedIndex);
int validateLabel(char* labelName);
int validateOperationStructure(char* operandStr, char* operationType, char* operationName,int group);
enum command getCommand(char* str);
int getOperationGroup(char* operationName);
char* getCodeFromOpTable(char* operationName, int row);
enum directive getDirectiveType(char* directiveName);
int getBitsSize(char* directiveName);
Label* getLabel(char* labelName);
Label* getLabelFromLine(char* line);
char* extractLabelName(char* str);
char* extractOperartionName(char* str);
char* extracDirectiveName(char* str);
int extractDirectiveNumericOperands(char* insructionName, Array *regirsterList, char* str);
int extractDirectiveStringOperands(char* insructionName, Array *regirsterList, char* str);
int extractOperands_R(char* operationName, int* regirsterList, char* operandStr, int group);
int extractOperands_I(char* operationName, int* regirsterList, char* operandStr, int group);
int extractOperands_J(char* operationName, int* regirsterList, char* operandStr);
Label* addLabel(Label* currentLabel, char* labelName, int value, char* attribute);
Extern* addExternElement(Extern* externList,char* labelName, int value);
Directive* addDataElement(Directive* directiveList,int size);
Operation* addOperationElement(Operation* operationList);
void firstPass(FILE* file);
void secondPass(FILE* file);
char* toBinary(int number,int numOfBits);
void freeAlloc();
void freeArray(Array *a);

#endif
