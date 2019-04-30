/**
 *	Projekt-IFJ-2017
 *	Part: IAL
 *	File: ial.c
 *	Responsibility lies on:
 *	Help:
 *
 *	From: 	18. Oct. 2017
 *	To:		--. ---. 2017
 *
 */

#include "ial.h"

int errflg;

//----------------------------------------------------------------------------------//
//--------------------------STACK-PRECEDENCE-ANALYZE--------------------------------//
//----------------------------------------------------------------------------------//

void stackInit ( tStack* s ) {

    if ( s == NULL )                //pokud je hodnota ukazatele s rovná NULL
        errflg = -1;
    else
        s->top = NULL;              //jinak nastavím vrchol zásobníku
    return;
}

int stackEmpty ( const tStack* s ) {

    return( ( s->top == NULL ) ? 1 : 0 );
}

//void stackTop ( const tStack* s, T_struct symbol ) {
//
//    if ( stackEmpty(s) == 0 ) {       //pokud zásobník není prázdný
//        symbol = s->top->symbol;        //do promìnné c uložím hodnotu z vrcholu zásobníku
//    }
//    return;
//}

void stackPop ( tStack* s ) {

    if (stackEmpty(s) == 0) {         //pokud zásobník není prázdný
        tSItemPtr tempItem = s->top;
        s->top = s->top->next;      //odstráním prvek z vrcholu zásobníku
        free (tempItem);
    }
    return;
}

void stackPushTerm ( tStack* s, T_struct term ) {

    tSItemPtr newItem = (tSItemPtr)malloc( sizeof(struct tSItem) );
    if ( newItem == NULL ) {
        errflg = -1;
        return;
    }

    newItem->symbol = term;
    newItem->symbol.type = term.type;
    if ( term.type == T_ID || term.type == T_STRING_EXP ){
        int len = strlen(term.value.string);
        newItem->symbol.value.string = (char *) malloc(sizeof(char)*( len )+1);
        if ( newItem->symbol.value.string == NULL ) {
            errflg = -1;
            return;
        }
        strcpy(newItem->symbol.value.string, term.value.string);
    }

    newItem->handle = false;
    newItem->term = true;
    newItem->reduceType = term.type;
    newItem->temp = false;

    newItem->next = s->top;
    s->top = newItem;

    return;
}

void stackPushRule ( tStack* s, T_struct rule, T_type redType ) {

    tSItemPtr newItem = (tSItemPtr)malloc( sizeof(struct tSItem) );
    if ( newItem == NULL ) {
        errflg = -1;
        return;
    }

    newItem->symbol = rule;
    newItem->symbol.type = rule.type;
    if ( rule.type == T_ID || rule.type == T_STRING_EXP ){
        int len = strlen(rule.value.string);
        newItem->symbol.value.string = (char *) malloc(sizeof(char)*( len )+1);
        if ( newItem->symbol.value.string == NULL ) {
            errflg = -1;
            return;
        }
        strcpy(newItem->symbol.value.string, rule.value.string);
    }

    newItem->handle = false;
    newItem->term = false;
    newItem->reduceType = redType;
    newItem->temp = true;

    newItem->next = s->top;
    s->top = newItem;
    return;
}

void stackPushRuleID ( tStack* s, T_struct rule, T_type reduceType ) {

    tSItemPtr newItem = (tSItemPtr)malloc( sizeof(struct tSItem) );
    if ( newItem == NULL ) {
        errflg = -1;
        return;
    }

    newItem->symbol = rule;
    newItem->symbol.type = rule.type;
    if ( rule.type == T_ID || rule.type == T_STRING_EXP ){
        int len = strlen(rule.value.string);
        newItem->symbol.value.string = (char *) malloc(sizeof(char)*( len )+1);
        if ( newItem->symbol.value.string == NULL ) {
            errflg = -1;
            return;
        }
        strcpy(newItem->symbol.value.string, rule.value.string);
    }

    newItem->reduceType = reduceType;
    newItem->handle = false;
    newItem->term = false;
    newItem->temp = false;

    newItem->next = s->top;
    s->top = newItem;
    return;
}

void stackDispose ( tStack* s ) {

    tSItemPtr tempItem;

    while ( s->top != NULL ){
        tempItem = s->top;
        s->top = s->top->next;
        free(tempItem);
    }

    return;
}


//----------------------------------------------------------------------------------//
//---------------------  -----LIST  ------------------------------//
//----------------------------------------------------------------------------------//

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/

    L->First = L->Act = L->Last = NULL;
    return;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free.
**/
    if ( (L->First != NULL) && (L->Last != NULL) ){         //pokud není seznam prázdný
        while ( L->First != L->Last ){                      //procházím seznamem od prvního po předposlední položku
            DLDeleteFirst (L);
        }
        free(L->Last);                                      //vymažu poslední položku
        L->First = L->Act = L->Last = NULL;                 //seznam uvedu do stavu po inicializaci
    }
    return;
}

