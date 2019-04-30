/**
 *  Project-IFJ-2018
 *  Part: Precedence analysis
 *  File: psa.h
 *  Responsibility lies on: Sterba Maros, xsterb12
 *  Help:
 *
 *  From:   13. Oct. 2018
 *  To:     05. Dec. 2018
 *
 */
#ifndef _IAL_H_
#define _IAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "errors.h"

//----------------------------------------------------------------------------------//
//--------------------------STACK-PRECEDENCE-ANALYZE--------------------------------//
//----------------------------------------------------------------------------------//

typedef struct tSItem {
    T_struct symbol;
    T_type reduceType;
    bool temp;
    bool term;
    bool handle;
    struct tSItem *next;
} *tSItemPtr;

typedef struct {
    tSItemPtr top;
} tStack;


void stackInit ( tStack *s );

int stackEmpty ( const tStack* s );

void stackTop ( const tStack* s, T_struct symbol );

void stackPop ( tStack* s );

void stackPushTerm ( tStack* s, T_struct term );

void stackPushRule ( tStack* s, T_struct rule, T_type reduceType );

void stackPushRuleID ( tStack* s, T_struct rule, T_type reduceType );

void stackDispose ( tStack* s );


//----------------------------------------------------------------------------------//
//-------------------------------------LIST-----------------------------------------//
//----------------------------------------------------------------------------------//

#define FALSE 0
#define TRUE 1

extern int errflg;

typedef struct tDLElem {                 /* prvek dvousměrně vázaného seznamu */
    T_struct symbol;                                            /* užitečná data */
    T_type reduceType;
    bool temp;
    struct tDLElem *lptr;          /* ukazatel na předchozí prvek seznamu */
    struct tDLElem *rptr;        /* ukazatel na následující prvek seznamu */
} *tDLElemPtr;

typedef struct {                                  /* dvousměrně vázaný seznam */
    tDLElemPtr First;                      /* ukazatel na první prvek seznamu */
    tDLElemPtr Act;                     /* ukazatel na aktuální prvek seznamu */
    tDLElemPtr Last;                    /* ukazatel na posledni prvek seznamu */
} tDLList;

void DLInitList (tDLList *L);
void DLDisposeList (tDLList *L);
void DLInsertFirst (tDLList *L, T_struct symbol, T_type type, bool temp);
void DLInsertLast(tDLList *L, T_struct symbol);
void DLFirst (tDLList *L);
void DLLast (tDLList *L);
void DLCopyFirst (tDLList *L, T_struct *symbol);
void DLCopyLast (tDLList *L, T_struct *symbol);
void DLDeleteFirst (tDLList *L);
void DLDeleteLast (tDLList *L);
void DLPostDelete (tDLList *L);
void DLPreDelete (tDLList *L);
void DLPostInsert (tDLList *L, T_struct symbol);
void DLPreInsert (tDLList *L, T_struct symbol);
void DLCopy (tDLList *L, T_struct *symbol);
void DLActualize (tDLList *L, T_struct symbol);
void DLSucc (tDLList *L);
void DLPred (tDLList *L);
int DLActive (tDLList *L);

#endif
