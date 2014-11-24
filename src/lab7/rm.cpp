#include <string>
#include <string.h>
#include <cstring>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string.h>
using namespace std;

bool rflag = false; 

void print ( char ** argv )
{
        if( argv == NULL )
        {
                return;
        }
        for (unsigned i = 0; argv[i] != '\0'; ++i )
        {
                printf( argv[i]);
                cout << " ";
        }
        cout << endl;
}


void flags (int argc, char *const* argv)
{
        int c;
        opterr = 0;

        while( (c = getopt (argc, argv, "r")) != -1 )
        {
                switch (c)
                {
                        case 'r':
                                rflag = true;
                                break;
                        case '?':
                                if (isprint (optopt))
                                  fprintf (stderr, "rm: invalid option -- '%c'.\nTry 'r'\n", optopt);
                                else
                                  fprintf (stderr,
                                           "Unknown option character `\\x%x'.\n", optopt);
                        default:
                                abort ();
                }
                  
	}
}


//bool is_dir(char *par_dir ,const char* pathname)
bool is_dir(const char* pathname)
{
	/*
        string p = static_cast<string>(par_dir)
        + "/" + static_cast<string>(pathname);
        char* p2 = const_cast<char*>(p.c_str());
	*/
        struct stat info;
        if( stat( pathname, &info ) != 0 )
        {
            	printf( "cannot access %s\n", pathname );
                perror("stat");
                exit(1);
        }
        else if( info.st_mode & S_IFDIR )
        {
            //printf( "%s is a directory\n", pathname );
            return true;
            exit(1);
        }
        //cout << pathname << " is not a directory." << endl; 
        return false;
}


bool is_file(const char* pathname)
{
        struct stat info;
        if( stat( pathname, &info ) != 0 )
        {
            printf( "ls: cannot access %s -- ", pathname );
                perror("stat");
                return false;

        }
        else if( info.st_mode & S_IFREG )
        {
                return true;
        }
        return false;
}

void check( char ** argv )
{
	for(unsigned i = 1; argv[i] != NULL ; ++i)
	{
		if( argv[i][0] == '-')
		{
			continue; 
		}
		//will also give error if invalid pathname
		if( !rflag && is_dir( argv[i] ) )
		{
			cerr << "error: r flag not passed in, but have directory as argument\n"; 
			exit(1); 
		}
		

		if(!is_dir(argv[i]) )
		{
			if( unlink(argv[i] ) == -1)
			{
				perror("unlink"); //printf("\n unlink() failed - [%s]\n",strerror(errno));
			}
			else continue; 
		}
		else{ //if( is_dir(argv[i])) { 
			if(rmdir(argv[i]) == -1)
			{
				perror("rmdir"); 
			}
		}
	    

	}
}



int main( int argc, char ** argv)
{

	if(argc == 1)
	{
		cerr << "error: invalied number of arguments.\n";
		exit(1); 
	}
	flags(argc, argv);
	check( argv );
	//print(argv);
	



	

	return 0; 
}