void DLInsertFirst (tDLList *L, T_struct symbol, T_type type, bool temp ) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    tDLElemPtr newElement = (tDLElemPtr) malloc(sizeof(struct tDLElem));    //vytvořím nový prvek

    if ( newElement == NULL ){                                              //testujem dostatek paměti
        errflg = -1;
        return;
    }

    newElement->symbol = symbol;                                                 //naplním složku data
    newElement->symbol.type = symbol.type;
    if ( symbol.type == T_ID ){
        int len = strlen(symbol.value.string);
        newElement->symbol.value.string = (char *) malloc(sizeof(char)*( len )+1);
        if ( newElement->symbol.value.string == NULL ) {
            errflg = -1;
            return;
        }
        strcpy(newElement->symbol.value.string, symbol.value.string);
    }
    newElement->reduceType = type;
    newElement->temp = temp;

    newElement->rptr = L->First;                                            //nastavím pravý ukazatel na púvodní první
    newElement->lptr = NULL;                                                //levý ukazatel ukazuje na NULL
    if ( L->First != NULL )                                                 //seznam není prazdní
        L->First->lptr = newElement;                                        //púvodní první ukazuje doleva na nový prvek
    else
        L->Last = newElement;                                               //seznam byl prázdný
    L->First = newElement;                                                  //nový element nastavím na první položku
    return;
}

void DLInsertLast(tDLList *L, T_struct symbol) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    tDLElemPtr newElement = (tDLElemPtr) malloc(sizeof(struct tDLElem));    //vytvořím nový prvek

    if ( newElement == NULL ){                                              //testujem dostatek paměti
        errflg = -1;
        return;
    }

    newElement->symbol = symbol;                                                 //naplním složku data
    newElement->rptr = NULL;                                                //pravý ukazatel ukazuje na NULL
    newElement->lptr = L->Last;                                             //nastavím levý ukazatel na púvodní poslední
    if ( L->Last != NULL )                                                  //seznam není prazdní
        L->Last->rptr = newElement;                                         //púvodní poslední ukazuje doprava na nový prvek
    else
        L->First = newElement;                                              //seznam byl prázdný
    L->Last = newElement;                                                   //nový element nastavím na poslední položku
    return;
}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/

    L->Act = L->First;
    return;
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/

    L->Act = L->Last;
    return;
}

void DLCopyFirst (tDLList *L, T_struct *symbol) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if ( (L->First == NULL) && (L->Last == NULL) )              //pokud je seznam prázdný
        errflg = -1;                                              //volám funkci DLError()
    else
        *symbol = L->First->symbol;                                  //jinak do proměnné val uložím hodnotu prvního prvku seznamu
    return;
}

void DLCopyLast (tDLList *L, T_struct *symbol) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if ( (L->First == NULL) && (L->Last == NULL) )              //pokud je seznam prázdný
        errflg = -1;                                              //volám funkci DLError()
    else
        *symbol = L->Last->symbol;                                   //jinak do proměnné val uložím hodnotu posledního prvku seznamu
    return;
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if ( (L->First != NULL) && (L->Last != NULL) ){             //pokud není seznam prázdný
        tDLElemPtr tempElement = L->First;                      //vytvořím dočasný pomocný prvek

        if ( L->First == L->Act )                               //pokud je první prvek aktivní, aktivita se ztrácí
            L->Act = NULL;
        if ( L->First == L->Last) {                             //pokud seznam obsahuje jeden prvek, odstranim jej
            L->First = L->Last = NULL;
        }
        else {
            L->First = L->First->rptr;                          //jinak první prvek bude následující
            tempElement->rptr->lptr = NULL;                     //levý ukazatel náslédujíciho prvku nastavím na NULL
            tempElement->rptr = NULL;                           //pravý ukazatel odstraňovaného prvku nastavím na NULL
        }
        free(tempElement);                                      //daný prvek odstraním
    }
    return;
}

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if ( (L->First != NULL) && (L->Last != NULL) ){             //pokud není seznam prázdný
        tDLElemPtr tempElement = L->Last;                       //vytvořím dočasný pomocný prvek

        if ( L->Last == L->Act )                                //pokud je poslední prvek aktivní, aktivita se ztrácí
            L->Act = NULL;
        if ( L->First == L->Last) {                             //pokud seznam obsahuje jeden prvek, odstranim jej
            L->First = L->Last = NULL;
        }
        else {
            L->Last = L->Last->lptr;                            //jinak poslední prvek bude předcházející
            tempElement->lptr->rptr = NULL;                     //pravý ukazatel předcházejíciho prvku nastavím na NULL
            tempElement->lptr = NULL;                           //levý ukazatel odstraňovaného prvku nastavím na NULL
        }
        free(tempElement);                                      //daný prvek odstraním
    }
    return;
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/

    if ( L->Act != NULL ){                                      //pokud je seznam aktivní
        if ( L->Act->rptr != NULL ){                            //za aktivním prvkem sa nachází prvek
            tDLElemPtr tempElement;                             //vytvořím dočasný pomocný prvek
            tempElement = L->Act->rptr;                         //uložím do nej rušený prvek
            L->Act->rptr = tempElement->rptr;                   //aktuální prvek bude ukazovat o dva prvky doprava
            if ( tempElement == L->Last )                       //pokud odstraňuji poslední prvek
                L->Last = L->Act;                               //posledním prvkem bude aktuální
            else
                tempElement->rptr->lptr = L->Act;               //jinak přepojím prvek za rušeným s aktuálním
            free(tempElement);                                  //daný prvek odstraním
        }
    }
    return;
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/

    if ( L->Act != NULL ){                                      //pokud je seznam aktivní
        if ( L->Act->lptr != NULL ){                            //před aktivním prvkem sa nachází prvek
            tDLElemPtr tempElement;                             //vytvořím dočasný pomocný prvek
            tempElement = L->Act->lptr;                         //uložím do nej rušený prvek
            L->Act->lptr = tempElement->lptr;                   //aktuální prvek bude ukazovat o dva prvky doleva
            if ( tempElement == L->First )                      //pokud odstraňuji první prvek
                L->First= L->Act;                               //prvním prvkem bude aktuální
            else
                tempElement->lptr->rptr = L->Act;               //jinak přepojím prvek před rušeným s aktuálním
            free(tempElement);                                  //daný prvek odstraním
        }
    }
    return;
}

