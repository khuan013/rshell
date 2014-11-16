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
#include <fcntl.h>

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
        char * d = strstr(pch, "<");
        char * e = strstr(pch, ">");
        char * f = strstr(pch, "|");

	    // If there are connectors, break up the string
	    // into parts and add them individually to the vector
	    if (a!=NULL || b!=NULL || c!=NULL || d!=NULL || e!=NULL || f!=NULL) {
		while (strlen(pch) != 0 ) {

		    // Checks for && and ||
		    if ((pch[0] == '&' && pch[1] == '&') ||
			(pch[0] == '|' && pch[1] == '|')) {
			
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
            // Check for input/output redirectors
            else if (*pch == '<') {
                input.push_back("<");
                memmove(pch, pch+1, strlen(pch) -1);
                pch[strlen(pch)-1] = '\0';
            }
            else if (*pch == '>') {
                if (*(pch+1) == '>') {
                    input.push_back(">>");
                    memmove(pch, pch+2, strlen(pch) -2);
                    pch[strlen(pch)-2] = '\0';
                }
                else {
                    input.push_back(">");
                    memmove(pch, pch+1, strlen(pch) -1);
                    pch[strlen(pch)-1] = '\0';
                }
            }
            else if (*pch == '|') {
                input.push_back("|");
                memmove(pch, pch+1, strlen(pch) -1);
                pch[strlen(pch) -1] = '\0';
            }
		    else {
			string word;
			int numletters = 0;

			char *z = pch;
			while (z!=a && z!=b && z!=c && z!=d && z!=e && z!=f) {
			    if (*z == '\0')
				break;
			    word += *z;
			    numletters++;
			    z++;
			}
			input.push_back(word);
			memmove(pch, pch+numletters, strlen(pch) -numletters);
			pch[strlen(pch)-numletters] = '\0';
		    }
		
		    a = strstr(pch, "&&");
		    b = strstr(pch, "||");
		    c = strstr(pch, ";");
            d = strstr(pch, "<");
            e = strstr(pch, ">");
            f = strstr(pch, "|");
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

        cerr << "execvp sent:\n";

	    for (i = 0; i < (end-start); i++) {
        cerr << input[i+start] << endl;
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

    int savestdin;
    if ((savestdin = dup(0)) == -1)
        perror("dup");

    int savestdout;
    if ((savestdout = dup(1)) == -1)
        perror("dup");

    // Get username
    char * usrname = getlogin();
    if (usrname == NULL){
	perror ("getlogin");
	exit(1);
    }

    // Get hostname
    char hostname[20];

    if (gethostname(hostname, sizeof hostname) ==-1) {
	perror("gethostname");
	exit(1);
    }



    // Main loop
    while (1) {

   	 //Restore stdin and stdout
        if ((dup2(savestdin,0)) == -1)
            perror("dup2");

        if ((dup2(savestdout,0)) == -1)
            perror("dup2");

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

    for (int i = 0; i < input.size(); i++) {
	    if (strcmp(input[i].c_str(), "exit") == 0) exit(0);
        
        //cout << input[i] << endl;
    }


	int pid=fork();
    if (pid == -1)
        perror ("fork");
	int pid2;
	if (pid == 0) {
	    

	    int start = 0;
	    int end = input.size();
	    unsigned i;
		
loop:		

	    end = input.size();
	    
        for (i = start; i < input.size(); i++) {
        
            //input output redirection
            if (input[i] == ">" || input[i] == ">>" || input[i]=="<" || input[i] == "|") {


                if (input[i] == ">") {

                        int fdo = open(input[i+1].c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
                        if (fdo==-1)
                            perror("open");

                        status = close(STDOUT_FILENO);
                        if (status == -1)
                            perror("close");

                        if ((dup(fdo)) == -1)
                            perror("dup");

                        if ((close(fdo)) == -1)
                            perror("close");

                        end = i;
                        break;
                    }
                    
            
                else if (input[i] == ">>") {
                    
                        int fdo = open(input[i+1].c_str(), O_CREAT | O_WRONLY | O_APPEND, 
                                     S_IRUSR | S_IWUSR);
                        if (fdo==-1)
                            perror("open");

                        status = close(STDOUT_FILENO);
                        if (status == -1)
                            perror("close");

                        if ((dup(fdo)) == -1)
                            perror("dup");

                        if ((close(fdo)) == -1)
                            perror("close");

                        end = i;
                        break;
                }

                else if (input[i] == "<") {
                 
                        int fdi = open(input[i+1].c_str(), O_RDONLY, 0);
                        if (fdi==-1)
                            perror("open");

                        status = close(STDIN_FILENO);
                        if (status == -1)
                            perror("close");

                        if ((dup(fdi)) == -1)
                            perror("dup");

                        if ((close(fdi)) == -1)
                            perror("close");
                    
                        end = i;
                        break;
                }

                else if (input[i] == "|") {
                    int pfd[2];

                    if ((pipe(pfd)) == -1)
                        perror("pipe");

                    
                    int pid3 = fork();
                    if (pid3 == -1) {
                        perror("fork");
                        exit(1);
                    } 
                    if (pid3==0) {
                        cout << "This is the child process";

                        if ((dup2(pfd[1], 1)) == -1)
                            perror("dup2");

                        if ((close(pfd[0])) == -1)
                            perror("close");
                        
                        end = i;
                        break;

                    }                    
                    else {
                        close(STDIN_FILENO);
                        dup2(pfd[0], 0);
                        close(pfd[1]);
                        
                        if (-1 == wait(0))
                            perror("wait");

                        start = i+1;
                        goto loop;

                    }


                }
            }
        

            else if (input[i] == ";" || input[i] == "&&" || input[i] == "||") {
		        string connector = input[i];

		        pid2 = fork();
		        if (pid2 == -1) {
			        perror("fork");
			        exit(1);
		        }
		        if (pid2!=0) {
			        if (-1 == wait(&status))
			            perror("wait");
			
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
		perror("wait");
	}
    }
    
    return 0; 
}
