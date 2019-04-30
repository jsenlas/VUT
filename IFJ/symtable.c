#include "symtable.h"

bool flag_vypis=true;

int HTSIZE = MAX_HTSIZE;
tHTItem* UNDEFPTR;
void htPrintTable( tHTable* ptrht ) {
    int maxlen = 0;
    int sumcnt = 0;
    
    printf ("------------HASH TABLE--------------\n");
    for ( int i=0; i<HTSIZE; i++ ) {
        printf ("%i:",i);
        int cnt = 0;
        tHTItem* ptr = (*ptrht)[i];
        while ( ptr != NULL ) {
            printf (" (%s,%s)",ptr->key,ptr->data.dFunction.name);
            int xxx;
            t_param *pointer;
            pointer = ptr->data.dFunction.params;

            for(xxx=0;xxx<ptr->data.dFunction.number_of_parametrs;xxx++)
                {
                    printf("     (name: %s, type: %d)",pointer->name,pointer->type);
                    
                    pointer = pointer->next_param;
                }
                printf("     (       AS  %d)",ptr->data.dFunction.type);
            if ( ptr != UNDEFPTR )
                cnt++;
            ptr = ptr->ptrnext;
        }
        printf ("\n");
    
        if (cnt > maxlen)
            maxlen = cnt;
        sumcnt+=cnt;
    }
    
    printf ("------------------------------------\n");
    printf ("Items count %i   The longest list %i\n",sumcnt,maxlen);
    printf ("------------------------------------\n");
}
/**
*
* @brief    Hash function - it processes key and assign index in interval 0..HTSize-1.
*
* @param    key Key of element
*
* @return   Function returns index of element.
*/

void htPrintTableLocal( tHTable* ptrht ) {
    int maxlen = 0;
    int sumcnt = 0;
    
    printf ("------------HASH TABLE LOCAL--------------\n");
    printf ("------------%s--------------\n",function_name);
    for ( int i=0; i<HTSIZE; i++ ) {
        printf ("%i:",i);
        int cnt = 0;
        tHTItem* ptr = (*ptrht)[i];
        while ( ptr != NULL ) {
            printf (" (%s,%s)",ptr->key,ptr->data.dDim.name);
           // int xxx;
            //t_dim *pointer;
            //pointer = &ptr->data.dDim;
            /*
            for(xxx=0;xxx<ptr->data.dFunction.number_of_parametrs;xxx++)
                {
                    printf("     (name: %s, type: %d)",pointer->name,pointer->type);
                    //printf("     (pointer %d)",pointer->next_param);
                    //if(pointer->next_param!=NULL)
                    pointer = pointer->next_param;
                }
                */
                printf("     (       type  %d)",ptr->data.dDim.type);
            if ( ptr != UNDEFPTR )
                cnt++;
            ptr = ptr->ptrnext;
        }
        printf ("\n");
    
        if (cnt > maxlen)
            maxlen = cnt;
        sumcnt+=cnt;
    }
    
    printf ("------------------------------------\n");
    printf ("Items count %i   The longest list %i\n",sumcnt,maxlen);
    printf ("------------------------------------\n");
}

int hashCode ( tKey key ) {
    int retval = 1;
    int keylen = strlen(key);
    for ( int i=0; i<keylen; i++ )
        retval += key[i];
    return ( retval % HTSIZE );
}


/**
*
* @brief    Initialization of hash table.
*
* @param    ptrht   Pointer to beginning of hash table.
*/
void htInit ( tHTable* ptrht ) {

    for (int i = 0; i < HTSIZE; i++)
        (*ptrht)[i] = NULL;
    return;
}


