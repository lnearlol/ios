#include "proj2.h"
FILE *file;

/*
	-PI je pocet procesu vygenerovanych v kategorii imm;

 	-IG je max hodnota doby (v milisekundech), po ktere je generovan novy proces immigrant.

 	-JG je max hodnota doby (v milisekundech), po ktere soudce opet vstoupi do budovy.

 	-IT je max hodnota doby (v milisekundech), ktera simuluje trvani vyzvedavani certifikatu imm.

 	-JT je max hodnota doby (v milisekundech), ktera simuluje trvani vydavani rozhodnuti soudcem.
*/

/*
	NE je pocet imm, kteri vstoupili do budovy a dosud o nich nebylo rozhodnuto
	NC je pocet imm, kteri se zaregistrovali a dosud o nich nebylo rozhodnuto
	NB je pocet imm, kteri jsou v budove

*/

int control_letters ( char * argv[] ){   
	char * ptrEnd = NULL;
    
	for(int c = 1; c < 6 ; c++){		
		
		double zk = strtod (argv[c], &ptrEnd);
       
		if (*ptrEnd!='\0'){
			return 1;
		}
   	
    } 
	return 0;
}



void dtor(){
	munmap(srk, 14*sizeof(int));
}

void sem_dtor(){
	
	sem_wait(sem1);
	fclose(file);
	sem_unlink("xstepa64_main_waits");
	sem_close(sem1);
	sem_unlink("xstepa64_sem_print");
	sem_close(print);
	sem_unlink("xstepa64_judge_enters");
	sem_close(judge_enters);	
	sem_unlink("xstepa64_judge_waits");
	sem_close(judge_waits);
	sem_unlink("xstepa64_judge_confirms");
	sem_close(judge_confirms);
	
}

void NE_NC_NB(){
	srk->NE = srk->IMM_ENTERS - srk->registered_imms;
	srk->NC = srk->IMM_CHECKS - srk->registered_imms;
	srk->NB = srk->IMM_ENTERS - srk->IMM_LEAVES;
}


