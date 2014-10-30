#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <pwd.h>
#include <grp.h>
#include <iomanip>
#include <time.h>

using namespace std;

#define FLAG_a 1
#define FLAG_l 2
#define FLAG_R 4

int fs_digits = 0;


void displayfile(string filename, struct stat* sb, int l_flag) {
    
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
        struct tm * lt = gmtime(&t);
        char date[80];
        strftime(date, sizeof(date), "%h %d %R ", lt);
        cout << " " << date << filename;



    }
    else {
        cout << filename << " ";
    }

    cout << endl;
    return;
}

int main(int argc, char ** argv) {

    int flags = 0;
    int status = 0;

    vector<string> filename;

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

    DIR * dp;
    struct dirent *dt;

    struct stat statbuf;
    int largest_fsize;

    // Open directory stream
    if ((dp = opendir(".")) == NULL) {
        perror("opendir");
        return 0;
    }

    while ((dt = readdir(dp)) != NULL) { 
        filename.push_back(dt->d_name);
        status = stat(dt->d_name, &statbuf);
        if (status == -1)
            perror ("stat");
        
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
        status = stat(filename[i].c_str(), &statbuf);
        if (status == -1)
            perror("stat");

        if (flags & FLAG_a && filename[i][0] == '.')
            displayfile(filename[i], &statbuf, flags & FLAG_l);

        else if (filename[i][0] != '.')
            displayfile(filename[i], &statbuf, flags & FLAG_l);

    }




    cout << endl;

    return 0;
}