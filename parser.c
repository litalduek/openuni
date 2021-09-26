#include "parser.h"
#include "data.h"

extern int g_icf;
extern int g_dcf;
extern int g_operations;
extern int g_directives;
extern int g_externals;
extern int g_entries;
extern char* g_operation_table[COL_SIZE_OPERATION_TABLE][ROW_SIZE_OPERATION_TABLE];
extern Extern* externalHead;
extern Directive* directiveHead;
extern Operation* operationHead;
extern Label* labelHead;

/*Initiate a dynamic array. */
/*Return Array struct initiate with the requested intial size */
void initArray(Array *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(int));
  a->used = 0;
  a->size = initialSize;
}

/*Insert into dynamic array element */
/*If Array size is used, two bytes allocated */
void insertArray(Array *a, int element) {
  if (a->used == a->size) {
    a->size += 2;
    a->array = realloc(a->array, a->size * sizeof(int));
  }
  a->array[a->used++] = element;
}

/*Validate immediate format */
/*Get operands string and immediate index in the command */
/*Return error code 0 if valid */
int validateImmed(char* operandStr, int immedIndex){
    char* immed, *start, *end;
    int i, signCounter = 0;
    start = operandStr + immedIndex;
    end = strstr(start,",");
    immed = (char*)malloc((end - start + 1 ) * sizeof(char));
    if(end != NULL){
        memcpy(immed,start ,end - start);
        for (i = 0; i < strlen(immed); i++){
            if(immed[i] == '-' || immed[i] == '+'){
                signCounter += 1;
            }
            if(((!isdigit(immed[i])) && (immed[i] != '-') && (immed[i] != '+')) || (signCounter > 1)){
                g_err_desc = immed; 
                return -12;
            }
        }
    }
    else{
        return -28;
    }
    return 0;
}

/*Validate registers format */
/*Receive registers list  */
/*Return error code 0 if valid */
int validateRegisters(char** registers, int regNum){
    char* reg = (char*)malloc(5 * sizeof(char));
    char c;
    int num, i, j;
    for (i = 0; i < regNum; i++){
        reg = registers[i];        
        endStr(reg);
        for (j = 0; j < strlen(reg); j++){
            c = reg[j];
            if((isdigit(c) == 0) || (c == '-') || (c == '+')){
                g_err_desc = reg;
                return -15;
            }
        }
        num = atoi(reg);
        if(num > 31 || num < 0){
            g_err_desc = reg;
            return -15;
        }
    }
    return 0;
}

/*Validate label format */
/*Receive label name*/
/*Return error code 0 if valid */
int validateLabel(char* labelName){
    int i;

    if(!strcmp(labelName,"")){ 
        return -22;
    }
    else if(getLabel(labelName) != NULL){
        return -19;
    }
    else if(strlen(labelName)>MAX_LABEL_LENGTH){
        return -20;
    }
    else if(isdigit(labelName[0])){
        return -27;
    }
    else if((strcmp(labelName,"db") == 0) || (strcmp(labelName,"dh") == 0) || (strcmp(labelName,"dw") == 0) || 
    (strcmp(labelName,"asciz") == 0) || (strcmp(labelName,"add") == 0) || (strcmp(labelName,"sub") == 0) || 
    (strcmp(labelName,"and") == 0) || (strcmp(labelName,"or") == 0) || (strcmp(labelName,"nor") == 0) || 
    (strcmp(labelName,"move") == 0) || (strcmp(labelName,"mvhi") == 0) || (strcmp(labelName,"mvlo") == 0) || 
    (strcmp(labelName,"addi") == 0) || (strcmp(labelName,"subi") == 0) || (strcmp(labelName,"andi") == 0) || 
    (strcmp(labelName,"ori") == 0) || (strcmp(labelName,"nori") == 0) || (strcmp(labelName,"bne") == 0) || 
    (strcmp(labelName,"beq") == 0) || (strcmp(labelName,"blt") == 0) || (strcmp(labelName,"bgt") == 0) || 
    (strcmp(labelName,"lb") == 0) || (strcmp(labelName,"sb") == 0) || (strcmp(labelName,"lw") == 0) || 
    (strcmp(labelName,"sw") == 0) || (strcmp(labelName,"lh") == 0) || (strcmp(labelName,"sh") == 0) || 
    (strcmp(labelName,"jmp") == 0) || (strcmp(labelName,"la") == 0) || 
    (strcmp(labelName,"call") == 0) || (strcmp(labelName,"stop") == 0)){
        return -21;
    }
    for (i = 0; i < strlen(labelName); i++)
    {
        if((!isdigit(labelName[i])) && (!isalpha(labelName[i]))){
            return -26;
        }
    }
    return 0;
}

