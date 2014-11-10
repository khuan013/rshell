#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <string.h>
#include <algorithm>
#include <pwd.h>
#include <grp.h>
#include <iomanip>
#include <time.h>

using namespace std;
int fs_digits = 0;

#define FLAG_a 1
#define FLAG_l 2
#define FLAG_R 4

#define RESET "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define BLUE    "\033[34m"      /* Blue */
#define CYAN    "\033[36m"      /* Cyan */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define GRAYBACK    "\033[100m"     /* Grey Background */

void displayfile(string filename, struct stat* sb, int l_flag) {
   
    cout << RESET;

    if (l_flag != 0) {

        struct passwd *pwd;
        struct group *grp;

        if (S_ISDIR(sb->st_mode))
            cout << "d";
        else
            cout << "-";

        // User permissions
        if (S_IRUSR & sb->st_mode)
            cout << "r";
        else
            cout << "-";
        if (S_IWUSR & sb->st_mode)
            cout << "w";
        else
            cout << "-";
        if (S_IXUSR & sb->st_mode)
            cout << "x";
        else
            cout << "-";
        
        // Group permissions
        if (S_IRGRP & sb->st_mode)
            cout << "r";
        else
            cout << "-";
        if (S_IWGRP & sb->st_mode)
            cout << "w";
        else
            cout << "-";
        if (S_IXGRP & sb->st_mode)
            cout << "x";
        else
            cout << "-";
        
        // Other permissions
        if (S_IROTH & sb->st_mode)
            cout << "r";
        else
            cout << "-";
        if (S_IWOTH & sb->st_mode)
            cout << "w";
        else
            cout << "-";
        if (S_IXOTH & sb->st_mode)
            cout << "x";
        else
            cout << "-";

        // Output number of links
        cout << " " << sb->st_nlink;

        // Output owner name
        if ((pwd = getpwuid(sb->st_uid)) != NULL)
            cout << " " << pwd->pw_name;
        else
            perror("getpwuid");

        // Output owner group
        if ((grp = getgrgid(sb->st_gid)) != NULL)
            cout << " " << grp->gr_name;
        else
            perror("getgrgid");

        // Output file size
        cout << " " << setw(fs_digits) << sb->st_size;

        // Output last modified time
        time_t t = sb->st_mtime;
        struct tm * lt = localtime(&t);
        char date[80];
        strftime(date, sizeof(date), "%h %d %R ", lt);
        cout << " " << date << " ";
        
        // Output file name
        if (filename[0] == '.')
            cout << GRAYBACK;

        if (S_ISDIR(sb->st_mode))
            cout << BOLDBLUE;
        else if (S_ISLNK(sb->st_mode))
            cout << CYAN;
        else if (sb->st_mode & S_IXUSR)
            cout << BOLDGREEN;
        

        cout << filename << RESET << endl;
        

    }
    else {
        if (filename[0] == '.')
            cout << GRAYBACK;

        if (S_ISDIR(sb->st_mode))
            cout << BOLDBLUE;
        else if (S_ISLNK(sb->st_mode))
            cout << CYAN;
        else if (sb->st_mode & S_IXUSR)
            cout <<  BOLDGREEN;
        
        cout << filename << RESET <<  "  ";
    }

    return;
}

void display_folder(string path, const int flags) {

    int status = 0;
    DIR * dp;
    struct dirent *dt;

    struct stat statbuf;
    int largest_fsize;

    vector<string> filename;

    // Open directory stream
    if ((dp = opendir(path.c_str())) == NULL) {
        perror("opendir");
        return;
    }

    if ((flags & FLAG_R) != 0)
        cout << path << ":\n";

    while ((dt = readdir(dp)) != NULL) { 
        filename.push_back(dt->d_name);
        string temp = path + "/" + dt->d_name;
        status = stat(temp.c_str(), &statbuf);
        if (status == -1) {
            perror ("stat"); 
            cerr << "EROR: " << temp << endl;
            }
        
        if (statbuf.st_size > largest_fsize)
            largest_fsize = statbuf.st_size;

    }

    // Keep track of how large the file size column must be
    do {
        largest_fsize /=10;
        fs_digits++;
    } while (largest_fsize != 0);

    sort(filename.begin(), filename.end());
    

    for (int i = 0; i<filename.size(); i++) {
        string temp = path + "/" + filename[i];
        status = stat(temp.c_str(), &statbuf);
        if (status == -1) 
            perror("stat");

        if (flags & FLAG_a && filename[i][0] == '.')
            displayfile(filename[i], &statbuf, flags & FLAG_l);

        else if (filename[i][0] != '.')
            displayfile(filename[i], &statbuf, flags & FLAG_l);

    }

    if ((flags & FLAG_l) == 0){
        cout << endl;
    }
    closedir(dp);
    dp = opendir(path.c_str());
 
    if ((flags & FLAG_R) != 0) {

        while ((dt = readdir(dp))) if (strncmp(dt->d_name, ".", 1))  {
            if (dt->d_type == 4) {
                string filePath = path + "/" + dt->d_name;
                cout << endl;
                display_folder(filePath, flags);
            }
        }
    }
    closedir(dp);



}

int main(int argc, char ** argv) {

    int flags = 0;
    int status = 0;


    // Check for ls arguments
    for (int i =1; i<argc; i++) {
        if (argv[i][0]=='-') {
            for (int j=1; argv[i][j]!=0; j++) {
                if (argv[i][j] == 'a')
                    flags  |= FLAG_a;
                else if (argv[i][j] == 'l')
                    flags |= FLAG_l;
                else if (argv[i][j] == 'R')
                    flags |= FLAG_R;
            }
        }
    }

    display_folder((char *)".", flags); 



    return 0;
}
