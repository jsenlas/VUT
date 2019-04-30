/* Toto je druhy pokus o parser 
 * Start 08 10 2018
 * God give me strength!
 */

#include "parser.h"

int prechod = 0;
int inc_cmd = 0;

/*--------- All the necessary helping functions ---------*/

/* const char *getTokenName(int t)
    ** Preklada cislo tokenu "t" na nazov tokenu
    ** pouzivam pri vypise chyby, aby som vedel,
    ** kde pocas spracovavania kodu vznikla chyba
    ** vrati NULL v pripade chyby
    */
const char *getTokenName(int t){
    switch (t){
        case T_DEF:     return "T_DEF"; 
        case T_DO:         return "T_DO"; 
        case T_ELSE:     return "T_ELSE"; 
        case T_END:     return "T_END"; 
        case T_IF:         return "T_IF"; 
        case T_NOT:     return "T_NOT"; 
        case T_NIL:     return "T_NIL"; 
        case T_THEN:     return "T_THEN"; 
        case T_WHILE:     return "T_WHILE"; 
        case T_INPUTS:     return "T_INPUTS"; 
        case T_INPUTI:     return "T_INPUTI"; 
        case T_INPUTF:     return "T_INPUTF"; 
        case T_PRINT:     return "T_PRINT"; 
        case T_LENGHT:     return "T_LENGHT"; 
        case T_SUBSTR:     return "T_SUBSTR"; 
        case T_ORD:     return "T_ORD"; 
        case T_INTEGER: return "T_INTEGER"; 
        case T_STRING:     return "T_STRING"; 
        case T_DOUBLE:     return "T_DOUBLE"; 
        case T_EOL:     return "T_EOL"; 
        case T_ID:         return "T_ID"; 
        case T_INT_NUM:     return "T_INT_NUM"; 
        case T_DOUBLE_NUM:     return "T_DOUBLE_NUM"; 
        case T_STRING_EXP:     return "T_STRING_EXP"; 
        case T_BRCKT_L:     return "T_BRCKT_L"; 
        case T_BRCKT_R:     return "T_BRCKT_R"; 
        case T_COMMA:         return "T_COMMA"; 
        case T_SEMICOLON:     return "T_SEMICOLON";  
        case T_LESS_EQ:     return "T_LESS_EQ"; 
        case T_NOT_EQ:         return "T_NOT_EQ"; 
        case T_LESS:         return "T_LESS"; 
        case T_GREATER:     return "T_GREATER"; 
        case T_GREATER_EQ:     return "T_GREATER_EQ"; 
        case T_ASSIGNMENT:     return "T_ASSIGNMENT"; 
        case T_EQUALS:     return "T_EQUALS"; 
        case T_PLUS:     return "T_PLUS"; 
        case T_MINUS:     return "T_MINUS"; 
        case T_MUL:     return "T_MUL"; 
        case T_DIV:     return "T_DIV";
    }
    return NULL;
}

/* void testing(int expectation, int reality)
    ** Vypisuje typ tokenu
    ** expectation - ocakavany vstup sem ide "t"
    ** reality - token, ktory som dostal od LEX "token.type"
    ** bezne pouzitie testing(t, TType);
    */
void testing(int expectation, int reality){
   // printf("%s vs. %s\n",         getTokenName(expectation),         getTokenName(reality));
}//

/* Kontroluje aktualny stav parseru 
**pouziva sa v makre CHECKSTATE - podla toho vracia hodnoty */
int check_p_state(){
    if (p_state == EXIT_SUCCESS){
        return FALSE; // makro =0
    }
    return TRUE; // makro =1
}

/* nastavuje navratovu hodnotu programu.*/
void pState(int state){    
    if (p_state == EXIT_SUCCESS && state != EXIT_SUCCESS && state != -1)
    {       
        p_state = state;
    }
}
/* Porovnava tokeny a nacitava dalsi
** int t - typ ocakavaneho tokenu */
void compareToken(int t){
    testing(t, TType);
    if (token.type == t) {
        MAKETOKEN;
    }
    else {
        pState(SYNTAX_ERR);
        //exit(p_state);
    }
}

