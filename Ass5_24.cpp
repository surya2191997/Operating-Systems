#include <stdio.h>
#include <bits/stdc++.h>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

using namespace std;

#define MAP_CYCLES 250
#define UNMAP_CYCLES 250
#define IN_CYCLES 3000
#define OUT_CYCLES 3000
#define RW_CYCLES 1
#define NUMBER_OF_PAGE_TRACES 10000
#define PROB 99

const char* plotter_command = 	"echo \"set terminal pdf\n"
"set output \'plot.pdf\'\n"
"set xlabel \\\"number of page traces\\(N\\)\\\"\n"
"set ylabel \\\"page number\\(pn\\)\\\"\n"
"set title \\\"Page Traces\\\"\n"
"plot \'page_trace.dat\' ;\" | gnuplot";


typedef struct{
	int frame_no:29;
	unsigned int valid:1;
	unsigned int modified:1;
	unsigned int referenced:1;
}pt_entry1;

pt_entry1 pagetable[64];
queue <int> qfifo;
deque <int> dqlru;
vector <int> pdata;

typedef struct{
	int flg;
	int old_page;
	int frame_no;
}print_info;

vector<int> class1;	// referenced bit 0 and modified bit 0
vector<int> class2;	// referenced bit 0 and modified bit 1
vector<int> class3;	// referenced bit 1 and modified bit 0
vector<int> class4;	// referenced bit 1 and modified bit 1

int pagetable_update(int op, int np, int *flg,int rw)
{
	if(pagetable[op].modified==1)
		*flg = 1;

	pagetable[op].valid =0;
	pagetable[op].modified=0;
	pagetable[op].referenced=0;

	int req_frame = pagetable[op].frame_no;

	pagetable[np].frame_no = (req_frame);
	pagetable[np].valid=1;
	if(rw == 1)
		pagetable[np].modified=1;
	pagetable[np].referenced=1;

	return req_frame;
}

print_info fifo(int p_number,int rw)
{
	int q = qfifo.front();
	qfifo.pop();
	print_info p;
	int flg=0;
	int req_frame = pagetable_update(q,p_number,&flg,rw);
	qfifo.push(p_number);
	p.flg = flg;
	p.old_page = q;
	p.frame_no = req_frame;

	return p;
}

print_info random_select(int p_number,int n_frames,int rw){
	int r = rand() % n_frames;
	int f=0;
	print_info p;
	for(int i=0;i<64;i++)
	{
		if(((pagetable[i].frame_no)) == r)
		{
			int np = pagetable_update(i,p_number,&f,rw);

			p.flg = f;
			p.old_page = i;
			p.frame_no = r;

			break;
		}
	}
	
	return p;
}


void classify_pages(){
	class1.clear();
	class2.clear();
	class3.clear();
	class4.clear();
	for(int i=0;i<64;i++)
	{
		if(pagetable[i].valid==1)
		{
			if((pagetable[i].referenced==0) && (pagetable[i].modified==0)){
				class1.push_back(i);
			} 
			else if((pagetable[i].referenced==0) && pagetable[i].modified==1){
				class2.push_back(i);
			}
			else if(pagetable[i].referenced==1 && (pagetable[i].modified==0)){
				class3.push_back(i);
			}
			else if(pagetable[i].referenced==1 && pagetable[i].modified==1){
				class4.push_back(i);
			}
		}
	}
}

void reset_referenced_bits(){
	for(int i=0;i<64;i++){
		if(pagetable[i].valid==1){
			pagetable[i].referenced=0;
		}
	}
}

print_info nru(int page_faults,int p_number,int rw)
{
	if(page_faults % 10 == 0){
		reset_referenced_bits();
	}
	classify_pages();
	int p;
	int f=0;
	if(!class1.empty()){
		p = class1[0];
	}
	else if(!class2.empty()){
		p = class2[0];
	}
	else if(!class3.empty()){
		p = class3[0];
	}
	else if(!class4.empty()){
		p = class4[0];
	}

	int fr = pagetable_update(p,p_number,&f,rw);

	print_info pi;
	pi.flg = f;
	pi.old_page = p;
	pi.frame_no = fr;
	return pi;
}

