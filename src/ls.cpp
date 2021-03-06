#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
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
#define FLAG_ARGS 8

#define RESET "\033[0m"
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define BLUE    "\033[34m"      /* Blue */
#define CYAN    "\033[36m"      /* Cyan */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define GRAYBACK    "\033[100m"     /* Grey Background */



void displayfile(string filename, struct stat* sb, int l_flag, int largest_name) {
   
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
        if (largest_name != 0)
            cout << setw(largest_name + 2) << left << filename << RESET ;

        else 
            cout << filename << "  " << RESET;
    }

    return;
}

void display_folder(string path, const int flags) {

    int status = 0;
    DIR * dp;
    struct dirent *dt;

    struct stat statbuf;
    int largest_fsize;
    int largest_name = 0;
    unsigned int total_length=0;

    unsigned int total_blocks = 0;

    struct winsize w;
    status = ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (status == -1)
        perror("ioctl");


    // In recursive call reset column of digits to 0
    fs_digits = 0;

    vector<string> filename;

    // Open directory stream
    if ((dp = opendir(path.c_str())) == NULL) {
        perror("opendir");
        return;
    }

    if ((flags & FLAG_R) != 0 || (flags & FLAG_ARGS) != 0)
        cout << path << ":\n";

    while ((dt = readdir(dp)) != NULL) { 
        filename.push_back(dt->d_name);
        string temp = path + "/" + dt->d_name;
        status = stat(temp.c_str(), &statbuf);
        if (status == -1) {
            perror ("stat"); 
            cerr << "EROR: " << temp << endl;
            }
       
        int i;
        for (i= 0; dt->d_name[i] != '\0'; i++);
            ;

        total_length += i + 1;

        if (i > largest_name) 
            largest_name = i;
        
        if (statbuf.st_size > largest_fsize)
            largest_fsize = statbuf.st_size;

        // Add blocksize of each file to total sum
        if ((flags & FLAG_a) == 0 && dt->d_name[0] == '.') ;
        else 
            total_blocks += statbuf.st_blocks;
        
        

    }


    int num_columns = w.ws_col / (largest_name+2);

    // Keep track of how large the file size column must be
    do {
        largest_fsize /=10;
        fs_digits++;
    } while (largest_fsize != 0);

    sort(filename.begin(), filename.end());
    
    // output the number of 1024 blocks by dividing the number of
    // 512 blocks in 2
    if ((flags & FLAG_l))
        cout << "total " << total_blocks / 2 << endl;

    if (total_length < w.ws_col)
        largest_name = 0;
        

    int count =0;
    for (unsigned int i = 0; i<filename.size(); i++) {
        string temp = path + "/" + filename[i];
        status = stat(temp.c_str(), &statbuf);
        if (status == -1) 
            perror("stat");
        
        if ((flags & FLAG_a) && filename[i][0] == '.') {
            displayfile(filename[i], &statbuf, flags & FLAG_l, largest_name);
            count++;
            }

        else if (filename[i][0] != '.') {
            displayfile(filename[i], &statbuf, flags & FLAG_l, largest_name);
            count++;
            }
        
        if (count == num_columns && largest_name != 0) {
            count = 0;
            cout << endl;
            }


    }

    if ((flags & FLAG_l) == 0){
        cout << endl;
    }

    status = closedir(dp);
    if (status == -1)
        perror("closedir");

   if ((dp = opendir(path.c_str())) == NULL) {
        perror("opendir");
        return;
    }

    if ((flags & FLAG_R) != 0) {

        while ((dt = readdir(dp))) if (strncmp(dt->d_name, ".", 1))  {
            if (dt->d_type == 4) {
                string filePath = path + "/" + dt->d_name;
                cout << endl;
                display_folder(filePath, flags);
            }
        }
    }
    status = closedir(dp);
    if (status == -1)
        perror("closedir");



}

int main(int argc, char ** argv) {

    int flags = 0;

    vector<string> folders;


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
        else
            folders.push_back(argv[i]);
    }

    
    if (folders.size() == 0)
        display_folder(".", flags);

    else if (folders.size() == 1)
        display_folder(folders[0], flags);
    else {
        flags |= FLAG_ARGS;
        for (unsigned int i = 0; i < folders.size(); i++) {
            
            display_folder(folders[i], flags);
            cout << endl;
        }
    }



    return 0;
}
