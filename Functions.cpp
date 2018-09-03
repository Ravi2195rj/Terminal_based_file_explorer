#include "assignment1.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>
#include <termios.h>
#include <stack>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <ftw.h>
#include <sys/ioctl.h>

using namespace std;

extern stack <string> stack1,stack2;
extern vector <string> Path,Name;

extern char buf[1000],path[1000],pop[1000];
extern int curser_row,curser_column,file_num,total_files,level,search_flag,total_rows,total_columns,listend,commandmodeline;
extern string dst_root;

extern DIR *point_to_directory;
extern struct dirent **point_to_file;
extern struct stat file_stat;
extern struct winsize win;

void snapsht(const char path[],const char destination[]) //for snapshot
{
    std::ofstream outfilepointer;
    outfilepointer.open(destination, std::ios_base::app);
    struct dirent **point_to_file1;
    struct stat file_status;

    char temparr[100],buf1[1000];
    int i,total_files;
    total_files = scandir(path, &point_to_file1, NULL, alphasort);

    outfilepointer << path<<":"<<endl;

    if(total_files>2)
    {
        for(i=2;i<total_files;i++)
            outfilepointer << point_to_file1[i]->d_name<<"   ";   
        
        outfilepointer <<endl<<endl;
    

        for(i=2;i<total_files;i++)
        {
            sprintf(buf1,"%s/%s",path, point_to_file1[i]->d_name);
            stat(buf1, &file_status);
            if((file_status.st_mode & S_IFMT)==S_IFDIR)
                snapsht(buf1,destination);
        }   
    }
    else
    {
        outfilepointer <<endl;
    }
    outfilepointer.close();
}

void printlist(int startline) // printing list of search found
{   
	cout<<"\033[3J";
    cout<<"\033[H\033[J";
    cout<<"\n";
    int lastline;
    lastline=total_files;

    if(lastline>listend)
        lastline=startline+listend;
    if(lastline>total_files)
        lastline=total_files;

    for(int i=startline;i<lastline;i++)
    {

        cout<<Name[i]<<"  "<<Path[i]<<flush;
        if(i<lastline-1)
            cout<<"\n"<<flush;
    }
}

int display_info(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf)
{
   // cout<<dst_root<<flush;
    if(strcmp((fpath + ftwbuf->base),dst_root.c_str())==0)
    {
        Path.push_back(fpath);
        Name.push_back(fpath + ftwbuf->base);
    }
    return 0;           /* To tell nftw() to continue */
}
int search()
{
    return nftw(stack1.top().c_str(), display_info, 20, FTW_DEPTH);
}
int copy_file(const char* src_path, const struct stat* sb, int typeflag)  //copy Directory
{
    std::string dst_path = dst_root + src_path;
    switch(typeflag) 
    {
        case FTW_D:
            mkdir(dst_path.c_str(), sb->st_mode);
            break;
        case FTW_F:
            std::ifstream  src(src_path, std::ios::binary);
            std::ofstream  dst(dst_path, std::ios::binary);
            stat(src_path, &file_stat);
    		chmod(dst_path.c_str(), file_stat.st_mode);
            dst << src.rdbuf();
    }
    return 0;
}
int removedir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) //remove directory
{
    int rv = remove(fpath);
    if (rv)
        perror(fpath);
    return rv;
}
int rmdir(const char *path)
{
    return nftw(path, removedir, 64, FTW_DEPTH);
}
int copy_directory(const char* src_root)
{
    return ftw(src_root, copy_file,20);
}
bool copyFile(const char *SRC, const char* DEST) /// copy file
{
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
    stat(SRC, &file_stat);
    chmod(DEST, file_stat.st_mode);
    return src && dest;
}
string makefullpath(string str1) //making absolute path
{
    string str2;
    if(str1[0]=='~')
    {
        str2.append(".");
        str2.append(str1.begin()+1,str1.end());
        return str2;
    }
    else if(str1[0]=='/')
    {
        str2.append(".");
        str2.append(str1.begin(),str1.end());
        return str2;
    }
    else if(str1[0]=='.' && str1[1]!='.')
    {
        str2.append(stack1.top());
        str2.append(str1.begin()+1,str1.end());
        return str2;   
    }
    else if(str1[0]=='.' && str1[1]=='.')
    {
        if(stack1.top()!=".")
        {
            string tempstr=stack1.top();
            int i=tempstr.length()-1;
            while(tempstr[i]!='/')
                i--;

            str2.append(tempstr.begin(),tempstr.begin()+i);
            str2.append(str1.begin()+2,str1.end());    
            return str2;
        }
        else
            return str1;
    }
    else
    {
        str2.append(stack1.top());
        str2.append("/");
        str2.append(str1);
        return str2;
    }
}
string findname(string str1) //finding current directory
{
    int i=str1.length()-1;
    string str2;
    while(str1[i]!='/')
        --i;

    str2=str1.substr(i+1,str1.length()-i-1); 
    return str2;
}
string findpath(string tempstr) //finding parent path
{
    int i=tempstr.length()-1;
    while(tempstr[i]!='/')
        i--;
    tempstr[i]='\0';
    return tempstr;
}