print_info second_chance(int p_number,int rw)
{
	print_info p;
	int flg=0;
	int q= qfifo.front();
	qfifo.pop();

	while((pagetable[q].referenced==1))
	{
		pagetable[q].referenced=0;
		qfifo.push(q);
		q = qfifo.front();
		qfifo.pop();
	}

	int req_frame = pagetable_update(q,p_number,&flg,rw);

	qfifo.push(p_number);

	p.flg = flg;
	p.old_page = q;
	p.frame_no = req_frame;

	return p;
}

void generate_output(int replace_flg, int out_flg, int line_number, int old_page, int p_number, int req_frame ,long int* total_memory_cycles)
{
	cout<<"Output generated for line :"<<line_number<<endl;
	if(!replace_flg){
		cout<<line_number<<": "<<"IN"<<"\t"<<p_number<<" "<<req_frame<<endl;
		cout<<line_number<<": "<<"MAP"<<"\t"<<p_number<<" "<<req_frame<<endl;
		*(total_memory_cycles) += IN_CYCLES + MAP_CYCLES;
	}
	else{
		cout<<line_number<<": "<<"UNMAP "<<old_page<<" "<<req_frame<<endl;
		if(out_flg) {
			cout<<line_number<<": "<<"OUT"<<"\t"<<old_page<<" "<<req_frame<<endl;
			*(total_memory_cycles) += OUT_CYCLES;
		}
		cout<<line_number<<": "<<"IN"<<"\t"<<p_number<<" "<<req_frame<<endl;
		cout<<line_number<<": "<<"MAP"<<"\t"<<p_number<<" "<<req_frame<<endl;
		*(total_memory_cycles) += UNMAP_CYCLES + IN_CYCLES + MAP_CYCLES;
	}
}

void update_dq(int p_number)
{
	dqlru.erase(find(dqlru.begin(),dqlru.end(),p_number));
	dqlru.push_front(p_number);
}

print_info lru(int p_number,int rw)
{
	int p = dqlru.back();
	dqlru.pop_back();

	int flg=0;

	int req_frame = pagetable_update(p,p_number,&flg,rw);

	if(find(dqlru.begin(),dqlru.end(),p_number) != dqlru.end())
	{
		dqlru.erase(find(dqlru.begin(),dqlru.end(),p_number));
		dqlru.push_front(p_number);
	}
	else	
		dqlru.push_front(p_number);

	print_info q;
	q.flg = flg;
	q.old_page = p;
	q.frame_no = req_frame;
	return q;
}

void generate_pagetrace(int np,int npt,int ws,int p)
{
	vector<int> wsinfo; 
	ofstream f;
	f.open("input.txt");
	int rw,pn;
	for(int i=0;i<ws;i++)
	{
		char ss[20];
		if((rand()/(double)RAND_MAX) > 0.5)
			rw=1;
		else
			rw=0;
		pn = rand() % np;
		wsinfo.push_back(pn);
		pdata.push_back(pn);
		sprintf(ss,"%d %d\n",rw,pn);
		f << ss;
	}
	int l=ws;
	while(l <= npt)
	{	
		char ss[20];
		if((rand()/(double)RAND_MAX) > 0.5)
			rw=1;
		else
			rw=0;
		if((rand()/(double)RAND_MAX) < (double)p/(double)100)
		{
			int i = rand() % ws;
			pn = wsinfo[i];
		}
		else
		{
			pn = rand() % np;
		}

		for(int j=0;j<ws-1;j++)
			wsinfo[j] = wsinfo[j+1];
		wsinfo[ws-1] = pn;

		pdata.push_back(pn);
		sprintf(ss,"%d %d\n",rw,pn);
		f << ss;

		l++;
	}
	f.close();
}

void save_results(char * s)
{
	ofstream of;
	of.open(s);
	for(int i=1;i<=pdata.size();i++)
	{
		of << i << " " << pdata[i-1] <<endl;
	}
	of.close();
}