/**
*
* @brief    Search for element with the given key.
*
* @param    ptrht   Pointer to beginning of hash table.
* @param    key     Key of element
*
* @return   Function returns pointer to found element or NULL.
*/
tHTItem* htSearch ( tHTable* ptrht, tKey key ) {

    int i = hashCode(key);                              //get the hash
    

    if ( (*ptrht)[i] != NULL ){                         //list is not empty
        tHTItem* tempPtr = (*ptrht)[i];

        while ( tempPtr != NULL ){                      //iterate through linked list
            if ( strcmp(tempPtr->key, key) == 0 ){       //element found
                
                return tempPtr;
            }
            else
                tempPtr = tempPtr->ptrnext;             //get next element
        }
       
        return NULL;                                    //element was not found
    }
    else{
       
        return NULL;                                    //list is empty
    }
}


/**
*
* @brief    Function inserts new element with its key and value into the hash table.
*           If element with the same value exists then value is actualized.
*
* @param    ptrht   Pointer to beginning of hash table.
* @param    key     Key of element
* @param    data    Value of element
*/
void htInsert ( tHTable* ptrht, tKey key, tData data ) {

    int i = hashCode(key);                                                  //get the hash

    tHTItem* tempPtr = htSearch(ptrht, key);

    if ( tempPtr != NULL ){
        //memcpy(&tempPtr->data,&data,sizeof(tData));
        //strcpy(tempPtr->data.dFunction.name,data.dFunction.name);
        
        //tempPtr->data.dFunction.name = key;   

    }                                                  //element with the same key exists
                                                    //overwrite existing data
    else {                                                                  //insert new element
        tHTItem* newItem = (tHTItem*) malloc( sizeof(struct tHTItem) );
        if ( newItem == NULL )
            return;
        newItem->data = data;
        //malloc()
        //memcpy(&tempPtr->data,&data,sizeof(tData));
        strcpy(newItem->key,key);
        newItem->data.dFunction.params = data.dFunction.params;
        newItem->data.dFunction.declare = data.dFunction.declare;
        newItem->data.dFunction.define = data.dFunction.define;
        strcpy(newItem->data.dFunction.name,data.dFunction.name);
        newItem->data.dFunction.type = data.dFunction.type;
        newItem->data.dFunction.number_of_parametrs = data.dFunction.number_of_parametrs;
        newItem->data.dFunction.local_symtable = data.dFunction.local_symtable;
       


        //newItem->key = key;
        newItem->ptrnext = (*ptrht)[i];                                     //insert new element at the beginning
        (*ptrht)[i] = newItem;
    }
    return;
}


/**
*
* @brief    Function reads value of an element with the given key.
*
* @param    ptrht   Pointer to beginning of hash table.
* @param    key     Key of element
*
* @return   Function returns pointer to found element or NULL.
*/
tData* htRead ( tHTable* ptrht, tKey key ) {

    tHTItem* tempPtr = htSearch(ptrht, key);

    return ( tempPtr != NULL ? &(tempPtr->data) : NULL );
}


/**
*
* @brief    Removes element with the given key.
*
* @param    ptrht   Pointer to beginning of hash table.
* @param    key     Key of element
*/
void htDelete ( tHTable* ptrht, tKey key ) {

    int i = hashCode(key);

    if ( (*ptrht)[i] != NULL ){                                                     //list is not empty
        tHTItem* tempPtr = (*ptrht)[i];
        tHTItem* prevPtr = NULL;

        while ( tempPtr != NULL ){                                                  //iterate through linked list
            if ( strcmp(tempPtr->key, key) == 0 ){                                  //element found
                if ( tempPtr == (*ptrht)[i] && tempPtr->ptrnext == NULL ){          //list contains one element
                    (*ptrht)[i] = NULL;
                }
                else if ( tempPtr == (*ptrht)[i] )                                  //element is at the beginning
                    (*ptrht)[i] = tempPtr->ptrnext;
                else if ( tempPtr != (*ptrht)[i] && tempPtr->ptrnext == NULL )      //element is at the end
                    prevPtr->ptrnext = NULL;
                else                                                                //element is inside list
                    prevPtr->ptrnext = tempPtr->ptrnext;
                free(tempPtr);
                return;
            }
            else{
                prevPtr = tempPtr;
                tempPtr = tempPtr->ptrnext;                                         //get next element
            }
        }
        return;                                                                     //element does not exist
    }
    else
        return;                                                                     //list is empty
}