/*Validate operation format */
/*Receive operand string, operation type, operation name and group */
/*Validate operation string corresponding to the operation type  */
/*Validate operation type corresponding to the operation group */
/*Return error code 0 if valid */
int validateOperationStructure(char* operandStr, char* operationType, char* operationName,int group){
    int immedCounter = 0, immedIndex = 0, registerCounter = 0,commaCounter = 0, errorCode = 0, i = 0;
    char c;
    enum validationState prevState, currState;
    switch (*operationType)
    {
    case 'R':{   
        if(group){ /*If copy operation, increasing comma and register couter to be equal to logic and arithmetic*/
            commaCounter+=1;
            registerCounter+=1;
        }
        do{
            if((currState != NUM)){
                prevState = currState;
            }
            c = operandStr[i];
            switch (c)
            {
            case '$':{
                currState = REGISTER;
                registerCounter+=1;
                break;
            }
            case ',':{
                currState = COMMA;
                commaCounter+=1;
                break;
            }
            default:
                currState = NUM;
                continue;
            }
            if(currState == COMMA && registerCounter == 0){ /*comma before first register */
                return -11;
            }
            else if(currState == COMMA && registerCounter == LEGAL_R_TYPE_REGISTER_NUM){ /*comma after last register*/
                return -8;
            }
            else if(currState == COMMA && prevState == COMMA){ /*Comma after comma */
                return -8;
            }
            else if(currState == REGISTER && prevState == REGISTER){ /*Register after register */
                return -10;
            }
            else{
                continue;
            }
        }while(i ++ < strlen(operandStr) - 1);
        
        if(registerCounter < LEGAL_R_TYPE_REGISTER_NUM){ /*Less than valid registers num in an operation*/
            return -11;
        }
        else if(registerCounter > (LEGAL_R_TYPE_REGISTER_NUM + 1)){ /*Greater than valid registers num in an operation*/
            return -13;
        }
        else if(commaCounter < LEGAL_R_TYPE_COMMA_NUM){  /*Less than valid commas num in an operation*/
            return -10;
        }
        else if(commaCounter > (LEGAL_R_TYPE_COMMA_NUM + 1)){  /*Greater than valid commas num in an operation*/
            return -8;
        }
        break;
    }
    case 'I':{
        if(!group){ /*Logic, arithmetic, load and store operations */
            do{
                if(currState != NUM){
                    prevState = currState;
                }
                c = operandStr[i];
                switch (c)
                {
                case '$':{
                    currState = REGISTER;
                    registerCounter+=1;
                    break;
                }
                case ',':{
                    currState = COMMA;
                    commaCounter+=1;
                    break;
                }
                default:
                    currState = NUM;
                    continue;
                }
                if(currState == COMMA && registerCounter == 0){ /*comma before first register*/
                    return -8;
                }
                else if(currState == COMMA && registerCounter == LEGAL_I_TYPE_REGISTER_NUM){  /*comma after last register*/
                    return -8;
                }
                else if(currState == REGISTER && prevState == REGISTER && immedCounter == 0){ /*Register after register and no immed is illegal*/
                    return -8;
                }
                else if((currState == COMMA && prevState == COMMA)){ /*Comma after comma */
                    return -8;
                }
                else if(immedCounter > 1){ /*If more than one immed */
                    return -12;
                }
                else if(currState == COMMA  && prevState == REGISTER && commaCounter == 1 && registerCounter == 1){ /*Save immed index for validation */
                    immedCounter ++;
                    immedIndex = i + 1;
                    currState = IMMED;
                }
                else{
                    continue;
                }
            }while(i ++ < strlen(operandStr) - 1);
            errorCode = validateImmed(operandStr,immedIndex);
            if(errorCode){
                return errorCode;
            }
        }
        else{
              do{
                if((currState != NUM)){
                    prevState = currState;
                }
                c = operandStr[i];
                switch (c)
                {
                case '$':{
                    currState = REGISTER;
                    registerCounter+=1;
                    break;
                }
                case ',':{
                    currState = COMMA;
                    commaCounter+=1;
                    break;
                }
                default:
                    currState = NUM;
                    continue;
                }
                if(currState == COMMA && registerCounter == 0){ /*comma before first register*/
                    return -8;
                }
                else if(currState == COMMA && registerCounter == LEGAL_I_TYPE_REGISTER_NUM && commaCounter > LEGAL_I_TYPE_COMMA_NUM){   /*comma after last register*/
                    return -8;
                }
                else if(currState == COMMA && prevState == COMMA){ /*Comma after comma */
                    return -8;
                }
                else if(currState == REGISTER && prevState == REGISTER){ /*Register after register */
                    return -12;
                }
            }while(i ++ < strlen(operandStr) - 1);
        }
    if(registerCounter < LEGAL_I_TYPE_REGISTER_NUM){ /*Less than valid registers num in an operation*/
        return -11;
    }
    else if(registerCounter > (LEGAL_I_TYPE_REGISTER_NUM + 1)){ /*Greater than valid registers num in an operation*/
        return -13;
    }
    else if(commaCounter < LEGAL_I_TYPE_COMMA_NUM){ /*Less than valid commas num in an operation*/
        return -10;
    }
    else if(commaCounter > (LEGAL_I_TYPE_COMMA_NUM + 1)){ /*Greater than valid commas num in an operation*/
        return -8;
    }
    break;
    }
    case 'J':{
        if(group){ /*stop operation */
            return 0;
        }
        do{
            c = operandStr[i];
            switch (c)
            {
            case '$':{
                currState = REGISTER;
                registerCounter+=1;
                break;
            }
            case ',':{
                currState = COMMA;
                commaCounter+=1;
                break;
            }
            default:
                continue;
            }
            if(commaCounter > LEGAL_J_TYPE_COMMA_NUM){ /*Greater than valid commas num in an operation*/
                return -8;
            }
            if(registerCounter > LEGAL_J_TYPE_REGISTER_NUM){ /*Greater than valid registers num in an operation*/
                return -15;
            }
        }while(i ++ < strlen(operandStr) - 1);
        break;
    }
    default:
        break;
    }
    return errorCode;
}

/*Receive line from a source code file */
/*Return Empty/Operation/Directive/Unrecongnize enum command */
enum command getCommand(char* str){ 
    char *endWord, *word;
    if(!strcmp(str,"")){ 
        return EMPTY;
    }
    else if(strchr(str,';') != NULL){
        return COMMENT;
    }
    else{
        if(!strcmp(str,"stop")){
            return OPERATION;
        }
        endWord = strstr(str," ");
        if(endWord != NULL){
            word = (char*)malloc((strlen(str) - strlen(endWord)) * sizeof(char));
            memcpy(word,str,endWord-str);
        }
        else{
            str[strlen(str)-1] = '\0';
            word = str;
        }
        if(strchr(word,'.') != NULL){
            return DIRECTIVE;
        }
        else if((isalpha(*word) != 0) && (islower(*word) != 0)){
            return OPERATION;
        }
    }
    return UNRECONGNIZE;
}