void statusbar(string tempstr,const char current_path[]) //printing status bar and path
{   
    string newpath;
    newpath=current_path;
    newpath[0]='~';
    int i=listend+1,j=1;

    cout<<"\033["<<i<<";"<<j<<"H";

    for(int i=0;i<total_columns;i++)
        cout<<"=";

    cout<<"\n";
    cout<<"\033[5;7m"<<tempstr<<"\033[0m";
    cout<<"  \033[5;7mPath:"<<newpath<<"\033[0m";

    // for(int i=0;i<66;i++)
    //     cout<<" ";

    cout<<"\n";

    for(int i=0;i<total_columns;i++)
        cout<<"=";
}

void ls(int startline,const char path[]) //listing files and folders
{

    cout<<"\033[3J";
    cout<<"\033[H\033[J";
    int i,lastline;
    char temparr[100];

    total_files = scandir(path, &point_to_file, NULL, alphasort);
    
    if (total_files == -1) 
    {
        perror("scandir");
        exit(EXIT_FAILURE);
    }

    lastline=total_files;
    if(lastline>listend)
        lastline=startline+listend;
    if(lastline>total_files)
        lastline=total_files;

   cout<<"\n";
    for(i=startline;i<lastline;i++)
    {   
        sprintf(buf,"%s/%s",path, point_to_file[i]->d_name);
        
        stat(buf, &file_stat);
 
        printf( (file_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf( (file_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (file_stat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (file_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (file_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (file_stat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (file_stat.st_mode & S_IROTH) ? "r" : "-");
        printf( (file_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (file_stat.st_mode & S_IXOTH) ? "x " : "- ");
       // char temparr[100]
        printf(" %5.1f KB  ",((file_stat.st_size)/1024.0));

        strcpy(temparr,ctime(&file_stat.st_mtime));
        temparr[24]='\0';
        printf(" %s  ", temparr);
        printf(" %-s ", point_to_file[i]->d_name);
        if(i<lastline-1)
            printf("\n");

    }
}

void gotoNonCanon() //going to non canon
{
    struct termios initial_settings, new_settings;
    FILE *input;
    FILE *output;

    input = fopen("/dev/tty", "r");
    output = fopen("/dev/tty", "w");
    tcgetattr(fileno(input),&initial_settings);
    new_settings = initial_settings;
    new_settings.c_lflag &= ~ICANON;
    new_settings.c_lflag &= ~ECHO;

    if(tcsetattr(fileno(input), TCSANOW, &new_settings) != 0) 
    {
        fprintf(stderr,"could not set attributes\n");
    }
  
//  tcsetattr(fileno(input),TCSANOW,&initial_settings);
}