/**
*
* @brief    Removes all elements in hash table.
*
* @param    ptrht   Pointer to beginning of hash table.
*/
void htClearAll ( tHTable* ptrht ) {

    for (int i = 0; i < HTSIZE; i++) {                                              //iterate through table
        while ( (*ptrht)[i] != NULL ) {                                             //iterate through linked list
            tHTItem* tempPtr = (*ptrht)[i];                                         //removing from first element to last
            (*ptrht)[i] = tempPtr->ptrnext;
            free(tempPtr);
        }
    }
    return;
}

/*--------------------------------------------------------------*/
/*                L A D A symtable               */


void initGlobalTable(tHTable* Globalsymtab )
    {
      htInit (Globalsymtab);

      hard_definition_inputs();

      hard_definition_inputi();

      hard_definition_inputf();

      hard_definition_lenght();

      hard_definition_substr(); 
    
      hard_definition_ord();
      
      hard_definition_chr();   

    }

void freeGlobalTable(tHTable* Globalsymtab )
    {
      htClearAll (Globalsymtab); 
    }

int checking_declare_function(tKey name_function)
    {
      //tHTItem* pointer;
      
       
        if(htSearch (Globalsymtab_pointer,name_function)==NULL)
            {
              Redeclare_flag=false; 
               //if() printf("------------------------------false");else printf("------------------------------OK");   
               
                
                
                tData data_function;

                data_function.dFunction.declare=true;
                data_function.dFunction.define=false;
                data_function.dFunction.params=NULL;
                data_function.dFunction.number_of_parametrs = 0;
                //data_function.dFunction.name=name_function;
                strcpy(data_function.dFunction.name,name_function);


                

                data_function.dFunction.type=t_void;
                
                htInsert (Globalsymtab_pointer,name_function,/**data_function2*/data_function);
                
                
                 
                
                
            }else
                {
                 Redeclare_flag=true;
                
                 return SEM_TYPES_ERR;   
                }

        return EXIT_SUCCESS;  
    }

void param_init(void)
    {
       Global_param.next_param=NULL;
    }
void loading_param_name(T_struct token)
    {
      strcpy(Global_param.name,token.value.string);
    }

void loading_param_type(t_type type)
    {
      Global_param.type = type;
    }

int loading_param_to_global_symtable(void)
    {
        if(!Define_flag)
            {   
           
               if(Redeclare_flag==false)
               {
                tHTItem* pointer;
               if((pointer = htSearch (Globalsymtab_pointer,function_name))!=NULL); 
               if(pointer->data.dFunction.number_of_parametrs==0)
                {
                    pointer->data.dFunction.params=(t_param*)malloc(sizeof(t_param));
                    pointer->data.dFunction.params->next_param = NULL;
                    strcpy(pointer->data.dFunction.params->name,Global_param.name);//!!!!!!!!!!!!!!!!!!!!!
                    pointer->data.dFunction.params->type=Global_param.type;     
                }else
                    {
                      
                      int i;
                      t_param  *pointer_param = pointer->data.dFunction.params;
                      

                      for(i=1;i<pointer->data.dFunction.number_of_parametrs;i++) 
                        {
                         
                         pointer_param = pointer_param->next_param ;

                        } 
                        t_param *new_param = (t_param*)malloc(sizeof(t_param));
                        strcpy(new_param->name,Global_param.name);
                        new_param->type = Global_param.type;
                        new_param->next_param = NULL;
                        pointer_param->next_param = new_param;




                    }

       
             pointer->data.dFunction.number_of_parametrs ++;

            // printf("pocet %d\n", pointer->data.dFunction.number_of_parametrs);
               }
            }else
                {
                 tHTItem* pointer;
                 
                 pointer = htSearch(Globalsymtab_pointer,function_name);
                 if ( pointer!=NULL){;}
                    t_param  *pointer_param = pointer->data.dFunction.params; 
                    int x = pointer->data.dFunction.number_of_parametrs-1;
                    
                    for(;(x - paramcounter)>=0;x--) 
                        {
                         
                         pointer_param = pointer_param->next_param ;

                        }
                            paramcounter--;
                            
                           // if(pointer_param->type == token.type)
                           // {}
                           // else{} 

                            
                        
                } 
    return 0;
    }