/*Receives operation name */
/*Case operation type equal to R */
/*Return 0 if arithmetic and logic operations, 1 if copy operations*/
/*Case operation type equals to I */
/*Return 0 if arithmetic, logic, load and store operations, 1 if branch operations */
/*Case operation type equals to J */
/*Return 0 if la, call, jmp operations, 1 if stop */
/*Return -23 if label operation group not found */
int getOperationGroup(char* operationName){
    if(!strcmp(operationName,"add") || !strcmp(operationName,"sub") || !strcmp(operationName,"and") || 
    !strcmp(operationName,"or") || !strcmp(operationName,"nor")){
        return 0;
    }
    else if(!strcmp(operationName,"move") || !strcmp(operationName,"mvlo") || !strcmp(operationName,"mvhi")){ 
        return 1; 
    }
    else if(!strcmp(operationName,"addi") || !strcmp(operationName,"subi") || !strcmp(operationName,"andi") || 
    !strcmp(operationName,"ori") || !strcmp(operationName,"nori") || !strcmp(operationName,"lb") || 
    !strcmp(operationName,"sb") || !strcmp(operationName,"lw") || !strcmp(operationName,"sw") || 
    !strcmp(operationName,"lh") || !strcmp(operationName,"sh")){ 
        return 0;
    }
    else if(!strcmp(operationName,"beq") || !strcmp(operationName,"bne") || !strcmp(operationName,"blt") || !strcmp(operationName,"bgt")){ 
        return 1;
    }
    else if(!strcmp(operationName,"jmp") || !strcmp(operationName,"la") || !strcmp(operationName,"call")){
        return 0;
    }
    else if(!strcmp(operationName,"stop")){
        return 1;
    }

    return -23; 
}

/*Receive an operation name and key */
/*Returns value in operation map, null if code not found */
char* getCodeFromOpTable(char* operationName, int row){
    char* code;
    int i;
    for (i = 0; i < COL_SIZE_OPERATION_TABLE; i++)
    {
        char *c = g_operation_table[i][ROW_SIZE_OPERATION_TABLE-1];
        if(!strcmp(operationName,c)){
            code = (char*)g_operation_table[i][row];            
            return code;
        }
    }
    return NULL;
}

/*Receive directive name */
/*Returns D in case of .dw, .db and .dh / ASCIZ/ ENTRY/ EXTERN / UNRECOGNIZEDIRECTIVE enum directive */
enum directive getDirectiveType(char* directiveName){
    if(!strcmp(directiveName,".dw") || !strcmp(directiveName,".db") || !strcmp(directiveName,".dh")){
        return D;    
        }
    else if(!strcmp(directiveName, ".asciz")){
        return ASCIZ;
    }
    else if(!strcmp(directiveName, ".entry")){
        return ENTRY;
    }
    else if(!strcmp(directiveName, ".extern")){
        return EXTERN;
    }
    return UNRECONGNIZEDIRECTIVE;
}

/*Receive a directive name */
/*Returns a bit size for the directive, 0 directive not found */
int getBitsSize(char* directiveName){
    if(!strcmp(directiveName,".db")){
        return BYTE_BITS_SIZE;
    }
    else if(!strcmp(directiveName,".dh")){
        return HALF_WORD_BITS_SIZE;
    }
    else if(!strcmp(directiveName,".dw")){
        return WORD_BITS_SIZE;
    }
    else if(!strcmp(directiveName,".asciz")){
        return BYTE_BITS_SIZE;
    }
    return 0;
}

/*Recieve label name */
/*Loop over labels list */
/*Returns Label struct pointer, null if label not found*/
Label* getLabel(char* labelName){
    if(labelHead != NULL && labelHead->name != NULL){
        Label* label = labelHead;
        do{
            if(!strcmp(label->name,labelName)){
                return label;
        }
        }while((label = label->next) != NULL);
    }    
    return NULL;
}

/*Recieve source file command line */
/*Loop over labels list */
/*Returns Label struct pointer, null if label not found*/
Label* getLabelFromLine(char* line){
    Label* label = labelHead;
    while(label != NULL){
        if((strstr(line,label->name) != NULL) && (strcmp(label->name,""))){
            return label;
        }
        label = label->next;
    }
    return NULL;
}

/*Receive source file command line */
/*Returns label name */
char* extractLabelName(char* str){
    char* labelName, *start, *end;
    start = str;
    end = strstr(str,":");
    labelName = (char*)malloc((end - start + 1 ) * sizeof(char));
    memcpy(labelName,start,end-start);
    labelName[end-start] = '\0';

    return labelName;
}

/*Receive source file command line */
/*Return operation name, -7 if operation name is upper case  */
char* extractOperartionName(char* str){
    char *start = str, *end = str;
    char* operationName = (char*)malloc((end - start + 1) * sizeof(char));
    end = strstr(str," ");
    if(end != NULL){
        memcpy(operationName, start, end - start);
    }
    else{
        strcpy(operationName,str);
    }
    endStr(operationName);
    if(isupper(*operationName) != 0){
        return "-7";
    }
    return operationName;
}

/*Receive source file command line */
/*Return directive name  */
char* extracDirectiveName(char* str){
    char *start = str, *end = str, *name = NULL;
    start = strstr(str,".");
    end = strstr(start," ");
    name = (char*)malloc((end - start + 1) * sizeof(char));
    memcpy(name,start,end - start);
    name[end-start] = '\0';

    return name;
}

/*Receive directive name, dynamic operand list pointeer and source file command line */
/*Extract directive operands from directive commands: .db .dh .dw and store each element in a dynamic operand list */
/*Return error code 0 if valid */
int extractDirectiveNumericOperands(char* directiveName, Array *operandList, char* str){
    char *start = str, *end = str, *target = NULL;
    int i = 0, commaCounter = 0, size, comma;
    initArray(operandList,MIN_DIRECTIVE_OPERANDS);

    while(i<strlen(str)){
        target = NULL;
        end = strstr(start,",");
        target = (char*)malloc(i);
        if(end == NULL){
            end = start;
            if(isspace(end[0])){
                end += 1;
            }
            endStr(end);
            target = end;
            end = &str[strlen(str)];
        }
        else{
            memcpy(target,start,end-start);
            endStr(target);
            end += 1;
            commaCounter+=1;
        }
        if(!strcmp(target,"")){ /*no number between commas */
            return -5;
        }
        insertArray(operandList,atoi(target));
        start = end;
        i = strlen(str) - strlen(end);
    }
    
    size = operandList->used, comma = commaCounter-1;
    if(size < comma){ 
        return -8;
    }
    return 0;
}

