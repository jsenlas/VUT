//
// Created by Marek on 1. 12. 2018.
//

#include "generator.h"

// "if" label counter
unsigned int lab_if;
// "do-while" label counter
unsigned int lab_do;
// convert label counter
unsigned int lab_conv;
unsigned int type_c;

//BUFFER FUNCTIONS
int addToBuff(T_buff *m_b, char *in_str) {
    int mem_size = strlen(in_str)*sizeof(char) + 1;
    if ((m_b->string = (char *) realloc(m_b->string, m_b->length + mem_size)) == NULL)
        return INTERNAL_ERR;

    strcat(m_b->string, in_str);
    m_b->length += mem_size;
    return EXIT_SUCCESS;
}

void resetBuff(T_buff *str) {
    str->string[0] = '\0';
    str->length = 0;
}

void freeBuff(T_buff *str) {
    free(str->string);
    str->string = NULL;
}

int buffInit() {
    my_buff = &buff;
    if ((my_buff->string = (char *) malloc(1)) == NULL)
        return INTERNAL_ERR;
    my_buff->length = 0;
    my_buff->string[0] = '\0';

    static_convert = 0;
    return EXIT_SUCCESS;
}

//-------------------------------------------------------------------------
/// BUFFER SUPPORT FUNCTIONS
void intToBuff(int val) {
    char *tmp_buff;
    size_t needed = snprintf(NULL, 0, "int@%d", val) + 1;
    if ((tmp_buff = malloc(needed)) == NULL) { 
        return; 
    }
    sprintf(tmp_buff, "int@%d", val);
    addToBuff(my_buff, tmp_buff);
    free(tmp_buff);
}

void floatToBuff(double val) {
    char *tmp_buff;
    size_t needed = snprintf(NULL, 0, "float@%a", val) + 1;
    if ((tmp_buff = malloc(needed)) == NULL) { 
        return; 
    }
    sprintf(tmp_buff, "float@%a", val);
    addToBuff(my_buff, tmp_buff);
    free(tmp_buff);
}

void strToBuff(char* val, char* type) {
    char *tmp_buff;
    size_t needed = snprintf(NULL, 0, "%s%s", type, val) + 1;
    if ((tmp_buff = malloc(needed)) == NULL) { 
        return; 
    }
    sprintf(tmp_buff, "%s%s", type, val);
    addToBuff(my_buff, tmp_buff);
    free(tmp_buff);
}

void labToBuff(int val, char* type) {
    char *tmp_buff;
    size_t needed = snprintf(NULL, 0, "%s%d", type, val) + 1;
    if ((tmp_buff = malloc(needed)) == NULL) { 
        return; 
    }
    sprintf(tmp_buff, "%s%d", type, val);
    addToBuff(my_buff, tmp_buff);
    free(tmp_buff);
}

//-------------------------------------------------------------------------
/// GENERATOR INITIALIZATOR
// generovat na zaciatku
void gen_init() {
    fprintf(OUTPUT, ".IFJcode18\n");
    fprintf(OUTPUT, "DEFVAR GF@%%tmp1\n");
    fprintf(OUTPUT, "DEFVAR GF@%%tmp2\n");
    fprintf(OUTPUT, "DEFVAR GF@%%tmp3\n");
    fprintf(OUTPUT, "CREATEFRAME\n");
    fprintf(OUTPUT, "PUSHFRAME\n");
    fprintf(OUTPUT, "DEFVAR LF@%%tmp\n");
    fprintf(OUTPUT, "DEFVAR LF@%%retval\n");
    buffInit();
}

void lineEndPrint() {
    addToBuff(my_buff, "\n");
    if (!(iminif || iminwhile)) {
        fprintf(OUTPUT, "%s", my_buff->string);
        resetBuff(my_buff);
    }
}

// -------------------------------------------------------------------------
/// DEF GENERATE
// generovat pri inicializacii premennej pokial este inicializovana nebola
void gen_def_var(char *val) {
    fprintf(OUTPUT, "DEFVAR LF@%s\n", val);
}

//--------------------------------------------------------------------------
/// ASSIGNMENT GENERATE
// generuj priradenie podla definicie a typov
void gen_assign_int(int val) {
    addToBuff(my_buff, "MOVE LF@%tmp ");
    intToBuff(val);
    lineEndPrint();
}

void gen_assign_float(double val) {
    addToBuff(my_buff, "MOVE LF@%tmp ");
    floatToBuff(val);
    lineEndPrint();
}

void gen_assign_string(char *val) {
    addToBuff(my_buff, "MOVE LF@%tmp ");
    strToBuff(val, "string@");
    lineEndPrint();
}

void gen_assign_bool(char *val) {
    addToBuff(my_buff, "MOVE LF@%tmp ");
    strToBuff(val, "bool@");
    lineEndPrint();
}

