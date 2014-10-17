#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>

using namespace std;

int main() {




    while (1) {
		
	string cmd;
	string arg1;
	string arg2;

	cout << "Please enter cmd: ";
	getline(cin, cmd);
	cout << "Please enter arg1: ";
	getline(cin, arg1);
	cout << "Please enter arg2: ";
	getline(cin, arg2);


	
	int pid=fork();
	if (pid ==0) {

	   cout << "i'm a child\n";
	    char * argv[4];

	    argv[0] = new char[3];
	    argv[1] = new char[3];
	    argv[2] = new char[3];

	    strcpy(argv[0], cmd.c_str());
	    strcpy(argv[1], arg1.c_str());
	    strcpy(argv[2], arg2.c_str());
	    argv[3] = NULL;

	    cout << "I REACHED EXECVP!!\n";

	    execvp(cmd.c_str(), argv);


	}
	else {
	    wait(NULL);
	

	}	

    }
    
    return 0;
}