/*Receive directive name, dynamic operand list pointeer and source file command line */
/*Extract directive operands from directive .asciz command and store each char in a dynamic operand list */
/*Return error code 0 if valid */
int extractDirectiveStringOperands(char* directiveName, Array *operandList, char* str){
    char* start;
    initArray(operandList,MIN_DIRECTIVE_OPERANDS);
    if(str[0] != '"' || str[strlen(str)-1] != '"'){ /*String is empty */
        return -25;
    }
    start  = strtok(str,"\"");
    if(start != NULL){
        start[strlen(start)] = '\0';
        do{
            insertArray(operandList,(char)*start++);
        }while(strlen(start));
        insertArray(operandList,'\0'); /*Including '\0' character */
    }
    else{
        return -6; 
    }
    return 0;
}

/*Receive directive name, operand list pointer, source file operand string and operation group */
/*There's a check if group is arithmetic and logic operation group 0, or copy operations group 1 */
/*Extracting operands corresponding to the operation group and store in a opearnds list */
/*Return error code 0 if registers are valid */
int extractOperands_R(char* operationName, int* operandList, char* operandStr, int group){
    char *start = operandStr, *end = operandStr, *operand = NULL;
    char** registers = (char**)malloc(REGISTERS_NUM * sizeof(char));
    int i = 0, step = 0, strLen = strlen(operandStr);
    int rs, rt, rd, registNum, errorCode; 

    if(group == 0){
        rs = rt = rd = -99999;  /*Initialize operands to -99999 */
        registNum = REGISTER_NUM_GROUP_0;
    }
    else if(group == 1){
        rt = 0; 
        rs = rd = -99999;  /*Initialize operands to -99999 */
        registNum = REGISTER_NUM_GROUP_1;
    }     
    while(i < strLen){
        operand = NULL;
        if((start = strstr(start,"$"))){
            start += 1;
            if((end = strstr(start,","))){
                operand = (char*)malloc((end - start + 1) * sizeof(char));
                memcpy(operand,start,end-start);
                endStr(operand);
                start = strstr(start,",");
            }
        }
        switch (group){
        case 0: /*Arithmetic and logic operations */
            if(rs == -99999){
                registers[0] = operand;
                rs = atoi(operand);
                step = strlen(end);
            }
            else if(rt == -99999){
                registers[1] = operand;
                rt = atoi(operand);
                step = strlen(end);
            }
            else if(rd == -99999){
                endStr(start);
                registers[2] = start;
                rd = atoi(start);
                step = 0;
            }
            break;
        case 1: /*Copy operations */
            if(rd == -99999){
                registers[0] = operand;
                rd = atoi(operand);
                step = strlen(end);
            }
            else if(rs == -99999){
                endStr(start);
                registers[1] = start;
                rs = atoi(start);
                step = 0;
            }
            break;
        default:
            break;
        }
        i = strlen(operandStr) - step + 1;    
    }

    errorCode = validateRegisters(registers,registNum); /*If registers valid, initialize operands list */
    if(errorCode){
        return errorCode;
    }
    operandList[0] = rs;
    operandList[1] = rt;
    operandList[2] = rd;
    
    return 0;
}

/*Receive directive name, operand list pointer, source file operand string and operation group */
/*There's a check if group is arithmetic and logic, load and store operation group 0, or branch operations group 1 */
/*Extracting operands corresponding to the operation group and store in a opearnds list */
/*Return error code 0 if registers and immediate are valid */
int extractOperands_I(char* operationName, int* operandList, char* operandStr, int group){

    int rs, rt, immed, errorCode;
    int i = 0, step = 0, strLen = strlen(operandStr);
    char *start, *end, *operand;
    char** registers = (char**)malloc(REGISTERS_NUM * sizeof(char));
    Label* label;
    rs = rt = immed = -99999; /*Initialize operands to -99999 */
    start = operandStr, end = operandStr, operand = NULL;

    switch (getOperationGroup(operationName))
    {
    case 0:{ /*Arithmetic, logic, load, store operations */
        while(i < strLen){
            operand = NULL;
            if(rs == -99999 || immed == -99999){
                if(rs == -99999){
                    start = strstr(start,"$");
                }
                else if(immed == -99999){ 
                    start = strstr(start,",");
                }
                start += 1;
                end = strstr(start,",");
                operand = (char*)malloc((end-start+1) * sizeof(char));
                memcpy(operand,start,end - start);
                endStr(operand);
                step = strlen(end);
            }
            else if(rt == -99999){
                start = strstr(start, ",") + 1;
                start = strstr(start, "$") + 1;
                if(!strcmp(start,"")){
                    start = end + 1;
                }
                step = 0;
            }       
            if(rs == -99999){
                registers[0] = operand;
                rs = atoi(operand);
            }
            else if(immed == -99999){
                immed = atoi(operand);
            }
            else if(rt == -99999){
                endStr(start);
                registers[1] = start;
                rt = atoi(start);
            }
            i = strlen(operandStr) - step + 1;    
        }
        break;
    }
    case 1:{ /*Branch operations */
        while(i < strLen){
            operand = NULL;
            start = strstr(start,"$") + 1;
            end = strstr(start,",");
            operand = (char*)malloc((end - start + 1 ) * sizeof(char));
            memcpy(operand,start,end-start);
            endStr(operand);
            if(rs == -99999){
                registers[0] = operand;
                rs = atoi(operand);
                step = strlen(end);
            }
            else if(rt == -99999){
                registers[1] = operand;
                rt = atoi(operand);
                step = strlen(end);
            }
            if (rs != -99999 && rt != -99999 && immed == -99999){
                start = strstr(start,",") + 1;
                step = 0;
                endStr(start);
                if(!strcmp(start,"")){
                    return -14;
                }
                label = getLabel(start);
                if(label == NULL){
                    immed = -998;
                }
                else{
                    immed = label->value;
                }
            }
            i = strlen(operandStr) - step + 1;    
        }
    }
    default:
        break;
    }

    errorCode = validateRegisters(registers,REGISTER_NUM_GROUP_1); /*If operands valid, initialize operands list */
    if(!errorCode){
        operandList[0] = rs;
        operandList[1] = rt;
        operandList[2] = immed;
    }
    else{
        return errorCode;
    }
    return 0;
}