void gen_assign_nil() {
    addToBuff(my_buff, "MOVE LF@%tmp ");
    addToBuff(my_buff, "nil@nil");
    lineEndPrint();
}

void gen_assign_id(char *val) {
    addToBuff(my_buff, "MOVE LF@%tmp ");
    strToBuff(val, "LF@");
    lineEndPrint();
}

void gen_assign_ret(char *val) {
    addToBuff(my_buff, "MOVE ");
    strToBuff(val, "LF@");
    addToBuff(my_buff, " TF@%retval");
    lineEndPrint();
}

void gen_assign_tmp_1_id(char *val) {
    addToBuff(my_buff, "MOVE GF@%tmp1 ");
    strToBuff(val, "LF@");
    lineEndPrint();
}

void gen_assign_tmp_1_int(int val) {
    addToBuff(my_buff, "MOVE GF@%tmp1 ");
    intToBuff(val);
    lineEndPrint();
}

void gen_assign_tmp_1_float(double val) {
    addToBuff(my_buff, "MOVE GF@%tmp1 ");
    floatToBuff(val);
    lineEndPrint();
}

void gen_assign_tmp_1_string(char *val) {
    addToBuff(my_buff, "MOVE GF@%tmp1 ");
    strToBuff(val, "string@");
    lineEndPrint();
}

void gen_assign_tmp_2_id(char *val) {
    addToBuff(my_buff, "MOVE GF@%tmp2 ");
    strToBuff(val, "LF@");
    lineEndPrint();
}

void gen_assign_tmp_3_id(char *val) {
    addToBuff(my_buff, "MOVE GF@%tmp2 ");
    strToBuff(val, "LF@");
    lineEndPrint();
}

void gen_assign_exp_ret(char *val) {
    addToBuff(my_buff, "MOVE ");
    strToBuff(val, "LF@");
    addToBuff(my_buff, " GF@%tmp1");
    lineEndPrint();
}

//---------------------------------------------------------------------------
/// STACK OPS GEN
void gen_push(char *val) {
    addToBuff(my_buff, "PUSHS ");
    strToBuff(val, "LF@");
    lineEndPrint();
}

void gen_pop(char *val) {
    addToBuff(my_buff, "POPS ");
    strToBuff(val, "LF@");
    lineEndPrint();
}

//---------------------------------------------------------------------------
/// CONVERTING
void gen_int_to_float_id(char *val_1, char *val_2) {
    addToBuff(my_buff, "INT2FLOAT ");
    strToBuff(val_1, "LF@");
    addToBuff(my_buff, " ");
    strToBuff(val_2, "LF@");
    lineEndPrint();
}

void gen_int_to_float_int(int val, int num) {
    addToBuff(my_buff, "INT2FLOAT LF@%tmp ");
    intToBuff(val);
    lineEndPrint();
    static_convert = num;
}

//---------------------------------------------------------------------------
/// TRANSFORM TYPE TO BOOL

void gen_if_while_true() {
    addToBuff(my_buff, "MOVE GF@%tmp1 bool@true");
    lineEndPrint();
}

void gen_if_while_false() {
    addToBuff(my_buff, "MOVE GF@%tmp1 bool@false");
    lineEndPrint();
}

//---------------------------------------------------------------------------
/// PRINT GENERATE
// generovanie printu podla typu konstanty
void gen_print_id(char *val) {
    addToBuff(my_buff, "WRITE ");
    strToBuff(val, "LF@");
    lineEndPrint();
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
}

void gen_print_str(char *val) {
    addToBuff(my_buff, "WRITE ");
    strToBuff(val, "string@");
    lineEndPrint();
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
}

void gen_print_int(int val) {
    addToBuff(my_buff, "WRITE ");
    intToBuff(val);
    lineEndPrint();
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
}

void gen_print_float(double val) {
    addToBuff(my_buff, "WRITE ");
    floatToBuff(val);
    lineEndPrint();
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
}

void gen_print_bool(char *val) {
    addToBuff(my_buff, "WRITE ");
    strToBuff(val, "bool@");
    lineEndPrint();
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
}

void gen_print_nil() {
    addToBuff(my_buff, "WRITE nil@nil");
    lineEndPrint();
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
}


//----------------------------------------------------------------------------
///FUNCTIONS
// zavolat pred prikazom call za ucelom predania parametrov
void gen_fce_call_init() {
    addToBuff(my_buff, "CREATEFRAME");
    lineEndPrint();
}

// predanie parametrov funkcii cez TF ramec
/*
    def fun c, d
        c = d
    end
    
    x = fun(a, b)
    ------------------
    MOVE LF@%%tmp .....

    DEFVAR TF@c
    MOVE TF@c LF@%%tmp
    
    DEFVAR TF@d
    MOVE TF@d LF@b
    
    CALL fun
    MOVE LF@x TF@%%retval
*/

