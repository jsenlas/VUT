/**
 *	Projekt - IFJ - 2018
 *	Part: Main
 *	File: main.c
 *	Responsibility:
 *	Help:
 *
 *	From: 	24. Sept. 2018
 *	To:		--. ---. 2018
 *
 */

#include <stdio.h>

#include "scanner.h"
#include "errors.h"
#include "parser.h"

int main(int argc, char *argv[]) { 
    FILE *fp = stdin;
    //if (argc != 2) {
    //    fprintf(stderr, "Wrong arguments!\n");
    //    return INTERNAL_ERR;
    //}    

    //if ((fp = fopen(argv[1], "r")) == NULL) {
    //    fprintf(stderr, "Cannot open specified file!\n");
    //    return INTERNAL_ERR;
    //}

    if (scannerInit(fp) == INTERNAL_ERR) {
        //fprintf(stderr, "ScannerInit: Cannot allocate memory!\n");
        //fclose(fp);
        return INTERNAL_ERR;
    }

    int ret_val = EXIT_SUCCESS;
    if ((ret_val = parser_first()) != EXIT_SUCCESS){
        //fprintf(stderr, "\nRetrun value> %d\n", ret_val);
        //fclose(fp);
        //return ret_val;
    }
    //makereturn p_state;
    //fclose(fp);
    //if ((fp = fopen(argv[1], "r")) == NULL) {
    //    fprintf(stderr, "Cannot open specified file!\n");
    //    return INTERNAL_ERR;
    //}
    rewind(fp);
    if (scannerInit(fp) == INTERNAL_ERR) {
        //fprintf(stderr, "ScannerInit: Cannot allocate memory!\n");
        //fclose(fp);
        return INTERNAL_ERR;
    }
    ret_val = EXIT_SUCCESS;
    if ((ret_val = parser_main()) > EXIT_SUCCESS){
        //fprintf(stderr, "Parser couldnot parse the file!\n");
       // fprintf(stderr, "\nRetrun value> %d\n", ret_val);
        //fclose(fp);
        return ret_val;
    }

    //fclose(fp);
    //fprintf(stderr, "Everything OK\n");
    return EXIT_SUCCESS;
}
