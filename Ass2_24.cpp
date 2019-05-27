#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#define BUFFSIZE 100

using namespace std;

#define BUFF 1000

void debug() {
	cout << "Entered!" << endl;
}

void displayMenu() {
	cout << "Which type of command do you want to execute? Enter the corresponding alphabet..." << endl;
	cout << "a. Run an internal command." << endl;
	cout << "b. Run an external command." << endl;
	cout << "c. Run an external command by redirecting standard input from a file." << endl;
	cout << "d. Run an external command by redirecting standard output to a file." << endl;
	cout << "e. Run an external command in the background." << endl;
	cout << "f. Run several external commands in the pipe mode." << endl;
	cout << "g. Quit the shell." << endl;
}

void runExternalCommandPipe() {
	cout << ">> ";
	string instr, temp;

	cin.ignore();
	getline (cin, instr);

	istringstream is (instr);
	vector<string> tokenizedInstr;
	int noOfPipes = 0;
	vector<int> indexOfPipes;

	while (is >> temp) tokenizedInstr.push_back (temp);


	for (int i = 0; i < tokenizedInstr.size(); i++) 
		if (tokenizedInstr[i] == "|") {
			noOfPipes++;
			indexOfPipes.push_back (i);
		}
	
	int p[noOfPipes][2];
	
	for (int i = 0; i < noOfPipes; i++) pipe (&p[i][0]);

	for (int i = 0; i < noOfPipes + 1; i++) {
		pid_t pid = fork();

		if (!pid) {
			char *args[2];
			int noOfTokens = 0;

			if (i != noOfPipes) {
				if (i != 0) {
					for (int j = indexOfPipes[i - 1] + 1; j < indexOfPipes[i]; j++) {
						args[j - indexOfPipes[i - 1] - 1] = (char *)tokenizedInstr[j].c_str();
						noOfTokens++;
					}
				}
				else {
					for (int j = 0; j < indexOfPipes[0]; j++) {
						args[j] = (char *)tokenizedInstr[j].c_str();
						noOfTokens++;
					}
				}
			}
			else {
				if (!noOfPipes) {
					for (int j = 0; j < tokenizedInstr.size(); j++) {
						args[j] = (char *)tokenizedInstr[i].c_str();
						noOfTokens++;
					}
				}
				else 
					for (int j = indexOfPipes[noOfPipes - 1] + 1; j < tokenizedInstr.size(); j++) {
						args[j - indexOfPipes[noOfPipes - 1] - 1] = (char *)tokenizedInstr[j].c_str(); 
						noOfTokens++;
					}
			}
			args[noOfTokens] = NULL;

			if (i != 0) {
				dup2 (p[i - 1][0], 0);
			}
			if (i != noOfPipes) {
				dup2 (p[i][1], 1);
			}

			execvp (args[0], args);

			cout << "Error in executing program!" << endl;

			exit(1);
		}
		else {
			pid_t statusPID;
			waitpid (pid, &statusPID, 0);
			
			if (i != 0) close (p[i - 1][0]);
			if (i != noOfPipes) close (p[i][1]);
		}
	}
}