void compareTokenX(int t){
    testing(t, TType);
    if (token.type == t) {}
    else {
        pState(SYNTAX_ERR);
        //exit(p_state);
    }
}

/*--------------- Derivation syntax tree ----------------*/
int call_function(){
    brckt_cnt = 0;
    if (TType != T_ID && TType != T_FUNCTION && (TType < 9 && TType > 16)) {
        pState(SYNTAX_ERR); // spravne??
        return p_state;
    }
    gen_fce_call_init();

    char name_f[100]; // vynulovat
    // Save ID
    strcpy(function_name, token.value.string);
    strcpy(name_f, token.value.string); 
    tHTItem* pointer = htSearch (Globalsymtab_pointer,function_name);
    int param_count = pointer->data.dFunction.number_of_parametrs;
    t_param *param_pointer = pointer->data.dFunction.params;
        //printf("\n\npocet parametru funkce %s je %d\n\n",function_name,param_count);
    
    t_param  param[param_count];
    for(int i = 0; i < param_count; i++)
    {
        param[i].type = param_pointer->type;
        strcpy(param[i].name, param_pointer->name);
        param_pointer = param_pointer->next_param;
    }
    //int typeik = 0;
    int param_counter = 0;
    if (brackets(LEFT, TRUE)){ 
        MAKETOKEN; CHECKSTATE;
        if (x == -1) {
            gen_fce_call(name_f);
            return p_state;
        }; 
    }

    int typeik;
    if (TType != T_EOL) {
        while (TType != T_COMMA) {

            param_counter++;
            switch(TType){
                case T_ID:    
                    strcpy(nf2,function_name);
                    strcpy(function_name, nf);
                    if (dim_precedence(&typeik,token.value.string)){strcpy(function_name, nf2);
                            Global_param.type=typeik;
                    }
                    else {
                        pState(SEMANTICS_ERR); //$#$ spravna navratova hodnbota<<<<<???
                        strcpy(function_name, nf2);
                        break;
                    }
    
                    break;
                case T_INT_NUM:
                    Global_param.type = T_INTEGER;
                    break;
                case T_DOUBLE_NUM:
                    Global_param.type = T_DOUBLE;
                    break;
                case T_STRING_EXP:
                    Global_param.type = T_STRING;
                    break;
                default:
                    pState(SYNTAX_ERR);
                    return p_state;
                    break;
            }
            // -1 preto6e som hodnotu už inkrementoval
            //$#$ /*potrebujem nazov i-tej premennej*/
            
            if (param_counter > param_count)
            {
                pState(SEM_CALL_FUN);
                return p_state;
            }

            //for(int xdd = 0;xdd<param_counter;xdd++)
            //{
            //    //printf("\n\nlalayurtfuit  %d\n", param_counter);
            //    if(xdd == param_counter-1){
            //        if(Global_param.type == param[xdd].type)
            //        {}
            //        else{                        
            //            pState(SEM_TYPES_ERR);
            //            return p_state;
            //        }
            //    }                       
            //}

            switch(TType){
                case T_ID:    
                    gen_assign_id(token.value.string);
                    break;
                case T_INT_NUM: 
                    gen_assign_int(token.value.val_i);
                    break;
                case T_DOUBLE_NUM:
                    gen_assign_float(token.value.val_d);
                    break;
                case T_STRING_EXP:
                    gen_assign_string(token.value.string);
                    break;
                //case T_NIL: printf("PARAM %s\n", token.value.string);                
                default:    
                    break;       
            }

            gen_fce_params(param[param_counter-1].name);

            testing(TType, TType);
            
            MAKETOKEN; CHECKSTATE;                
            if (x == -1) { 
                if (brckt_cnt != 0) { pState(SYNTAX_ERR); return p_state; }
                break; 
            }
            if (TType == T_BRCKT_R) {
                brckt_cnt--;
                MAKETOKEN; CHECKSTATE;
                //if (brackets(RIGHT, FALSE)){ MAKETOKEN; CHECKSTATE;}
                if (brckt_cnt != 0) { pState(SYNTAX_ERR); return p_state; }
                if (x == -1) { break; }
                compareTokenX(T_EOL);                
                gen_fce_call(name_f);          
                return p_state;
            }
            //semicolon
            if (TType == T_EOL) { break; }
    
            
            if (brackets(RIGHT, FALSE)){ MAKETOKEN; CHECKSTATE; CHECKEOF; break;}
            compareToken(T_COMMA);
            if (x == -1) { break; }
            CHECKSTATE;
        }
    }

    if(param_counter != param_count)// && ret_val == 0)
    {
        pState(SEM_CALL_FUN);
        return p_state;
    }

    gen_fce_call(name_f);
    CHECKEOF;
    if (brackets(RIGHT, FALSE)){ MAKETOKEN; CHECKSTATE;}
    compareTokenX(T_EOL);
    if (brckt_cnt != 0) { pState(SYNTAX_ERR); }
    
    return p_state;
}

