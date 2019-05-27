#include <bits/stdc++.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 

using namespace std;

int N,*status, *oldstatus;
pthread_t *worker;
queue<int> q;

void sig_handle(int signo)
{
	if(signo == SIGUSR1)
		pause();
}



void* worker_func(void* param)
{
	signal(SIGUSR1, sig_handle);
	signal(SIGUSR2, sig_handle);
	pause();
	int i =  *((int *) param);
	status[i] = 1;  // running
	int j,a[1000];
	for(j=0;j<1000;j++)
		a[j] = 1000*(rand()/RAND_MAX);
	sort(a, a+N);
	int rand_t = 10;//1 + 9*rand()/RAND_MAX;

	while(rand_t) rand_t = sleep(rand_t);
	status[i] = 2; // terminated 
}


void* scheduler_func(void *)
{
	int i;

	

	while(!q.empty())
	{
	
	i = q.front();
	if(status[i] == 0)
	{
		status[i] = 1;
		pthread_kill(worker[i], SIGUSR2);


	}

	if(status[i] == 1)
	{
		q.pop();
		q.push(i);
		status[i] = 0;
		pthread_kill(worker[i], SIGUSR1);
		status[q.front()] = 1;
		pthread_kill(worker[q.front()], SIGUSR2);
	}
	
	if(status[i] == 2)
	{
		q.pop();
		status[q.front()] = 1;
		pthread_kill(worker[q.front()], SIGUSR2);
	}

	sleep(1);

	}

}



void* reporter_func(void *)
{
	int i;
	while(!q.empty()){
	for(i=0;i<N;i++){


		if(oldstatus[i] != status[i])
		{
			if(status[i] == 1)
				printf("Thread %d started running !\n",i );
			if(status[i] == 2)
				printf("Thread %d terminated\n", i );
		}

		oldstatus[i] = status[i];

	}

	}
		
}



int main()
{
	int i;
	pthread_t scheduler, reporter;
	cin>>N;
	status = (int*)malloc(sizeof(int)*N);
	oldstatus = (int*)malloc(sizeof(int)*N);
	worker = (pthread_t*)malloc(sizeof(pthread_t)*N);
	for(i = 0; i < N; i++)
	{
		int* arg =  (int*)malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&worker[i], NULL, worker_func, arg);
		q.push(i);
		status[i] = 0;
		oldstatus[i] = 0;
	}

	pthread_create(&reporter, NULL, reporter_func, NULL);
	pthread_create(&scheduler, NULL, scheduler_func, NULL);

	for (int i = 0; i < N; ++i)
		pthread_join(worker[i], NULL);

	pthread_join(reporter, NULL);
	pthread_join(scheduler, NULL);
	

}
