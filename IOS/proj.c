#include "proj.h"

/**********************************\
|***   The Senate Bus Problem   ***|
\**********************************/

/***
 * Autor: Jakub Sencak
 * login: xsenca00
 * Start = 2018-04-21
 * END = 2018-04-27
 * RLY = 2018-
 */

void rider_f(int i, S_Args *arg, FILE *fp){    
    arg = arg;

    if (proc_init()) {}
    
    sem_wait(vypis);
    (*poradie)++;
    (*rid_to_generate)--;
    (*uteka_na_bus)++;
    printf("%d\t: RID %d\t\t: start\n", *poradie, i);
    fprintf(fp, "%d\t: RID %d\t\t: start\n", *poradie, i);
    sem_post(vypis);

    sem_wait(bus_coming);

    //sem_wait(mutex);
    sem_wait(vypis);
    (*poradie)++;
    (*na_zastavke)++;
    (*uteka_na_bus)--;
    printf("%d\t: RID %d\t\t: enter: %d\n", *poradie, i, *na_zastavke);
    fprintf(fp, "%d\t: RID %d\t\t: enter: %d\n", *poradie, i, *na_zastavke);
    sem_post(vypis);
    //sem_post(mutex);

    sem_wait(bus);

    sem_wait(vypis);
    (*poradie)++;
    (*na_zastavke)--;
    (*kap_bus)--;
    printf("%d\t: RID %d\t\t: boarding\n", *poradie, i);
    fprintf(fp, "%d\t: RID %d\t\t: boarding\n", *poradie, i);
    sem_post(vypis);
    
    sem_post(rid);

    sem_wait(koniec);

    sem_wait(vypis);
    (*poradie)++;
    printf("%d\t: RID %d\t\t: finish\n", *poradie, i);
    fprintf(fp, "%d\t: RID %d\t\t: finish\n", *poradie, i);
    sem_post(vypis);

    proc_dest();
    exit(0);
}
void bus_f(S_Args *arg, FILE *fp){
    proc_init();
    int m;

    sem_wait(vypis);
    (*poradie)++;
    printf("%d\t: BUS\t\t: start\n", *poradie);
    fprintf(fp, "%d\t: BUS\t\t: start\n", *poradie);
    sem_post(vypis);

    do {
        sem_wait(mutex);

        sem_wait(vypis);
        (*poradie)++;
        printf("%d\t: BUS\t\t: arrival\n", *poradie);
        fprintf(fp, "%d\t: BUS\t\t: arrival\n", *poradie);
        sem_post(vypis);

        if (*na_zastavke > 0)
        {
            sem_wait(vypis);            
            (*poradie)++;
            printf("%d\t: BUS\t\t: start boarding : %d\n", *poradie, *na_zastavke);
            fprintf(fp, "%d\t: BUS\t\t: start boarding : %d\n", *poradie, *na_zastavke);
            sem_post(vypis);

            m = min(*na_zastavke, arg->C);
            for (int i = 0; i < m; i++)
            {        
                sem_post(bus);
                sem_wait(rid);
            }

            sem_wait(vypis);
            (*poradie)++;
            printf("%d\t: BUS\t\t: end boarding : %d\n", *poradie, *na_zastavke);
            fprintf(fp, "%d\t: BUS\t\t: end boarding : %d\n", *poradie, *na_zastavke);        
            sem_post(vypis);
            //*na_zastavke = max(*na_zastavke - arg->C, 0);    
            
        }
        sem_post(mutex);
        
        sem_wait(vypis);
        (*poradie)++;
        printf("%d\t: BUS\t\t: depart\n", *poradie);
        fprintf(fp, "%d\t: BUS\t\t: depart\n", *poradie);
        sem_post(vypis);


        for (int i = 0; i < *uteka_na_bus; i++){
            sem_post(bus_coming);
        
        }



        if (arg->ABT)
        {
            usleep((rand()%arg->ABT) * 1000);
        } 
    
        sem_wait(vypis);
        (*poradie)++;
        printf("%d\t: BUS\t\t: end\n", *poradie);
        fprintf(fp, "%d\t: BUS\t\t: end\n", *poradie); 
        sem_post(vypis);
        
        for (int i = 0; i < m; i++)
        {
            sem_post(koniec);
        }

        //printf("rid_to_generate = %d\nna_zastavke = %d\nuteka_na_bus = %d\n",*rid_to_generate,*na_zastavke,*uteka_na_bus);

        //(*rid_to_generate)--;
        //(*na_zastavke)--;
        //(*uteka_na_bus)--;

    } while ((*rid_to_generate > 0) || 
            (*na_zastavke > 0) || 
            (*uteka_na_bus > 0));


    sem_wait(vypis);
    (*poradie)++;
    printf("%d\t: BUS\t\t: finish\n", *poradie);
    fprintf(fp, "%d\t: BUS\t\t: finish\n", *poradie); 
    sem_post(vypis);

    proc_dest();
}

