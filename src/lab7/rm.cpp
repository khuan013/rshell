#include <string>
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

void removeDir(string dirname) {

	DIR * dp;
	struct dirent *dt;
	struct stat statbuf;
	int status = 0;	

	vector<string> filename;

	if ((dp = opendir(dirname.c_str())) == NULL) {
		perror("opendir");
		return;
	}
	
	
	while ((dt = readdir(dp)) != NULL) {
		if (dt->d_name[0] == '.')
			continue;
		string tmp = dirname + "/" + dt->d_name;
		//cerr << "tmp: " << tmp << endl;
		filename.push_back(tmp);
	}

	closedir(dp);

	if ((dp=opendir(dirname.c_str()))== NULL) {
		perror("opendir");
		return;
	}
	
	while ((dt = readdir(dp))) if (strncmp(dt->d_name, ".", 1)) {
		if (dt->d_type == 4) {
			string dirPath = dirname + "/" + dt->d_name;
			removeDir(dirPath);
		}
	}

	
	for (int i = 0; i < filename.size(); i++) {
		if (!is_dir(filename[i].c_str())) {
			//cerr << "file name: " << filename[i] << endl;
			if (unlink(filename[i].c_str()) == -1)
				perror("unlink");
		}

		else{
			//cerr << "dir name: " << filename[i] << endl;
			if (rmdir(filename[i].c_str()) == -1)
				perror("rmdir");
		}
	}

	closedir(dp);

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
	   		removeDir(argv[i]);
			rmdir(argv[i]);
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