int id_cmd(){

    T_struct id;
    id.value.string = (char *) malloc(strlen(token.value.string)*sizeof(char)+1);
    id.type = token.type;
    strcpy(id.value.string, token.value.string);
    //printf("\n\n token.value.string  %s\n\n", token.value.string);
    strcpy(nf, function_name); 
    if (checking_existing_function(token.value.string) == true){
        strcpy(function_name, token.value.string);
        pState(call_function());
        CHECKSTATE;
        strcpy(function_name, nf);
        free(id.value.string);
    }
    else {  // variable ID
        int type;
        if (dim_precedence(&type, token.value.string) == true)
        {
            //memcpy(id, &token, sizeof(T_struct));
            MAKETOKEN; CHECKSTATE;// nevidime efekt
            switch(TType){
                case T_EOL:
                    free(id.value.string);
                    return p_state;
                case T_ASSIGNMENT: // priradenie
                    /*ak premennu poznam, tak sa jej typ moze casom zmenit, tuto zmenu je potrebne ulozit do tabulky symbolov*/
                    MAKETOKEN; CHECKSTATE;
                    strcpy(nf, function_name);

                    if (TType == T_EOL)
                    {
                        pState(SYNTAX_ERR);
                        return p_state;
                    }
                    else if (TType == T_INT_NUM || TType == T_DOUBLE_NUM || TType == T_STRING_EXP || TType == T_BRCKT_L)
                    {
                        pState(precedenceAnalysis(&id)); // $#$ 
                        loading_var_name(id);
                        loading_var_type(id);//*prevtoken);
                        pState(save_var_to_correct_local_table());
                        CHECKSTATE;
                        gen_assign_exp_ret(id.value.string);
                        // vratit na spravnu funkciu???
                    }
                    else if ( (TType == T_ID || TType == T_FUNCTION || (TType > 8 && TType < 17)) 
                            &&(token.value.string != NULL && checking_existing_function(token.value.string) == true)){
                        strcpy(function_name, token.value.string);
                        pState(call_function());
        
                        gen_assign_ret(id.value.string);
        
                        CHECKSTATE;
                        strcpy(function_name, nf);
                    }
                    else {                        
                        CHECKSTATE;
                        pState(precedenceAnalysis(&id)); //$#$ nepotrebuje ten ukazatel
                        //printf("\n\n id->value.string  %s\n\n", id->value.string);                        
                        loading_var_name(id);
                        loading_var_type(id);    
                        pState(save_var_to_correct_local_table(&id));
                        //print_local_symtable_by_name(function_name);
                        CHECKSTATE;                                    

                        gen_assign_exp_ret(id.value.string);
                    }
                    break;
                case T_PLUS: /// vyraz na riadku
                case T_MINUS:
                case T_MUL:
                case T_DIV:
                case T_GREATER:
                case T_GREATER_EQ:
                case T_LESS:
                case T_LESS_EQ:
                //case T_EQUALS:
                case T_NOT_EQ:
                case T_NOT:
                    pState(precedenceAnalysis(&id));// tu ho veľmo potrebuje
                    CHECKSTATE;                
                    break;
                default:
                    pState(SYNTAX_ERR);
                    free(id.value.string);
                    return p_state;
            }

            //pState(precedenceAnalysis(prevtoken));
            //CHECKSTATE; 
            free(id.value.string);     
            return p_state;      
        }
        else {
            MAKETOKEN; CHECKSTATE;
            testing(TType, TType);
            switch (TType){
                case T_ASSIGNMENT:
                    gen_def_var(id.value.string);
                    
                    loading_var_name(id);
                    id.type = T_NIL;
                    loading_var_type(id);
                    pState(save_var_to_correct_local_table());
                    MAKETOKEN; CHECKSTATE;
                    strcpy(nf, function_name);

                    if (TType == T_EOL)
                    {
                        pState(SYNTAX_ERR);
                        return p_state;
                    }

                    else if (TType == T_INT_NUM || TType == T_DOUBLE_NUM || TType == T_STRING_EXP || TType == T_BRCKT_L)
                    {
                        pState(precedenceAnalysis(&id)); // $#$ nepotrebuje
                        CHECKSTATE;

                        loading_var_name(id);
                        loading_var_type(id);
                        //printf("%d\n", id->type);
                        pState(save_var_to_correct_local_table());
                        gen_assign_exp_ret(global_var.name);
                    }
                    else if (TType > 8 && TType < 17)
                    {                        
                        strcpy(function_name, token.value.string);
                        pState(call_function());
        
                        gen_assign_ret(id.value.string);
        
                        CHECKSTATE;
                        strcpy(function_name, nf);
                    }
                    else if ((TType == T_ID || TType == T_FUNCTION) 
                            && (token.value.string != NULL && checking_existing_function(token.value.string) == true)) 
                    {
                        strcpy(function_name, token.value.string);
                        pState(call_function());
        
                        gen_assign_ret(id.value.string);
        
                        CHECKSTATE;
                        strcpy(function_name, nf);
                    }
                    else {                        
                        CHECKSTATE;
                        //loading_var_name(id);
                        //loading_var_type(id);
                        //pState(save_var_to_correct_local_table());
                        pState(precedenceAnalysis(&id));
                        
                        loading_var_name(id);
                        loading_var_type(id);
                        pState(save_var_to_correct_local_table());
                        //print_local_symtable_by_name(function_name);
                        CHECKSTATE;

                        gen_assign_exp_ret(id.value.string);
                    }
                    break;
                default:
                    pState(SYNTAX_ERR);
                    free(id.value.string);
                    return p_state;
            }//switch
            free(id.value.string);
            return p_state;
        }                
    }
    return p_state;
}

