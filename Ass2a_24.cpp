#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
using namespace std;

vector<int> process;
int prime[1000];

typedef struct{
int buffer[5];
int in;
int out; 
}item;

int isPrime(int n)
{
	int k = 0;
	for(int i =1;i<=n;i++)
		if(n%i==0)
			k++;

	if(k==2)
		return 1;
	else 
		return 0;
}


void generate_prime_array()
{
	int i=2,k=0;
	while(k<1000)
	{
		if(isPrime(i))
			prime[k++] = i;
		i++;
	}
		

}


int gen_random_prime(int i)
{
	srand(time(0)-i*2);
	int n = rand()%1000;
	return prime[n];

}





void createProducers(int shmid, int NP)
{
	for(int i=0;i<NP;i++)
	{
		
		int pid= fork();
		if(!pid)
		{
			char* shm = (char*)shmat(shmid, NULL, 0);
			item* I = (item*)shm;
			int next_produced = gen_random_prime(i);
			srand(time(0)-i*2);
		    float _rand = 5*(float)rand()/(float)RAND_MAX;
			sleep(_rand);
			while((I->in+1) % 5 == I->out);
			I->buffer[I->in] = next_produced;
			I->in = (I->in + 1) % 5;
			cout<<"Producer "<<i<<" : "<<next_produced<<", time :"<<time(0)<<endl;
			exit(0);
		}
		process.push_back(pid);

	}
}


void createConsumers(int shmid, int NC)
{
	for(int i=0;i<NC;i++)
	{
		int pid= fork();
		if(!pid)
		{
			char* shm = (char*)shmat(shmid, NULL, 0);
			item* I = (item*)shm;
			srand(time(0)-i*4);
		    float _rand = 5*(float)rand()/(float)RAND_MAX;
			sleep(_rand);
			while(I->in == I->out);
			int next_consumed = I->buffer[I->out];
			I->out = (I->out + 1) % 5;
  			cout<<"Consumer "<<i<<" : "<<next_consumed<<", time :"<<time(0)<<endl;
			exit(0);
		}
		process.push_back(pid);

	}
}





int main()
{

	int NP, NC;
	cin>>NP>>NC;
	generate_prime_array();

	

	key_t key = ftok("shmfile", 65);
	int shmid = shmget(key, 1024, 0666|IPC_CREAT);
	char* shm = (char*)shmat(shmid, NULL, 0);
	item* I = (item*)shm;
	I->in = 0;
	I->out = 0;  

	createProducers(shmid, NP);
	createConsumers(shmid, NC);
	sleep(30);

	for(int i = 0 ; i< 5; i++)
		cout<<"Item "<<i<<": "<<I->buffer[i]<<endl;

	shmdt(shm);
	shmctl(shmid,IPC_RMID,NULL);
	for(int i = 0; i < process.size() ; i++)
		kill(process[i], SIGKILL);

	return 0;

}
