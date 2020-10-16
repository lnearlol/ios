#include "proj2.h"


void struct_unmap()
{
  munmap(shared, sizeof(int)*15);
}

void file_close(FILE *file_ptr)
{
  fclose(file_ptr); // zavirani file
}

void semaphore_destruct()
{
  sem_unlink("xstepa64_vypis");
  sem_close(VYPIS);
  sem_unlink("xstepa64_jwait");
  sem_close(JWAIT_SEM);
  sem_unlink("xstepa64_dec");
  sem_close(DECISION_SEM);
  sem_unlink("xstepa64_q");
  sem_close(QUEUE_SEM);
  sem_unlink("xstepa64_j");
  sem_close(JUDGE_SEM);
  sem_unlink("xstepa64");
  sem_close(MAIN_PROC_SEM);
}

int kontrola_argumentu(int argc, char *argv[], int kontrola_vstupu)
{
  if(argc != 6)
  { 
     kontrola_vstupu++;
  } else
  {
    if(atoi(argv[1]) < 1) //kontrolujeme hodnoty argumentu
    {
       kontrola_vstupu++;
    } else 
    {  
     for (int i = 1; i <= 5; i++)
      {
        char *sym_var = NULL;
        double value = strtod(argv[i], &sym_var); //kontrolujeme na cokoliv krome cislovych hodnot
        if(*sym_var != '\0')
         kontrola_vstupu++;
         
        if((value < 0 || value > 2000) && i > 1) //kontrola na zaporne hodnoty
          kontrola_vstupu++; 
         
         if(atoi(argv[i]) != atof(argv[i]))
           kontrola_vstupu++;      
      }
    }
  }
  return kontrola_vstupu;
}

int process_error()
{
   perror("Process error!\n"); //vypis chybneho vstupu
   exit(EXIT_FAILURE);
}

void soudce(int argc, char *argv[], FILE *file_ptr)
{
  
  while (shared->i_decided < atoi(argv[1]))
  {
    int flag = 0;       // promenna pro check NC po kazdem cyklu JUDGE
    srand(time(NULL));
    usleep((rand() % (atoi(argv[3])+1)) * 1000);

    sem_wait(VYPIS);
    fprintf(file_ptr, "%5d : JUDGE    : wants to enter\n", shared->counter+=1);
    sem_post(VYPIS);
    sem_wait(JUDGE_SEM);

    sem_wait(VYPIS);  
    shared->NE= shared->i_enter - shared->i_decided;   
    shared->NC= shared->i_check - shared->i_decided;
    shared->NB= shared->i_enter - shared->i_out;
    
    fprintf(file_ptr, "%5d : JUDGE    : enters                : %2d : %2d : %2d\n", shared->counter+=1, shared->NE, shared->NC, shared->NB);
    sem_post(VYPIS);
    sem_wait(VYPIS);
    if(shared->NE != shared->NC)
    {
      shared->judge_wait = 1;
      
      fprintf(file_ptr, "%5d : JUDGE    : waits for imm	 : %2d : %2d : %2d\n", shared->counter+=1, shared->NE, shared->NC, shared->NB);
      sem_post(VYPIS);
      sem_wait(JWAIT_SEM);
    } else
    {
      sem_post(VYPIS);
    }

    shared->NE= shared->i_enter - shared->i_decided;
    shared->NC= shared->i_check - shared->i_decided;
    shared->NB= shared->i_enter - shared->i_out;
    sem_wait(VYPIS);
    fprintf(file_ptr, "%5d : JUDGE    : starts confirmation   : %2d : %2d : %2d\n", shared->counter+=1, shared->NE, shared->NC, shared->NB);
    sem_post(VYPIS);
    srand(time(NULL));
    usleep((rand() % (atoi(argv[5])+1)) * 1000);
    sem_wait(VYPIS);
    if(shared->i_w_s == 0)
    {
      flag = 1;  // judge opens semaphore only when shared->i_w_s
    } else 
    {
      flag = 0;
    }
    shared->i_w_s = shared->i_w_s + shared->NC; // inkrementujeme mnozstvi lidi ktere budou zadat o sertifikat
    shared->i_decided = shared->i_check;
    
    shared->NE= shared->i_enter - shared->i_decided;
    shared->NC= shared->i_check - shared->i_decided;
    shared->NB= shared->i_enter - shared->i_out;
    sem_post(VYPIS);
    sem_wait(VYPIS);
    fprintf(file_ptr, "%5d : JUDGE    : ends confirmation     : %2d : %2d : %2d\n", shared->counter+=1, shared->NE, shared->NC, shared->NB);
    sem_post(VYPIS);
    if (shared->i_w_s > 0 && flag == 1)
      sem_post(DECISION_SEM);
    


    srand(time(NULL));
    usleep((rand() % (atoi(argv[5])+1)) * 1000);
    sem_wait(VYPIS);
    shared->NE= shared->i_enter - shared->i_decided;
    shared->NC= shared->i_check - shared->i_decided;
    shared->NB= shared->i_enter - shared->i_out;
    fprintf(file_ptr, "%5d : JUDGE    : leaves                : %2d : %2d : %2d\n", shared->counter+=1, shared->NE, shared->NC, shared->NB);
    sem_post(VYPIS);
    sem_post(JUDGE_SEM);
  } 
    sem_wait(VYPIS);
    fprintf(file_ptr, "%5d : JUDGE    : finishes\n", shared->counter+=1);
    sem_post(VYPIS);

  sem_wait(VYPIS);
  shared->main_proc_counter+=1;

  if(shared->main_proc_counter == 0)
    {
      sem_post(MAIN_PROC_SEM);
    }
    sem_post(VYPIS);

  if (shared->exit_code == 1)
  {
    exit(EXIT_FAILURE);
  }
  exit(0);
}

