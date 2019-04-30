/**
 *  Project-IFJ-2018
 *  Part: Scanner
 *  File: scanner.h
 *  Responsibility lies on: Salon Marek, xsalon00
 *  Help:
 *
 *  From:   13. Oct. 2018
 *  To:     05. Dec. 2018
 *
 */

#ifndef IFJ2018_SCANNER_H
#define IFJ2018_SCANNER_H

// C libraries headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// Custom headers
#include "errors.h"

// size of keywords table
#define KW_NUM 20

// lexem error
#define LEXEM_ERR -2

// amount of allocated bytes for buffer
#define MEM_STR 8

// EOF == -1

// test flag
#define TEST_FLAG 0

// option constants for determination of ascii value
#define LOW_ASCII 1
#define MID_ASCII 2
#define HIGH_ASCII 3

// macro used for adding next char to buffer
#define ADDCHAR if (addNextChar(str, in_char)) return INTERNAL_ERR
#define RET(x, y) x = y; return y;

// state type
typedef enum {
    START,
    ID,
    NUMBER,
    ASSIGNMENT,
    NOT_EQ,
    LINE_COMM,
    BRCKT_BEGIN,
    BRCKT_COMM,
    BRCKT_END,
    BRCKT_ENDING,
    LESS,
    GREATER,
    STRING,
    NUM_EXP,
    NUM_DOUBLE,
    DOUBLE_EXP,
    NUM_EXP_OP,
    NUM_EXP_END,
    ESC_EXPR
} fsm_state;

// token type
typedef enum {
    T_DEF,          //0
    T_DO,           //1
    T_ELSE,         //2
    T_END,          //3
    T_IF,           //4
    T_NOT,          //5
    T_NIL,          //6
    T_THEN,         //7
    T_WHILE,        //8
    T_INPUTS,       //9
    T_INPUTI,       //10
    T_INPUTF,       //11
    T_PRINT,        //12
    T_LENGHT,       //13
    T_SUBSTR,       //14
    T_ORD,          //15
    T_CHR,          //16
    T_INTEGER,      //17
    T_STRING,       //18
    T_DOUBLE,       //19
    T_EOL,          //20

    // main types
    T_ID,           //21
    T_INT_NUM,      //22
    T_DOUBLE_NUM,   //23
    T_STRING_EXP,   //24
    T_BRCKT_L,      //25
    T_BRCKT_R,      //26
    T_COMMA,        //27
    T_SEMICOLON,    //28
    // comparation
    T_LESS_EQ,      //29
    T_NOT_EQ,       //30
    T_LESS,         //31
    T_GREATER,      //32
    T_GREATER_EQ,   //33
    T_ASSIGNMENT,   //34
    T_EQUALS,       //35
    // binary operators
    T_PLUS,         //36
    T_MINUS,        //37
    T_MUL,          //38
    T_DIV,          //39
	T_BOOLEAN,		//40
	T_FUNCTION		//41
} T_type;


// structure defining final token
typedef struct {
    T_type type;
    union {
        int val_i;
        double val_d;
        char* string;
    } value;
} T_struct;

// dynamic buffer structure
typedef struct
{
    int length;
    int alloc_mem;
    char* string;
} T_string;

T_string *str;
T_string buffer;
T_struct token;
T_type last_Ttoken;

int scannerInit(FILE *fp);
int scannerInit(FILE *fp);
int addNextChar(T_string *str, char in_char);
void strReset(T_string *str);
void tokenFree();
void strFree(T_string *str);
int addEscSeq(T_string *str, char in_char, char* str_t, int option);
void escSeq(int in_char);
int hexToDec(char hexNum[]);
int getToken();
int makeToken(int Ttoken);

#endif //IFJ2018_SCANNER_H
