//Roll No.: 2018201018
//Name: Ravi Jakhania


#ifndef ASSIGNMENT1
#define ASSIGNMENT1

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

void snapsht(const char path[],const char destination[]);
void printlist(int startline);
int display_info(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf);
int search();
int copy_file(const char* src_path, const struct stat* sb, int typeflag);
int removedir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf);
int rmdir(const char *path);
int copy_directory(const char* src_root);
bool copyFile(const char *SRC, const char* DEST);
string makefullpath(string str1);
string findname(string str1);
string findpath(string tempstr);
void statusbar(string tempstr,const char current_path[]);
void ls(int startline,const char path[]);
void gotoNonCanon();


#endif