int main(int argc, char **argv) {
    // checks parameters ------------------------------------------

    //S_Args *arg;
    arg = (S_Args *)malloc(sizeof(int)*4);
/***  Uvod  ***/


    /** Random TIME **/
        time_t t;
        srand((unsigned) time(&t));
        //usleep((rand() % arg.ART) * 1000);
    /** Parametre **/
        if (argc != 5){
            fprintf(stderr, "Wrong number of parameters\n");
            return FAILURE;
        }
        // Naplnenie argumentov
        arg->R = atoi(argv[1]);
        arg->C = atoi(argv[2]);
        arg->ART = atoi(argv[3]);
        arg->ABT = atoi(argv[4]);
        
        /*printf("Parameters:\nR = %d\nC = %d\nART = %d\nABT = %d\n",\
            arg->R, arg->C, arg->ART, arg->ABT);
        */
        // Kontrola rozsahu hodnot
        if (arg->R > 0) { /*OK*/ } else { p_flag = 1; }
        if (arg->C > 0) { /*OK*/ } else { p_flag = 1; }      
        if ((arg->ART >= 0) && (arg->ART <= 1000)) { /*OK*/ } else { p_flag = 1; }
        if ((arg->ABT >= 0) && (arg->ABT <= 1000)) { /*OK*/ } else { p_flag = 1; }
        if (p_flag == 1) {
            fprintf(stderr, "Parameters in wrong range of values\n");   
            return FAILURE;
        } 
        else {
            //printf("Parameters OK\n");  
        } 

    /** Open File **/
        // opens the file file proj2.out
        fp = fopen(filename, "w");
        if (fp == NULL) {
        fprintf(stderr, "Couldn't open the file.\n");
        return FAILURE;
        }
    // Uvod
    
        int pid;

        setbuf(stdout,NULL);
        setbuf(stderr,NULL);
        setbuf(fp,NULL);//$#$ asi nemusi byt
        
    /***  Memory init  ***/    
        if (shmem_init(arg)){
            destroy(fp);
            return FAILURE;
        }
    /***  Semaphores init  ***/    
        if (shsem_init()){
            destroy(fp);
            if (shmem_dest()) {
                fprintf(stderr, "Couldn't destroy shared memory\n");
            }
            return FAILURE;
        }

/***  Generovanie procesov  ***/
    if ((pid = fork()) < 0) {
        perror("fork"); 
        exit(2);
    }

    if (pid == 0) { // child
        /**** RIDERS ****/
        int *pid_arr = malloc(sizeof(int)*arg->R);
        for (int i = 0; i < arg->R; i++)
        {
            pid = fork();
            if (pid == 0)
            {
                if (arg->ART)
                {
                    usleep((rand()%arg->ART) * 1000);
                }
                rider_f(i+1, arg, fp);
                //exit(0);
            }
            else {
                pid_arr[i] = pid;
            }
        }
        for (int i = 0; i < arg->R; i++) {
            waitpid(pid_arr[i], NULL, 0);
        }
        if (pid_arr != NULL){free(pid_arr);}
        //exit(0);
    } else { // parent.
        consPID = pid;
        //--
        bus_f(arg, fp);
        //exit(0);

    }

    // pockame az vsichni skonci
    waitpid(consPID, NULL, 0);
    waitpid(prodPID, NULL, 0);

    // zrusime zdroje
    sem_close(mutex);
    sem_close(bus);
    sem_unlink(semMUTEX);
    sem_unlink(semBUS);
    sem_unlink(semRID);
    sem_unlink(semKONIEC);
    sem_unlink(semODCHOD);
    if (shmem_dest()){
        fprintf(stderr, "Couldn't destroy shared memory\n");
        return FAILURE;
    }

    /*
    printf("Parameters:\nR = %d\nC = %d\nART = %d\nABT = %d\n",\
                arg->R, arg->C, arg->ART, arg->ABT);
    */
    
    if (arg != NULL){free(arg);}
    return SUCCESS;
}

