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
#include <pwd.h>
#include <signal.h>


using namespace std;


void parsePath(const char * line, vector<string> &pathing) {
    
    if (line == NULL)
        return;
    char * s = strdup(line);
    if (!s)
        exit(1);

    char * pch;
    pch = strtok(s, ":");
    while (pch!=NULL) {
        string tmp = string(pch);
        pathing.push_back(tmp);
        pch = strtok(NULL, ":");
    }

    free(s);
}

void parse(char * line, vector<string> & input) {
     
    if (line == NULL)
        return;

    char * pch;
    int find_quote = 0;
	pch = strtok (line, " \n");
	while (pch!= NULL) {
      
        if (pch == NULL)
                break;

     

        find_quote = 0;
        if ((pch + strlen(pch) + 1) == NULL)
                cerr << "hello\n";
        if (((pch +strlen(pch) +1) != NULL) && (*(pch + strlen(pch) + 1) == '"'))
            find_quote = 1;
                    
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
	    if (a!=NULL || b!=NULL || c!=NULL || d!=NULL || e!=NULL || f!=NULL
                    || (isdigit(*pch) && pch[1] == '>')) {
		while (strlen(pch) != 0 ) {
            if (isdigit(pch[0]) && pch[1] == '>') {
                string tmp;
                int stringlen = 0;
                tmp += *pch;
                tmp += pch[1];
                if (pch[1] == '>' && pch[2] == '>') {
                    tmp += pch[2];
                    stringlen = 3;
                    tmp[3] = '\0';
                    }
                else {
                    stringlen = 2;
                    tmp[2] = '\0';
                }
                input.push_back(tmp);
                memmove(pch, pch +stringlen, strlen(pch) - stringlen);
                pch[strlen(pch) - stringlen] = '\0';
            
            }
                
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
            else if (*pch == '<' || *pch =='>' || *pch == '|') {
                if (*pch == '>' && *(pch+1) == '>') {
                    input.push_back(">>");
                    memmove(pch, pch+2, strlen(pch)-2);
                    pch[strlen(pch)-2] = '\0';
                }
                else if (*pch == '<' && *(pch+1) == '<' && *(pch+2)=='<') {
                    input.push_back("<<<");
                    memmove(pch, pch+3, strlen(pch)-3);
                    pch[strlen(pch)-3] = '\0';
                }
                else {
                    string tmp;
                    tmp += *pch;
                    tmp[1] = '\0';
                    input.push_back(tmp);
                    memmove(pch, pch+1, strlen(pch)-1);
                    pch[strlen(pch)-1] = '\0';
                }
            }
            
            else {
			string word;
			int numletters = 0;
			char z = *pch;
			int i = 1;
            while ( z!= '\0' && z!= ';' && z!= '|' && z!= '&' && z!= '<' && z!= '>'
                            && !(isdigit(z) && pch[i] == '>')) {
			    word += z;
			    numletters++;
			    z = pch[i];
                i++;
			}
            if (numletters!=0) 
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
	    if (*pch != '\0' && *pch != '\n' && *pch != '#') {
            input.push_back(pch);

        }

        if (find_quote == 1)
            pch = strtok(NULL, "\"");
        else
	        pch = strtok (NULL, " \n");
	}
}

void execute(const vector<string> & input, const vector<string> & pathing,
        int start, int end) {

    //Call execv, based on which elements in the string vector to use
    char * argv[20];
	    
	    int i = 0;
	    int status;

	    for (i = 0; i <= (end - start); i++) {
		argv[i] = new char[20];
	    }

	    for (i = 0; i < (end-start); i++) {
		strcpy(argv[i], input[i+start].c_str());
            }
	    
	    argv[i] = NULL;
            string cmd = argv[0];

            for (unsigned int j = 0; j < pathing.size(); j++) {
                string tmp = pathing[j] + "/" + cmd;
                strcpy(argv[0], tmp.c_str());
                status = execv(argv[0], argv);
                
            }
	    if (status == -1)
		perror("execv");
		exit(1); 
}

void printusrhost() {

    // Get username
    char * usrname;
    struct passwd *pass = getpwuid(getuid());
    usrname = pass->pw_name;
    if (pass == NULL) {
	perror ("getpwuid");
    exit(1);    
    }

    // Get hostname
    char hostname[20];

    if (gethostname(hostname, sizeof hostname) ==-1) {
	perror("gethostname");
	exit(1);
    }

    //get cwd
    char cwd[512];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("cwd");
        exit(1);
    }

    cout << usrname << "@" << hostname << ":" << cwd << "$ ";

}

void int_handler(int x) {
        cout << endl;
}


int main() {

    if ((signal(SIGINT, int_handler)) == SIG_ERR)
        perror("signal");
    
    vector<string> input; 
    vector<string> pathing;
    int status=0;
    
    const char *pPath;
    pPath = getenv("PATH");
	if (pPath == NULL)
	    perror("PATH");
   
    parsePath(pPath, pathing);
   
    

    int savestdin;
    if ((savestdin = dup(0)) == -1)
        perror("dup");

    int savestdout;
    if ((savestdout = dup(1)) == -1)
        perror("dup");




    // Main loop
    while (1) {
        //Restore stdin and stdout
        if ((dup2(savestdin,0)) == -1)
            perror("dup2");

        if ((dup2(savestdout,0)) == -1)
            perror("dup2");
	
    printusrhost();

	status = 0;

	if (input.size() != 0)
	    input.clear();
		

	string string1;
	getline(cin, string1);

	parse((char *)string1.c_str(), input);

    //debugging
    //cerr << "AFTER PARSING: \n";
    //for (int i = 0; i < input.size(); i++)
            //cerr << input[i] << endl;


	if (input.size() == 0)
	    continue;

    
    for (unsigned int i = 0; i < input.size(); i++) 
	    if (strcmp(input[i].c_str(), "exit") == 0) return 0;
    

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
	    
        for (i = start; i < input.size(); ) {
            
            if (input[i] == "cd") {
                string directory =  input[i+1];
                int ret;
                ret = chdir(directory.c_str());
                if (ret == -1) 
                    perror ("chdir");

                if ((i+2) >= input.size())
                    exit(0);
                else if (input[i+2] == ";" || input[i+2] == "&&") {
                    start = i+3;
                    goto loop;   
                }

            }

            //input output redirection
            if (input[i] == ">" || input[i] == ">>" || input[i]=="<" 
                            || input[i] == "<<<" || input[i] == "|"
                            || (isdigit(input[i][0]) && input[i][1] == '>')) {

                if ((isdigit(input[i][0]) && input[i][1] == '>') || input[i][0] == '>') {
                    
                    int flags = 0;
                    int file_no = 1;
                    flags |= O_CREAT | O_WRONLY;
                    
                    if ((input[i][0] == '>' && input[i][1] == '>') ||
                                input[i][2] == '>')
                            flags |= O_APPEND;
                        else
                            flags |= O_TRUNC;


                    int fdo = open(input[i+1].c_str(), flags, S_IRUSR | S_IWUSR);
                    if (fdo==-1) {
                        perror("open");
                        exit(1);
                    }
                    
                    if (isdigit(input[i][0]))
                        file_no = input[i][0] - '0';

                    status = close(file_no);
                    if (status == -1)
                        perror("close");

                    if ((dup(fdo)) == -1)
                        perror("dup");

                    if ((close(fdo)) == -1)
                        perror("close");

                    input.erase(input.begin() + i);
                    input.erase(input.begin() + i);
    
                    if (input[i][0] == '>' || input[i][0] == '<' || input[i][0] == '|'
                                            || (isdigit(input[i][0]) && input[i][1] == '>'))
                        continue;
                    
                    else {
                        end = i;
                        break;
                    }



                }

                else if (input[i] == "<") {
                 
                        int fdi = open(input[i+1].c_str(), O_RDONLY, 0);
                        if (fdi==-1) {
                            perror("open");
                            exit(1);
                        }

                        status = close(STDIN_FILENO);
                        if (status == -1)
                            perror("close");

                        if ((dup(fdi)) == -1)
                            perror("dup");

                        if ((close(fdi)) == -1)
                            perror("close");
                        
                        input.erase(input.begin() + i);
                        input.erase(input.begin() + i);
                        
                        if (input[i][0] == '>' || input[i][0] == '<' || input[i][0] == '|'
                                            || (isdigit(input[i][0]) && input[i][1] == '>'))
                            continue;
                    
                        else {
                            end = i;
                            break;
                        }
                }

                else if (input[i] == "<<<") {
                        
                    int pfd[2];

                    if ((pipe(pfd)) == -1)
                        perror("pipe");


                        char buf[128];
                        memset(buf, 0, 128);
                        strcpy(buf, input[i+1].c_str());
                            
                        buf[strlen(buf)] = '\n';
                        buf[strlen(buf)+1] = '\0';

                        if ((write(pfd[1], buf, strlen(buf)) == -1))
                            perror("write");

                        if ((dup2(pfd[0], 0)) == -1)
                            perror("dup2");

                        if (close (pfd[1]) == -1)
                            perror("close");
                            
                        input.erase(input.begin() + i);
                        input.erase(input.begin() + i);
                        
                        if (input[i][0] == '>' || input[i][0] == '<' || input[i][0] == '|'
                                            || (isdigit(input[i][0]) && input[i][1] == '>'))
                            continue;
                    
                        else {
                            end = i;
                            break;
                        }
                        
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

                        if ((dup2(pfd[1], 1)) == -1)
                            perror("dup2");

                        if ((close(pfd[0])) == -1)
                            perror("close");
                        
                        end = i;
                        break;

                    }                    
                    else {
                        if((close(STDIN_FILENO)) == -1)
                            perror("close");
                        
                        if ((dup2(pfd[0], 0)) == -1)
                            perror("dup2");
                        
                        if ((close(pfd[1])) == -1)
                            perror("close");
                        
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
	    i++; 
        }

	    execute(input, pathing, start, end);

	}   	
	else { 
	    status = wait(NULL);
	    if (status == -1)
		perror("wait");
	 
            //chdir command
        for (unsigned int i = 0; i < input.size(); i++) {
            if (strcmp(input[i].c_str(), "cd") == 0) {
                string directory =  input[i+1];
                int ret;
                ret = chdir(directory.c_str());
                if (ret == -1) 
                    perror ("chdir");
                
            }
        }
    }
    }
   

   
    
    return 0; 
}
