#include "parser.h"


// NENUELOVY POCET PARAMETROV
// termy 3.1
// Jakub
int print_cmd(){
    int left = 0;
    if (brackets(LEFT, TRUE)){ MAKETOKEN; left = 1;}
    int typeik;
    while (TType != T_COMMA) {
        switch(TType){
            case T_ID:
                if (dim_precedence(&typeik,token.value.string)){}
                else {
                    pState(SEMANTICS_ERR); //$#$ spravna navratova hodnbota<<<<<???                    
                    return p_state;
                    break;
                }
                gen_print_id(token.value.string);
                break;
            case T_INT_NUM: 
                gen_print_int(token.value.val_i);
                break;
            case T_DOUBLE_NUM:
                gen_print_float(token.value.val_d);
                break;
            case T_STRING_EXP:     
                gen_print_str(token.value.string);
            //case T_NIL:            
                break;
            case T_NIL:
                gen_print_str("");
                break;
            default:
                pState(SYNTAX_ERR);
                return p_state;
                break;
        }
        MAKETOKEN;
        if (x == -1 && left == 0)
        {
            return p_state;
        }
        else if (x == -1 && left == 1)
        {
            pState(SYNTAX_ERR);
            return p_state;
        }
        if (TType == T_BRCKT_R && left == 1)
        {
            MAKETOKEN;
            if (x == -1)
            {
                return p_state;
            }
            compareTokenX(T_EOL);
            return p_state;
        }
        else if (TType == T_BRCKT_R && left == 0){
            pState(SYNTAX_ERR);
        }
        if (TType == T_EOL && left == 0) { break; }

        
        if (brackets(RIGHT, FALSE)){ MAKETOKEN; break;}     
        compareToken(T_COMMA);
        CHECKSTATE;
    }
    compareTokenX(T_EOL);

    return p_state;
}