void generator(int argc, char *argv[], FILE *file_ptr)
{ 
  
  pid_t pid = getpid();

  for(int i = 0; i < atoi(argv[1]); i++)
  {
    srand(time(NULL));
    usleep((rand() % (atoi(argv[2])+1)) * 1000);
    if(pid != 0)
      pid = fork();
    if (pid < 0)
    {
      fprintf(stderr, "fork error\n");
      exit(1);
    }
    else if (pid == 0)
      initial(argc, argv, file_ptr);
  }
  

  sem_wait(VYPIS);
  shared->main_proc_counter+=1;
  if(shared->main_proc_counter == 0)
    {
      sem_post(MAIN_PROC_SEM);
    }
    sem_post(VYPIS);
  if (shared->exit_code == 1)
  {
    exit(EXIT_FAILURE);
  }
  exit(0);
}

void initial(int argc, char *argv[], FILE *file_ptr)
{
  shared->i_start = shared->i_start + 1;
  sem_wait(VYPIS);
  fprintf(file_ptr, "%5d : IMM %2d   : starts\n", shared->counter+=1, shared->i_start);
   sem_post(VYPIS);

  sem_wait(JUDGE_SEM);
  shared->i_enter = shared->i_enter + 1;
  shared->NE= shared->i_enter - shared->i_decided;
  shared->NC= shared->i_check - shared->i_decided;
  shared->NB= shared->i_enter - shared->i_out;
  sem_wait(VYPIS);
  fprintf(file_ptr, "%5d : IMM %2d   : enters                : %2d : %2d : %2d\n", shared->counter+=1, shared->i_enter, shared->NE, shared->NC, shared->NB);
  sem_post(VYPIS);
  sem_post(JUDGE_SEM);
  sem_wait(QUEUE_SEM);
  

  sem_wait(VYPIS);
  shared->i_check= shared->i_check + 1;
  fprintf(file_ptr, "%5d : IMM %2d   : checks                : %2d : %2d : %2d\n", shared->counter+=1, shared->i_check, shared->NE= shared->i_enter - shared->i_decided,  shared->NC= shared->i_check - shared->i_decided, shared->NB= shared->i_enter - shared->i_out);
 
  sem_post(VYPIS);
  sem_post(QUEUE_SEM); 
  
  sem_wait(QUEUE_SEM);
  if (shared->judge_wait == 1 && shared->NE == shared->NC)
    {
      shared->judge_wait = 0;
      sem_post(JWAIT_SEM);
    }
  sem_post(QUEUE_SEM);

  sem_wait(DECISION_SEM);    // cekani kdyz soudce otevre semafor pro lidi ktere byly checked a podtvrzeni aby oni mohli vyzvedavat certifikaty
   shared->i_w_s = shared->i_w_s - 1;
  if(shared->i_w_s > 0)
  {  // poustime jen te mnozstvi lidi, ktere maji podtvrzeni od soudce
    sem_post(DECISION_SEM);
  }
  sem_wait(QUEUE_SEM);
  shared->NE= shared->i_enter - shared->i_decided;
  shared->NC= shared->i_check - shared->i_decided;
  shared->NB= shared->i_enter - shared->i_out;
  sem_wait(VYPIS);
  fprintf(file_ptr, "%5d : IMM %2d   : wants certificate     : %2d : %2d : %2d\n", shared->counter+=1, shared->i_want = shared->i_want+1, shared->NE, shared->NC, shared->NB);
  sem_post(VYPIS);
  sem_post(QUEUE_SEM); 
  
  srand(time(NULL));
  usleep((rand() % (atoi(argv[4])+1)) * 1000);
  sem_wait(QUEUE_SEM);
  sem_wait(VYPIS);
  fprintf(file_ptr, "%5d : IMM %2d   : got certificate       : %2d : %2d : %2d\n", shared->counter+=1, shared->i_get = shared->i_get+1, shared->NE, shared->NC, shared->NB);
  sem_post(VYPIS);
  sem_post(QUEUE_SEM); 

  sem_wait(JUDGE_SEM);
  shared->i_out=shared->i_out+1;
  shared->NE= shared->i_enter - shared->i_decided;
  shared->NC= shared->i_check - shared->i_decided;
  shared->NB= shared->i_enter - shared->i_out;
  sem_wait(VYPIS);
  fprintf(file_ptr, "%5d : IMM %2d   : leaves                : %2d : %2d : %2d\n", shared->counter+=1, shared->i_out, shared->NE, shared->NC, shared->NB);
  sem_post(VYPIS);
  sem_post(JUDGE_SEM);

  sem_wait(VYPIS);
  shared->main_proc_counter+= 1;
  if(shared->main_proc_counter == 0)
    {
      sem_post(MAIN_PROC_SEM);
    }
    sem_post(VYPIS);
  if (shared->exit_code == 1)
  {
    exit(EXIT_FAILURE);
  }
  exit(0);
}

