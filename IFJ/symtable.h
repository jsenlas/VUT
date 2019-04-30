/**
 *  Project-IFJ-2018
 *  Part: Symtable
 *  File: symtable.h
 *  Responsibility lies on: Michalik Ladislav, xmicha70
 *  Help:
 *
 *  From:   13. Oct. 2018
 *  To:     05. Dec. 2018
 *
 */



#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <stdlib.h>
#include <string.h>
#include "scanner.h"
#include "errors.h"
#define NAME_LENGTH 100

//#include "parser.h"
typedef enum {
	t_int,
	t_double,
	t_string,
	t_void,
	t_nil
}t_type;
//Struktura pro promennou
typedef struct T_DIM{ 
	char name[NAME_LENGTH];		//nazev;
	//unsigned int length; 	//delka
	t_type type;
}t_dim;

typedef struct T_PARAM{
	char name[NAME_LENGTH];
	struct T_PARAM *next_param;
	int type;
}t_param;

typedef struct T_FUNKCE{
	t_param* params;
	bool declare;
	bool define;
	char name[NAME_LENGTH]; //nazev
	t_type type;
	int number_of_parametrs;
	void* local_symtable;
}t_function;

/**
* @brief    Maximal size of array-to-linked list based hash table.
*/
#define MAX_HTSIZE 101


/**
* @brief    Key of element (for example identification of goods).
*/
typedef char tKey[100];


/**
* @brief    Value of element (for example price of goods).
*/
typedef union tData{
	t_function dFunction;
	t_dim dDim;
} tData;


/**
* @brief    Item of hash table.
*/
 typedef struct tHTItem{
	tKey key;				/**< key  */
	tData data;				/**< value */
	struct tHTItem* ptrnext;	/**< pointer to next element */
} tHTItem;


/**
* @brief    Array-to-linked list based hash table.
*/
typedef tHTItem* tHTable[MAX_HTSIZE];


/**
* @brief    Size of array-to-linked list based hash table.
*/
extern int HTSIZE;

/**
* @brief    Global definition for checking declaration and definition function
*/
char function_name[100];
tHTable Globalsymtab;
tHTable*  Globalsymtab_pointer;
int paramcounter;

t_param Global_param;
t_param *Global_param_;

bool Redeclare_flag;
bool Define_flag;
bool scope_flag;

t_dim global_var;

/**
* @brief    Declarations of functions.
*/
int hashCode ( tKey key );

void htInit ( tHTable* ptrht );

tHTItem* htSearch ( tHTable* ptrht, tKey key );

void htPrintTable( tHTable* ptrht );

void htPrintTableLocal( tHTable* ptrht );

void htInsert ( tHTable* ptrht, tKey key, tData data );
void htInsert_local ( tHTable* ptrht, tKey key, tData data );

tData* htRead ( tHTable* ptrht, tKey key );

void htDelete ( tHTable* ptrht, tKey key );

void htClearAll ( tHTable* ptrht );

//LADA

void initGlobalTable(tHTable* Globalsymtab);

void freeGlobalTable(tHTable* Globalsymtab );

int checking_declare_function( tKey name_function);

void loading_param_name(T_struct token);

void param_init(void);

void loading_param_type(t_type type);

int loading_param_to_global_symtable(void);

void loading_return_type(T_struct token);

int checking_definition_function( tKey name_function);

int initLocallTable(tHTable* Globalsymtab,tKey name_function);

void loading_var_name(T_struct token);

void loading_var_type(T_struct token);

int save_var_to_correct_local_table();

void print_local_symtable_by_name(char * name);

void scope_init();

void hard_declare_define_and_created_local_symtable(char *hard_function);

bool dim_precedence(int *type,char *name);

int change_var_type(void);

//void hard_definition_Length(void);
//
//void hard_definition_Substr(void);
//
//void hard_definition_Asc(void);
//    
//void hard_definition_Chr(void);

bool checking_existing_function(char *name);

//////////////////////////////////////////////////////////////////////////////////new ifj 2018

void hard_definition_inputs(void);

void hard_definition_inputi(void);

void hard_definition_inputf(void);

void hard_definition_lenght(void);

void hard_definition_substr(void);

void hard_definition_ord(void);

void hard_definition_chr(void);












//#define velikost_prom 255
//typedef struct xPROM{
//char jmeno[velikost_prom];
//int type;
//}xprom;
//
//

//
//typedef struct xFUNKCE{
//	t_param* params;
//	//bool declare;
//	bool define;
//	char name[velikost_prom]; //nazev
//	//t_type type;
//	int number_of_parametrs;
//	void* local_symtable;
//}xfunction;
////int  compare_define_and_declare_function(T_struct token);
#endif