void loading_return_type(T_struct token)
    {
     tHTItem *function_pointer = htSearch (Globalsymtab_pointer,function_name); 
     function_pointer->data.dFunction.type = token.type;  
    }


int checking_definition_function( tKey name_function)
    {
        tHTItem* pointer;
    if((pointer = htSearch (Globalsymtab_pointer,function_name))!=NULL) 
        {

        }else 
        {
           
           Define_flag = false;
           tData data_function;
           data_function.dFunction.declare=true;
           data_function.dFunction.define=false;
           data_function.dFunction.params=NULL;
           data_function.dFunction.number_of_parametrs = 0;
           strcpy(data_function.dFunction.name,name_function);
           data_function.dFunction.type=t_void;
           htInsert (Globalsymtab_pointer,name_function,/**data_function2*/data_function);
           pointer = htSearch (Globalsymtab_pointer,name_function);

        }
      

        

        if(pointer->data.dFunction.define==true)
        {
            return SEM_TYPES_ERR;//4
        }else
            {
             pointer->data.dFunction.define=true;   
             pointer->data.dFunction.declare=true;
             initLocallTable(Globalsymtab_pointer,function_name);
            }
        return EXIT_SUCCESS;
    }


void htInsert_local ( tHTable* ptrht, tKey key, tData data) {

    int i = hashCode(key);                                                  //get the hash

    tHTItem* tempPtr = htSearch(ptrht, key);

    if ( tempPtr != NULL ){  //element with the same key exists
    //overwrite existing data
        tempPtr->data = data;

        strcpy(tempPtr->key, key); // copy key
        strcpy(tempPtr->data.dDim.name, key);//dDim->name
        tempPtr->data.dDim.type = data.dDim.type;
        
        //strcpy(newItem->data.dFunction.name,data.dFunction.name);
        //newItem->data.dFunction.type = data.dFunction.type;
        //newItem->data.dFunction.number_of_parametrs = data.dFunction.number_of_parametrs;
       


        //newItem->key = key;
        tempPtr->ptrnext = (*ptrht)[i];                                     //insert new element at the beginning
        (*ptrht)[i] = tempPtr;
        return;
    }
    else {                                                                  //insert new element
        tHTItem* newItem = (tHTItem*) malloc( sizeof(struct tHTItem) );
        if ( newItem == NULL )
            return;
        
        // copying data to new structure
        newItem->data = data;

        strcpy(newItem->key, key); // copy key
        strcpy(newItem->data.dDim.name, key);//dDim->name
        newItem->data.dDim.type = data.dDim.type;
        
        //strcpy(newItem->data.dFunction.name,data.dFunction.name);
        //newItem->data.dFunction.type = data.dFunction.type;
        //newItem->data.dFunction.number_of_parametrs = data.dFunction.number_of_parametrs;
       


        //newItem->key = key;
        newItem->ptrnext = (*ptrht)[i];                                     //insert new element at the beginning
        (*ptrht)[i] = newItem;
    }
    return;
}