void gg(){printf("Gary was here :)\ns");}

void destroy(FILE *fp) {
    if (fp != NULL){
        fclose(fp);
    }
}

int shmem_init(S_Args *arg){

    int protection = PROT_READ | PROT_WRITE;
    int visibility = MAP_SHARED;
    int status_flag = O_CREAT | O_EXCL | O_RDWR | S_IRUSR;

    // SHM_OPEN
        int shm1 = shm_open(s1, status_flag, 0666);
        int shm2 = shm_open(s2, status_flag, 0666);
        int shm3 = shm_open(s3, status_flag, 0666);
        int shm4 = shm_open(s4, status_flag, 0666);
        int shm5 = shm_open(s5, status_flag, 0666);
        int shm6 = shm_open(s6, status_flag, 0666);
        int shm7 = shm_open(s7, status_flag, 0666);

    if (
        (shm1 == -1) ||
        (shm2 == -1) ||
        (shm3 == -1) ||
        (shm4 == -1) ||
        (shm5 == -1) ||
        (shm6 == -1) ||
        (shm7 == -1)
        ){
        return FAILURE;
    }

    // FTRUNCATE
        if (
            (ftruncate(shm1, shmSIZE) == -1) ||
            (ftruncate(shm2, shmSIZE) == -1) ||
            (ftruncate(shm3, shmSIZE) == -1) ||
            (ftruncate(shm4, shmSIZE) == -1) ||
            (ftruncate(shm5, shmSIZE) == -1) ||
            (ftruncate(shm6, shmSIZE) == -1) ||
            (ftruncate(shm7, shmSIZE) == -1)
            )
        {
            return FAILURE;
        }
    // MMAP
        poradie = (int*) mmap(NULL, shmSIZE, protection, visibility, shm1, 0);
        na_zastavke = (int*) mmap(NULL, shmSIZE, protection, visibility, shm2, 0);
        uteka_na_bus = (int*) mmap(NULL, shmSIZE, protection, visibility, shm3, 0);
        kap_bus = (int*) mmap(NULL, shmSIZE, protection, visibility, shm4, 0);
        rid_to_generate = (int*) mmap(NULL, shmSIZE, protection, visibility, shm5, 0);
        a = (int*) mmap(NULL, shmSIZE, protection, visibility, shm6, 0);
        b = (int*) mmap(NULL, shmSIZE, protection, visibility, shm7, 0);

    if (
        (na_zastavke == MAP_FAILED) ||
        (poradie == MAP_FAILED) ||
        (uteka_na_bus == MAP_FAILED) ||
        (kap_bus == MAP_FAILED) ||
        (a == MAP_FAILED) ||
        (b == MAP_FAILED) 
        )
    {
        return FAILURE; 
    }
    
    /*
        printf("IUINTINITNITNParameters:\nR = %d\nC = %d\nART = %d\nABT = %d\n",\
            arg->R, arg->C, arg->ART, arg->ABT);
            */
    // DEFINE
        *poradie = 0;
        *na_zastavke = 0;
        *uteka_na_bus = 0;
        *kap_bus = arg->C;
        *rid_to_generate = arg->R;
        *a = 0;
        *b = 0;

     /*           printf("IUINTINITNITNParameters:\nR = %d\nC = %d\nART = %d\nABT = %d\n",\
            arg->R, arg->C, arg->ART, arg->ABT);
    */
        //printf("poradie = %d\nna_zastavke = %d\nuteka_na_bus = %d\nkap_bus = %d\nrid_to_generate = %d\n",*poradie,*na_zastavke,*uteka_na_bus,*kap_bus,*rid_to_generate);
    /*
    // MUNMAP
        munmap(na_zastavke, shmSIZE);
        munmap(poradie, shmSIZE);
        munmap(uteka_na_bus, shmSIZE);
        munmap(kap_bus, shmSIZE);
        munmap(rid_to_generate, shmSIZE);
        munmap(a, shmSIZE);
        munmap(b, shmSIZE);
    */
    // CLOSE
        close(shm1);
        close(shm2);
        close(shm3);
        close(shm4);
        close(shm5);
        close(shm6);
        close(shm7);

    //printf("dva\n");
    //usleep(5000000);
    
    return SUCCESS;
}

