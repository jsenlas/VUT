#ifndef projh
#define projh

// Libraries -----------
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

#define filename "proj2.out"

#define shmSIZE sizeof(int)
#define semBUS "/jakubsencak-ios-proj2-bus"
#define semRID "/jakubsencak-ios-proj2-rid"
#define semMUTEX "/jakubsencak-ios-proj2-mutex"
#define semKONIEC "/jakubsencak-ios-proj2-KONIEC"
#define semODCHOD "/jakubsencak-ios-proj2-ODCHOD"
#define semPRICHOD "/jakubsencak-ios-proj2-PRICHOD"


#define s1 "/jakubsencak-ios-proj2-shm-ONE"
#define s2 "/jakubsencak-ios-proj2-shm-TWO"
#define s3 "/jakubsencak-ios-proj2-shm-THREE"
#define s4 "/jakubsencak-ios-proj2-shm-FOUR"
#define s5 "/jakubsencak-ios-proj2-shm-FIVE"
#define s6 "/jakubsencak-ios-proj2-shm-SIX"
#define s7 "/jakubsencak-ios-proj2-shm-SEVEN"


pid_t consPID;
pid_t prodPID;

// Flags ---------------
int p_flag = 0;


// Global variables ----


typedef struct {
    int R; // Number of RIDERS
    int C; // Bus capacity
    int ART; // generate new new rider
    int ABT; // bus is runnig down da street
} S_Args;

S_Args *arg;

int *poradie;   //vypis
int *na_zastavke;   // cakajuce procesy na Bus
int *uteka_na_bus;  // pocet vytvorenych procesov, ktore este nie su  na zastavke
int *kap_bus;
int *rid_to_generate; // pocet procesov rider 
int *a;
int *b;

// Error codes ---------
#define FAILURE 1
#define SUCCESS 0

sem_t *mutex;
sem_t *bus;
sem_t *rid;
sem_t *koniec;
sem_t *vypis;
sem_t *bus_coming;

FILE *fp;

// Functions

void gg();
void destroy(FILE *fp);
int shmem_init(S_Args *arg);
int shmem_dest();

int shsem_init();

void rider_f(int i, S_Args *arg, FILE *fp);
void bus_f(S_Args *arg, FILE *fp);

int min(int x, int y);
int max(int x, int y);

int proc_init();
void proc_dest();

#endif