/**
 *  Project-IFJ-2018
 *  Part: Parser
 *  File: parser.h
 *  Responsibility lies on: Sencak Jakub, xsenca00
 *  Help:
 *
 *  From:   13. Oct. 2018
 *  To:     05. Dec. 2018
 *
 */

#ifndef IFJ2018_PARSER_H
#define IFJ2018_PARSER_H


// Libraries


// Headers
#include "errors.h"
#include "scanner.h"
#include "psa.h"
#include "symtable.h"
#include "built_in_fun.h"
#include "generator.h"

// Macros
/*memcpy(prevtoken, &token, sizeof(T_struct));*/
/*return x; printf("   %d\n", token.type);*/
#define MAKETOKEN do{if((x = makeToken(getToken()))){pState(x);}}while(0)
/*return x; printf("   %d\n", token.type);*/
#define MAKETOKENFIRST do{if((x = makeToken(getToken()))){pState(x);}}while(0)
#define CHECKSTATE do{if(check_p_state())return p_state;}while(0)
#define CHECKEOF do{if (x == -1)return p_state;} while (0)

#define BUBU do{fprintf(stderr, "BUBU\n");}while(0)
#define TType token.type
#define TVal token.value
#define LEFT 1
#define RIGHT 0

// Flags
int commaF;
int isBracket;

// Variables
int x;
int lala;
int p_state;
int ret_val;
int defining_fun;

int brckt_cnt;

int ifindex;

char nf[100];
char nf2[100];
// Structs
T_struct *prevtoken;
T_struct prev_real;

// Function declarations
// helping fun
void testing(int expectation, int reality);
int check_p_state();
const char *getTokenName(int t);
void pState(int state);
void compareToken(int t);   // using MAKETOKEN
void compareTokenX(int t);  // without MAKETOKEN
int brackets(int side, int condition);
const char *getTokenName(int t);
// grammar functions
int id_cmd();
int call_function();
int while_cmd();
int if_cmd();
int params();
int fundef_cmd();
int cmd();
int def_cmds();
int program();
int init_built_in_fun();
int parser_main();
int parser_first();
void gen_builtin();
// #Tags
// #precedence
// #generator



#endif //IFJ2018_PARSER_H