int shsem_init(){
    sem_t *semid = sem_open(semMUTEX, O_CREAT, 0666, 1);
    sem_close(semid);
    semid = sem_open(semBUS, O_CREAT, 0666, 0);
    sem_close(semid);
    semid = sem_open(semRID, O_CREAT, 0666, 0);
    sem_close(semid);
    semid = sem_open(semKONIEC, O_CREAT, 0666, 0);
    sem_close(semid);
    semid = sem_open(semODCHOD, O_CREAT, 0666, 1);
    sem_close(semid);
    semid = sem_open(semPRICHOD, O_CREAT, 0666, 1);
    sem_close(semid);
    //sem_t *mutex = sem_open(semMUTEX, O_CREAT, 0666, 0);
    //sem_t *mutex = sem_open(semMUTEX, O_CREAT, 0666, 0);
    return SUCCESS;
}

int shmem_dest(){/*
    if (
        (shm_unlink(s1) == -1) ||
        (shm_unlink(s2) == -1) ||
        (shm_unlink(s3) == -1) ||
        (shm_unlink(s4) == -1) ||
        (shm_unlink(s5) == -1) ||
        (shm_unlink(s6) == -1) ||
        (shm_unlink(s7) == -1)
    ) {
        fprintf(stderr, "Couldn't do shm_unlink on memory\n");
        return FAILURE;
    }*/
    shm_unlink(s1);
    shm_unlink(s2);
    shm_unlink(s3);
    shm_unlink(s4);
    shm_unlink(s5);
    shm_unlink(s6);
    shm_unlink(s7);
    return SUCCESS;
}

int proc_init(){
    int shm1, shm2, shm3, shm4, shm5, shm6, shm7;

    shm1 = shm_open(s1, O_RDWR, S_IRUSR | S_IWUSR);
    shm2 = shm_open(s2, O_RDWR, S_IRUSR | S_IWUSR);
    shm3 = shm_open(s3, O_RDWR, S_IRUSR | S_IWUSR);
    shm4 = shm_open(s4, O_RDWR, S_IRUSR | S_IWUSR);
    shm5 = shm_open(s5, O_RDWR, S_IRUSR | S_IWUSR);
    shm6 = shm_open(s6, O_RDWR, S_IRUSR | S_IWUSR);
    shm7 = shm_open(s7, O_RDWR, S_IRUSR | S_IWUSR);
    if (
        (shm1 == -1) ||
        (shm2 == -1) ||
        (shm3 == -1) ||
        (shm4 == -1) ||
        (shm5 == -1) ||
        (shm6 == -1) ||
        (shm7 == -1)
        ) {
        return FAILURE;
    }

    poradie = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm1, 0);
    na_zastavke = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm2, 0);
    uteka_na_bus = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm3, 0);
    kap_bus = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm4, 0);
    rid_to_generate = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm5, 0);
    a = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm6, 0);
    b = (int*)mmap(NULL, shmSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm7, 0);

    if (
        (poradie == MAP_FAILED) ||
        (na_zastavke == MAP_FAILED) ||
        (uteka_na_bus == MAP_FAILED) ||
        (kap_bus == MAP_FAILED) ||
        (rid_to_generate == MAP_FAILED) ||
        (a == MAP_FAILED) ||
        (b == MAP_FAILED)
        )
    {
        return FAILURE;
    }

    close(shm1);
    close(shm2);
    close(shm3);
    close(shm4);
    close(shm5);
    close(shm6);
    close(shm7);

    mutex = sem_open(semMUTEX, O_RDWR);
    bus = sem_open(semBUS, O_RDWR);
    rid = sem_open(semRID, O_RDWR);
    koniec = sem_open(semKONIEC, O_RDWR);
    vypis = sem_open(semODCHOD, O_RDWR);
    bus_coming = sem_open(semPRICHOD, O_RDWR);

    return SUCCESS;
}

void proc_dest(){
    munmap(na_zastavke, shmSIZE);
    munmap(poradie, shmSIZE);
    munmap(uteka_na_bus, shmSIZE);
    munmap(kap_bus, shmSIZE);
    munmap(rid_to_generate, shmSIZE);
    munmap(a, shmSIZE);
    munmap(b, shmSIZE);
    
    sem_close(mutex);
    sem_close(bus);
    sem_close(rid);
    sem_close(koniec);
    sem_close(vypis);
    sem_close(bus_coming);
}

int min(int x, int y) {
    if (x < y)
    {
        return x;
    }
    return y;
}

int max(int x, int y) {
    if (x > y)
    {
        return x;
    }
    return y;
}


/*****************************************/
/**  It's nice to see you down here :)  **/
/*****************************************/
