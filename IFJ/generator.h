/**
 *  Project-IFJ-2018
 *  Part: Generator
 *  File: generator.h
 *  Responsibility lies on: Salon Marek, xsalon00
 *  Help:
 *
 *  From:   13. Oct. 2018
 *  To:     05. Dec. 2018
 *
 */


#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "symtable.h"
#include "scanner.h"
#include "ial.h"
#include "errors.h"
#include <stdlib.h>

#define OUTPUT stdout

typedef enum { ADD, SUB, MUL, DIV, IDIV, LESST, GREATERT, EQUALST, NEQUALST, LESSEQT, GREATEREQT, AND, OR, NOT, CONCAT
}Op_type;

// "if" label counter
unsigned int lab_if;
// "do-while" label counter
unsigned int lab_do;

int iminif; // [áj am in if] z Ang. Som v Ifku :P
int iminwhile;
// using tmp when constant is converted
int static_convert;

typedef struct {
    int length;
    char* string;
} T_buff;

T_buff buff;
T_buff *my_buff;

int addToBuff(T_buff *m_b, char *in_str);
void freeBuff(T_buff *str);
void resetBuff(T_buff *str);
int buffInit();
void intToBuff(int val);
void floatToBuff(double val);
void strToBuff(char* val, char* type);
void labToBuff(int val, char* type);
// GENERATING
void gen_init();
void lineEndPrint();
void gen_def_var(char *val);
void gen_assign_int(int val);
void gen_assign_float(double val);
void gen_assign_string(char *val);
void gen_assign_bool(char *val);
void gen_assign_nil();
void gen_assign_id(char *val);
void gen_assign_ret(char *val);
void gen_assign_tmp_1_id(char *val);
void gen_assign_tmp_1_int(int val);
void gen_assign_tmp_1_float(double val);
void gen_assign_tmp_1_string(char *val);
void gen_assign_tmp_2_id(char *val);
void gen_assign_tmp_3_id(char *val);
void gen_assign_exp_ret(char *val);
void gen_push(char *val);
void gen_pop(char *val);
void gen_int_to_float_id(char *val_1, char *val_2);
void gen_int_to_float_int(int val, int num);
void gen_if_while_true();
void gen_if_while_false();
void gen_print_id(char *val);
void gen_print_str(char *val);
void gen_print_int(int val);
void gen_print_float(double val);
void gen_print_bool(char *val);
void gen_print_nil();
void gen_fce_call_init();
void gen_fce_params(char *val);
void gen_fce_call(char *val);
void gen_fce_def(char *val);
void gen_fce_ret(char *val);
void opTypeSwitch(Op_type type);
void opValSwitch(tDLList *op_list, int num);
void opComplexSwitch(tDLList *op_list, Op_type type);
void gen_op_next(tDLList *op_list, Op_type type);
void gen_op_first(tDLList *op_list, Op_type type);
void gen_then_eq();
void gen_else();
void gen_end_if();
void gen_while();
void gen_while_post();
void gen_loop();
void gen_check_first(tDLList *op_list, Op_type type, T_type second, int mode, int first_next);
void gen_check_both(tDLList *op_list, Op_type type, T_type second, int first_next);
void gen_fce_lenght();
void gen_fce_chr();
void gen_fce_ord();
void gen_fce_inputs();
void gen_fce_inputi();
void gen_fce_inputf();
void gen_fce_print();

//#endif