void gen_fce_params(char *val) {
    addToBuff(my_buff, "DEFVAR ");
    strToBuff(val, "TF@");
    lineEndPrint();
    
    addToBuff(my_buff, "MOVE ");
    strToBuff(val, "TF@");
    addToBuff(my_buff, " LF@%tmp");
    lineEndPrint();
}

// generovanie volania funkcie
void gen_fce_call(char *val) {
    addToBuff(my_buff, "CALL ");
    strToBuff(val, "");
    lineEndPrint();
}

// netreba do bufferu, pretoze definicia funkcie nemoze byt v podmienke a cykle
// generovanie zaciatku definicie funkcie
void gen_fce_def(char *val) {
    fprintf(OUTPUT, "JUMP %s$end\n", val);
    fprintf(OUTPUT, "LABEL %s\n", val);
    fprintf(OUTPUT, "PUSHFRAME\n");
    fprintf(OUTPUT, "DEFVAR LF@%%retval\n");
    fprintf(OUTPUT, "DEFVAR LF@%%tmp\n");
    fprintf(OUTPUT, "MOVE GF@%%tmp1 nil@nil\n");
}

// generovanie konca definicie funkcie
void gen_fce_ret(char *val) {
    fprintf(OUTPUT, "MOVE LF@%%retval GF@%%tmp1\n");
    fprintf(OUTPUT, "POPFRAME\n");
    fprintf(OUTPUT, "RETURN\n");
    fprintf(OUTPUT, "LABEL %s$end\n", val);
}


//-----------------------------------------------------------------------------
/// OPERATIONS
//TODO
/*
    x = 4 + 3 -> GF@%%tmp1
    y = x + 5
    
    
    a = x + y * 2
    --------------------
    ADD GF@%%tmp1 int@4 int@3 (psa)
    MOVE LF@x GF@%%tmp1 (parser)
    
    ADD GF@%%tmp1 LF@x int@5 (psa)
    MOVE LF@y GF@%%tmp1 (parser)
    
    
    MUL GF@%%tmp1 LF@y int@2
    ADD GF@%%tmp1 GF@%%tmp1 LF@x
    
*/
void opTypeSwitch(Op_type type) {
    switch (type) {
    case ADD:
        addToBuff(my_buff, "ADD ");
        break; 
    case SUB:
        addToBuff(my_buff, "SUB ");
        break; 
    case MUL:
        addToBuff(my_buff, "MUL ");
        break; 
    case DIV:
        addToBuff(my_buff, "DIV ");
        break; 
    case IDIV:
        addToBuff(my_buff, "IDIV ");
        break; 
    case LESST:
        addToBuff(my_buff, "LT ");
        break; 
    case GREATERT:
        addToBuff(my_buff, "GT ");
        break; 
    case EQUALST:
        addToBuff(my_buff, "EQ ");
        break; 
    case NEQUALST:
        addToBuff(my_buff, "EQ ");
        break; 
    case LESSEQT:
        addToBuff(my_buff, "LT ");
        break; 
    case GREATEREQT:
        addToBuff(my_buff, "GT ");
        break; 
    case CONCAT:
        addToBuff(my_buff, "CONCAT ");
        break; 
    default:
        break;
    }
}

void opValSwitch(tDLList *op_list, int num) {
    if (num == 1) {
        switch(op_list->First->symbol.type) {
            case T_ID:
                strToBuff(op_list->First->symbol.value.string, "LF@");      
                break;
            case T_STRING_EXP:
                strToBuff(op_list->First->symbol.value.string, "string@");
                break;
            case T_INT_NUM:
                intToBuff(op_list->First->symbol.value.val_i);          
                break;
            case T_DOUBLE_NUM:
                floatToBuff(op_list->First->symbol.value.val_d);            
                break;
            default:
                break;
        }
    }
    else {
        switch(op_list->First->rptr->rptr->symbol.type) {
            case T_ID:
                strToBuff(op_list->First->rptr->rptr->symbol.value.string, "LF@");      
                break;
            case T_STRING_EXP:
                strToBuff(op_list->First->rptr->rptr->symbol.value.string, "string@");
                break;
            case T_INT_NUM:
                intToBuff(op_list->First->rptr->rptr->symbol.value.val_i);          
                break;
            case T_DOUBLE_NUM:
                floatToBuff(op_list->First->rptr->rptr->symbol.value.val_d);            
                break;
            default:
                break;
        }       
    }
}

void opComplexSwitch(tDLList *op_list, Op_type type) {
    switch(type) {
        case NEQUALST:
            addToBuff(my_buff, "NOT ");
            strToBuff("%tmp1", "GF@");
            addToBuff(my_buff, " ");
            strToBuff("%tmp1", "GF@");
            lineEndPrint();
            break;
        case LESSEQT:
        case GREATEREQT:
            addToBuff(my_buff, "PUSHS ");
            strToBuff("%tmp1", "GF@");
            lineEndPrint();
            gen_op_first(op_list, EQUALST);
            addToBuff(my_buff, "PUSHS ");
            strToBuff("%tmp1", "GF@");
            lineEndPrint();
            addToBuff(my_buff, "ORS");
            lineEndPrint();
            addToBuff(my_buff, "POPS ");
            strToBuff("%tmp1", "GF@");
            lineEndPrint();
            break;
        default:
            break;
    }
}

