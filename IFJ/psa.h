/**
 *	Project-IFJ-2018
 *	Part: Precedence analysis
 *	File: psa.h
 *	Responsibility lies on: Sterba Maros, xsterb12
 *	Help:
 *
 *	From: 	13. Oct. 2018
 *	To:		05. Dec. 2018
 *
 */

#ifndef psa_h
#define psa_h

#include "parser.h"
#include "ial.h"

/// Size of precedence table
#define TABLE_SIZE 10

/// Table containing rules for precedence analysis
/// Head of rows contains terms closest to the top of stack
/// Head of columns contains characters on input
static const char precedence_table [TABLE_SIZE][TABLE_SIZE] = {

/*			 +   -	 *	 /	 ( 	 )	ID	GL	EQ   $ */
/*+*/	{ 	'>','>','<','<','<','>','<','>','>','>'	},
/*-*/	{	'>','>','<','<','<','>','<','>','>','>'	},
/***/	{	'>','>','>','>','<','>','<','>','>','>'	},
/*/*/	{	'>','>','>','>','<','>','<','>','>','>'	},
/*(*/	{	'<','<','<','<','<','=','<','<','<',' '	},
/*)	*/	{	'>','>','>','>',' ','>',' ','>','>','>'	},
/*ID*/	{	'>','>','>','>',' ','>',' ','>','>','>'	},
/*GL*/	{	'<','<','<','<','<','>','<',' ','>','>'	},
/*EQ*/  {   '<','<','<','<','<','>','<','<',' ','>' },
/*$	*/	{	'<','<','<','<','<',' ','<','<','<',' '	}
};

//Declarations of functions

/**
 * @brief   Select term closest to top on precedence analysis stack
 * @param   PAStack      Pointer to precedence analysis stack \p tStack
 * @return  Pointer to first term \p tSItemPtr on precedence analysis stack
 */
tSItemPtr topFirstTerm ( tStack* PAStack );

/**
 * @brief   Select rule for derivation of expression
 * @param   psa_state    Pointer to return value of precedence analysis \p tStack
 * @param   i            
 * @param   tDLList      Pointer to derived expression \p tDLList
 * @param   PAStack      Pointer to precedence analysis stack \p tStack
 * @return  Function returns data type \c void
 */
void derivate( int* psa_state, int i, tDLList* L, tStack* PAStack, bool* firstExp, bool* alone );

/**
 * @brief   Select content from stack to derive
 * @param   psa_state    Pointer to return value of precedence analysis
 * @param   PAStack      Pointer to precedence analysis stack \p tStack
 * @return  Function returns data type \c void
 */
void reduce( int* psa_state, tStack* PAStack, bool* firstExp, bool* alone );

/**
 * @brief   Push actual term on precedence analysis stack and label begin of expression to derive
 * @param   PAStack      Pointer to precedence analysis stack \p tStack
 * @param   firstTerm    Pointer to first term \p tSItemPtr on precedence analysis stack
 * @return  Function returns data type \c void
 */
void pushAndChange( tStack* PAStack, tSItemPtr* firstTerm, int* var_cnt, bool* alone );

/**
 * @brief   If previous token was variable then push previous term on precedence analysis stack and label begin of expression to derive
 * @param   PAStack      Pointer to precedence analysis stack \p tStack
 * @param   firstTerm    Pointer to first term \p tSItemPtr on precedence analysis stack
 * @param	previousToken Pointer to previous token which was reading in parser \p T_struct
 * @return  Function returns data type \c void
 */
void pushAndChangePreviousToken( tStack* PAStack, tSItemPtr* firstTerm, T_struct* previousToken, int* var_cnt, bool* alone );

/**
 * @brief   Push actual term on precedence analysis stack
 * @param   PAStack      Pointer to precedence analysis stack \p tStack
 * @return  Function returns data type \c void
 */
void pushNoChange( tStack* PAStack );

/**
 * @brief   Select index of row or column to precedence analysis table by given token
 * @param   type    Type \p T_type of given token
 * @return  Return index \p int to precedence analysis table
 */
int getIndex ( T_type type );

/**
 * @brief   Select rule from precedence table by actual token in expression and first term on precedence analysis stack
 * @param   psa_state    Pointer to return value of precedence analysis 
 * @param   PAStack      Pointer to precedence analysis stack
 * @param   firstTerm    Term closest to the top of precedence analysis stack
 * @return  Function returns data type \c void
 */
void selectRule( int* psa_state, tStack* PAStack, tSItemPtr* firstTerm, int* var_cnt, bool* firstExp, bool* alone );

/**
 * @brief   Select rule from precedence table by previous token in expression and first term on precedence analysis stack
 * @param   psa_state    Pointer to return value of precedence analysis 
 * @param   PAStack      Pointer to precedence analysis stack
 * @param   firstTerm    Term closest to the top of precedence analysis stack
 * @return  Function returns data type \c void
 */
void selectRulePreviousToken ( int* psa_state, tStack* PAStack, tSItemPtr* firstTerm, T_struct* previousToken, int* var_cnt, bool* firstExp, bool* alone );

/**
 * @brief   Syntactic analysis for expressions
 * @param	previousToken Pointer to previous token which was reading in parser \p T_struct
 * @return  @c EXIT_SUCCESS if analysis was successful or @c SYNTAX_ERR, @c SEMANTICS_ERR, @c SEM_TYPES_ERR if not
 */
int precedenceAnalysis( T_struct* previousToken );

#endif // psa_h