void DLPostInsert (tDLList *L, T_struct symbol) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    if ( L->Act != NULL ){                                                      //pokud je seznam aktivní
        tDLElemPtr newElement = (tDLElemPtr) malloc(sizeof(struct tDLElem));    //vytvořím nový prvek

        if ( newElement == NULL ){                                              //testujem dostatek paměti
            errflg = -1;
            return;
        }

        newElement->symbol = symbol;                                                 //naplním složku data
        newElement->lptr = L->Act;                                              //levý ukazatel nového prvku ukazuje na aktivní
        newElement->rptr = L->Act->rptr;                                        //pravý ukazatel nového prvku ukazuje na prvek za aktivním
        L->Act->rptr = newElement;                                              //pravý ukazatel aktivního prvku ukazuje na nový prvek
    if ( L->Act == L->Last )                                                    //pokud je aktivní prvek posledním
        L->Last = newElement;                                                   //posledním prvkem bude nový prvek
    else
        newElement->rptr->lptr = newElement;                                    //jinak propjím prvek za aktivním s novým prvkem
    }
    return;
}

void DLPreInsert (tDLList *L, T_struct symbol) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/

    if ( L->Act != NULL ){                                                      //pokud je seznam aktivní
        tDLElemPtr newElement = (tDLElemPtr) malloc(sizeof(struct tDLElem));    //vytvořím nový prvek

        if ( newElement == NULL ){                                              //testujem dostatek paměti
            errflg = -1;
            return;
        }

        newElement->symbol = symbol;                                                 //naplním složku data
        newElement->rptr = L->Act;                                              //pravý ukazatel nového prvku ukazuje na aktivní
        newElement->lptr = L->Act->lptr;                                        //levý ukazatel nového prvku ukazuje na prvek před aktivním
        L->Act->lptr = newElement;                                              //levý ukazatel aktivního prvku ukazuje na nový prvek
    if ( L->Act == L->First )                                                   //pokud je aktivní prvek prvním
        L->First = newElement;                                                  //prvním prvkem bude nový prvek
    else
        newElement->lptr->rptr = newElement;                                    //jinak propjím prvek před aktivním s novým prvkem
    }
    return;
}

void DLCopy (tDLList *L, T_struct *symbol) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/

    if ( DLActive(L) == 1 )                 //pokud je seznam aktivní
        *symbol = L->Act->symbol;                //do proměnné val uložím hodnotu aktivního prvku
    return;
}

void DLActualize (tDLList *L, T_struct symbol) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if ( DLActive(L) == 1 )                 //pokud je seznam aktivní
        L->Act->symbol = symbol;                 //přepíšem hodnotu aktivního prvku parametrem val
    return;
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    if ( DLActive(L) == 1 ){                //pokud je seznam aktivní
        if ( L->Act == L->Last )            //pokud je aktivní prvek posledním
            L->Act = NULL;                  //aktivita se ruší
        else
            L->Act = L->Act->rptr;          //jinak posunu aktivitu na následujíci prvek
    }
    return;
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
    if ( DLActive(L) == 1 ){                //pokud je seznam aktivní
        if ( L->Act == L->First )           //pokud je aktivní prvek prvním
            L->Act = NULL;                  //aktivita se ruší
        else
            L->Act = L->Act->lptr;          //jinak posunu aktivitu na předcházejíci prvek
    }
    return;
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/

    return( (L->Act != NULL) ? 1 : 0 );
}