/*Receive directive name, operand list pointer, source file operand string */
/*Relevant for jmp, call, la operations check if register or label defined */
/*Extracting operands corresponding to the operation group and store in a opearnds list */
/*In case of register, there's a validation of the register  */
/*In case of label, there's a validation of the label */
/*Return error code 0 if valid */
int extractOperands_J(char* operationName, int* operandList, char* operandStr){
    
    int reg, address, errorCode = 0;
    char** registers = (char**)malloc(REGISTERS_NUM * sizeof(char));
    Label* label;

    if((strcmp(operationName,"jmp") == 0) && (strchr(operandStr,'$') != NULL)){ /*jmp register operation */
        char *start = operandStr;
        start = strstr(start,"$") + 1;
        endStr(start);
        reg = 1;
        address = atoi(start);
        registers[0] = start;
        if((errorCode = validateRegisters(registers, 1))){
            return errorCode;
        }
    }
    else if (((strcmp(operationName,"jmp") == 0) && (strchr(operandStr,'$') == NULL)) || 
        (strcmp(operationName,"la") == 0) || (strcmp(operationName,"call") == 0)){ /*Not register operation */
        reg = 0;
        endStr(operandStr);
        if(!strcmp(operandStr,"")){
            return -14;
        }
        label = getLabel(operandStr);
        if(label == NULL){
            address = -998;
        }
        else{
            address = label->value;
        }
    }
    else{
        reg = 0;
        address = 0;
    }
    
    operandList[0] = reg;
    operandList[1] = address;

    return 0;
}

/*Adding label into label linked list */
/*Receive current label node, label name, value and attribute */
/*Allocate memory of the next label node and assign to the current label node */
/*Assign label name, value and attribute to the current label struct */
/*Return current label node */
Label* addLabel(Label* currentLabel, char* labelName, int value, char* attribute){
    
    Label* nextLabel = (Label*)malloc(sizeof(struct Label));
    nextLabel->name = (char*)malloc(strlen(labelName) * sizeof(char));
    nextLabel->attribute = (char*)malloc(strlen(attribute) * sizeof(char));
    currentLabel->name = labelName;
    currentLabel->value = value;
    currentLabel->attribute = attribute;   
    currentLabel->next = nextLabel;

    return currentLabel;
}

/*Adding extern directive into externs linked list */
/*Receive current extern node, label name and value */
/*Allocate memory of the next extern node and assign to the current extern node */
/*Assign label name and value to the current extern struct */
/*Return current extern node */
Extern* addExternElement(Extern* externList,char* labelName, int value){
    Extern* nextElement = (Extern*)malloc(sizeof(Extern));
    externList->next = nextElement;
    externList->labelName = labelName; 
    externList->value = value;
    return externList;
}

/*Adding .db, .dh, .dw, .asciz directives into directives linked list */
/*Receive current directive node and bits size of the specific directive */
/*Allocate memory of the next directive node and assign to the current directive node */
/*Allocates memory of machine code*/
/*Return next directive node*/
Directive* addDataElement(Directive* directiveList,int size){
    Directive* nextElement = (Directive*)malloc(sizeof(Directive));
    directiveList->next = nextElement;
    nextElement->machineCode = (char*)malloc(size * sizeof(char)); 
    return nextElement;
}

/*Adding operation into operations linked list */
/*Receive current operation node */
/*Allocate memory of the next operation node and assign to the current operation node */
/*Allocates memory of machine code*/
/*Return next operation node*/
Operation* addOperationElement(Operation* operationList){
    Operation* nextElement = (Operation*)malloc(sizeof(Operation));
    operationList->next = nextElement;
    nextElement->machineCode = (char*)malloc(OPERATION_BITS_SIZE*sizeof(char)); 
    return nextElement;
}