void vypis_imm (int a){
	NE_NC_NB();
	srk->A+=1;
		
	if(a == 1){
		fprintf(file,"%d    : IMM %d        : starts\n",srk->A, srk->IMM_STARTS+=1);
	}              
	else if(a == 2){
		fprintf(file,"%d    : IMM %d        : enters              : %d : %d : %d\n", srk->A, srk->IMM_ENTERS, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 3){
		fprintf(file,"%d    : IMM %d        : checks              : %d : %d : %d\n", srk->A, srk->IMM_CHECKS, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 4){
		fprintf(file,"%d    : IMM %d        : wants certificate              : %d : %d : %d\n", srk->A, srk->IMM_WANTS+=1, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 5){
		fprintf(file,"%d    : IMM %d        : got certificate              : %d : %d : %d\n", srk->A, srk->IMM_GOT+=1, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 6){
		fprintf(file,"%d    : IMM %d        : leaves              : %d : %d : %d\n", srk->A, srk->IMM_LEAVES, srk->NE, srk->NC, srk->NB);
	}
}

void vypis_j (int a){
	srk->A+=1;
	NE_NC_NB();
	if(a == 1){
		fprintf(file,"%d    : JUDGE          : wants to enter\n", srk->A);
	}
	else if(a == 2){
		fprintf(file,"%d    : JUDGE          : enters              : %d : %d : %d\n", srk->A, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 3){
		fprintf(file,"%d    : JUDGE          : waits for imm              : %d : %d : %d\n", srk->A, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 4){
		fprintf(file,"%d    : JUDGE          : starts confirmation              : %d : %d : %d\n", srk->A, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 5){
		fprintf(file,"%d    : JUDGE          : ends confirmation              : %d : %d : %d\n", srk->A, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 6){
		fprintf(file,"%d    : JUDGE          : leaves              : %d : %d : %d\n", srk->A, srk->NE, srk->NC, srk->NB);
	}
	else if(a == 7){
		fprintf(file,"%d    : JUDGE          : finishes\n", srk->A);
	}
	
}

void zkouska(){
	srk->dead+=1;
	int ps = 2 + PI;
	if( srk->dead == ps ){
		sem_post(sem1);
	}
}

void immigrant(){

	sem_wait(print);	
	vypis_imm(1);		//starts
	sem_post(print);

	sem_wait(judge_enters);
	sem_wait(print);
	srk->IMM_ENTERS+=1;
	vypis_imm(2);		//enters
	sem_post(print);
	sem_post(judge_enters);

	
	sem_wait(print);
	srk->IMM_CHECKS+=1;
	vypis_imm(3);		//checks
	sem_post(print);

	if(srk->judge_waits_flag == 1 && srk->IMM_ENTERS == srk->IMM_CHECKS ){
	srk->judge_waits_flag = 0;
	sem_post(judge_waits);
	}
	
	
	
	sem_wait(judge_confirms);
	srk->judge_confirms_counter-=1;
	if(srk->judge_confirms_counter > 0){
		sem_post(judge_confirms);
	}
	
	
	

	sem_wait(print);
	vypis_imm(4);		//wants certificate
	sem_post(print);


	srand(time(NULL));
	usleep((rand() % IT)*1000);

	sem_wait(print);
	vypis_imm(5);		//got certificate
	sem_post(print);
	
	sem_wait(judge_enters);
	sem_wait(print);
	srk->IMM_LEAVES+=1;
	vypis_imm(6);		//leaves
	sem_post(print);
	sem_post(judge_enters);

	zkouska();
	exit (0);
}

void generovani (){
	pid_t imm;
	for (int f = 0 ; f < PI; f++ ){
	srand(time(NULL));
	usleep((rand() % IG)*1000);
	imm = fork();
	if( imm == 0 ){
		immigrant();
		}
	}
	zkouska();
	exit (0);
}

void judge (){
	int vlajka;
	while(srk->registered_imms < PI){

		srand(time(NULL));
		usleep((rand() % JG)*1000);
		

		sem_wait(print);
		vypis_j(1);			//wants to enter
		sem_post(print);
		
		sem_wait(judge_enters);

		sem_wait(print);
		vypis_j(2);			//enters
		sem_post(print);
		
		
																	

		if(srk->IMM_ENTERS != srk->IMM_CHECKS ){
				sem_wait(print);
				vypis_j(3);		//waits for imm
				sem_post(print);
				srk->judge_waits_flag=1;
				sem_wait(judge_waits);
			}
			
				
		sem_wait(print);
		vypis_j(4);			//starts confirmation
		sem_post(print);
		
		
		srand(time(NULL));
		usleep((rand() % JT)*1000);

		srk->judge_confirms_counter += srk->NC;
		if(srk->NC != 0){
			vlajka = 1;
		}
		srk->registered_imms = srk->IMM_CHECKS;

		sem_wait(print);
		vypis_j(5);			//ends confirmation
		sem_post(print);
		
		if( vlajka == 1){
		sem_post(judge_confirms);
		vlajka = 0;
		}


		srand(time(NULL));
		usleep((rand() % JT)*1000);

		sem_wait(print);
		vypis_j(6);			//leaves
		sem_post(print);

		sem_post(judge_enters);
		
	}
	
	sem_wait(print);
	vypis_j(7);			//finishes
	sem_post(print);
	

	zkouska();
	exit (0);
}

int main( int argc, char * argv[] ){
	
	if (argc!=6){           
        fprintf(stderr,"error: invalid arguments \n");
        return 1;
        }
	
	int letter = control_letters( argv );
	if (letter == 1){
		fprintf(stderr,"error: invalid arguments \n");
		return 1;
    }	

	for (int fr = 1; fr < 6; fr++){
		if( atoi(argv[fr]) != atof(argv[fr])  ){
			fprintf(stderr,"error: invalid arguments \n");
        	return 1;
		}
	}

	PI = atoi(argv[1]);
	IG = atoi(argv[2]) + 1; 
	JG = atoi(argv[3]) + 1; 
	IT = atoi(argv[4]) + 1; 
	JT = atoi(argv[5]) + 1; 
	if ( PI < 1){
	fprintf(stderr,"error: invalid arguments \n");
	return 1;
	}

	if ( IG < 1 || IG > 2001 || JG < 1 || JG > 2001 || IT < 1 || IT > 2001 || JT < 1 || JT > 2001 ) { 
	fprintf(stderr,"error: invalid arguments \n"); 
	return 1; 
	}


	srk = mmap(NULL, 14*sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
	if (srk == MAP_FAILED){
		dtor();
		fprintf(stderr,"error: Mapping failed \n");
		return 1;
	}
	srk->dead = 0;
	srk->A = 0;

	file = fopen("proj2.out","w");
	setbuf(file, NULL);

	sem1 = sem_open("xstepa64_main_waits", O_CREAT, 0666, 0);
		if( sem1 == SEM_FAILED ){
			dtor();
			sem_dtor();
			fprintf(stderr,"error: sem_open(`xstepa64_main_waits`) failed \n");
			return 1;
		}
	print = sem_open("xstepa64_sem_print", O_CREAT, 0666, 1);
		if( print == SEM_FAILED ){
		dtor();
		sem_dtor();
		fprintf(stderr,"error: sem_open(`xstepa64_sem_print`) failed \n");
		return 1;
		}
	judge_enters = sem_open("xstepa64_judge_enters", O_CREAT, 0666, 1);
		if( judge_enters == SEM_FAILED ){
		dtor();
		sem_dtor();
		fprintf(stderr,"error: sem_open(`xstepa64_judge_enters`) failed \n");
		return 1;
		}
	judge_waits = sem_open("xstepa64_judge_waits", O_CREAT, 0666, 0);
		if( judge_waits == SEM_FAILED ){
		dtor();
		sem_dtor();
		fprintf(stderr,"error: sem_open(`xstepa64_judge_waits`) failed \n");
		return 1;
		}
	judge_confirms = sem_open("xstepa64_judge_confirms", O_CREAT, 0666, 0);
		if( judge_confirms == SEM_FAILED ){
		dtor();
		sem_dtor();
		fprintf(stderr,"error: sem_open(`xstepa64_judge_confirms`) failed \n");
		return 1;
		}

	pid_t child = fork();
	if (child == 0){
		generovani();
	}

	
	child = fork();
	if (child == 0){
	judge ();
	}
	

	dtor();
	sem_dtor();

	return 0;
}
