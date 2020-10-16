#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <stdio.h>

#include<stdlib.h>
#include<sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/mman.h>
#include <time.h>
int kontrola_argumentu(int argc, char *argv[], int kontrola_vstupu);
int process_error();
void soudce(int argc, char *argv[], FILE *file_ptr);
void generator(int argc, char *argv[], FILE *file_ptr);
void initial(int argc, char *argv[], FILE *file_ptr);



sem_t *MAIN_PROC_SEM;
sem_t *JUDGE_SEM;
sem_t *QUEUE_SEM;
sem_t *DECISION_SEM;
sem_t *JWAIT_SEM;
sem_t *VYPIS;
sem_t *ALL_CERTIFICATES;
//
struct 
{
    int counter;
    int NE;
    int NC;
    int NB;
    int i_start;
    int i_enter;
    int i_check;
    int i_want;
    int i_get;
    int i_out;
    int i_decided;
    int i_w_s;
    int judge_wait;
    int main_proc_counter;
    int exit_code;
} *shared;