int initLocallTable(tHTable* Globalsymtab,tKey name_function)
    {
       tHTItem* pointer;
       if((pointer = htSearch (Globalsymtab_pointer,function_name))!=NULL)
        {} else return SEM_TYPES_ERR;
       if(pointer->data.dFunction.define==true && pointer->data.dFunction.declare==true) 
           {
             
            pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
            htInit (pointer->data.dFunction.local_symtable);
            tData datax;
                     t_param *pointer_x;
                     pointer_x = pointer->data.dFunction.params;
                    
                    for(int i = 0;i<pointer->data.dFunction.number_of_parametrs;i++)
                        {
                         strcpy(datax.dDim.name,pointer_x->name);
                         datax.dDim.type = pointer_x->type;                         
                         htInsert_local (pointer->data.dFunction.local_symtable,datax.dDim.name,datax);

                         pointer_x = pointer_x->next_param;   
                        }
                    

                   
                       
                   


            
            
           
           } 
           return EXIT_SUCCESS;
    }



void loading_var_name(T_struct token)
    {
        strcpy(global_var.name,token.value.string);
    }





void loading_var_type(T_struct token)
    {
        global_var.type=token.type;   
    }




int save_var_to_correct_local_table()
{
    tHTItem* pointer;
    if((pointer = htSearch (Globalsymtab_pointer,function_name))!=NULL)
    {    
        tData data;
        strcpy(data.dDim.name,global_var.name);
        data.dDim.type = global_var.type;
        // chyba z minuleho projektu
        //if(htSearch (pointer->data.dFunction.local_symtable,global_var.name)!=NULL) return SEM_TYPES_ERR;// 
        htInsert_local (pointer->data.dFunction.local_symtable,global_var.name,data);
        return EXIT_SUCCESS;
    } else return SEM_TYPES_ERR;
}

void print_local_symtable_by_name(char * name)
    {
      if(htSearch(Globalsymtab_pointer,name)!=NULL)
      htPrintTableLocal(htSearch(Globalsymtab_pointer,name)->data.dFunction.local_symtable);
    }


void hard_declare_define_and_created_local_symtable(char *hard_function)
    {
     tData data;
     data.dFunction.params = NULL;
     data.dFunction.declare = true;
     data.dFunction.define = true;
     strcpy(data.dFunction.name,hard_function);
     data.dFunction.type = 12;
     data.dFunction.number_of_parametrs = 0;
     data.dFunction.local_symtable = NULL;
     htInsert (Globalsymtab_pointer,hard_function,data);
     tHTItem* pointer;
     pointer = htSearch (Globalsymtab_pointer,hard_function);
     pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
     htInit (pointer->data.dFunction.local_symtable);   
    }


void scope_init()
    {
      hard_declare_define_and_created_local_symtable("scope");  
    }


bool dim_precedence(int *type,char *name)
{
    tHTItem *local; 
    tHTItem *local_dim;
    if((local = htSearch(Globalsymtab_pointer,function_name))==NULL) return false;
    if((local_dim= htSearch(local->data.dFunction.local_symtable,name))==NULL) return false;
    *type = local_dim->data.dDim.type;
    return true;
}