/*This method receives a pointer to the source file and passes it first */
/*It removes unwanted characters */
/*It checks if label defined, validate it and cut the label from line  */
/*It checks command's type: COMMENT/ DIRECTIVE/ OPERATION/ UNRECONGIZE */
/*Case COMMENT: the method ignores it and continue to the next line */
/*Case DIRECTIVE: the directive type is checked: ENTRY/ EXTERN/ D/ ASCIZ/ UNRECONGNIZEDIRECTIVE*/
/*Case D / ASCIZ: Extract directive operands from operands string corresponding to the directive type */
/*Transform operands into machine code corresponding to the bits size */
/*Store the directive binary machine code in a directive linked list */
/*Case EXTERN: label will be extracted and noted as an extern, extern global variable increased by 1 */
/*Extern will be store in an external linked list */
/*Case ENTRY: will be treat in the second pass, the method continues to the next line */
/*Case OPERATION: operation name will be extract and validate */
/*Extract operation operands from operands string corresponding to operation type and check its validity */
/*Transform operands, operation code and funct from decimal to binary and store in an operation struct by type  */
/*Store the operation binary for each element i.e (rs,rd,rt,opcode,funct,immed) in an operation type struct */
/*Case UNRECONGIZE: skip line and error is thrown */
/*If any errors, the method stops and doesn't continues to the second pass */
/*If no errors, icf and dcf values is calculate */
/*The method frees allocated variables and exits */
void firstPass(FILE* file){

    Label* label = (struct Label*)malloc(sizeof(struct Label));
    Directive* directiveList;
    Operation* operationList;
    Array directiveOperandsList;
    enum command command;
    enum directive directiveType;
    unsigned int lineCount = 0;
    int i, bitsSize, startOperandsIndex;
    int ic = 100,  dc = 0, isLabel = 0, labelLen = 0, errorCode = 0;
    int* operationOperandsList = (int*)malloc(REGISTERS_NUM * sizeof(int));
    char *line, *operationName = NULL, *labelName = NULL, *directiveName = NULL;

    while(fgets(line=(char*)malloc(2*MAX_LINE_LENGTH*sizeof(char)),MAX_LINE_LENGTH*2,file)){

        int lineLen = strlen(line);
        isLabel = 0, labelLen = 0, startOperandsIndex = 0, labelName = NULL;
        printf("line[%d]: %s", ++lineCount,line);
        strcpy(line, (char*)removeGarbge(line));

        if(lineLen > MAX_LINE_LENGTH){
            printError(-24,lineCount,line);
            continue;
        }
        else if(strchr(line,':') != NULL){ 
                isLabel = 1;
                labelName = extractLabelName(line);
                if(labelHead == NULL){
                    labelHead = label;
                }
                else{
                    if((errorCode = validateLabel(labelName)) != 0){
                        printError(errorCode,lineCount,labelName);
                        continue;
                    }
                }
                labelLen = strlen((char*)labelName) + 2; 
                memcpy(line,line+labelLen,strlen(line)); /*Cut label from line including semicolon and space*/
        }

        command = getCommand(line);
        switch (command){
        case EMPTY:
            break;
        case DIRECTIVE:{
            directiveName = extracDirectiveName(line);
            startOperandsIndex = strlen((char*)directiveName) + 1;
            memcpy(line,line+startOperandsIndex,strlen(line));  /*Cut directive name from line including space */
            directiveType = getDirectiveType(directiveName); 
            switch (directiveType)
            {
            case D:{
                errorCode = extractDirectiveNumericOperands(directiveName,&directiveOperandsList,line);
                break;
            }
            case ASCIZ:{ 
                errorCode = extractDirectiveStringOperands(directiveName,&directiveOperandsList,line);
                break;
            }
            case ENTRY:{
                continue; 
            }
            case EXTERN:{
                endStr(line); /*Remained label in line */
                if(getLabel(line) != NULL){
                    errorCode = -18;
                    printError(-18,lineCount,directiveName);
                    break;
                }
                if(labelHead == NULL){ /*If extern directive is first in source file */
                    labelHead = label;
                }
                label = addLabel(label,line, EXTERN_SYMBOL_DEFAULT_VALUE, EXTERNAL_LABEL_ATTRIBUTE);
                label = label->next;
                g_externals ++;
                break;
            }
            case UNRECONGNIZEDIRECTIVE:{
                errorCode = -4;
                printError(-4,lineCount,directiveName);  
                break;
            }
            default:{              
                break;
            }
            }
            if(directiveType == D || directiveType == ASCIZ){ 
                if(errorCode != 0){
                    printError(errorCode,lineCount,directiveName);
                    break;
                }
                bitsSize = getBitsSize(directiveName); /*Gets bits size depending on directive type */
                if(directiveHead == NULL){ /*Defining directive head node here */
                    directiveList = (Directive*)malloc(sizeof(Directive));
                    directiveList->machineCode = (char*)malloc(bitsSize * sizeof(char)); 
                    directiveHead = directiveList;
                }
                if(isLabel == 1){ /*If no errors occurred and label flag is on, adding directive label */
                    label = addLabel(label,labelName, dc, DATA_LABEL_ATTRIBUTE);
                    label = label->next;
                }   
                for (i = 0; i < directiveOperandsList.used; i++){ /*Transforms each directive and its bits size from decimal to binary */
                    directiveList->machineCode = toBinary(directiveOperandsList.array[i],bitsSize);
                    directiveList->dc = dc+i;
                    directiveList = addDataElement(directiveList,bitsSize); /*Adding nodes to directive linked list */
                }
                dc += (directiveOperandsList.used * (bitsSize / 8)); /*Dc count increasing corresponding to the bits size of each directive */
            }
            directiveName = NULL;
            break;
        }
        case OPERATION:{
            char* opCode;
            int group;
            operationName = (char*)malloc(OPERATION_NAME_LENGTH * sizeof(char));
            operationName = extractOperartionName(line); 
            group = getOperationGroup(operationName);
            if(operationHead == NULL){ /*Defining operation head node here */
                operationList = (Operation*)malloc(sizeof(Operation));
                operationList->machineCode = (char*)malloc(OPERATION_BITS_SIZE * sizeof(char)); 
                operationHead = operationList;
            }
            else{
                operationList = addOperationElement(operationList); /*Adding operation node to operations linked list */
            }
            if((opCode = getCodeFromOpTable(operationName,OPERATION_TABLE_COL_NAME_INDEX)) != NULL){
                char* operationType = (char*)getCodeFromOpTable(operationName,OPERATION_TABLE_COL_TYPE_INDEX);
                startOperandsIndex =  strlen(operationName) + 1; 
                memcpy(line, line + startOperandsIndex, strlen(line) + 1); /*Cut operation name from line */
                removeSpaces(line); /*Remove spaces from operand string */
                errorCode = validateOperationStructure(line,operationType,operationName,group); /*Validate operands */
                if(errorCode){
                    printError(errorCode,lineCount,operationName);
                    break;
                }
                switch (*operationType){
                    case 'R':{
                        Operation_R* op_r;
                        errorCode = extractOperands_R(operationName,operationOperandsList,line,group);
                        if(errorCode != 0){
                            printError(errorCode,lineCount,operationName);
                            break;
                        }
                        op_r = (Operation_R*)malloc(sizeof(Operation_R)); /*Store R type operation in this struct */
                        op_r->opcode = opCode;
                        op_r->funct = getCodeFromOpTable(operationName,1);
                        op_r->rs = toBinary(operationOperandsList[0],REGISTER_BITS_SIZE);
                        op_r->rt = toBinary(operationOperandsList[1],REGISTER_BITS_SIZE);
                        op_r->rd = toBinary(operationOperandsList[2],REGISTER_BITS_SIZE);
                        op_r->notinuse = "000000";
                        operationList->operation = op_r; /*Add R type operation into operation field in the main operations linked list */
                        operationList->type = 'R'; 
                        break;
                    }
                    case 'I':{
                        Operation_I* op_i; /*Store I type operation in this struct */
                        errorCode = extractOperands_I(operationName,operationOperandsList,line,group);
                        if(errorCode != 0){
                            printError(errorCode,lineCount,operationName);
                            break;
                        }
                        op_i = (Operation_I*)malloc(sizeof(Operation_I));
                        op_i->opcode = opCode;
                        op_i->rs = toBinary(operationOperandsList[0],REGISTER_BITS_SIZE);
                        op_i->rt = toBinary(operationOperandsList[1],REGISTER_BITS_SIZE);
                        
                        if(group == 1){ /*Branch operations might not have an immediate value */
                            if(operationOperandsList[2] != -998 && operationOperandsList[2] != -999){
                                op_i->immed = toBinary(operationOperandsList[2] - ic,IMMED_BITS_SIZE);
                            }
                            else{
                                op_i->immed = "?";
                            }
                        }
                        else{ /*Other I operations has known immediate value */
                            op_i->immed = toBinary(operationOperandsList[2],IMMED_BITS_SIZE);
                        }
                        operationList->operation = op_i; /*Store I type operation in this struct */
                        operationList->type = 'I'; 
                        break;
                    }
                    case 'J':{
                        Operation_J* op_j; /*Store J type operation in this struct */
                        errorCode = extractOperands_J(operationName,operationOperandsList,line); 
                        if(errorCode != 0){
                            printError(errorCode,lineCount,operationName);
                            break;
                        }
                        op_j = (Operation_J*)malloc(sizeof(Operation_J));
                        op_j->opcode = opCode;
                        op_j->reg = toBinary(operationOperandsList[0],REG_BITS_SIZE);
                        if(operationOperandsList[1] != -998 && operationOperandsList[1] != -999 ){ /*All J operations might not have an immediate value */
                            op_j->address = toBinary(operationOperandsList[1],ADDRESS_BITS_SIZE);
                        }
                        else{
                            op_j->address = "?";
                        }
                        operationList->operation = op_j; /*Store J type operation in this struct */
                        operationList->type = 'J'; 
                        break;
                    }
                    default:{
                        break;
                    }
                }
                memset(operationOperandsList,0,3); /*Reset operands list */
            }
            else{
                errorCode = -9;
                printError(errorCode,lineCount,operationName);
                break;
            }
            
            if(isLabel == 1 && !errorCode){ /*If no errors occurred and label flag is on, adding operation label */
                label = addLabel(label,labelName, ic, CODE_LABEL_ATTRIBUTE);
                label = label->next;
            }

            operationList->ic = ic;
            ic += 4;
            operationName = NULL;
            break;
        }
        case COMMENT:
            continue;
        case UNRECONGNIZE:{
            errorCode = -16;
            printError(errorCode,lineCount,line);
            break;
        }
        default:{
         break;
        }
        }
        if(errorCode != 0){
            g_err_code++;
        }
    }

    if(g_err_code == 0){
        g_icf = ic;
        g_dcf = dc;
        label = labelHead;
        directiveList = directiveHead;

        while(label != NULL){
            if(!strcmp(label->attribute,"data")){
                label->value += g_icf;
            }
            label = label->next;
        }
        while(directiveList != NULL){
            directiveList->dc += g_icf;
            directiveList = directiveList->next;
        }
        if(directiveOperandsList.used > 0){
            freeArray(&directiveOperandsList);
        }
        free(operationOperandsList);
    }
}