void runExternalCommandOP() {
	cout << ">> ";
	string instr, temp;
	cin.ignore();
	getline(cin, instr);
	istringstream is (instr);
	vector<string> tokenizedInstr;
	while (is >> temp) tokenizedInstr.push_back(temp);

	if (tokenizedInstr[0] == "cd") {
		if (tokenizedInstr.size() == 2) {
			int chk = chdir(tokenizedInstr[1].c_str());
			if (chk != 0) {
				cout << "*** Error in cd! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}
	else {
		//Handle incorrect ip redirection
		char *args[tokenizedInstr.size() - 1];
		pid_t pid;
		
		if (tokenizedInstr[tokenizedInstr.size() - 2] != ">") {
			cout << "*** Error in redirection! ***" << endl;
			return;
		}
		
		for (int i = 0; i < tokenizedInstr.size() - 2; i++) {
			args[i] = (char *)tokenizedInstr[i].c_str();
		}
		args[tokenizedInstr.size() - 2] = NULL;
		
		pid = fork();
		
		if(!pid) {
			cout << tokenizedInstr[tokenizedInstr.size() - 1] << endl;
			int fileDesc = open (tokenizedInstr[tokenizedInstr.size() - 1].c_str(), O_WRONLY | O_CREAT, 0777);
			close (1);
			dup (fileDesc);
			close (fileDesc);
			execvp (args[0], args);
			cout << "*** Error in execution! ***" << endl;
			exit(0);
		}
		else {
			int status;
			waitpid (pid, &status, 0);
		}
	}	
}

void runExternalCommandIP() {
	cout << ">> ";
	string instr, temp;
	cin.ignore();
	getline(cin, instr);
	istringstream is (instr);
	vector<string> tokenizedInstr;
	while (is >> temp) tokenizedInstr.push_back(temp);

	if (tokenizedInstr[0] == "cd") {
		if (tokenizedInstr.size() == 2) {
			int chk = chdir(tokenizedInstr[1].c_str());
			if (chk != 0) {
				cout << "*** Error in cd! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}
	else {
		//Handle incorrect ip redirection
		char *args[tokenizedInstr.size() - 1];
		pid_t pid;
		
		if (tokenizedInstr[tokenizedInstr.size() - 2] != "<") {
			cout << "*** Error in redirection! ***" << endl;
			return;
		}
		
		for (int i = 0; i < tokenizedInstr.size() - 2; i++) {
			args[i] = (char *)tokenizedInstr[i].c_str();
		}
		args[tokenizedInstr.size() - 2] = NULL;
		
		pid = fork();
		
		if(!pid) {
			cout << tokenizedInstr[tokenizedInstr.size() - 1] << endl;
			int fileDesc = open (tokenizedInstr[tokenizedInstr.size() - 1].c_str(), O_RDONLY | O_CREAT, 0777);
			close (0);
			dup (fileDesc);
			close (fileDesc);
			execvp (args[0], args);
			
			cout << "*** Error in execution! ***" << endl;
			exit(0);
		}
		else {
			int status;
			waitpid (pid, &status, 0);
		}
	}	
}

void runExternalCommand() {
	cout << ">> ";
	string instr, temp;
	cin.ignore();
	getline(cin, instr);
	istringstream is (instr);
	vector<string> tokenizedInstr;
	while (is >> temp) tokenizedInstr.push_back(temp);

	if (tokenizedInstr[0] == "cd") {
		if (tokenizedInstr.size() == 2) {
			int chk = chdir(tokenizedInstr[1].c_str());
			if (chk != 0) {
				cout << "*** Error in cd! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}	
	else {
		char *args[tokenizedInstr.size() + 1];

		for (int i = 0; i < tokenizedInstr.size(); i++) {
			args[i] = (char *)tokenizedInstr[i].c_str();
		}
		args[tokenizedInstr.size()] = NULL;
		pid_t pid = fork();
		if (pid == 0) {
			execvp (tokenizedInstr[0].c_str(), args);
			cout << "*** Error in execution! ***" << endl;
			exit(0);
		}
		else {
			int status;
			waitpid(pid, &status, 0);
		}
	}
}

void runInternalCommand() {
	cout << ">> ";
	string instr, temp;
	cin.ignore();
	getline(std::cin, instr);
	istringstream s2 (instr);
	debug();
	vector<string> tokenizedInstr;
	while (s2 >> temp) tokenizedInstr.push_back(temp);

	if (tokenizedInstr[0] == "mkdir") {
		if (tokenizedInstr.size() == 2) {
			int chk = mkdir(tokenizedInstr[1].c_str(), 0777);
			if (chk == -1) {
				cout << "*** Error in mkdir command! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}
	else if (tokenizedInstr[0] == "rmdir") {
		if (tokenizedInstr.size() == 2) {
			int chk = rmdir(tokenizedInstr[1].c_str());
			if (chk == -1) {
				cout << "*** Error in rmdir command! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}
	else if (tokenizedInstr[0] == "cd") {
		if (tokenizedInstr.size() == 2) {
			int chk = chdir(tokenizedInstr[1].c_str());
			if (chk != 0) {
				cout << "*** Error in cd! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}
	else if (tokenizedInstr[0] == "pwd") {
		if (tokenizedInstr.size() == 1) {
			char path[BUFF];
			char *chk = getcwd(path, BUFF);
			if (chk != NULL) {
				string temp (path);
				cout << path << endl;
			}
			else {
				cout << "*** Error in pwd! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}
	else if (tokenizedInstr[0] == "ls") {
		string path;
		struct dirent **namelist;
		int chk;
		if (tokenizedInstr.size() == 1) {
			path = ".";
		}
		else if (tokenizedInstr.size() == 2) {
			path = tokenizedInstr[1];
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
			return;
		}
		chk = scandir (path.c_str(), &namelist, NULL, alphasort);
		if (chk == -1) cout << "*** Error in ls! ***" << endl;
		else {
			while (chk--) {
				cout << "\t" << string(namelist[chk]->d_name) << endl;
				free (namelist[chk]);
			}
			free (namelist);
		}
	}
}

void runExternalCommandBackground() {
	cout << ">> ";
	string instr, temp;
	cin.ignore();
	getline(cin, instr);
	istringstream is (instr);
	vector<string> tokenizedInstr;
	while (is >> temp) tokenizedInstr.push_back(temp);

	if (tokenizedInstr[0] == "cd") {
		if (tokenizedInstr.size() == 2) {
			int chk = chdir(tokenizedInstr[1].c_str());
			if (chk != 0) {
				cout << "*** Error in cd! ***" << endl;
			}
		}
		else {
			cout << "*** Invalid parameters! ***" << endl;
		}
	}	
	else {
		char *args[tokenizedInstr.size() + 1];

		for (int i = 0; i < tokenizedInstr.size(); i++) {
			args[i] = (char *)tokenizedInstr[i].c_str();
		}
		args[tokenizedInstr.size()] = NULL;
		pid_t pid = fork();
		if (pid == 0) {
			execvp (tokenizedInstr[0].c_str(), args);
			cout << "*** Error in execution! ***" << endl;
		}
	}
}

int main() {
	while (1) {
		char opt;
		displayMenu();
		cout << "~> ";
		cin >> opt;
		switch (opt) {
			case 'a':
				runInternalCommand();
				break;
			case 'b':
				runExternalCommand();
				break;
			case 'c':
				runExternalCommandIP();
				break;
			case 'd':
				runExternalCommandOP();
				break;
			case 'e':
				runExternalCommandBackground();
				break;
			case 'f':
				runExternalCommandPipe();
				break;
			case 'g':
				exit(0);
				break;
			default:
				cout << "Command not recognized! :'(" << endl;
		}
	}
	return 0;
}
