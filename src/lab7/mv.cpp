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


bool is_dir(const char* pathname)
{
        /*
 *         string p = static_cast<string>(par_dir)
 *                 + "/" + static_cast<string>(pathname);
 *                         char* p2 = const_cast<char*>(p.c_str());
 *                                 */
        struct stat info;
        if( stat( pathname, &info ) != 0 )
        {
                return false; 
                //printf( "cannot access %s\n", pathname );
                //perror("stat");
                //exit(1);
        }
        else if( info.st_mode & S_IFDIR )
        {
            return true;
            //exit(1);
        }
        return false;
}









bool dir = false; //checks if 2nd path is directory
int main( int argc, char ** argv)
{

	if(argc != 3 )
	{
		cerr << "error: invalid number of arguments\n"; 
		exit(1); 
	}

	if( is_dir( argv[2] ) )
	{
		dir = true; 
	}
	string tmp;
	if (dir == true) {	
		tmp = string( argv[2] ) + "/" + string(argv[1] ) + "\0"; 
		cerr << " tmp: " << tmp << endl;

	}
	else {
		tmp = string(argv[2]);
	}
	if( link( argv[1], tmp.c_str() ) == -1) //argv[2]) == -1 )
	{
		perror("link"); 
	}
	if( unlink( argv[1]) == -1 )
	{
		perror("unlink"); 
	}





	return 0; 
}