void gen_op_next(tDLList *op_list, Op_type type) {  
    opTypeSwitch(type);
    strToBuff("%tmp1 ", "GF@");
    strToBuff("%tmp1 ", "GF@");
    
    if (static_convert == 1) {
        strToBuff("%tmp", "LF@");
        static_convert = 0;
    }
    else {
        opValSwitch(op_list, 2);
    }
    
    lineEndPrint();
    opComplexSwitch(op_list, type);
}

void gen_op_first(tDLList *op_list, Op_type type) {
    opTypeSwitch(type);
    strToBuff("%tmp1 ", "GF@");
    
    if (static_convert == 1) {
        strToBuff("%tmp", "LF@");
        static_convert = 0;
    }
    else {
        opValSwitch(op_list, 1);
    }
    
    addToBuff(my_buff, " ");
    
    if (static_convert == 2) {
        strToBuff("%tmp", "LF@");
        static_convert = 0;
    }
    else {
        opValSwitch(op_list, 2);
    }

    lineEndPrint();
    opComplexSwitch(op_list, type);
}

//---------------------------------------------------------------------------------------
/// COMMANDS

void gen_then_eq() {
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
    addToBuff(my_buff, "JUMPIFNEQ ");
    labToBuff(lab_if, "lab$if$");
    addToBuff(my_buff, " GF@%tmp1 bool@true");
    lineEndPrint();
}

void gen_else() {
    addToBuff(my_buff, "JUMP ");
    labToBuff(lab_if + 1, "lab$if$");
    lineEndPrint();
    addToBuff(my_buff, "LABEL ");
    labToBuff(lab_if, "lab$if$");
    lineEndPrint();
    lab_if++;
}

void gen_end_if() {
    addToBuff(my_buff, "LABEL ");
    labToBuff(lab_if, "lab$if$");
    lineEndPrint();
    lab_if++;
}

void gen_while() {
    addToBuff(my_buff, "MOVE LF@%retval nil@nil");
    lineEndPrint();
    addToBuff(my_buff, "LABEL ");
    labToBuff(lab_do, "lab$do$");
    lineEndPrint();
}

// after precedence process expression
void gen_while_post() {
    addToBuff(my_buff, "JUMPIFNEQ ");
    labToBuff(lab_do + 1, "lab$do$");
    addToBuff(my_buff, " GF@%tmp1 bool@true");
    lineEndPrint();
}

void gen_loop() {
    addToBuff(my_buff, "JUMP ");
    labToBuff(lab_do, "lab$do$");
    lineEndPrint();
    lab_do++;
    addToBuff(my_buff, "LABEL ");
    labToBuff(lab_do, "lab$do$");
    lineEndPrint();
    lab_do++;
}

//----------------------------------------------------------------------------------------
/// CONVERTING TYPES CHECKINGS

