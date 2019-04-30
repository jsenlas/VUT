//
// Created by Marek on 24. 9. 2018.
//

#include "scanner.h"

// table of keywords
char *keyWTab[] = {"def",
                   "do",
                   "else",
                   "end",
                   "if",
                   "not",
                   "nil",
                   "then",
                   "while",
                   "inputs",
                   "inputi",
                   "inputf",
                   "print",
                   "lenght",
                   "substr",
                   "ord",
                   "chr",
                   "integer",
                   "string",
                   "float"
};

// FLAGS
bool token_alloc = false;
bool white_space = false;
int test_cnt = 0;

FILE *src_file;

// initialize file pointer globally, allocate memory for buffer "string"
int scannerInit(FILE *fp) {
    str = &buffer;
    src_file = fp;
    last_Ttoken = T_EOL;
    white_space = false;

    if ((str->string = (char *) malloc(MEM_STR)) == NULL)
        return INTERNAL_ERR;

    str->alloc_mem = MEM_STR;
    str->string[0] = '\0';
    str->length = 0;
    return EXIT_SUCCESS;
}

// add char to buffer and increase allocated memory
int addNextChar(T_string *str, char in_char) {
    if (str->length + 1 >= str->alloc_mem) {
        if ((str->string = (char *) realloc(str->string, str->length + MEM_STR)) == NULL)
            return INTERNAL_ERR;
        str->alloc_mem = str->length + MEM_STR;
    }
    str->string[str->length] = in_char;
    str->length++;

    str->string[str->length] = '\0';
    return EXIT_SUCCESS;
}

// clean buffer
void strReset(T_string *str) {
    str->string[0] = '\0';
    str->length = 0;
}

// free token string
void tokenFree() {
    free(token.value.string);
    token.value.string = NULL;
    token_alloc = false;
}

// free buffer string
void strFree(T_string *str) {
    free(str->string);
    str->string = NULL;
}

// add char in escape seq. form to buffer and allocate more memory
int addEscSeq(T_string *str, char in_char, char* str_t, int option) {
    if (str->length + 4 >= str->alloc_mem) {
        if ((str->string = (char *) realloc(str->string, str->length + MEM_STR)) == NULL)
            return INTERNAL_ERR;
        str->alloc_mem = str->length + MEM_STR;
    }
    str->string[str->length] = in_char;
    str->length++;

    if (option == HIGH_ASCII) {
		str->string[str->length] = str_t[0];
		str->length++;
		str->string[str->length] = str_t[1];
		str->length++;
		str->string[str->length] = str_t[2];
		str->length++;
	}
	
	if (option == MID_ASCII) {
		str->string[str->length] = '0';
		str->length++;
		str->string[str->length] = str_t[0];
		str->length++;
		str->string[str->length] = str_t[1];
		str->length++;
	}
	
	if (option == LOW_ASCII) {
		str->string[str->length] = '0';
		str->length++;
		str->string[str->length] = '0';
		str->length++;
		str->string[str->length] = str_t[0];
		str->length++;
	}
	
    str->string[str->length] = '\0';
    return EXIT_SUCCESS;
}


// transform char to escape seq. form and add to buffer
void escSeq(int in_char) {
    int option;
	char str_t[3];
    sprintf(str_t, "%d", in_char);
    in_char = '\\';
    // if ascii value is < 10
    if (str_t[1] == '\0') {
		option = LOW_ASCII;
	}
	// if ascii value is > 9 && < 100
	else if (str_t[2] == '\0') {
		option = MID_ASCII;
	}
	// if ascii value is > 99
	else {
		option = HIGH_ASCII;
	}   
    addEscSeq(str, in_char, str_t, option);
}

int hexToDec(char hexNum[]) 
{    
    int len = 2;
    int base = 1; 
    int decNum = 0;
      
    // Extracting characters as digits from last character 
    for (int i = len; i >= 0; i--) {    
        if (hexNum[i] >= '0' && hexNum[i] <= '9') { 
            decNum += (hexNum[i] - 48) * base;       
            // incrementing base
            base = base * 16;
        } 
        else if (hexNum[i] >= 'A' && hexNum[i] <= 'F') { 
            decNum += (hexNum[i] - 55) * base; 
            base = base * 16; 
        } 
    } 
    return decNum;
}