int main(int argc, char *argv[])
{
  int kontrola_vstupu = 0;
  kontrola_vstupu = kontrola_argumentu(argc, argv, kontrola_vstupu); // kontrola argumentu
  if (kontrola_vstupu != 0)
  {
    fprintf(stderr, "error: invalid arguments\n"); //vypis chybneho vstupu
    return 1;
  }

  FILE *file_ptr;
  file_ptr = fopen("proj2.out", "w");

  setbuf(file_ptr, NULL);

  if(file_ptr == NULL)
  {
    fprintf(stderr, "Unfortunally file was not created.\n");
    return 1;
  }
  /*            ARGUMENTS + FILE CONDITIONS                  */
  
  shared = mmap(NULL, sizeof(int)*15, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
  if(shared == MAP_FAILED)
  {
    struct_unmap();
    file_close(file_ptr);
    fprintf(stderr, "Struct mmap error\n");
    return 1;
  }
  shared->main_proc_counter = 1 - (atoi(argv[1]) + 3); // spocita ukoncene processy a pak kdyz vsechno ukonci prijde post na semafor main_procesu a ten se ukonci

 
  MAIN_PROC_SEM = sem_open("xstepa64", O_CREAT, 0666, 0);
  JUDGE_SEM = sem_open("xstepa64_j", O_CREAT, 0666, 1);
  QUEUE_SEM = sem_open("xstepa64_q", O_CREAT, 0666, 1);  //                      
  DECISION_SEM = sem_open("xstepa64_dec", O_CREAT, 0666, 0);
  JWAIT_SEM = sem_open("xstepa64_jwait", O_CREAT, 0666, 0);
  VYPIS = sem_open("xstepa64_vypis", O_CREAT, 0666, 1);

  if(MAIN_PROC_SEM == SEM_FAILED || JUDGE_SEM == SEM_FAILED || QUEUE_SEM == SEM_FAILED|| DECISION_SEM == SEM_FAILED|| JWAIT_SEM == SEM_FAILED|| VYPIS == SEM_FAILED)
  {
    struct_unmap();
    semaphore_destruct();
    file_close(file_ptr);
    fprintf(stderr, "error sem_open\n");
    return 1;
  }

  pid_t pid = getpid();

  pid = fork();
  if (pid < 0)
  {
    shared->exit_code = 1;
    process_error();
  }
  else if (pid == 0)
    generator(argc, argv, file_ptr);

  pid = fork();
  if (pid < 0)
  {
    shared->exit_code = 1;
    process_error();
  }
  else if (pid == 0)
    soudce(argc, argv, file_ptr);
  

  sem_wait(MAIN_PROC_SEM);

  
  file_close(file_ptr);
  semaphore_destruct();
  
  if(shared->exit_code == 1)
  {
    struct_unmap();
    return 1;
  }

  struct_unmap();
  return 0;
}