void gen_check_first(tDLList *op_list, Op_type type, T_type second, int mode, int first_next) {
    /* MODE
     * 1 first operand check
     * 2 second operand check
     */
    char bb[100];
    char *val;
    char *name;

    if (mode == 1) {
        val = (char *) malloc(strlen(op_list->First->symbol.value.string) * sizeof(char) + 1);
        name = (char *) malloc(strlen(op_list->First->symbol.value.string) * sizeof(char) + 1);
        strcpy(val, op_list->First->symbol.value.string);
        strcpy(name, op_list->First->symbol.value.string);
    }
    else {
        val = (char *) malloc(strlen(op_list->First->rptr->rptr->symbol.value.string) * sizeof(char) + 1);
        name = (char *) malloc(strlen(op_list->First->rptr->rptr->symbol.value.string) * sizeof(char) + 1);
        strcpy(val, op_list->First->rptr->rptr->symbol.value.string);
        strcpy(name, op_list->First->rptr->rptr->symbol.value.string);
    }

    addToBuff(my_buff, "DEFVAR ");
    strToBuff(strcat(val, "$type"), "LF@");
    sprintf(bb, "%d", type_c++);
    addToBuff(my_buff, bb);
    strcat(val, bb);
    lineEndPrint();

    gen_push(name);
    addToBuff(my_buff, "TYPE ");
    strToBuff(val, "LF@");
    addToBuff(my_buff, " ");
    strToBuff(name, "LF@");
    lineEndPrint();

    if (type == ADD && second == T_STRING_EXP) {
        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@string");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();

        lab_conv++;
    }
    else if (type == IDIV && second == T_INT_NUM) {
        addToBuff(my_buff, "JUMPIFEQ ");
        if (mode == 1) {
            labToBuff(lab_conv, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 1, "lab$conv$");
        }
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        if (mode == 1) {
            labToBuff(lab_conv, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 2, "lab$conv$");
        }
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode != 1) {
            addToBuff(my_buff, "JUMPIFNEQ ");
            labToBuff(lab_conv, "lab$conv$");
            addToBuff(my_buff, " ");
            strToBuff(name, "LF@");
            addToBuff(my_buff, " ");
            addToBuff(my_buff, "int@0");
            lineEndPrint();

            addToBuff(my_buff, "EXIT int@9");
            lineEndPrint();

            addToBuff(my_buff, "LABEL ");
            labToBuff(lab_conv, "lab$conv$");
            lineEndPrint();
            lab_conv++;
        }

        if (mode == 1) {
            gen_int_to_float_int(op_list->First->rptr->rptr->symbol.value.val_i, 2);
        }
        else {
            gen_int_to_float_int(op_list->First->symbol.value.val_i, 2);
        }

        addToBuff(my_buff, "JUMP ");
        if (mode == 1) {
            labToBuff(lab_conv + 1, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 2, "lab$conv$");
        }
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode != 1) {
            addToBuff(my_buff, "JUMPIFNEQ ");
            labToBuff(lab_conv, "lab$conv$");
            addToBuff(my_buff, " ");
            strToBuff(name, "LF@");
            addToBuff(my_buff, " ");
            addToBuff(my_buff, "int@0");
            lineEndPrint();

            addToBuff(my_buff, "EXIT int@9");
            lineEndPrint();

            addToBuff(my_buff, "LABEL ");
            labToBuff(lab_conv, "lab$conv$");
            lineEndPrint();
            lab_conv++;
        }

        if (first_next == 1) {
            gen_op_first(op_list, IDIV);
        }
        else {
            gen_op_next(op_list, IDIV);
        }

        addToBuff(my_buff, "JUMP ");
        if (mode == 1) {
            labToBuff(lab_conv + 1, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 2, "lab$conv$");
        }
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode != 1) {
            addToBuff(my_buff, "JUMPIFNEQ ");
            labToBuff(lab_conv, "lab$conv$");
            addToBuff(my_buff, " ");
            strToBuff(name, "LF@");
            addToBuff(my_buff, " ");
            addToBuff(my_buff, "float@0x0p+0");
            lineEndPrint();

            addToBuff(my_buff, "EXIT int@9");
            lineEndPrint();

            addToBuff(my_buff, "LABEL ");
            labToBuff(lab_conv, "lab$conv$");
            lineEndPrint();
            lab_conv++;
        }

        if (first_next == 1) {
            gen_op_first(op_list, DIV);
        }
        else {
            gen_op_next(op_list, DIV);
        }

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode == 1) {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            intToBuff(op_list->First->rptr->rptr->symbol.value.val_i);
            lineEndPrint();
        }
        else {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            intToBuff(op_list->First->symbol.value.val_i);
            lineEndPrint();
        }
    }
    else if (type == IDIV && second == T_DOUBLE_NUM) {
        addToBuff(my_buff, "JUMPIFEQ ");
        if (mode == 1) {
            labToBuff(lab_conv, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 1, "lab$conv$");
        }
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        if (mode == 1) {
            labToBuff(lab_conv, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 2, "lab$conv$");
        }
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode != 1) {
            addToBuff(my_buff, "JUMPIFNEQ ");
            labToBuff(lab_conv, "lab$conv$");
            addToBuff(my_buff, " ");
            strToBuff(name, "LF@");
            addToBuff(my_buff, " ");
            addToBuff(my_buff, "int@0");
            lineEndPrint();

            addToBuff(my_buff, "EXIT int@9");
            lineEndPrint();

            addToBuff(my_buff, "LABEL ");
            labToBuff(lab_conv, "lab$conv$");
            lineEndPrint();
            lab_conv++;
        }

        gen_int_to_float_id(name, name);

        addToBuff(my_buff, "JUMP ");
        if (mode == 1) {
            labToBuff(lab_conv + 1, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 2, "lab$conv$");
        }
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode != 1) {
            addToBuff(my_buff, "JUMPIFNEQ ");
            labToBuff(lab_conv, "lab$conv$");
            addToBuff(my_buff, " ");
            strToBuff(name, "LF@");
            addToBuff(my_buff, " ");
            addToBuff(my_buff, "float@0x0p+0");
            lineEndPrint();

            addToBuff(my_buff, "EXIT int@9");
            lineEndPrint();

            addToBuff(my_buff, "LABEL ");
            labToBuff(lab_conv, "lab$conv$");
            lineEndPrint();
            lab_conv++;
        }

        if (first_next == 1) {
            gen_op_first(op_list, DIV);
        }
        else {
            gen_op_next(op_list, DIV);
        }

        addToBuff(my_buff, "JUMP ");
        if (mode == 1) {
            labToBuff(lab_conv + 1, "lab$conv$");
        }
        else {
            labToBuff(lab_conv + 2, "lab$conv$");
        }
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode != 1) {
            addToBuff(my_buff, "JUMPIFNEQ ");
            labToBuff(lab_conv, "lab$conv$");
            addToBuff(my_buff, " ");
            strToBuff(name, "LF@");
            addToBuff(my_buff, " ");
            addToBuff(my_buff, "int@0");
            lineEndPrint();

            addToBuff(my_buff, "EXIT int@9");
            lineEndPrint();

            addToBuff(my_buff, "LABEL ");
            labToBuff(lab_conv, "lab$conv$");
            lineEndPrint();
            lab_conv++;
        }

        if (first_next == 1) {
            gen_op_first(op_list, IDIV);
        }
        else {
            gen_op_next(op_list, IDIV);
        }

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (mode == 1) {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            floatToBuff(op_list->First->rptr->rptr->symbol.value.val_d);
            lineEndPrint();
        }
        else {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            floatToBuff(op_list->First->symbol.value.val_d);
            lineEndPrint();
        }
    }
    else if (second == T_INT_NUM) {
        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        
        lab_conv++;

        if (mode == 1) {
            gen_int_to_float_int(op_list->First->rptr->rptr->symbol.value.val_i, 2);
        }
        else {
            gen_int_to_float_int(op_list->First->symbol.value.val_i, 2);
        }

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();

        if (mode == 1) {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            intToBuff(op_list->First->rptr->rptr->symbol.value.val_i);
            lineEndPrint();
        }
        else {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            intToBuff(op_list->First->symbol.value.val_i);
            lineEndPrint();
        }
    }
    else if (second == T_DOUBLE_NUM) {
        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        
        lab_conv++;

        gen_int_to_float_id(name, name);

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();

        if (mode == 1) {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            floatToBuff(op_list->First->rptr->rptr->symbol.value.val_d);
            lineEndPrint();
        }
        else {
            addToBuff(my_buff, "MOVE LF@%tmp ");
            floatToBuff(op_list->First->symbol.value.val_d);
            lineEndPrint();
        }
    }

    lab_conv++;
    free(val);
    free(name);
}

