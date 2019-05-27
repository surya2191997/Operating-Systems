#include <bits/stdc++.h>
using namespace std;
int simul;
double ATN;
double FCFS_avg_ATN[3], PSJF_avg_ATN[3], RR1_avg_ATN[3], RR2_avg_ATN[3], RR5_avg_ATN[3];

bool sortbyfirst(const pair<int,int> &a,
              const pair<int,int> &b)
{
    return (a.first < b.first);
}


double genRandExponential(double lambda)
{
	double f = (double)rand() / RAND_MAX, fMin = exp(-10*lambda), fMax = 1;
    double r = fMin + f * (fMax - fMin);
    return -(log(r))/lambda;
}


double genRandUniform()
{
	double f = (double)rand() / RAND_MAX, fMin = 0, fMax = 20;
    return fMin + f * (fMax - fMin);
}


void genProcesses(vector< pair<double, double> > &v, int N, double lambda)
{	
	double arrival = 0, CPUBurst = genRandUniform();
	v.push_back(make_pair(arrival,CPUBurst));
	for(int i = 1 ; i < N ; i++)
	{
		arrival += genRandExponential(lambda);
		CPUBurst = genRandUniform();
		v.push_back(make_pair(arrival, CPUBurst));
	}


}





void computeTurnAround(double ta[], int n)
{
	float total = 0;
	for(int i = 0 ; i < n ; i++)
	{
		if(!simul)
		printf("Turnaround time for process %d : %f\n",i, ta[i]);
		total += ta[i];
	}
	
	if(!simul)
	printf("Average turnaround time : %lf\n", ATN);
	ATN = total/n;
}


void FCFS(vector< pair<double,double> > &v)
{
	int n = v.size();
	double start_time = 0, arrival_old = 0, CPUBurst_old = 0;
	double ta[n];
	sort(v.begin(), v.end(), sortbyfirst);
	for(int i = 0 ; i<n; i++)
	{
		
		if(v[i].first <= CPUBurst_old + arrival_old){
			start_time = CPUBurst_old + arrival_old;
			arrival_old = start_time;
			ta[i] = v[i].second + start_time - v[i].first;
		}
		else{
			start_time  = v[i].first;
			arrival_old = start_time;
			ta[i] = v[i].second;
		}
		if(!simul)
		printf("Process %d starts at t = %f\n",i, start_time );
		CPUBurst_old = v[i].second;

	}
	computeTurnAround(ta, n);
}



void PreEmptive_SJF(vector< pair<double,double> > &v)
{
	int complete = 0, current = 0, n = v.size();
	double start_time = 0, CPUBurst, minm = INT_MAX;
	bool check = true;

	double rt[n], ta[n];
 	sort(v.begin(), v.end(), sortbyfirst);
    // Copy the burst time into remaining time array
    for (int i = 0; i < n; i++)
        rt[i] = v[i].second;

    CPUBurst = v[0].second; 
	// Process until all processes are complete
	while(complete != n)
	{
		if(!simul)
		printf("Process %d starts at t = %f\n",current, start_time );
		check = true;
		for(int i =0; i<n; i++)
			if(start_time <  v[i].first && v[i].first < start_time + CPUBurst)
				if(start_time + CPUBurst - v[i].first > v[i].second)
				{
					rt[current] = start_time + CPUBurst - v[i].first;
					current = i;
					check = false;
					start_time = v[i].first;
					CPUBurst = rt[i];
					break;
				}


		if(check)
		{
			rt[current] = INT_MAX;
			ta[current] = start_time + CPUBurst - v[current].first;
			complete++;
			minm = INT_MAX;
			for(int i = 0 ; i < n ; i++)
				if(v[i].first < start_time + CPUBurst)
				{

					if(minm > rt[i]){
						minm = rt[i];
						current = i;
					}
					
				
				}

			
			if(minm == INT_MAX)
			{
				for(int i=current ; i<n; i++)
					if(v[i].first >= start_time + CPUBurst)
					{
						
						current = i;
						start_time = v[i].first;
						CPUBurst = rt[i];
						break;
					}
			}

			else
			{
				start_time = start_time + CPUBurst;
				CPUBurst = rt[current];	
			}

		}
	}
	computeTurnAround(ta, n);

}