int while_cmd(){
    iminwhile++;

    gen_while();

    //precedence
    pState(precedenceAnalysis(prevtoken));
    if (check_p_state()){
        return p_state;
    }

    gen_while_post();

    MAKETOKEN; CHECKSTATE;
    compareTokenX(T_EOL);

    if (def_cmds() != EXIT_SUCCESS){
        return p_state;
    }

    //MAKETOKEN; CHECKSTATE;
    testing(TType, TType);
    compareToken(T_END);
    iminwhile--;
    gen_loop();
    if (x == -1) {
        return p_state;
    }
    compareTokenX(T_EOL);


    return p_state;
}

int if_cmd(){
    iminif++;
    ifindex = 0;
    /* Potrebujem tabulku symbolov na if-y globalna, 
    ** ktora si pamata cislo a string. Cislo zanorenia v IFe a string je nazov premennej.    
    */
    //precedence
    pState(precedenceAnalysis(prevtoken));
    if (check_p_state()){
        return p_state;
    }
    
    compareToken(T_THEN);
    gen_then_eq();
    compareTokenX(T_EOL);

    if (def_cmds() != EXIT_SUCCESS){
        return p_state;
    }

    //ELSIF

    //MAKETOKEN;
    compareToken(T_ELSE);
    gen_else();
    compareTokenX(T_EOL);

    if (def_cmds() != EXIT_SUCCESS){
        return p_state;
    }

    //MAKETOKEN;
    compareToken(T_END);
    iminif--;    
    gen_end_if();
    if (x == -1) {
        return p_state;
    }
    compareTokenX(T_EOL);

    return p_state;
}