void gen_check_both(tDLList *op_list, Op_type type, T_type second, int first_next) {
    char *val_1;
    char *val_2;
    char *name_1;
    char *name_2;
    char bb[100];

    val_1 = (char *) malloc(strlen(op_list->First->symbol.value.string) * sizeof(char) + 1);
    name_1 = (char *) malloc(strlen(op_list->First->symbol.value.string) * sizeof(char) + 1);
    val_2 = (char *) malloc(strlen(op_list->First->rptr->rptr->symbol.value.string) * sizeof(char) + 1);
    name_2 = (char *) malloc(strlen(op_list->First->rptr->rptr->symbol.value.string) * sizeof(char) + 1);
    strcpy(val_1, op_list->First->symbol.value.string);
    strcpy(name_1, op_list->First->symbol.value.string);
    strcpy(val_2, op_list->First->rptr->rptr->symbol.value.string);
    strcpy(name_2, op_list->First->rptr->rptr->symbol.value.string);

    addToBuff(my_buff, "DEFVAR ");
    strToBuff(strcat(val_1, "$type"), "LF@");
    sprintf(bb, "%d", type_c++);
    addToBuff(my_buff, bb);
    strcat(val_1, bb);
    lineEndPrint();

    addToBuff(my_buff, "DEFVAR ");
    strToBuff(strcat(val_2, "$type"), "LF@");
    sprintf(bb, "%d", type_c++);
    addToBuff(my_buff, bb);
    strcat(val_2, bb);
    lineEndPrint();

    gen_push(name_1);
    gen_push(name_2);

    addToBuff(my_buff, "TYPE ");
    strToBuff(val_1, "LF@");
    addToBuff(my_buff, " ");
    strToBuff(name_1, "LF@");
    lineEndPrint();

    addToBuff(my_buff, "TYPE ");
    strToBuff(val_2, "LF@");
    addToBuff(my_buff, " ");
    strToBuff(name_2, "LF@");
    lineEndPrint();

    if (type == ADD) {
        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 3, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 5, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@string");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (first_next == 1) {
            gen_op_first(op_list, CONCAT);
        }
        else {
            gen_op_next(op_list, CONCAT);
        }

        addToBuff(my_buff, "JUMP ");
        labToBuff(lab_conv + 5, "lab$conv$");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (first_next == 1) {
            gen_op_first(op_list, type);
        }
        else {
            gen_op_next(op_list, type);
        }

        addToBuff(my_buff, "JUMP ");
        labToBuff(lab_conv + 4, "lab$conv$");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        gen_int_to_float_id(name_1, name_1);

        if (first_next == 1) {
            gen_op_first(op_list, type);
        }
        else {
            gen_op_next(op_list, type);
        }

        addToBuff(my_buff, "JUMP ");
        labToBuff(lab_conv + 2, "lab$conv$");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        gen_int_to_float_id(name_2, name_2);

        if (first_next == 1) {
            gen_op_first(op_list, type);
        }
        else {
            gen_op_next(op_list, type);
        }

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;
    }
    else if (type == IDIV) {
        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 4, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 6, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@string");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFNEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(name_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "int@0");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@9");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (first_next == 1) {
            gen_op_first(op_list, IDIV);
        }
        else {
            gen_op_next(op_list, IDIV);
        }

        addToBuff(my_buff, "JUMP ");
        labToBuff(lab_conv + 4, "lab$conv$");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        gen_int_to_float_id(name_1, name_1);

        if (first_next == 1) {
            gen_op_first(op_list, DIV);
        }
        else {
            gen_op_next(op_list, DIV);
        }

        addToBuff(my_buff, "JUMP ");
        labToBuff(lab_conv + 2, "lab$conv$");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFNEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(name_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "int@0");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@9");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        gen_int_to_float_id(name_2, name_2);

        if (first_next == 1) {
            gen_op_first(op_list, DIV);
        }
        else {
            gen_op_next(op_list, DIV);
        }

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;
    }
    else {
        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 3, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 5, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@string");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_1, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        if (first_next == 1) {
            gen_op_first(op_list, type);
        }
        else {
            gen_op_next(op_list, type);
        }

        addToBuff(my_buff, "JUMP ");
        labToBuff(lab_conv + 4, "lab$conv$");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@float");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        gen_int_to_float_id(name_1, name_1);

        if (first_next == 1) {
            gen_op_first(op_list, type);
        }
        else {
            gen_op_next(op_list, type);
        }

        addToBuff(my_buff, "JUMP ");
        labToBuff(lab_conv + 2, "lab$conv$");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        addToBuff(my_buff, "JUMPIFEQ ");
        labToBuff(lab_conv + 1, "lab$conv$");
        addToBuff(my_buff, " ");
        strToBuff(val_2, "LF@");
        addToBuff(my_buff, " ");
        addToBuff(my_buff, "string@int");
        lineEndPrint();

        addToBuff(my_buff, "EXIT int@4");
        lineEndPrint();

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;

        gen_int_to_float_id(name_2, name_2);

        if (first_next == 1) {
            gen_op_first(op_list, type);
        }
        else {
            gen_op_next(op_list, type);
        }

        addToBuff(my_buff, "LABEL ");
        labToBuff(lab_conv, "lab$conv$");
        lineEndPrint();
        lab_conv++;
    }

    gen_pop(name_2);
    gen_pop(name_1);

    lab_conv++;
    free(val_1);
    free(name_1);
    free(val_2);
    free(name_2);
}