int main(){
	int line_number = 0;
	long int total_memory_cycles = 0;
	int page_faults = 0;
	int page_transfer = 0;
	int n_frames,alg_id;
	int sows;
	cout<<"Enter the number of frames\n";
	cin >> n_frames;
	cout<<"Enter the size of working set\n";
	cin >> sows;
	cout<<"Enter the algorithm which you want to use\n";
	cout<<"1.FIFO\n2.Random\n3.LRU\n4.NRU\n5.Second Chance\n";
	cin >> alg_id;

	// generate page traces
	generate_pagetrace(64,NUMBER_OF_PAGE_TRACES,sows,PROB);

	// checks whether the frame is free or not
	bool free_list[n_frames];
	for(int i=0;i<n_frames;i++){
		free_list[i] = true;
	}

	ifstream f;
	f.open("input.txt");
	if(!f){
		cout<<"Error while opening the file\n";
		exit(1);
	}

	int rw,p_number;
	string line;
	while(getline(f,line)){
		line_number++;
		if(line[0]=='#')
			continue;
		istringstream iss(line);
		if(!(iss >> rw >> p_number))
		{
			cout<<"Error while reading the file\n";
			exit(1);
		}
		if(p_number > 63)
		{
			cout<<"Page number should be less than 64"<<endl;
			exit(1);
		}
		
		if(pagetable[p_number].valid==1)
		{
			if(rw==1)
				pagetable[p_number].modified=1;
			pagetable[p_number].referenced=1;
			total_memory_cycles += RW_CYCLES;
			if(alg_id == 3)
			{
				update_dq(p_number);
			}
		}
		else
		{
			total_memory_cycles += RW_CYCLES;
			page_faults += 1;
			int req_frame = -1;
			for(int i=0;i<n_frames;i++)
			{
				if(free_list[i]){
					req_frame = i;
					free_list[i] = false;
					break;
				}
			}
			if(req_frame >= 0)
			{
				pagetable[p_number].frame_no = req_frame;
				//cout<<pagetable[p_number].frame_no<<endl;
				pagetable[p_number].valid = 1;
				if(rw==1)
					pagetable[p_number].modified=1;
				else
					pagetable[p_number].modified=0;
				pagetable[p_number].referenced=1;
				
				if(alg_id == 1 || alg_id == 5)
					qfifo.push(p_number);
				if(alg_id == 3)
					dqlru.push_front(p_number);

				page_transfer++;
				generate_output(0,0,line_number,-1,p_number,req_frame,&total_memory_cycles);
				cout<<"Page "<<p_number<<" is loaded in Frame "<<req_frame<<endl;
				
			}
			else
			{
				int flg,q;
				if(alg_id ==1)
				{
					print_info p = fifo(p_number,rw);
					flg = p.flg;
					q = p.old_page;
					req_frame = p.frame_no;

				}
				else if(alg_id == 2)
				{
					print_info p = random_select(p_number,n_frames,rw);
					flg = p.flg;
					q = p.old_page;
					req_frame = p.frame_no;
				}
				else if(alg_id == 3)
				{
					print_info p = lru(p_number,rw);
					flg = p.flg;
					q = p.old_page;
					req_frame = p.frame_no;
				}
				else if(alg_id == 4)
				{
					print_info p = nru(page_faults,p_number,rw);
					flg = p.flg;
					q = p.old_page;
					req_frame = p.frame_no;
				}
				else if(alg_id == 5)
				{
					print_info p = second_chance(p_number,rw);
					flg = p.flg;
					q = p.old_page;
					req_frame = p.frame_no;
				}
				page_transfer++;
				if(flg)
					page_transfer++;
				generate_output(1,flg,line_number,q,p_number,req_frame,&total_memory_cycles);
				cout<<"Page "<<p_number<<" is loaded in Frame "<<req_frame<<endl;
			
			}
		}

	}

	f.close();
	cout<<"Total memory cycles consumed : "<<total_memory_cycles<<endl;
	cout<<"Total number of page faults: "<<page_faults<<endl;
	cout<<"Total number of page transfers: "<<page_transfer<<endl;

	save_results("page_trace.dat");
	system(plotter_command);
	cout << "Opening Page Trace plot\n";
	sleep(3);
	system("xdg-open plot.pdf");

	return 0;
}
