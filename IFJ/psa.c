#include "psa.h"

bool inIf = false;

tSItemPtr topFirstTerm ( tStack* PAStack ) {

    tSItemPtr temp_item = PAStack->top;

    while ( temp_item->term != true ) {
        temp_item = temp_item->next;
    }
    return temp_item;   //return first term or NULL
}

void derivate ( int* psa_state, int i, tDLList* L, tStack* PAStack, bool* firstExp, bool* alone ) {
    T_type redType;
    //variable or constant
    if ( i == 1 ) {
        T_type term_type = L->First->symbol.type;
        static_convert = 0;
        //variable
        if ( term_type == T_ID ) {
            int type;

            //check definition of variable
            if ( ( dim_precedence( &type, L->First->symbol.value.string ) ) == false ){
                //not defined
                *psa_state = SEMANTICS_ERR;
                return;
            }
            else {
                switch ( type ) {
                    case 22: redType = T_INT_NUM; break;
                    case 23: redType = T_DOUBLE_NUM; break;
                    case 24: redType = T_STRING_EXP; break;
                    default : redType = T_NIL; break;
                }
                stackPushRule( PAStack, L->First->symbol, redType );

                //assignment with one value
                //gen MOVE
                if ( *alone == true ) {
                    gen_assign_tmp_1_id( L->First->symbol.value.string );
                    if ( inIf ) {
                        gen_if_while_true();
                    } 
                    *alone = false;
                }
            }
        }
        else if ( term_type == T_INT_NUM ) {
            stackPushRule( PAStack, L->First->symbol, T_INT_NUM );
            //assignment with one value
            //gen MOVE
            if ( *alone == true ) {
                gen_assign_tmp_1_int( L->First->symbol.value.val_i );
                if ( inIf ) {
                    gen_if_while_true();
                } 
                *alone = false;
            }
        }
        else if ( term_type == T_DOUBLE_NUM ) {
            stackPushRule( PAStack, L->First->symbol, T_DOUBLE_NUM );
            //assignment with one value
            //gen MOVE
            if ( *alone == true ) {
                gen_assign_tmp_1_float( L->First->symbol.value.val_d );
                if ( inIf ) {
                    gen_if_while_true();
                } 
                *alone = false;
            }
        }
        else if ( term_type == T_STRING_EXP ) {
            stackPushRule( PAStack, L->First->symbol, T_STRING_EXP );
            //assignment with one value
            //gen MOVE
            if ( *alone == true ) {
                gen_assign_tmp_1_string( L->First->symbol.value.string );
                if ( inIf ) {
                    gen_if_while_true();
                } 
                *alone = false;
            }
        }
        else if ( term_type == T_NIL ) {
            stackPushRule( PAStack, L->First->symbol, T_NIL );
            //assignment with one value
            //gen MOVE
            if ( *alone == true ) {
                gen_assign_tmp_1_string( L->First->symbol.value.string );
                if ( inIf ) {
                    gen_if_while_false();
                } 
                *alone = false;
            }
        }
        else {
            *psa_state = SYNTAX_ERR;
            return;
        }
    }
    //empty brackets ()
    else if ( i == 2 ) {
        if ( L->First->symbol.type == T_BRCKT_L ){
            if ( L->First->rptr->symbol.type == T_BRCKT_R ) {
                stackPushRule( PAStack, L->First->symbol, T_EOL );
            }
        }
    }
    //expressions
    else if ( i == 3 ) {
        T_type firstOperand = L->First->reduceType;
        T_type secondOperand = L->First->rptr->rptr->reduceType;
        T_type operation = L->First->rptr->symbol.type;

        //Concatenate
        if ( operation == T_PLUS && firstOperand == T_STRING_EXP && secondOperand == T_STRING_EXP ) {
            redType = firstOperand;
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen CONCAT
            if ( *firstExp ) {
                gen_op_first(L, CONCAT);
                *firstExp = false;
            }
            else {
                gen_check_first(L, ADD, T_DOUBLE_NUM, 2, 0);
                gen_op_next(L, ADD);
                gen_pop(L->First->rptr->rptr->symbol.value.string);
            }
        }
        else if ( operation == T_PLUS && firstOperand == T_NIL && secondOperand == T_STRING_EXP ) {
            redType = firstOperand;
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen CONCAT
            if ( *firstExp ) {
                gen_check_first(L, ADD, T_STRING_EXP, 1, 0);
                gen_op_first(L, CONCAT);
                gen_pop(L->First->symbol.value.string);
                *firstExp = false;
            }
            else {
                gen_check_first(L, ADD, T_DOUBLE_NUM, 2, 0);
                gen_op_next(L, ADD);
                gen_pop(L->First->rptr->rptr->symbol.value.string);
            }
        }
        else if ( operation == T_PLUS && firstOperand == T_STRING_EXP && secondOperand == T_NIL ) {
            redType = secondOperand;
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen CONCAT
            if ( *firstExp ) {
                gen_check_first(L, ADD, T_STRING_EXP, 2, 0);
                gen_op_first(L, CONCAT);
                gen_pop(L->First->rptr->rptr->symbol.value.string);
                *firstExp = false;
            }
            else {
                gen_check_first(L, ADD, T_DOUBLE_NUM, 2, 0);
                gen_op_next(L, ADD);
                gen_pop(L->First->rptr->rptr->symbol.value.string);
            }
        }
        //IF String Then Concatenate
        else if ( operation == T_PLUS && firstOperand == T_NIL && secondOperand == T_NIL ) {
            redType = firstOperand;
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen CONCAT
            if ( *firstExp ) {
                gen_check_both(L, ADD, T_NIL, 1);
                *firstExp = false;
            }
            else {
                gen_check_both(L, ADD, T_NIL, 2);
            }
        }
        //Math operations + - * /
        //Addition
        else if ( operation == T_PLUS ) {
            //change data types
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                //gen ADD
                if ( *firstExp ) {
                    gen_op_first(L, ADD);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, ADD);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                redType = secondOperand;
                //variable
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen ADD
                if ( *firstExp ) {
                    gen_op_first(L, ADD);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, ADD);
                }
                if ( L->First->symbol.type == T_ID ) {
                        gen_pop( L->First->symbol.value.string );
                    }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                redType = T_DOUBLE_NUM;
                //variable
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen ADD
                if ( *firstExp ) {
                    gen_op_first(L, ADD);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, ADD);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                //gen ADD
                if ( *firstExp ) {
                    gen_op_first(L, ADD);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, ADD);
                }
            }
            //NIL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, ADD, T_INT_NUM, 2, 0);
                    gen_op_first(L, ADD);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, ADD, T_INT_NUM, 2, 0);
                    gen_op_next(L, ADD);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, ADD, T_INT_NUM, 1, 0);
                    gen_op_first(L, ADD);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, ADD, T_INT_NUM, 2, 0);
                    gen_op_next(L, ADD);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, ADD, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, ADD);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, ADD, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, ADD);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, ADD, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, ADD);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, ADD, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, ADD);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, ADD, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, ADD, T_NIL, 2);
                }
            }
            else {
                *psa_state = SEM_TYPES_ERR;
                return;
            }
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen
        }
        //Subtraction
        else if ( operation == T_MINUS ) {
            //change data types
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                redType = L->First->symbol.type;
                //gen SUB
                if ( *firstExp ) {
                    gen_op_first(L, SUB);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, SUB);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                redType = secondOperand;
                //variable
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen SUB
                if ( *firstExp ) {
                    gen_op_first(L, SUB);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, SUB);
                }
                if ( L->First->symbol.type == T_ID ) {
                        gen_pop( L->First->symbol.value.string );
                    }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                redType = T_DOUBLE_NUM;
                //variable
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen SUB
                if ( *firstExp ) {
                    gen_op_first(L, SUB);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, SUB);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                //gen SUB
                if ( *firstExp ) {
                    gen_op_first(L, SUB);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, SUB);
                }
            }
            //NIL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, SUB, T_INT_NUM, 2, 0);
                    gen_op_first(L, SUB);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, SUB, T_INT_NUM, 2, 0);
                    gen_op_next(L, SUB);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, SUB, T_INT_NUM, 1, 0);
                    gen_op_first(L, SUB);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, SUB, T_INT_NUM, 2, 0);
                    gen_op_next(L, SUB);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, SUB, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, SUB);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, SUB, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, SUB);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, SUB, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, SUB);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, SUB, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, SUB);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, SUB, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, SUB, T_NIL, 2);
                }
            }
            else {
                *psa_state = SEM_TYPES_ERR;
                return;
            }
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen
        }
        //Multiplication
        else if ( operation == T_MUL ) {
            //change data types
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, MUL);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, MUL);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                redType = secondOperand;
                //variable
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, MUL);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, MUL);
                }
                if ( L->First->symbol.type == T_ID ) {
                        gen_pop( L->First->symbol.value.string );
                    }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                //variable
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, MUL);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, MUL);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, MUL);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, MUL);
                }
            }
            //NIL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, MUL, T_INT_NUM, 2, 0);
                    gen_op_first(L, MUL);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, MUL, T_INT_NUM, 2, 0);
                    gen_op_next(L, MUL);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, MUL, T_INT_NUM, 1, 0);
                    gen_op_first(L, MUL);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, MUL, T_INT_NUM, 2, 0);
                    gen_op_next(L, MUL);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, MUL, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, MUL);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, MUL, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, MUL);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, MUL, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, MUL);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, MUL, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, MUL);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, MUL, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, MUL, T_NIL, 2);
                }
            }
            else {
                *psa_state = SEM_TYPES_ERR;
                return;
            }
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen
        }
        //division
        else if ( operation == T_DIV ) {
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                //divide by zero, if constant
                if ( L->First->rptr->rptr->symbol.value.val_i == 0 ) {
                    *psa_state = ZERO_DIV;
                    return;
                }

                //gen IDIV
                if ( *firstExp ) {
                    gen_op_first(L, IDIV);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, IDIV);
                }
            }
            else {
                if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                    redType = secondOperand;
                    //divide by zero, if constant
                    if ( L->First->rptr->rptr->symbol.value.val_d == 0.0 ) {
                        *psa_state = ZERO_DIV;
                        return;
                    }

                    //variable
                    if ( L->First->symbol.type == T_ID ) {
                        gen_push( L->First->symbol.value.string );
                        gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                    }
                    //constant
                    else {
                        //add function
                        gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                    }
                    //gen DIV
                    if ( *firstExp ) {
                        gen_op_first(L, DIV);
                        *firstExp = false;
                    }
                    else {
                        gen_op_next(L, DIV);
                    }
                    if ( L->First->symbol.type == T_ID ) {
                        gen_pop( L->First->symbol.value.string );
                    }
                }
                else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                    redType = firstOperand;
                    //divide by zero, if constant
                    if ( L->First->rptr->rptr->symbol.value.val_i == 0 ) {
                        *psa_state = ZERO_DIV;
                        return;
                    }

                    //variable
                    if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                        gen_push( L->First->rptr->rptr->symbol.value.string );
                        gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );

                    }
                    //constant
                    else {
                        gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                    }
                    //gen DIV
                    if ( *firstExp ) {
                        gen_op_first(L, DIV);
                        *firstExp = false;
                    }
                    else {
                        gen_op_next(L, DIV);
                    }
                    if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                        gen_pop( L->First->rptr->rptr->symbol.value.string );
                    }

                }
                else if (firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                    redType = firstOperand;
                    //divide by zero, if constant
                    if ( L->First->rptr->rptr->symbol.value.val_d == 0.0 ) {
                        *psa_state = ZERO_DIV;
                        return;
                    }

                    //gen DIV
                    if ( *firstExp ) {
                        gen_op_first(L, DIV);
                        *firstExp = false;
                    }
                    else {
                        gen_op_next(L, DIV);
                    }
                }
                //NIL
                else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                    redType = secondOperand;
                    if ( *firstExp ) {
                        gen_check_first(L, IDIV, T_INT_NUM, 2, 1);
                        gen_pop(L->First->rptr->rptr->symbol.value.string);
                        *firstExp = false;
                    }
                    else {
                        gen_check_first(L, IDIV, T_INT_NUM, 2, 2);
                        gen_pop(L->First->rptr->rptr->symbol.value.string);
                    }
                }
                else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                    redType = firstOperand;
                    if ( *firstExp ) {
                        gen_check_first(L, IDIV, T_INT_NUM, 1, 1);
                        gen_pop(L->First->symbol.value.string);
                        *firstExp = false;
                    }
                    else {
                        gen_check_first(L, IDIV, T_INT_NUM, 2, 2);
                        gen_pop(L->First->rptr->rptr->symbol.value.string);
                    }
                }
                else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                    redType = secondOperand;
                    if ( *firstExp ) {
                        gen_check_first(L, IDIV, T_DOUBLE_NUM, 2, 1);
                        gen_pop(L->First->rptr->rptr->symbol.value.string);
                        *firstExp = false;
                    }
                    else {
                        gen_check_first(L, IDIV, T_DOUBLE_NUM, 2, 2);
                        gen_pop(L->First->rptr->rptr->symbol.value.string);
                    }
                }
                else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                    redType = firstOperand;
                    if ( *firstExp ) {
                        gen_check_first(L, IDIV, T_DOUBLE_NUM, 1, 1);
                        gen_pop(L->First->symbol.value.string);
                        *firstExp = false;
                    }
                    else {
                        gen_check_first(L, IDIV, T_DOUBLE_NUM, 2, 2);
                        gen_pop(L->First->rptr->rptr->symbol.value.string);
                    }
                }
                else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                    redType = firstOperand;
                    if ( *firstExp ) {
                        gen_check_both(L, IDIV, T_NIL, 1);
                        *firstExp = false;
                    }
                    else {
                        gen_check_both(L, IDIV, T_NIL, 2);
                    }
                }
                else {
                    *psa_state = SEM_TYPES_ERR;
                    return;
                }
            }
            stackPushRule( PAStack, L->First->symbol, redType );
        }
        //Bool expressions
        else if ( operation == T_GREATER_EQ ) {
            redType = T_BOOLEAN;
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                //gen GreaterEq
                if ( *firstExp ) {
                    gen_op_first(L, GREATEREQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATEREQT);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                //L->First->symbol.type = T_DOUBLE_NUM;
                //gen GreaterEq
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, GREATEREQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATEREQT);
                }
                if ( L->First->symbol.type == T_ID ) {
                        gen_pop( L->First->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                //L->First->rptr->rptr->symbol.type = T_DOUBLE_NUM;
                //gen GreaterEq
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen SUB
                if ( *firstExp ) {
                    gen_op_first(L, GREATEREQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATEREQT);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                //gen GreaterEq
                if ( *firstExp ) {
                    gen_op_first(L, GREATEREQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATEREQT);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_STRING_EXP ) {
                //gen GreaterEq
                if ( *firstExp ) {
                    gen_op_first(L, GREATEREQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATEREQT);
                }
            }
            //NIL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATEREQT, T_INT_NUM, 2, 0);
                    gen_op_first(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATEREQT, T_INT_NUM, 2, 0);
                    gen_op_next(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATEREQT, T_INT_NUM, 1, 0);
                    gen_op_first(L, GREATEREQT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATEREQT, T_INT_NUM, 2, 0);
                    gen_op_next(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATEREQT, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATEREQT, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATEREQT, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, GREATEREQT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATEREQT, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, GREATEREQT, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, GREATEREQT, T_NIL, 2);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATEREQT, T_STRING_EXP, 2, 0);
                    gen_op_first(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATEREQT, T_STRING_EXP, 2, 0);
                    gen_op_next(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_STRING_EXP ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATEREQT, T_STRING_EXP, 1, 0);
                    gen_op_first(L, GREATEREQT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATEREQT, T_STRING_EXP, 2, 0);
                    gen_op_next(L, GREATEREQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else {
                *psa_state = SEM_TYPES_ERR;
                return;
            }
            stackPushRule( PAStack, L->First->symbol, redType );
        }
        //LESS EQ
        else if ( operation == T_LESS_EQ ) {
            redType = T_BOOLEAN;
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                //gen GreaterEq
                if ( *firstExp ) {
                    gen_op_first(L, LESSEQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESSEQT);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                //L->First->symbol.type = T_DOUBLE_NUM;
                //gen GreaterEq
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, LESSEQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESSEQT);
                }
                if ( L->First->symbol.type == T_ID ) {
                        gen_pop( L->First->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                //L->First->rptr->rptr->symbol.type = T_DOUBLE_NUM;
                //gen GreaterEq
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen SUB
                if ( *firstExp ) {
                    gen_op_first(L, LESSEQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESSEQT);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                //gen GreaterEq
                if ( *firstExp ) {
                    gen_op_first(L, LESSEQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESSEQT);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_STRING_EXP ) {
                //gen GreaterEq
                if ( *firstExp ) {
                    gen_op_first(L, LESSEQT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESSEQT);
                }
            }
            //NIL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESSEQT, T_INT_NUM, 2, 0);
                    gen_op_first(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESSEQT, T_INT_NUM, 2, 0);
                    gen_op_next(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESSEQT, T_INT_NUM, 1, 0);
                    gen_op_first(L, LESSEQT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESSEQT, T_INT_NUM, 2, 0);
                    gen_op_next(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESSEQT, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESSEQT, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESSEQT, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, LESSEQT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESSEQT, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, LESSEQT, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, LESSEQT, T_NIL, 2);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESSEQT, T_STRING_EXP, 2, 0);
                    gen_op_first(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESSEQT, T_STRING_EXP, 2, 0);
                    gen_op_next(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_STRING_EXP ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESSEQT, T_STRING_EXP, 1, 0);
                    gen_op_first(L, LESSEQT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESSEQT, T_STRING_EXP, 2, 0);
                    gen_op_next(L, LESSEQT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else {
                *psa_state = SEM_TYPES_ERR;
                return;
            }
            stackPushRule( PAStack, L->First->symbol, redType );
        }
        //GREATER
        else if ( operation == T_GREATER ) {
            redType = T_BOOLEAN;
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, GREATERT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATERT);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                //L->First->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    //add function
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen DIV
                if ( *firstExp ) {
                    gen_op_first(L, GREATERT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATERT);
                }
                if ( L->First->symbol.type == T_ID ) {
                    gen_pop( L->First->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                //L->First->rptr->rptr->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, GREATERT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATERT);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, GREATERT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATERT);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_STRING_EXP ) {
                if ( *firstExp ) {
                    gen_op_first(L, GREATERT);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, GREATERT);
                }
            }
            //NIL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATERT, T_INT_NUM, 2, 0);
                    gen_op_first(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATERT, T_INT_NUM, 2, 0);
                    gen_op_next(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATERT, T_INT_NUM, 1, 0);
                    gen_op_first(L, GREATERT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATERT, T_INT_NUM, 2, 0);
                    gen_op_next(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATERT, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATERT, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATERT, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, GREATERT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATERT, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, GREATERT, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, GREATERT, T_NIL, 2);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATERT, T_STRING_EXP, 2, 0);
                    gen_op_first(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATERT, T_STRING_EXP, 2, 0);
                    gen_op_next(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_STRING_EXP ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, GREATERT, T_STRING_EXP, 1, 0);
                    gen_op_first(L, GREATERT);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, GREATERT, T_STRING_EXP, 2, 0);
                    gen_op_next(L, GREATERT);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else {
                *psa_state = SEM_TYPES_ERR;
                return;
            }
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen code
        }
        else if ( operation == T_LESS ) {
            redType = T_BOOLEAN;
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, LESST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESST);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                //L->First->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    //add function
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen DIV
                if ( *firstExp ) {
                    gen_op_first(L, LESST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESST);
                }
                if ( L->First->symbol.type == T_ID ) {
                    gen_pop( L->First->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                //L->First->rptr->rptr->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, LESST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESST);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, LESST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESST);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_STRING_EXP ) {
                if ( *firstExp ) {
                    gen_op_first(L, LESST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, LESST);
                }
            }
            //NIL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESST, T_INT_NUM, 2, 0);
                    gen_op_first(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESST, T_INT_NUM, 2, 0);
                    gen_op_next(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESST, T_INT_NUM, 1, 0);
                    gen_op_first(L, LESST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESST, T_INT_NUM, 2, 0);
                    gen_op_next(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESST, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESST, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESST, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, LESST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESST, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, LESST, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, LESST, T_NIL, 2);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESST, T_STRING_EXP, 2, 0);
                    gen_op_first(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESST, T_STRING_EXP, 2, 0);
                    gen_op_next(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_STRING_EXP ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, LESST, T_STRING_EXP, 1, 0);
                    gen_op_first(L, LESST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, LESST, T_STRING_EXP, 2, 0);
                    gen_op_next(L, LESST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else {
                *psa_state = SEM_TYPES_ERR;
                return;
            }
            stackPushRule( PAStack, L->First->symbol, redType );
            //gen code
        }
        //Equals
        else if ( operation == T_EQUALS ) {
            redType = T_BOOLEAN;
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, EQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, EQUALST);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                //L->First->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    //add function
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen DIV
                if ( *firstExp ) {
                    gen_op_first(L, EQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, EQUALST);
                }
                if ( L->First->symbol.type == T_ID ) {
                    gen_pop( L->First->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                //L->First->rptr->rptr->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, EQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, EQUALST);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, EQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, EQUALST);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_STRING_EXP ) {
                if ( *firstExp ) {
                    gen_op_first(L, EQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, EQUALST);
                }
            }
           //NILL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, EQUALST, T_INT_NUM, 2, 0);
                    gen_op_first(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, EQUALST, T_INT_NUM, 2, 0);
                    gen_op_next(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, EQUALST, T_INT_NUM, 1, 0);
                    gen_op_first(L, EQUALST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, EQUALST, T_INT_NUM, 2, 0);
                    gen_op_next(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, EQUALST, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, EQUALST, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, EQUALST, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, EQUALST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, EQUALST, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, EQUALST, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, EQUALST, T_NIL, 2);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, EQUALST, T_STRING_EXP, 2, 0);
                    gen_op_first(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, EQUALST, T_STRING_EXP, 2, 0);
                    gen_op_next(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_STRING_EXP ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, EQUALST, T_STRING_EXP, 1, 0);
                    gen_op_first(L, EQUALST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, EQUALST, T_STRING_EXP, 2, 0);
                    gen_op_next(L, EQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            //different data types
            else {
                redType = T_BOOLEAN;
                if ( *firstExp ) {
                    gen_op_first(L, EQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, EQUALST);
                }
            }
            stackPushRule( PAStack, L->First->symbol, redType );
        }
        //Not EQ
        else if ( operation == T_NOT_EQ ) {
            redType = T_BOOLEAN;
            if ( firstOperand == T_INT_NUM && secondOperand == T_INT_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, NEQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, NEQUALST);
                }
            }
            else if ( firstOperand == T_INT_NUM && secondOperand == T_DOUBLE_NUM ) {
                //L->First->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->symbol.type == T_ID ) {
                    gen_push( L->First->symbol.value.string );
                    gen_int_to_float_id( L->First->symbol.value.string, L->First->symbol.value.string );
                }
                //constant
                else {
                    //add function
                    gen_int_to_float_int( L->First->symbol.value.val_i, 1 );
                }
                //gen DIV
                if ( *firstExp ) {
                    gen_op_first(L, NEQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, NEQUALST);
                }
                if ( L->First->symbol.type == T_ID ) {
                    gen_pop( L->First->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_INT_NUM ) {
                //L->First->rptr->rptr->symbol.type = T_DOUBLE_NUM;
                //variable
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_push( L->First->rptr->rptr->symbol.value.string );
                    gen_int_to_float_id( L->First->rptr->rptr->symbol.value.string, L->First->rptr->rptr->symbol.value.string );
                }
                //constant
                else {
                    gen_int_to_float_int( L->First->rptr->rptr->symbol.value.val_i, 2 );
                }
                //gen MUL
                if ( *firstExp ) {
                    gen_op_first(L, NEQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, NEQUALST);
                }
                if ( L->First->rptr->rptr->symbol.type == T_ID ) {
                    gen_pop( L->First->rptr->rptr->symbol.value.string );
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_DOUBLE_NUM ) {
                if ( *firstExp ) {
                    gen_op_first(L, NEQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, NEQUALST);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_STRING_EXP ) {
                if ( *firstExp ) {
                    gen_op_first(L, NEQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, NEQUALST);
                }
            }
            //NILL
            else if ( firstOperand == T_INT_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, NEQUALST, T_INT_NUM, 2, 0);
                    gen_op_first(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, NEQUALST, T_INT_NUM, 2, 0);
                    gen_op_next(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_INT_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, NEQUALST, T_INT_NUM, 1, 0);
                    gen_op_first(L, NEQUALST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, NEQUALST, T_INT_NUM, 2, 0);
                    gen_op_next(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_DOUBLE_NUM && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, NEQUALST, T_DOUBLE_NUM, 2, 0);
                    gen_op_first(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, NEQUALST, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_DOUBLE_NUM ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, NEQUALST, T_DOUBLE_NUM, 1, 0);
                    gen_op_first(L, NEQUALST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, NEQUALST, T_DOUBLE_NUM, 2, 0);
                    gen_op_next(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_NIL ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_both(L, NEQUALST, T_NIL, 1);
                    *firstExp = false;
                }
                else {
                    gen_check_both(L, NEQUALST, T_NIL, 2);
                }
            }
            else if ( firstOperand == T_STRING_EXP && secondOperand == T_NIL ) {
                redType = secondOperand;
                if ( *firstExp ) {
                    gen_check_first(L, NEQUALST, T_STRING_EXP, 2, 0);
                    gen_op_first(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, NEQUALST, T_STRING_EXP, 2, 0);
                    gen_op_next(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            else if ( firstOperand == T_NIL && secondOperand == T_STRING_EXP ) {
                redType = firstOperand;
                if ( *firstExp ) {
                    gen_check_first(L, NEQUALST, T_STRING_EXP, 1, 0);
                    gen_op_first(L, NEQUALST);
                    gen_pop(L->First->symbol.value.string);
                    *firstExp = false;
                }
                else {
                    gen_check_first(L, NEQUALST, T_STRING_EXP, 2, 0);
                    gen_op_next(L, NEQUALST);
                    gen_pop(L->First->rptr->rptr->symbol.value.string);
                }
            }
            //different data types
            else {
                redType = T_BOOLEAN;
                if ( *firstExp ) {
                    gen_op_first(L, NEQUALST);
                    *firstExp = false;
                }
                else {
                    gen_op_next(L, NEQUALST);
                }
            }
            stackPushRule( PAStack, L->First->symbol, redType );
        }
        //brackets with expression inside
        else if ( L->First->symbol.type == T_BRCKT_L ) {
            redType = L->First->rptr->reduceType;
            if ( redType == T_INT_NUM || redType == T_DOUBLE_NUM || redType == T_STRING_EXP || redType == T_BOOLEAN ) {
                if ( L->First->rptr->rptr->symbol.type == T_BRCKT_R ) {
                    stackPushRule( PAStack, L->First->rptr->symbol, redType );
                }
                else {
                    *psa_state = SYNTAX_ERR;    //not right bracket
                    return;
                }
            }
            else {
                *psa_state = SYNTAX_ERR;    //not variable or constant
                return;
            }
        }
        else {
            *psa_state = SYNTAX_ERR;    //not exist rule
            return;
        }
    }
    return;
}

void reduce ( int* psa_state, tStack* PAStack, bool* firstExp, bool* alone ) {


    tSItemPtr stackItem = PAStack->top;
    tDLList rightSideList;  //list that contains right side of derivation rule
    int i = 0;  //size of list

    DLInitList( &rightSideList );

    //reduce all terms until first handle to the top of stack
    while ( !stackItem->handle ) {
        //insert term to the end of list
        DLInsertFirst( &rightSideList, stackItem->symbol, stackItem->reduceType, stackItem->temp );
        //move to next item in stack
        stackItem = stackItem->next;
        stackPop( PAStack );
        i++;
    }
    //remove handle tag
    stackItem->handle = false;

    //
    derivate( psa_state, i, &rightSideList, PAStack, firstExp, alone );
    //delete list
    DLDisposeList( &rightSideList );
    return;
}

void pushAndChange( tStack* PAStack, tSItemPtr* firstTerm, int* var_cnt, bool* alone ) {

    (*firstTerm)->handle = true;    //change first term to begin of handle
    stackPushTerm( PAStack, token );    //push token from input on PA stack

    MAKETOKEN;
    (*var_cnt)++;

    if ( (token.type == T_EOL || token.type == T_THEN || token.type == T_DO || x == -1 ) && *var_cnt == 1 ) {
        *alone = true;
    }

    return;
}

void pushAndChangePreviousToken( tStack* PAStack, tSItemPtr* firstTerm, T_struct* previousToken, int* var_cnt, bool* alone ) {

    (*firstTerm)->handle = true;    //change first term to begin of handle
    stackPushTerm( PAStack, *previousToken );    //push token from input on PA stack

    //MAKETOKEN;
    (*var_cnt)++;

    if ( (token.type == T_EOL || token.type == T_THEN || token.type == T_DO || x == -1 ) && *var_cnt == 1 ) {
        *alone = true;
    }

    return;
}

void pushNoChange( tStack* PAStack ) {

    stackPushTerm( PAStack, token );    //insert token from input on PA stack

    MAKETOKEN;
    return;
}

int getIndex ( T_type type ) {

    switch( type ) {
        case T_PLUS:
            return 0;
        case T_MINUS:
            return 1;
        case T_MUL:
            return 2;
        case T_DIV:
            return 3;
        case T_BRCKT_L:
            return 4;
        case T_BRCKT_R:
            return 5;
        case T_ID:
        case T_INT_NUM:
        case T_DOUBLE_NUM:
        case T_STRING_EXP:
            return 6;
        case T_GREATER:
        case T_GREATER_EQ:
        case T_LESS:
        case T_LESS_EQ:
            return 7;
        case T_EQUALS:
        case T_NOT_EQ:
            return 8;
        case T_EOL:
        case T_THEN:
        case T_DO:
        case T_SEMICOLON:
            return 9;
        default:
            return -1;
    }
}

void selectRule ( int* psa_state, tStack* PAStack, tSItemPtr* firstTerm, int* var_cnt, bool* firstExp, bool* alone ) {

    int i,j;    //indexes for selecting the rule from the precedence table

    i = getIndex( (*firstTerm)->symbol.type );    //get index for term closest to the top of stack
    if ( x == -1 )
        j = 9;
    else
        j = getIndex( token.type );    //get index for character on input

    if ( i == -1 || j == -1 ) {
        *psa_state = SYNTAX_ERR;
        return;
    }

    char rule = precedence_table[i][j];
    //select rule
    switch( rule ) {
        case '<':   //push character on stack
            pushAndChange( PAStack, firstTerm, var_cnt, alone );
            break;
        case '>':   //reduction
            reduce( psa_state, PAStack, firstExp, alone );
            break;
        case '=':   //push character on stack without change
            pushNoChange( PAStack );
            break;
        default:    //syntax error in input string
            *psa_state = SYNTAX_ERR;
            return;
    }
    return;
}

void selectRulePreviousToken ( int* psa_state, tStack* PAStack, tSItemPtr* firstTerm, T_struct* previousToken, int* var_cnt, bool* firstExp, bool* alone ) {

    int i,j;    //indexes for selecting the rule from the precedence table

    i = getIndex( (*firstTerm)->symbol.type );    //get index for term closest to the top of stack
    j = getIndex( previousToken->type );    //get index for character on input
    if ( i == -1 || j == -1 ) {
        *psa_state = SYNTAX_ERR;
        return;
    }

    char rule = precedence_table[i][j];
    //select rule
    switch( rule ) {
        case '<':   //push character on stack
            pushAndChangePreviousToken( PAStack, firstTerm, previousToken, var_cnt, alone );
            break;
        case '>':   //reduction
            reduce( psa_state, PAStack, firstExp, alone );
            break;
        case '=':   //push character on stack without change
            pushNoChange( PAStack );
            break;
        default:    //syntax error in input string
            *psa_state = SYNTAX_ERR;
            return;
    }
    return;
}

int precedenceAnalysis( T_struct* previousToken ) {

    int psa_state = EXIT_SUCCESS;  //return value of precedence analysis
    tStack PAStack; //stack for precedence analysis
    T_struct term;
    tSItemPtr firstTerm;    //term closest to the top of stack
    T_type actTokenType = token.type;

    //create stack
    stackInit( &PAStack );

    //insert EOL on top of stack
    term.type = T_EOL;
    stackPushTerm( &PAStack, term );

    //get first term
    firstTerm = topFirstTerm( &PAStack );
    int i = 0;
    int var_cnt = 0;
    bool firstExp = true;
    bool alone = false;

    //processing
    switch ( actTokenType ) { //token on input
        case T_IF:
        //case T_ELSEIF:
            inIf = true;
            MAKETOKEN;
            actTokenType = token.type;
            while ( firstTerm->symbol.type != T_EOL || actTokenType != T_THEN ) {
                selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                firstTerm = topFirstTerm( &PAStack );
                actTokenType = token.type;

                if ( x == -1 || actTokenType == T_EOL || actTokenType == T_DO || actTokenType == T_SEMICOLON )
                    psa_state = SYNTAX_ERR;

                if ( psa_state != EXIT_SUCCESS ) {
                    actTokenType = T_THEN;
                    firstTerm->symbol.type = T_EOL;
                }
            }
            break;

        case T_WHILE:
            inIf = true;
            MAKETOKEN;
            actTokenType = token.type;
            while ( firstTerm->symbol.type != T_EOL || actTokenType != T_DO ) {
                selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                firstTerm = topFirstTerm( &PAStack );
                actTokenType = token.type;

                if ( x == -1 || actTokenType == T_EOL || actTokenType == T_THEN || actTokenType == T_SEMICOLON )
                    psa_state = SYNTAX_ERR;

                if ( psa_state != EXIT_SUCCESS ) {
                    actTokenType = T_DO;
                    firstTerm->symbol.type = T_EOL;
                }
            }
            break;

        //assignment a = 5 + b
        case T_ASSIGNMENT:
            MAKETOKEN;
            actTokenType = token.type;
            while ( firstTerm->symbol.type != T_EOL || actTokenType != T_EOL ) {
                selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                firstTerm = topFirstTerm( &PAStack );
                actTokenType = token.type;

                if ( actTokenType == T_THEN || actTokenType == T_DO )
                    psa_state = SYNTAX_ERR;

                //semicolon, end of file or error detected
                if ( actTokenType == T_SEMICOLON || x == -1 || psa_state != EXIT_SUCCESS ) {
                    actTokenType = T_EOL;
                    firstTerm->symbol.type = T_EOL;
                }
            }
            break;

        //expression on line or constant after assignment
        case T_INT_NUM:
        case T_DOUBLE_NUM:
        case T_STRING_EXP:
            while ( firstTerm->symbol.type != T_EOL || actTokenType != T_EOL ) {
                selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                firstTerm = topFirstTerm( &PAStack );
                actTokenType = token.type;
                i++;

                if ( actTokenType == T_THEN || actTokenType == T_DO )
                    psa_state = SYNTAX_ERR;

                if ( x == -1 ) {
                    selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                    firstTerm = topFirstTerm( &PAStack );
                    if ( i > 1 ) {
                        actTokenType = token.type;
                        selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                        actTokenType = T_EOL;
                        firstTerm->symbol.type = T_EOL;
                    }
                    else 
                        actTokenType = T_EOL;
                }

                //semicolon, end of file or error detected
                if ( actTokenType == T_SEMICOLON || psa_state != EXIT_SUCCESS ) {
                    actTokenType = T_EOL;
                    firstTerm->symbol.type = T_EOL;
                }
            }
            break;

        //id first
        case T_PLUS:
        case T_MINUS:
        case T_MUL:
        case T_DIV:
        case T_GREATER:
        case T_GREATER_EQ:
        case T_LESS:
        case T_LESS_EQ:
        case T_EQUALS:
        case T_NOT_EQ:
            //load token from parameter
            selectRulePreviousToken( &psa_state, &PAStack, &firstTerm, previousToken, &var_cnt, &firstExp, &alone );
            firstTerm = topFirstTerm( &PAStack );
            actTokenType = token.type;

            while ( firstTerm->symbol.type != T_EOL || actTokenType != T_EOL ) {
                selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                firstTerm = topFirstTerm( &PAStack );
                actTokenType = token.type;

                if ( actTokenType == T_THEN || actTokenType == T_DO )
                    psa_state = SYNTAX_ERR;

                //semicolon, end of file or error detected
                if ( actTokenType == T_SEMICOLON || x == -1 || psa_state != EXIT_SUCCESS ) {
                    actTokenType = T_EOL;
                    firstTerm->symbol.type = T_EOL;
                }
            }
            break;
        case T_INPUTI: //<inputi>
        case T_INPUTF: //<inputf>
        case T_INPUTS: //<inputs>
        case T_LENGHT: //<length>
        case T_SUBSTR: //<substr>
        case T_PRINT: //<print>
        case T_ORD: //<ord>
        case T_CHR: //<chr>
           // built_in_fun(token.type);
            break;
        case T_ID:
            while ( firstTerm->symbol.type != T_EOL || actTokenType != T_EOL ) {
                selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                firstTerm = topFirstTerm( &PAStack );
                actTokenType = token.type;

                if ( x == -1 ) {
                    selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                    firstTerm = topFirstTerm( &PAStack );
                    if ( i > 1 ) {
                        actTokenType = token.type;
                        selectRule( &psa_state, &PAStack, &firstTerm, &var_cnt, &firstExp, &alone );
                        actTokenType = T_EOL;
                        firstTerm->symbol.type = T_EOL;
                    }
                    else 
                        actTokenType = T_EOL;
                }

                if ( actTokenType == T_THEN || actTokenType == T_DO )
                    psa_state = SYNTAX_ERR;

                //semicolon, end of file or error detected
                if ( actTokenType == T_SEMICOLON || psa_state != EXIT_SUCCESS ) {
                    actTokenType = T_EOL;
                    firstTerm->symbol.type = T_EOL;
                }
            }
            break;
        default:
            break;
    }
    //previousToken->type = PAStack.top->symbol.type;
    inIf = false;
    previousToken->type = PAStack.top->reduceType;
    stackDispose( &PAStack );
    return psa_state;
}