int fundef_cmd(){
    defining_fun = TRUE;
    MAKETOKEN; CHECKSTATE;
    testing(TType, TType);

    if (TType != T_ID && TType != T_FUNCTION)
    {
        pState(SYNTAX_ERR);// navratova hodnota???
        return p_state;
    }

    strcpy(function_name, token.value.string);
    pState(checking_definition_function(function_name));
    CHECKSTATE;
        
    param_init();    

    gen_fce_def(function_name);

    MAKETOKEN; CHECKSTATE;
    switch (TType){
        case T_BRCKT_L:
            MAKETOKEN; CHECKSTATE;
            switch (TType){
                case T_BRCKT_R:
                    MAKETOKEN; CHECKSTATE;
                    compareTokenX(T_EOL);
                    break;
                case T_ID:
                    //loading_param_name(token);
                    //loading_param_type(t_nil);
                    //pState(loading_param_to_global_symtable());
                    CHECKSTATE;
                    MAKETOKEN; CHECKSTATE;
                    if (TType == T_BRCKT_R)
                    {
                        MAKETOKEN; CHECKSTATE;
                        compareTokenX(T_EOL);
                        //return p_state;
                        break;
                    }                    
                    while (TType == T_COMMA){
                        
                        //testing(T_DIV, TType);
                        MAKETOKEN; CHECKSTATE;

                        //loading_param_name(token);
                        //loading_param_type(t_nil);
                        //pState(loading_param_to_global_symtable());
                        compareToken(T_ID);
                        if (TType == T_BRCKT_R){
                            break;
                        }
                    }
                    MAKETOKEN; CHECKSTATE;
                    compareTokenX(T_EOL);
                    //return p_state;
                    break;
                default:
                    pState(SYNTAX_ERR);
                    return p_state;
            }
            break;
        case T_ID:
            //loading_param_name(token);
            //loading_param_type(t_nil);
            //pState(loading_param_to_global_symtable());
            CHECKSTATE;

            MAKETOKEN; CHECKSTATE;
            while (TType == T_COMMA){                
                MAKETOKEN; CHECKSTATE;
                // moze prist aj hodnota $#$
                //loading_param_name(token);
                //loading_param_type(t_nil);
                //pState(loading_param_to_global_symtable());
                CHECKSTATE;

                compareToken(T_ID);
                if (TType == T_EOL) { break; }
            }
            if (TType != T_EOL) { pState(SYNTAX_ERR); }
            break;
        default:    
            pState(SYNTAX_ERR);
            return p_state;
            break;
    }//switch
    initLocallTable(Globalsymtab_pointer,function_name);
    if (def_cmds() != EXIT_SUCCESS){
        return p_state;
    }

    //MAKETOKEN;
    defining_fun = FALSE;

    gen_fce_ret(function_name);
    
    strcpy(function_name, "scope");
    compareToken(T_END);
    if (x == -1) {
        return p_state;
    }
    compareTokenX(T_EOL);

    return p_state;
}

/* Pomocna funkcia, ktora nacita zatvorku a dalsi parameter
** alebo nacita dalsi token
** int side = LEFT | RIGHT - zalezi na zatvorke
** int condition = TRUE | FALSE 
**     - mam nacitat dalsi token??? {podla kontextu} */
int brackets(int side, int condition){    
    if (condition) { MAKETOKEN; CHECKSTATE; if (x == -1){return TRUE;} }
    testing(TType, TType);
    switch (side){
        case LEFT:
            if (TType == T_BRCKT_L)
            {
                brckt_cnt++;
                isBracket = TRUE;
                return TRUE;
            }
            return FALSE;
        case RIGHT:
            if (TType == T_BRCKT_R)
            {
                brckt_cnt--;
                isBracket = FALSE;
                return TRUE;
            }
            return FALSE;
    }
    return FALSE;
}

int print_parameter(){

    return p_state;
}

