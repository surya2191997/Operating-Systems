#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

using namespace std;

typedef struct{
int x;
int y;
} Item;



int c = 0;
Item* I;



// Signal Handler for part c, global value c stores number of time SIGINT is called
void sighandler(int sigid)
{
	if(sigid == SIGINT)
	{
		if(c<5)
		{
			cout<<"Best of luck for lab test !"<<endl;
			c++;
		}
		
		else
		exit(0);
	}
	
	signal(SIGINT, sighandler);
}
		



void createProcessAndAttach(int shmid)
{

	pid_t pid = fork();
	
	// Process P1
	if(!pid)
	{
	
		// attaching a shared memory as in part a
		char* myseg;
		Item* I;
		myseg = (char*) shmat(shmid, NULL, 0666);
		I = (Item*) myseg;
		// I->x, I->y can now be used
		
		shmdt(myseg);
		
		
		// redirecting stdout as in part b
		string instr;
		char* args[2];
		cin >> instr;
		args[0] = (char*)instr.c_str();
		args[1] = NULL;
		int fileDesc = open("myfile.txt",O_CREAT | O_RDWR);
		close(1);
		dup(fileDesc);
		close(fileDesc);
		execvp(args[0], args);
		
		
		
	}
	
	else
	{
	int status;
	waitpid(pid, &status, 0);	
	}
	
	
	
}


void createProcessAndAttach2(int shmid)
{
	
	pid_t pid = fork();
	if(!pid)
	{
		
		// attaching shared memory as in part a
		char* myseg;
		Item* I;
		myseg = (char*) shmat(shmid, NULL, 0666);
		I = (Item*) myseg;
		// I->x, I->y can now be used
		
		
		// Reading I->x, I->y
		cin>>I->x;
		cin>>I->y;
		signal(SIGINT, sighandler);
		//pause();

		
		shmdt(myseg);
		exit(0);
	}
	

	
	else
	{
	int status;
	waitpid(pid, &status, 0);
	}
	
}
	
	
	
void* thread_handler(void* id)
{
	
	int* ID = (int*)id;
	for(int i = 0 ; i < I->y; i++)
	{
		printf("I am thread <%d>\n",*ID );
		sleep(1);
	}
}
	
	

		


int main()
{
	int shmid, i;
	char* myseg;

	shmid = shmget(IPC_PRIVATE, sizeof(Item), IPC_CREAT | 0666);
	
	
	
	// Creating processes P1 and P2
	
	createProcessAndAttach(shmid);
	createProcessAndAttach2(shmid);
	
		
	
	
	
	// Shared memory attached to P3
	myseg = (char*) shmat(shmid, NULL,0666);
	I = (Item*) myseg;

	
	
	
	// Creating threads as in part d
	
	pthread_t threads[I->x];
		
	for(i = 0 ; i < I->x ; i++)
		pthread_create(&threads[i], NULL, thread_handler, (void*)(&threads[i]));
	
	
	//for(i = 0 ; i < I->x ; i++)
	//	pthread_join(threads[i], NULL);
	
	shmctl(shmid, IPC_RMID, NULL);
}
	