/*This method receives a pointer to source file and pass it for the second time  */
/*It removes unwanted charachters */
/*Extracts label if any */
/*It checks command's type: COMMENT/ DIRECTIVE/ OPERATION/ UNRECONGIZE */
/*Case COMMENT: the method ignores it and continues to the next line */
/*Case DIRECTIVE: the directive type is checked: ENTRY/ EXTERN/ D/ ASCIZ*/
/*Case D / ASCIZ: directive global variable increased by 1  */
/*Case EXTERN: the method ignores it and continue to the next line */
/*Case ENTRY: label will be extracted and validated, if not exist, will throw an error */
/*If a label is found, it will be noted in the symbol table as an entry */
/*Entry will be store in an entry linked list, entry global variable increased by 1 */
/*Case OPERATION: operation type will be checked */
/*Case R: each element in struct will be appended to binary machine code and stored in the operations linked list */
/*Case I: if immed field isn't coded, the label value will be extracted from the symbol list, distance will be calculated and coded into binary */
/*Each element in struct will be appended to binary machine code and stored in the operations linked list */
/*Case J: if immed field isn't coded, the label value will be extract and code into binary */
/*Each element in struct will be appended to binary machine code and stored in the operations linked list */
/*Operation global variable increased by 1 */
/*If any errors, the method stops and doesn't continues to export files */
/*The method frees allocated variables and exits */
void secondPass(FILE* file){

    Label* label = labelHead;
    Extern* externList = externalHead;
    Operation* operationList = operationHead;
    enum command command;
    unsigned int lineCount = 0;
    int labelLen = 0, startOperandsIndex = 0, errorCode = 0, group = 0; 
    char *line, *labelName = NULL, *operationName = NULL, *directiveName = NULL;
    
    while(fgets(line=(char*)malloc(MAX_LINE_LENGTH*sizeof(char)),MAX_LINE_LENGTH,file)){

        labelLen = 0, labelName = NULL;
        printf("line[%d]: %s", ++lineCount,line);
        strcpy(line, (char*)removeGarbge(line));

        if(strchr(line,':') != NULL){ 
            labelName = extractLabelName(line);
            labelLen = strlen((char*)labelName) + 2; 
            memcpy(line,line+labelLen,strlen(line)); 
        }

        command = getCommand(line);
        switch (command)
        {
        case EMPTY:
            break;
        case DIRECTIVE:{
            directiveName = extracDirectiveName(line);
            switch (getDirectiveType(directiveName))
            {
            case ENTRY:{
                startOperandsIndex = strlen((char*)directiveName) + 1;
                memcpy(line,line+startOperandsIndex,strlen(line)); /*Cut entry directive from line */
                endStr(line); /*Remained label in line */
                label = getLabel(line);
                if(label == NULL){
                    errorCode = -998;
                    printError(-998,lineCount,line);
                    break;
                }
                label->attribute = ENTRY_LABEL_ATTRIBUTE; /*Adding label into symbol table */
                startOperandsIndex = 0; 
                g_entries++;
                break;
            }
            case D:{
                g_directives ++;
                break;
            }
            case ASCIZ:{
                g_directives ++;
                break;
            }
            default:
                break;
            }
            directiveName = NULL;
            break;
        }
        case OPERATION:{
            void* op = operationList->operation; 
            char* machineCode = (char*)malloc(OPERATION_BITS_SIZE * sizeof(char));
            operationName = (char*)malloc(OPERATION_NAME_LENGTH * sizeof(char));
            operationName = extractOperartionName(line);
            group = getOperationGroup(operationName);
            switch (operationList->type)
            {
            case 'R':{
                Operation_R *op_r = (void*)op;
                strcat(machineCode,op_r->opcode); 
                strcat(machineCode,op_r->rs);
                strcat(machineCode,op_r->rt);
                strcat(machineCode,op_r->rd);
                strcat(machineCode,op_r->funct);
                strcat(machineCode,op_r->notinuse);
                operationList->machineCode = machineCode; /*Concatenate fields from R operation struct to machine code */
                break;
            }
            case 'I':{
                Operation_I *op_i = (void*)op;
                int dist;
                if(group){ /*Only branch operation might have undefined immdiate value */
                    if(!strcmp(op_i->immed,"?")){ /*If label haven't defined in first pass */
                        label = getLabelFromLine(line);
                        if(label == NULL){
                            errorCode = -998;
                            printError(-998,lineCount,"");
                            break;
                        }
                        dist = (label->value) - (operationList->ic);
                        op_i->immed = toBinary(dist,IMMED_BITS_SIZE);
                    }
                    if(!strcmp(label->attribute,"external")){ /*If label defined as external first*/
                        errorCode = -17;
                        printError(-17,lineCount,label->name);
                        break;
                    }
                }
                strcat(machineCode,op_i->opcode);
                strcat(machineCode,op_i->rs);
                strcat(machineCode,op_i->rt);
                strcat(machineCode,op_i->immed);
                operationList->machineCode = machineCode;  /*Concatenate fields from I operation struct to machine code */
                break;
            }
            case 'J':{
                Operation_J *op_j = (void*)op;
                if(!strcmp(op_j->reg,"0") && !group){ /*Only operations with reg field equal to 0 might have undefined immdiate value */
                    label = getLabelFromLine(line);
                    if(label == NULL){
                        errorCode = -998;
                        printError(-998,lineCount,"");
                        break;
                    }
                    if(!strcmp(op_j->address,"?")){
                        op_j->address = toBinary(label->value,ADDRESS_BITS_SIZE);    
                    }
                    if(!strcmp(label->attribute,"external")){
                        if(externalHead == NULL){
                            externList = (struct Extern*)malloc(sizeof(struct Extern));
                            externalHead = externList;
                        }
                    externList = addExternElement(externList,label->name,operationList->ic);
                    externList = externList->next;
                    }
                }
                strcat(machineCode,op_j->opcode);
                strcat(machineCode,op_j->reg);
                strcat(machineCode,op_j->address);
                operationList->machineCode = machineCode; /*Concatenate fields from I operation struct to machine code */
                break;            
            }
            default:
                break;
            }
            g_operations ++;
            operationList = operationList->next;
            break;
        }
        default:
            break;
        }
        
        if(errorCode != 0){
            g_err_code++;
        }
    }
    free(line);
    free(labelName);
    free(operationName);
    free(directiveName);
}

