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

void execute(vector<string> & input, int start, int end) {

    //Call execvp, based on which elements in the string vector to use
    char * argv[(end - start)+1];
	    int i = 0;
	    
	    for (i = 0; i <= (end - start); i++) {
		argv[i] = new char[5];
	    }

	    for (i = 0; i < (end-start); i++) {
		strcpy(argv[i], input[i+start].c_str());

	    }
	    
	    argv[i] = NULL;

	    
	    int r = execvp(argv[0], argv);
	    if (r == -1)
		perror("execvp");
		exit(1); 
	
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
	int pid2;
	if (pid == 0) {
	    

	    int start = 0;
	    int end = input.size();
	    int i;
	    

		
loop:		
		end = input.size();
		for (i = start; i < input.size(); i++) {

		    // Check for semi colons
		    if (input[i][input[i].size() - 1] == ';' || input[i] == ";") {
			
			pid2 = fork();
			
			if (pid2!=0) {
			    start = i + 1;
			    goto loop;
			}
			if (input[i] != ";") {
			    input[i] = input[i].substr(0, input[i].size() -1);
			    end = i+1;
			}
			else 
			    end = i;
		    
			break;
		    }
		}

		if (pid2 != 0)
		    wait(NULL);
	   
		

		execute(input, start, end);
	
	
	
	}   	
	else {
	    while(wait(NULL) > 0);
	}

    
    
    }
    
    return 0; 
}