// returns exact type of token
int getToken() {
    bool zero_first = false;
    fsm_state state = START;
    int in_char;

    do {
        in_char = fgetc(src_file);

        ///TEST
        if (TEST_FLAG) {
            test_cnt++;
            printf("%d.|%d|%s\n", test_cnt, in_char, str->string);
            printf("LAST_TOKEN: %d\n", last_Ttoken);
            printf("STATE: %d\n", state);
        }

        switch (state) {
            case START:
                // end of file
                if (feof(src_file))
                    return EOF;
                    // end of the line
                else if (in_char == '\n') {
                    white_space = false;
                    RET(last_Ttoken, T_EOL);
                }
                    // unprintable character, nothing to care about
                else if (isspace(in_char)) {
                    white_space = true;
                    break;
                }
                    // comment till the end of the line
                else if (in_char == '#')
                    state = LINE_COMM;
                    // div or block comment
                else if (in_char == '/') {
                    RET(last_Ttoken, T_DIV);
                }
                else if (in_char == '!')
                    state = NOT_EQ;
                    // compare operator
                else if (in_char == '<')
                    state = LESS;
                else if (in_char == '>')
                    state = GREATER;
                    // string
                else if (in_char == '\"')
                    state = STRING;
                    // brackets
                else if (in_char == '(') {
                    RET(last_Ttoken, T_BRCKT_L);
                }
                else if (in_char == ')') {
                    RET(last_Ttoken, T_BRCKT_R);
                }
                    // comma
                else if (in_char == ',') {
                    RET(last_Ttoken, T_COMMA);
                }
                    // operators
                else if (in_char == '+') {
                    RET(last_Ttoken, T_PLUS);
                }
                else if (in_char == '-') {
                    RET(last_Ttoken, T_MINUS);
                }
                else if (in_char == '*') {
                    RET(last_Ttoken, T_MUL);
                }
                    // equal or assignment
                else if (in_char == '=') {
                    if (last_Ttoken == T_EOL && !white_space)
                        state = BRCKT_BEGIN;
                    else
                        state = ASSIGNMENT;
                }
                    // semicolon
                else if (in_char == ';') {
                    RET(last_Ttoken, T_SEMICOLON);
                }
                    // maybe identifier
                else if ((isalpha(in_char) && islower(in_char)) || in_char == '_') {
                    ADDCHAR;
                    state = ID;
                }
                    // number
                else if (isdigit(in_char)) {
                    if (in_char == '0') {
                        zero_first = true;
                    }
                    ADDCHAR;
                    state = NUMBER;
                }
                    // that's not what I expected
                else {
                    return LEXEM_ERR;
                }
                break;

                // Identifier or keyword
            case ID:
                if (isalnum(in_char) || in_char == '_') {
                    ADDCHAR;
                    break;
                }
                else {
                    if (in_char == '?' || in_char == '!') {
                        ADDCHAR;
						RET(last_Ttoken, T_FUNCTION);
                    }
                    else {
                        ungetc(in_char, src_file);
                        // searching in keywords table
                        for (int i = 0; i < KW_NUM; i++) {
                            if (!strcmp(str->string, keyWTab[i])) {
                                // functions -> save name to value.string
								if (i >= 9 && i <= 19) {
									strcpy(str->string, keyWTab[i]);
								}
								RET(last_Ttoken, i);
                            }
                        }
                    }
                }
                RET(last_Ttoken, T_ID);

            case LINE_COMM:
                if (in_char == '\n') {
                    RET(last_Ttoken, T_EOL);
                }
                if (feof(src_file))
                    return EOF;
                break;

            case ASSIGNMENT:
                if (in_char == '=') {
                    RET(last_Ttoken, T_EQUALS);
                }
                else {
                    ungetc(in_char, src_file);
                    RET(last_Ttoken, T_ASSIGNMENT);
                }

            case NOT_EQ:
                if (in_char == '=') {
                    RET(last_Ttoken, T_NOT_EQ);
                }
                else {
                    ungetc(in_char, src_file);
                    return LEXEM_ERR;
                }

            case BRCKT_BEGIN:
                if (in_char == 'b') {
                    while (!isspace(in_char)) {
                        if (in_char == EOF)
                            break;
                        ADDCHAR;
                        in_char = fgetc(src_file);
                    }
                    ungetc(in_char, src_file);
                    if (!strcmp(str->string, "begin")) {
                        state = BRCKT_COMM;
                        strReset(str);
                        last_Ttoken = -1;
                    } 
					else {
                        for (int i = str->length - 1; i >= 0; i--) {
                            in_char = str->string[i];
                            ungetc(in_char, src_file);
                        }
                        return LEXEM_ERR;
                    }
                }
				else {
					ungetc(in_char, src_file);
                    return LEXEM_ERR;
				}
				break;

            case BRCKT_COMM:
				if (isspace(in_char)) {
                    white_space = true;
                }
                if (in_char == '=' && !white_space) {
                    if (last_Ttoken == T_EOL)
                        state = BRCKT_END;
                }
				last_Ttoken = -3;
                if (in_char == '\n') {
                    last_Ttoken = T_EOL;
                    white_space = false;
                }
                if (feof(src_file))
                    return EOF;
                break;

            case BRCKT_END:
                if (in_char == 'e') {
                    while (!isspace(in_char)) {
                        if (in_char == EOF)
                            break;
                        ADDCHAR;                        
                        in_char = fgetc(src_file);
                    }
                    ungetc(in_char, src_file);
                    if (!strcmp(str->string, "end")) {
                        state = BRCKT_ENDING;
                        strReset(str);
                        last_Ttoken = -2;
                    }
                }
				else 
					state = BRCKT_COMM;
				break;

            case BRCKT_ENDING:
                if (in_char == '\n') {
                    last_Ttoken = T_EOL;
                    state = START;
                }
                else if (feof(src_file))
                    return EOF;
                break;

                // general number state
            case NUMBER:
                if (isdigit(in_char)) {
                    if (in_char == '0' && zero_first) {
                        return LEXEM_ERR;
                    }
                    ADDCHAR;
                }
                else if (in_char == 'E' || in_char == 'e') {
                    ADDCHAR;
                    state = NUM_EXP;
                }
                else if (in_char == '.') {
                    ADDCHAR;
                    state = NUM_DOUBLE;
                }
                else {
                    // return char back to file stream
                    ungetc(in_char, src_file);
                    RET(last_Ttoken, T_INT_NUM);
                }
                break;

                // double number -> ensures at least one digit after "."
            case NUM_DOUBLE:
                if (isdigit(in_char)) {
                    ADDCHAR;
                    state = DOUBLE_EXP;
                }
                else
                    return LEXEM_ERR;
                break;

                // double num -> accept exp form
            case DOUBLE_EXP:
                if (in_char == 'E' || in_char == 'e') {
                    ADDCHAR;
                    state = NUM_EXP;
                }
                else if (isdigit(in_char)) {
                    ADDCHAR;
                }
                else {
                    ungetc(in_char, src_file);
                    RET(last_Ttoken, T_DOUBLE_NUM);
                }
                break;

                // number in exponential form
            case NUM_EXP:
                if (isdigit(in_char)) {
                    ADDCHAR;
                    state = NUM_EXP_END;
                }
                else if (in_char == '+' || in_char == '-') {
                    ADDCHAR;
                    state = NUM_EXP_OP;
                }
                else
                    return LEXEM_ERR;
                break;

                // ensures number after optional operator
            case NUM_EXP_OP:
                if (isdigit(in_char)) {
                    ADDCHAR;
                    state = NUM_EXP_END;
                }
                else
                    return LEXEM_ERR;
                break;

                // finish state of exp form
            case NUM_EXP_END:
                if (isdigit(in_char)) {
                    ADDCHAR;
                }
                else {
                    ungetc(in_char, src_file);
                    RET(last_Ttoken, T_DOUBLE_NUM);
                }
                break;

                // comparators
            case LESS:
                if (in_char == '=') {
                    RET(last_Ttoken, T_LESS_EQ);
                }
                else {
                    ungetc(in_char, src_file);
                    RET(last_Ttoken, T_LESS);
                }

            case GREATER:
                if (in_char == '=') {
                    RET(last_Ttoken, T_GREATER_EQ);
                }
                else {
                    ungetc(in_char, src_file);
                    RET(last_Ttoken, T_GREATER);
                }

                // if string, then load it to buffer, transform if whitespace or '#'
            case STRING:
                if (in_char == '\n' || in_char == EOF)
                    return LEXEM_ERR;
                else if (in_char == '\\')
                    state = ESC_EXPR;
                else if (isspace(in_char))
                    escSeq(in_char);
                else if (in_char == '#')
                    escSeq(in_char);
                else if (in_char == '\"') {
                    RET(last_Ttoken, T_STRING_EXP);
                }
                else {
                    ADDCHAR;
                }
                break;

                // process escape sequences
            case ESC_EXPR:
                if (in_char == 'n') {
                    in_char = '\n';
                    escSeq(in_char);
                    state = STRING;
                }
                else if (in_char == 't') {
                    in_char = '\t';
                    escSeq(in_char);
                    state = STRING;
                }
				else if (in_char == 's') {
					in_char = ' ';
                    escSeq(in_char);
                    state = STRING;
				}
                else if (in_char == '\\') {
                    escSeq(in_char);
                    state = STRING;
                }
                else if (in_char == '"') {
                    escSeq(in_char);
                    state = STRING;
                }
                else if (in_char == 'x') {
                    // process escape sequences in "\xyz" form as valid
                    char hexNum[3];
                    char tmp_char;
					in_char = fgetc(src_file);
                    tmp_char = in_char;
                    if (in_char == EOF) {
                        return LEXEM_ERR;
                    }

					if (islower(in_char)) {
						in_char = toupper(in_char);
					}
					
					if (isxdigit(in_char)) {
						hexNum[0] = in_char;
						hexNum[1] = '\0';
					}
					else
						return LEXEM_ERR;
					
					in_char = fgetc(src_file);
                    tmp_char = in_char;
                    if (in_char == EOF) {
                        return LEXEM_ERR;
                    }

					if (islower(in_char)) {
						in_char = toupper(in_char);
					}
					
					if (isxdigit(in_char)) {
						hexNum[1] = in_char;
						hexNum[2] = '\0';
					}
					else {
						ungetc(tmp_char, src_file);
					}

					escSeq(hexToDec(hexNum));
                    state = STRING;		
                }
                else {
                    ADDCHAR;
                    state = STRING;
                }
                break;
        }

    } while (1);
}


