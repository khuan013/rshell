#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <vector>

using namespace std;

int main() {

    char buffer[512];
    vector<string> input; 

    while (1) {

	if (input.size() != 0)
	    input.clear();
		
	cout << "Please enter cmd: ";
	fgets(buffer, 512, stdin);
	
	char * pch;
	pch = strtok (buffer, " \n");
	while (pch!= NULL) {
	    input.push_back(pch);
	    pch = strtok (NULL, " \n");
	}



	
	int pid=fork();
	if (pid ==0) {

	    char * argv[input.size()+1];


	    for (int i = 0; i <= input.size(); i++) {
		argv[i] = new char[5];
		if (i != input.size())
		    strcpy(argv[i], input[i].c_str());
	    }

	    argv[input.size()] = NULL;



	    int r = execvp(argv[0], argv);
	    if (r == -1)
		perror("execvp");


	}
	else {
	    wait(NULL);
	

	}	

    }
    
    return 0; 
}
