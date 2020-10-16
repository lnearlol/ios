#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <sys/wait.h>
#include <sys/mman.h>

int PI, IG, JG, IT, JT ;


sem_t *sem1;
sem_t *print;
sem_t *judge_enters;
sem_t *judge_waits;
sem_t *judge_confirms;

void dtor();
void sem_dtor();
void NE_NC_NB();
void vypis_imm (int a);
void vypis_j (int a);
void zkouska();
void immigrant();
void generovani();
void judge();
int control_letters ( char * argv[] );


struct {

int dead;
int A;
int NE;
int NC;
int NB;
int IMM_STARTS;
int IMM_ENTERS;
int IMM_CHECKS;
int IMM_WANTS;
int IMM_GOT;
int IMM_LEAVES;
int judge_waits_flag;
int registered_imms; //the ones, about which were confirmed (judge rozhoduje)
int judge_confirms_counter;

} *srk ;