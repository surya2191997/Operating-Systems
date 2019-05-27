#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <bits/stdc++.h>

using namespace std;

int main()
{

	// string for reading the command
	string s;

	// loop for reading commands continuously, just like a shell
	while(1)
	{

			
		vector<string> v;	
		cin>>s;
		istringstream s2(s);

		// if the string is quit break from the loop
		if(s == "quit")
			break;

		// store the strings separated by white spaces in a vector
		string tmp;
  		while (s2 >> tmp) {
    	v.push_back(tmp);
  		}
  		
  		
  		// store the strings in an array of char* (which needs to be passed to execvp)
		char* argv[v.size() + 1];
		for(int i = 0  ; i < v.size() ; i++)
			argv[i] = (char*)v[i].c_str();


		argv[v.size()] = NULL;
		

		// start a child process
		int pid = fork();
		int status;

		// if the current process is child process, call execvp() which takes the place of child process
		if(!pid)
		{
				// if execvp() < 0 print error
				if(execvp(argv[0], argv) < 0){
					cout<<"exec command failed"<<endl;
					exit(1);
				}
		}

		else
		{
			// wait till the execvp from the child process has been executed 
			waitpid(pid, &status, 0);
		}


	
     
	}





}