// makes token structure containing it's optional value
// return EXIT_SUCCESS = 0 if OK
// return LEXICAL_ERR = 1 if wrong lexem found
// return EOF = -1 if it come to end of source file
int makeToken(int Ttoken) {

    if (token_alloc)
        tokenFree();

    if (Ttoken == EOF) {
        strFree(str);
        return EOF;
    }
    else if (Ttoken == LEXEM_ERR) {
        strFree(str);
        return LEXICAL_ERR;
    }
    else {
        token.type = Ttoken;
        if (token.type >= 17 && token.type <= 19) {
            Ttoken = T_ID;
        }
        if (Ttoken == T_ID || Ttoken == T_STRING_EXP || Ttoken == T_FUNCTION || (Ttoken >= 9 && Ttoken <= 16)) {
            token_alloc = true;
            if ((token.value.string = (char *) malloc(sizeof(char)*(str->length)+1)) == NULL)
                return INTERNAL_ERR;
            strcpy(token.value.string, str->string);
        }
        else if (Ttoken == T_INT_NUM)
            token.value.val_i = atol(str->string);
        else if (Ttoken == T_DOUBLE_NUM)
            token.value.val_d = atof(str->string);
        strReset(str);
        ///TEST
        if (TEST_FLAG) {
            printf("BUFFER:%s\n", str->string);
            if (Ttoken == T_ID || Ttoken == T_STRING_EXP || Ttoken == T_FUNCTION)
                printf("TOKEN:%d VALUE: %s\n", token.type, token.value.string);
            else if (Ttoken == T_INT_NUM)
                printf("TOKEN:%d VALUE: %d\n", token.type, token.value.val_i);
            else if (Ttoken == T_DOUBLE_NUM)
                printf("TOKEN:%d VALUE: %g\n", token.type, token.value.val_d);
            else
                printf("TOKEN:%d\n", token.type);
        }

        return EXIT_SUCCESS;
    }
}