///////////////////////////////////////////////////////// built in function 
  void hard_definition_lenght(void)
    {
     tData data;
     data.dFunction.params = (t_param*)malloc(sizeof(t_param));
     data.dFunction.params->type = T_STRING;
     strcpy(data.dFunction.params->name,"s");
     data.dFunction.declare = true;////
     data.dFunction.define = true;////
     strcpy(data.dFunction.name,"length");
     data.dFunction.type = T_INTEGER;
     data.dFunction.number_of_parametrs = 1;
     data.dFunction.local_symtable = NULL;
     htInsert (Globalsymtab_pointer,"length",data);
     tHTItem* pointer;
     pointer = htSearch (Globalsymtab_pointer,"length");
     pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
     htInit (pointer->data.dFunction.local_symtable);   
    }

  void hard_definition_substr(void)
    {
     tData data;
     //parameters
     data.dFunction.params = (t_param*)malloc(sizeof(t_param));
     data.dFunction.params->type = T_STRING;
     strcpy(data.dFunction.params->name,"s");
     
     data.dFunction.params->next_param = (t_param*)malloc(sizeof(t_param));
     data.dFunction.params->next_param->type = T_INTEGER;
     strcpy(data.dFunction.params->next_param->name,"i");
     
     data.dFunction.params->next_param->next_param = (t_param*)malloc(sizeof(t_param));
     data.dFunction.params->next_param->next_param->type = T_INTEGER;
     strcpy(data.dFunction.params->next_param->next_param->name,"n");

     data.dFunction.declare = true;////
     data.dFunction.define = true;////
     strcpy(data.dFunction.name,"substr");
     data.dFunction.type = T_INTEGER;
     data.dFunction.number_of_parametrs = 3;
     data.dFunction.local_symtable = NULL;
     htInsert (Globalsymtab_pointer,"substr",data);
     
     tHTItem* pointer;
     pointer = htSearch (Globalsymtab_pointer,"substr");
     pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
     htInit (pointer->data.dFunction.local_symtable);   
    }
//
//    void hard_definition_Asc(void)
//    {
//     tData data;
//     //parameters
//     data.dFunction.params = (t_param*)malloc(sizeof(t_param));
//     data.dFunction.params->type = T_STRING;
//     strcpy(data.dFunction.params->name,"s");
//     
//     data.dFunction.params->next_param = (t_param*)malloc(sizeof(t_param));
//     data.dFunction.params->next_param->type = T_INTEGER;
//     strcpy(data.dFunction.params->next_param->name,"i");
//     
//
//     data.dFunction.declare = true;////
//     data.dFunction.define = true;////
//     strcpy(data.dFunction.name,"Asc");
//     data.dFunction.type = T_INTEGER;
//     data.dFunction.number_of_parametrs = 2;
//     data.dFunction.local_symtable = NULL;
//     htInsert (Globalsymtab_pointer,"Asc",data);
//     
//     tHTItem* pointer;
//     pointer = htSearch (Globalsymtab_pointer,"Asc");
//     pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
//     htInit (pointer->data.dFunction.local_symtable);   
//    }
//
//    void hard_definition_Chr(void)
//    {
//     tData data;
//     //parameters   
//     data.dFunction.params = (t_param*)malloc(sizeof(t_param));
//     data.dFunction.params->type = T_INTEGER;
//     strcpy(data.dFunction.params->name,"i");
//     
//     data.dFunction.declare = true;////
//     data.dFunction.define = true;////
//     strcpy(data.dFunction.name,"Chr");
//     data.dFunction.type = T_STRING;
//     data.dFunction.number_of_parametrs = 1;
//     data.dFunction.local_symtable = NULL;
//     htInsert (Globalsymtab_pointer,"Chr",data);
//     
//     tHTItem* pointer;
//     pointer = htSearch (Globalsymtab_pointer,"Chr");
//     pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
//     htInit (pointer->data.dFunction.local_symtable);   
//    }

    bool checking_existing_function(char *name)
        {
         if(htSearch (Globalsymtab_pointer,name)!=NULL)
         return true; else return false;
        }

////////////////////////////////////////////////////////////////////////////new ifj2018

  void hard_definition_inputs(void)
  {
    tData data;
    data.dFunction.params = NULL;
    data.dFunction.declare = true;////
    data.dFunction.define = true;////
    strcpy(data.dFunction.name,"inputs");
    data.dFunction.type = T_STRING;
    data.dFunction.number_of_parametrs = 0;
    data.dFunction.local_symtable = NULL;
    htInsert (Globalsymtab_pointer,"inputs",data);
    tHTItem* pointer;
    pointer = htSearch (Globalsymtab_pointer,"inputs");
    pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
    htInit (pointer->data.dFunction.local_symtable);
  }  

   void hard_definition_inputi(void)
  {
    tData data;
    data.dFunction.params = NULL;
    data.dFunction.declare = true;////
    data.dFunction.define = true;////
    strcpy(data.dFunction.name,"inputi");
    data.dFunction.type = T_INTEGER;
    data.dFunction.number_of_parametrs = 0;
    data.dFunction.local_symtable = NULL;
    htInsert (Globalsymtab_pointer,"inputi",data);
    tHTItem* pointer;
    pointer = htSearch (Globalsymtab_pointer,"inputi");
    pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
    htInit (pointer->data.dFunction.local_symtable);
  }          