/*Receive a decimal number and number of bits */
/*Returns a binary number in a size of bits num */
char* toBinary(int number,int numOfBits){
    unsigned int mask, i;   
    char *code = (char*)malloc(numOfBits*sizeof(char));    

    for(i = 0; i < numOfBits ; i++){
        mask = 1u << (numOfBits - 1 - i);
        code[i] = ((number & mask) ? '1' : '0');
    }                                              
    code[i] = '\0';
    return code;
}

/*The method iterates over linked extern, directive, operations and labels linked list*/
/*And frees allocated variables*/
void freeAlloc(){
    Extern* externList = externalHead;
    void* operation;
    Operation* operationList = operationHead;
    Directive* directiveList = directiveHead;
    Label* label = labelHead;

    while(directiveList != NULL){
        if(directiveList->machineCode){
            free(directiveList->machineCode);
            directiveList->machineCode = NULL;
        }
        free(directiveList);
        directiveHead = directiveHead->next;
        directiveList = directiveHead;
    }

    while(operationList != NULL){
        operation = operationList->operation;
        if(operation){
            free(operation);
            operation = NULL;
        }
        if(operationList->machineCode){
            free(operation);
            operationList->machineCode = NULL;
        }
        free(operationList);
        operationHead = operationHead->next;
        operationList = operationHead;
    }

    while(label != NULL){
        if(label->name){
            free(label->name);
            label->name = NULL;
        }
        label->attribute = NULL;
        free(label);
        label = NULL;
        labelHead = labelHead->next;
        label = labelHead;
    }

    while(externList != NULL){
        if(externList->labelName){
            /*free(externList->labelName);*/ 
            externList->labelName = NULL;
        }
        free(externList);
        externalHead = externalHead->next;
        externList = externalHead; 
    }
}

/*The method iterates over Array struct */
/*And frees allocated variables */
void freeArray(Array *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}