void RoundRobin(vector < pair<double,double> > &v, int delta)
{
	int current, j, n = v.size(), remain;
	double rt[n], ta[n], CPUBurst, start_time = 0;
	queue<int> q;
	sort(v.begin(), v.end(), sortbyfirst);

	for(int i = 0 ; i < n ; i++)
		rt[i] = v[i].second;
	q.push(0);
	remain = n;
	CPUBurst = rt[0];
	while(remain!=0)
	{	
		if(q.size() > 0)
		{
		current = q.front();
		q.pop();
		if(!simul)
		printf("Process %d starts at t = %f\n",current, start_time );
		CPUBurst = rt[current];
		}

		else
		{
			for(int i = 0; i< n; i++)
				if(v[i].first >= start_time)
				{
					q.push(i);
					start_time = v[i].first;
					break;
				}
			continue;
		}

		if(CPUBurst <= delta && CPUBurst > 0)
		{
			rt[current] = 0;
			remain--;
			for(int i=0;i<n;i++)
				if((start_time <= v[i].first && v[i].first <= start_time + CPUBurst) && i!= current)
					q.push(i);
			start_time = start_time + CPUBurst;
			ta[current] = start_time + CPUBurst - v[current].first;

		}

		else if(CPUBurst > 0)
		{
			rt[current] = CPUBurst - delta;
			for(int i=0;i<n;i++)
				if((start_time <= v[i].first && v[i].first <= start_time + delta) && i!= current)
					q.push(i);
			q.push(current);
			start_time = start_time + delta;
		}



	}
		computeTurnAround(ta, n);


}



double runSimulation(int N)
{
	vector< pair<double, double> > v;
	double total[5];
	int j;
	for (int i = 0; i < 5; ++i)
		total[i] = 0;

	for(int i =0 ;i< 10; i++)
	{
		v.clear();

		genProcesses(v, N, 5);
		FCFS(v);
		total[0]+=ATN;
		PreEmptive_SJF(v);
		total[1]+=ATN;
		RoundRobin(v, 1);
		total[2]+=ATN;
		RoundRobin(v,2);
		total[3]+=ATN;
		RoundRobin(v,5);
		total[4]+=ATN;

	}

	if(N==10) j =0;
	else if(N==50) j = 1;
	else if(N == 100) j = 2;

	FCFS_avg_ATN[j] = total[0]/10; PSJF_avg_ATN[j] = total[1] /10;
	RR1_avg_ATN[j] = total[2]/10; RR2_avg_ATN[j] = total[3]/10 ; 
	RR5_avg_ATN[j] = total[4]/ 10;
	

}


int main()
{
	srand(time(NULL));
	int N;
	double first, second, lambda;

	simul = 1;


	if(!simul)
	{

	cout<<"Enter the value of N : ";
	cin>>N;
	cout<<"Enter lambda(mean) : ";
	cin>>lambda;

	vector< pair<double, double> >  v;
	genProcesses(v,N,lambda);
	FILE* fp;
	fp = fopen("Processes.txt", "w");
 
	for(int i =0 ; i< N; i++)
		 fprintf (fp, "Process %3d : Arrival Time = %10lf    CPU Burst = %10lf \n",i, v[i].first, v[i].second);
	fclose(fp);
	printf("\n........FCFS........\n");
	FCFS(v);
	printf("\n........PSJF........\n");
	PreEmptive_SJF(v);
	printf("\n........RRO1........\n");
	RoundRobin(v, 1);
	printf("\n........RRO2........\n");
	RoundRobin(v, 2);
	printf("\n........RRO5........\n");
	RoundRobin(v, 5);
	}


	else
	{
	runSimulation(10); 
	runSimulation(50);
	runSimulation(100);

	printf("%lf %lf %lf\n", FCFS_avg_ATN[0], FCFS_avg_ATN[1], FCFS_avg_ATN[2]);
	printf("%lf %lf %lf\n", PSJF_avg_ATN[0], PSJF_avg_ATN[1], PSJF_avg_ATN[2]);
	printf("%lf %lf %lf\n", RR1_avg_ATN[0], RR1_avg_ATN[1], RR1_avg_ATN[2]);
	printf("%lf %lf %lf\n", RR2_avg_ATN[0], RR2_avg_ATN[1], RR2_avg_ATN[2]);
	printf("%lf %lf %lf\n", RR5_avg_ATN[0], RR5_avg_ATN[1], RR5_avg_ATN[2]);
	
	}





}