/* Decides which command is used
**
*/
int cmd(){
    if (lala)
    {
        MAKETOKENFIRST;
        lala = 0;
    }
    else
        MAKETOKEN; CHECKSTATE;  
    
    testing(TType, TType);
    if (x == -1)
    {
        return p_state;
    }

    switch (TType){
        case T_EOL:
            break;
        case T_ELSE:
            if (iminif == 0){
                pState(SYNTAX_ERR);
            }
            return p_state;
            break;
        case T_PRINT:
            pState(print_cmd());
            break;
        case T_INPUTS:
        case T_INPUTI:
        case T_INPUTF:
        case T_LENGHT:
        case T_SUBSTR:
        case T_ORD:
        case T_CHR:            
        case T_FUNCTION:
            pState(call_function());
            CHECKSTATE;
            break;
        case T_ID:
            pState(id_cmd());
            break;
        case T_STRING_EXP:
        case T_DOUBLE_NUM:
        case T_INT_NUM:
            //precedence
            pState(precedenceAnalysis(prevtoken));
            CHECKSTATE;
            break;        
        case T_IF:            
            pState(if_cmd());
            break;
        case T_WHILE:
            pState(while_cmd());
            break;
        case T_DEF:
            if (iminif == 0 || iminwhile == 0)
            {
                pState(fundef_cmd(prechod));   
            }
            else{
                pState(SYNTAX_ERR);
                return p_state;
            }
            break;
        case T_END:            
            return p_state;            
        default:
            if (x == -1){
                exit(p_state);
            }
            pState(SYNTAX_ERR);
            return p_state;
    }    
    return p_state;
}

int def_cmds(){
    while (1){        
        // Detekujem chybu alebo koniec suboru x == -1 == EOF
        if ((p_state != EXIT_SUCCESS) || (x == -1))
        {
            return p_state;
        }            
        cmd();  // spracovanie jedného príkazu
        if (TType == T_END || TType == T_ELSE)
        {
            return p_state;
        }        
    }
    return p_state;
}

void gen_builtin(){
    gen_fce_lenght();
    gen_fce_chr();
    gen_fce_ord();
    gen_fce_inputs();
    gen_fce_inputi();
    gen_fce_inputf();    
}

int program(){
    gen_init();
    gen_builtin();
    iminif = 0;
    iminwhile = 0;
    ifindex = 0;
    // telo suboru {MAINvRUBY} sa bude spravat ako scope
    // bude potrebne, aby sa po spracovani funkcie znovu nastavilo
    // *function_name* na "scope"
    strcpy(function_name,"scope");
    if (init_built_in_fun())
    {   // doplnnit vlozenie definicii vstavanych funkcii
        return p_state; // $#$ zmenit na spravny navratovy kod
    }
    if (def_cmds()){
        return p_state; // 
    }

    return p_state;
}

int init_built_in_fun(){

    // Generovanie built in funkcii
    // vkladanie do tabulky symbolov
    scope_init();
    hard_definition_lenght();
    hard_definition_substr();
    hard_definition_inputs();
    hard_definition_inputi();
    hard_definition_inputf();
    hard_definition_ord();
    hard_definition_chr();
        //strcpy(function_name,"length");
        //print_local_symtable_by_name(function_name);
        //strcpy(function_name,"substr");
        //print_local_symtable_by_name(function_name);
        //strcpy(function_name,"inputs");
        //print_local_symtable_by_name(function_name);
        //strcpy(function_name,"inputi");
        //print_local_symtable_by_name(function_name);
        //strcpy(function_name,"inputf");
        //print_local_symtable_by_name(function_name);
        //strcpy(function_name,"ord");
        //print_local_symtable_by_name(function_name);
        //strcpy(function_name,"chr");
        //print_local_symtable_by_name(function_name);    

    return EXIT_SUCCESS;
}