//----------------------------------------------------------------------------------------
/// IMPLEMENTED FUNCTIONS

void gen_fce_lenght() {
    fprintf(OUTPUT,
        "JUMP lenght$end\n"
        "LABEL length\n"
        "PUSHFRAME\n"
        "DEFVAR LF@%%retval\n"
        
        "DEFVAR LF@s$type\n"
        "TYPE LF@s$type LF@s\n"
        "JUMPIFEQ $$continue LF@s$type string@string\n"
        "EXIT int@4\n"
        "LABEL $$continue\n"
        
        "MOVE LF@%%retval int@0\n"
        "STRLEN LF@%%retval LF@s\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL lenght$end\n"
        );
}

void gen_fce_chr() {    
    fprintf(OUTPUT,
        "JUMP chr$end\n"
        "LABEL chr\n"
        "PUSHFRAME\n"
        "DEFVAR LF@%%retval\n"
        
        "DEFVAR LF@i$type\n"
        "TYPE LF@i$type LF@i\n"
        "JUMPIFEQ $$$continue LF@i$type string@int\n"
        "JUMPIFEQ $$$convert LF@i$type string@float\n"
        "EXIT int@4\n"
        "LABEL $$$convert\n"
        "FLOAT2INT LF@i LF@i\n"
        "LABEL $$$continue\n"
        
        "INT2CHAR LF@%%retval LF@i\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL chr$end\n"
        );
}