void hard_definition_inputf(void)
  {
    tData data;
    data.dFunction.params = NULL;
    data.dFunction.declare = true;////
    data.dFunction.define = true;////
    strcpy(data.dFunction.name,"inputf");
    data.dFunction.type = t_double;
    data.dFunction.number_of_parametrs = 0;
    data.dFunction.local_symtable = NULL;
    htInsert (Globalsymtab_pointer,"inputf",data);
    tHTItem* pointer;
    pointer = htSearch (Globalsymtab_pointer,"inputf");
    pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
    htInit (pointer->data.dFunction.local_symtable);
  }  

  void hard_definition_ord(void)
    {
     tData data;
     //parameters
     data.dFunction.params = (t_param*)malloc(sizeof(t_param));
     data.dFunction.params->type = T_STRING;
     strcpy(data.dFunction.params->name,"s");
     
     data.dFunction.params->next_param = (t_param*)malloc(sizeof(t_param));
     data.dFunction.params->next_param->type = T_INTEGER;
     strcpy(data.dFunction.params->next_param->name,"i");
     
     

     data.dFunction.declare = true;////
     data.dFunction.define = true;////
     strcpy(data.dFunction.name,"ord");
     data.dFunction.type = T_INTEGER;
     data.dFunction.number_of_parametrs = 2;
     data.dFunction.local_symtable = NULL;
     htInsert (Globalsymtab_pointer,"ord",data);
     
     tHTItem* pointer;
     pointer = htSearch (Globalsymtab_pointer,"ord");
     pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
     htInit (pointer->data.dFunction.local_symtable);   
    }

    void hard_definition_chr(void)
    {
     tData data;
     //parameters
     
     data.dFunction.params = (t_param*)malloc(sizeof(t_param));
     data.dFunction.params->type = T_INTEGER;
     strcpy(data.dFunction.params->name,"i");
     
     

     data.dFunction.declare = true;////
     data.dFunction.define = true;////
     strcpy(data.dFunction.name,"chr");
     data.dFunction.type = T_INTEGER;
     data.dFunction.number_of_parametrs = 1;
     data.dFunction.local_symtable = NULL;
     htInsert (Globalsymtab_pointer,"chr",data);
     
     tHTItem* pointer;
     pointer = htSearch (Globalsymtab_pointer,"chr");
     pointer->data.dFunction.local_symtable = (tHTable*)malloc(sizeof(tHTable));
     htInit (pointer->data.dFunction.local_symtable);   
    }





int change_var_type(void)
{
    tHTItem* pointer;
    if((pointer = htSearch (Globalsymtab_pointer,function_name))!=NULL)
    {    
        //tData data;
        //strcpy(data.dDim.name,global_var.name);
        //data.dDim.type = global_var.type;
        if(htSearch (pointer->data.dFunction.local_symtable,global_var.name)!=NULL) return SEM_TYPES_ERR;
        tHTItem* myptr = htSearch (pointer->data.dFunction.local_symtable,global_var.name);
      
      myptr->data.dDim.type=global_var.type;;
      // myptr->data.dFunction.local_symtable.dDim;       //htInsert_local (pointer->data.dFunction.local_symtable,global_var.name,data);
        return EXIT_SUCCESS;
    } else return SEM_TYPES_ERR;
}