/* It all began here...
*/
int parser_main(){
    x = 0;
    lala = 1;
    last_Ttoken = T_EOL;
    defining_fun = FALSE;
    p_state = EXIT_SUCCESS;
    commaF = FALSE;            

    //Globalsymtab_pointer = &Globalsymtab; initGlobalTable(Globalsymtab_pointer);

    //strcpy(function_name, "fun1");    
    //tHTItem* pointer = htSearch (Globalsymtab_pointer,function_name);
    //int param_count = pointer->data.dFunction.number_of_parametrs;
    //t_param *param_pointer = pointer->data.dFunction.params;
        //printf("\n\npocet parametru funkce %s je %d\n\n",function_name,param_count);
    
    //t_param  param[param_count];
    //for(int i = 0; i < param_count; i++)
    //{
    //    param[i].type = param_pointer->type;
    //    strcpy(param[i].name, param_pointer->name);
    //    param_pointer = param_pointer->next_param;
    //}


    program();    
    
    
    
    return p_state; // THE END!!!
}


int parser_first(){
    p_state = EXIT_SUCCESS;
    lala = 1;
    prevtoken = &prev_real;
    Globalsymtab_pointer = &Globalsymtab;
    initGlobalTable(Globalsymtab_pointer);
    param_init();
    while (1){
        if (lala)
        {
            MAKETOKENFIRST;
            lala = 0;
        }
        else
            MAKETOKEN; CHECKSTATE; 

        CHECKSTATE;
        if (x == -1) break;//return p_state;

        if (TType == T_DEF){            
            MAKETOKEN; CHECKSTATE;
            testing(TType, TType);
            
            if (TType != T_ID && TType != T_FUNCTION)
            {
                pState(SYNTAX_ERR);// navratova hodnota???
                return p_state;
            }

            strcpy(function_name, token.value.string);
            p_state = checking_declare_function(function_name);                
            
            param_init();    
            
            // gen_fce_def(function_name);
            
            MAKETOKEN; CHECKSTATE;                   

            // ulozit do tabulky symbolov novy zaznam o funkcii
            
            switch (TType){
                case T_BRCKT_L:
                    MAKETOKEN;
                    switch (TType){
                        case T_BRCKT_R:
                            MAKETOKEN; CHECKSTATE;
                            compareTokenX(T_EOL);
                            //return p_state;
                            break;
                        case T_ID:
                            loading_param_name(token);
                            loading_param_type(t_nil);
                            pState(loading_param_to_global_symtable());
                            MAKETOKEN; CHECKSTATE;
                            if (TType == T_BRCKT_R)
                            {
                                MAKETOKEN;
                                CHECKSTATE;
                                compareTokenX(T_EOL);
                                //return p_state;
                                break;
                            }                    
                            while (TType == T_COMMA){
                                
                                //testing(T_DIV, TType);
                                MAKETOKEN;CHECKSTATE;
        
                                loading_param_name(token);
                                loading_param_type(t_nil);
                                pState(loading_param_to_global_symtable());
                                compareToken(T_ID);
                                if (TType == T_BRCKT_R){
                                    break;
                                }
                            }
                            MAKETOKEN;CHECKSTATE;
                            compareTokenX(T_EOL);
                            //return p_state;
                            break;
                        default:
                            pState(SYNTAX_ERR);
                            return p_state;
                    }
                    break;
                case T_ID:
                    loading_param_name(token);
                    loading_param_type(t_nil);
                    pState(loading_param_to_global_symtable());
                    CHECKSTATE;
                    MAKETOKEN;
                    CHECKSTATE;
                    while (TType == T_COMMA){
                        //testing(T_DIV, TType);
                        MAKETOKEN;CHECKSTATE;
                        // moze prist aj hodnota $#$                        
                        loading_param_name(token);
                        loading_param_type(t_nil);
                        pState(loading_param_to_global_symtable());
                        CHECKSTATE;

                        compareToken(T_ID);
                        if (TType == T_EOL) { break; }
                    }
                    if (TType != T_EOL) { pState(SYNTAX_ERR); }
                    break;
                default:
                    return p_state;
                    break;
            }// switch
            initLocallTable(Globalsymtab_pointer,function_name);
            defining_fun = FALSE;
        }      
    }
    return p_state;
}
/*   |^| When you hit rock bottom, the only way to go is up |^|   */