void gen_fce_ord() {    
    fprintf(OUTPUT,
        "JUMP ord$end\n"
        "LABEL ord\n"
        "PUSHFRAME\n"
        "DEFVAR LF@%%retval\n"
        "DEFVAR LF@%%tmp\n"
        "DEFVAR LF@%%decide\n"
        
        "DEFVAR LF@s$type\n"
        "TYPE LF@s$type LF@s\n"
        "JUMPIFEQ $$$$continue LF@s$type string@string\n"
        "EXIT int@4\n"
        "LABEL $$$$continue\n"
        "STRLEN LF@%%tmp LF@s\n"
        
        "DEFVAR LF@i$type\n"
        "TYPE LF@i$type LF@i\n"
        "JUMPIFEQ $$$$continue2 LF@i$type string@int\n"
        "JUMPIFEQ $$$$convert LF@i$type string@float\n"
        "EXIT int@4\n"
        "LABEL $$$$convert\n"
        "FLOAT2INT LF@i LF@i\n"
        "LABEL $$$$continue2\n"
        
        "LT LF@%%decide LF@i LF@%%tmp\n"
        "JUMPIFEQ ord$true LF@%%decide bool@true\n"
        "MOVE LF@%%retval nil@nil\n"
        "JUMP ord$false\n"
        "LABEL ord$true\n"
        "STRI2INT LF@%%retval LF@s LF@i\n"
        "LABEL ord$false\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL ord$end\n"
        );
}

void gen_fce_substr() {
    fprintf(OUTPUT,
        "JUMP substr$end"
        "LABEL substr\n"
        "PUSHFRAME\n"
        "DEFVAR LF@%%retval\n"
        
        "DEFVAR LF@s$type\n"
        "TYPE LF@s$type LF@s\n"
        "JUMPIFEQ $continue LF@s$type string@string\n"
        "EXIT int@4\n"
        "LABEL $continue\n"
        "STRLEN LF@%%tmp LF@s\n"
        
        "DEFVAR LF@i$type\n"
        "TYPE LF@i$type LF@i\n"
        "JUMPIFEQ $continue2 LF@i$type string@int\n"
        "JUMPIFEQ $convert LF@i$type string@float\n"
        "EXIT int@4\n"
        "LABEL $convert\n"
        "FLOAT2INT LF@i LF@i\n"
        "LABEL $continue2\n"
        
        "DEFVAR LF@n$type\n"
        "TYPE LF@n$type LF@n"
        "JUMPIFEQ $continue3 LF@i$type string@int\n"
        "JUMPIFEQ $convert2 LF@i$type string@float\n"
        "EXIT int@4\n"
        "LABEL $convert2\n"
        "FLOAT2INT LF@n LF@n\n"
        "LABEL $continue3\n"

        "MOVE LF@%%retval nil@nil\n"

        "DEFVAR LF@%%len\n"
        "STRLEN LF@%%len LF@s\n"

        "LT GF@%%tmp2 LF@i int@0\n"
        "JUMPIFEQ substr$almost GF@%%tmp2 bool@true\n"
        "SUB LF@%%len LF@%%len int@1\n"
        "GT GF@%%tmp2 LF@i LF@%%len\n"
        "JUMPIFEQ substr$almost GF@%%tmp2 bool@true\n"
        "LT GF@%%tmp2 LF@n int@0\n"
        "JUMPIFEQ substr$almost GF@%%tmp2 bool@true\n"
    
        "MOVE LF@%%retval string@\n"
        "LABEL sub$loop\n"
        "EQ GF@%%tmp2 LF@n int@0\n"
        "JUMPIFEQ substr$almost GF@%%tmp2 bool@true\n"
        "GT GF@%%tmp2 LF@i LF@%%len\n"
        "JUMPIFEQ substr$almost GF@%%tmp2 bool@true\n"
        "GETCHAR GF@%%tmp3 LF@s LF@i\n"
        "CONCAT LF@%%retval LF@%%retval GF@%%tmp3\n"
        "SUB LF@n LF@n int@1\n"
        "ADD LF@i LF@i int@1\n"
        "JUMP sub$loop\n"

        "LABEL substr$almost\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL substr$end"
        );
}

void gen_fce_inputs() {
    fprintf(OUTPUT,
        "JUMP inputs$end\n"
        "LABEL inputs\n"
        "PUSHFRAME\n"
        "DEFVAR LF@%%retval\n"
        "MOVE LF@%%retval nil@nil\n"
        "READ LF@%%retval string\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL inputs$end\n"
        );
}

void gen_fce_inputi() {
    fprintf(OUTPUT,
        "JUMP inputi$end\n"
        "LABEL inputi\n"
        "PUSHFRAME\n"
        "DEFVAR LF@%%retval\n"
        "MOVE LF@%%retval int@0\n"
        "READ LF@%%retval int\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL inputi$end\n"
        );
}

void gen_fce_inputf() {
    fprintf(OUTPUT,
        "JUMP inputf$end\n"
        "LABEL inputf\n"
        "PUSHFRAME\n"
        "DEFVAR LF@%%retval\n"
        "MOVE LF@%%retval float@0x0p+0\n"
        "READ LF@%%retval float\n"
        "POPFRAME\n"
        "RETURN\n"
        "LABEL inputf$end\n"
        );
}

//...