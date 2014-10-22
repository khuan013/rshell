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


void parse(char * line, vector<string> & input) {
    

    char * pch;
	pch = strtok (line, " \n");
	while (pch!= NULL) {
	    
	    // Ignore after comments
	    if (*pch == '#')
		break;
	    
	    // Check for && and ||
	    // Break input up
	    char * a = strstr(pch, "&&");
	    char * b = strstr(pch, "||");
	    char * c = strstr(pch, ";");

	    // If there are connectors, break up the string
	    // into parts and add them individually to the vector
	    if (a!=NULL || b!=NULL || c!=NULL) {
		while (strlen(pch) != 0 ) {

		    // Checks for && and ||
		    if (*pch == '&' || *pch == '|') {
			
			string tmp;
			tmp += *pch;
			tmp += *(pch+1);
			tmp[2] = '\0';
			
			input.push_back(tmp);
			memmove(pch, pch+2, strlen(pch) - 2);
			pch[strlen(pch)-2] = '\0';
		    }
		    // Check for semicolons
		    else if (*pch == ';') {
			input.push_back(";");
			memmove(pch, pch+1, strlen(pch) -1);
			pch[strlen(pch)-1] = '\0';
		    }
		    else {
			string word;
			int numletters = 0;

			char *d = pch;
			while (d!=a && d!=b && d!=c) {
			    if (*d == '\0')
				break;
			    word += *d;
			    numletters++;
			    d++;
			}
			input.push_back(word);
			memmove(pch, pch+numletters, strlen(pch) -numletters);
			pch[strlen(pch)-numletters] = '\0';
		    }
		
		    a = strstr(pch, "&&");
		    b = strstr(pch, "||");
		    c = strstr(pch, ";");
		}
	    }

	    // If no connectors, just add to vector	
	    if (*pch != '\0' && *pch != '\n' && *pch != '#')
		input.push_back(pch);
	    pch = strtok (NULL, " \n");
	}
}

void execute(vector<string> & input, int start, int end) {

    //Call execvp, based on which elements in the string vector to use
    char * argv[5];
	    
	    int i = 0;
	    
	    for (i = 0; i <= (end - start); i++) {
		argv[i] = new char[5];
	    }

	    for (i = 0; i < (end-start); i++) {
		strcpy(argv[i], input[i+start].c_str());
	    }
	    
	    argv[i] = NULL;

	    int status = execvp(argv[0], argv);
	    if (status == -1)
		perror("execvp");
		exit(1); 
}


int main() {

    vector<string> input; 
    int status=0;

    // Get username
    char * usrname = getlogin();
    if (usrname == NULL){
	perror ("Error getting user name");
	exit(1);
    }

    // Get hostname
    char hostname[20];

    if (gethostname(hostname, sizeof hostname) ==-1) {
	perror("Error getting hostname");
	exit(1);
    }



    // Main loop
    while (1) {
	
	status = 0;
	if (input.size() != 0)
	    input.clear();
		
	cout << usrname << "@" << hostname <<"$ ";

	string string1;
	getline(cin, string1);
	char * line = new char [string1.length() + 1];
	strcpy(line, string1.c_str());

	parse(line, input);

	delete line;

	if (input.size() == 0)
	    continue;
	
	if (strcmp(input[0].c_str(), "exit") == 0)
	    exit(0);


	int pid=fork();
	int pid2;
	if (pid == 0) {
	    

	    int start = 0;
	    int end = input.size();
	    unsigned i;
		
loop:		

	    end = input.size();
	    for (i = start; i < input.size(); i++) {

		
		if (input[i] == ";" || input[i] == "&&" || input[i] == "||") {
		    string connector = input[i];

		    pid2 = fork();
		    if (pid2 == -1) {
			perror("There was an error with fork(). ");
			exit(1);
		    }
		    if (pid2!=0) {
			if (-1 == wait(&status))
			    perror("There was an error with wait().");
			
			if (connector == "&&" && status != 0)
			    exit(1);

			if (connector == "||" && status == 0)
			    exit(0);

			start = i + 1;
			goto loop;

		    }
		    else {
			end = i;
			break;
		    }
		}
	    }

	    execute(input, start, end);

	}   	
	else { 
	    status = wait(NULL);
	    if (status == -1)
		perror("There was an error with wait().");
	}
    }
    
    return 0; 
}
