#include <iostream>
#include <unistd.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <vector>
#include <stdlib.h>

using namespace std;


void parse(char* line, vector<string> & input) {
    
    char * pch;
	pch = strtok (line, " \n");
	while (pch!= NULL) {
	    if (*pch == '#')
		break;
	    input.push_back(pch);
	    pch = strtok (NULL, " \n");
	}
}

int main() {

    char buffer[512];
    vector<string> input; 

    while (1) {

	if (input.size() != 0)
	    input.clear();
		
	cout << "Please enter cmd: ";
	fgets(buffer, 512, stdin);

	parse(buffer, input);	
	

	if (strcmp(input[0].c_str(), "exit") == 0)
		exit(0);


	
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
		exit(1); 
	}
	
	
	else {


	    wait(NULL);
	}	

    }
    
    return 0; 
}
