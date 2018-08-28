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
#include <iostream>
#include <termios.h>
#include <stack>
using namespace std;
stack <string> s1,s2;
char *name[1000];
int i=2,j=1,p=2,n,level=0;
char arr[100],buf[512];
    DIR *thedirectory;

    struct dirent **thefile;
    struct stat thestat;
    struct passwd *tf; 
    struct group *gf;
string findpath(string s1)
{
  int i=s1.length()-1;
  while(s1[i]!='/')
        i--;
      s1[i]='\0';
    return s1;
}
void statusbar(string str,const char path[])
{   
    string s1;
    s1=path;
    s1[0]='~';
   // cout<<"\033[5;7mPath:"<<s1<<"\033[0m\n";
    int i=34,j=1;
    cout<<"\033["<<i<<";"<<j<<"H";
    for(int i=0;i<143;i++)
      cout<<"=";
    cout<<"\n";
    cout<<"\033[5;7m"<<str<<"\033[0m";
    cout<<"  \033[5;7mPath:"<<s1<<"\033[0m";
    for(int i=0;i<66;i++)
      cout<<" ";
    cout<<"\n";
    for(int i=0;i<143;i++)
      cout<<"=";
    
  // cout<<"\nhello";
  // cout<<"\nhello";
  }

void ls(int a,const char path[])
{

    cout<<"\033[3J";
    cout<<"\033[H\033[J";

  //  cout<<level;
    int i,k;
    n = scandir(path, &thefile, NULL, alphasort);
    if (n == -1) 
    {
      perror("scandir");
      exit(EXIT_FAILURE);
    }
  //  thedirectory = opendir(path);

    k=n;
    if(k>33)
      k=a+33;
    if(k>n)
      k=n;
 //  // cout<<thefile[i]->d_name<<"\n";
   /* string s1;
    s1=path;
    s1[0]='~';
    cout<<"\033[5;7mPath:"<<s1<<"\033[0m\n"; */
   // char s[10000]="Name";
  //  printf("\033[0;7m%-25s\033[0m",s);
   cout<<"\n";
    for(i=a;i<k;i++)
    {   
        sprintf(buf,"%s/%s",path, thefile[i]->d_name);
        
        stat(buf, &thestat);

        printf("%-25s ", thefile[i]->d_name); 
       // cout<<c<<" ";
        printf("%2.1f KB  ",(thestat.st_size)/1024.0);
           
        printf( (thestat.st_mode & S_IRUSR) ? "r" : "-");
        printf( (thestat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (thestat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (thestat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (thestat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (thestat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (thestat.st_mode & S_IROTH) ? "r" : "-");
        printf( (thestat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (thestat.st_mode & S_IXOTH) ? "x" : "-");

        strcpy(arr,ctime(&thestat.st_mtime));
        arr[24]='\0';
        printf(" %s ", arr);
        if(i<k-1)
        printf("\n");

    }
}

void gotoNonCanon()
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

int main()
{
	char ch;
  char pop[10000];
  char path[1000]=".";

  gotoNonCanon();

  s1.push(path);
  ls(0,path);
  //cout<<path;
  statusbar("NORMAL MODE",path);
  cout<<"\033["<<i<<";"<<j<<"H";
 // cout<<p;
  while(1)
  {
    ch = getchar();
  //  cout<<ch;
    /*if(ch == ':')
    {
      i=36;
      cout<<"\033["<<i<<";"<<j<<"H";
      cout<<":";
      //
    }*/
    if((ch == 'h' || ch == 'H')  && !(strcmp(s1.top().c_str(),".")==0))
    {
      strcpy(path,".");
      ls(0,path);
      statusbar("NORMAL MODE",path);
      s1.push(".");
      i=2;
      j=1;
      p=2;
      cout<<"\033["<<i<<";"<<j<<"H";
    }
    if(ch == 127 && !(strcmp(s1.top().c_str(),".")==0))
    {
        string S1=findpath(s1.top());
        strcpy(path,S1.c_str());
        ls(0,path);
        statusbar("NORMAL MODE",path);
        s1.push(path);
        i=2;
        j=1;
        p=2;
        cout<<"\033["<<i<<";"<<j<<"H";    
    }

    else if (ch == '\033') 
    { // if the first value is esc
      getchar(); // skip the [
      switch(getchar()) 
      { // the real value
        case 'A':
            if(i>2)
            { 
              i--;
              p--;
              cout<<"\033["<<i<<";"<<j<<"H";
            //  cout<<p;
            }
            else if(i==2 && p>2)
            {
              p--;
              ls(p-i,path);
              statusbar("NORMAL MODE",path);
              cout<<"\033["<<i<<";"<<j<<"H";
            //  cout<<p;
            } 
            break;  
        case 'B':
          if(p<n+1 && i<33)
          {
            i++;
            p++;
            cout<<"\033["<<i<<";"<<j<<"H";
          //  cout<<p;
            //cout<<"hhhh";
          }
          else if(p<n+1 && i==33)
          {
              p++;
              ls(p-i,path);
              statusbar("NORMAL MODE",path);
              cout<<"\033["<<i<<";"<<j<<"H";
            //  cout<<p;
          }
          break;
        case 'C':
        //  printf("hel");
          if(!s2.empty())
          {
              string S3;              
              S3=s2.top();
              s2.pop();
              s1.push(S3);
          
              strcpy (path,S3.c_str());
              ls(0,path);
              statusbar("NORMAL MODE",path);
              i=p=2;
              j=1;
              cout<<"\033["<<i<<";"<<j<<"H";
              
          }
          break;
        case 'D':
          if(s1.size()!=1)
          {
              string S3;
              S3=s1.top();
              s2.push(S3);
              s1.pop();

              S3=s1.top();
              strcpy (path,S3.c_str());

              ls(0,path);
              statusbar("NORMAL MODE",path);
              i=p=2;
              j=1;
              cout<<"\033["<<i<<";"<<j<<"H";
               
          }
          break;
      }
    }
    else if(ch == '\n')
    {
      string s4,s5,s6;
      stat(thefile[p-2]->d_name, &thestat);
      if((thestat.st_mode & S_IFMT)==S_IFDIR)
      {// cout<<"this is dir";
          s4=s1.top();

        if(!(strcmp(thefile[p-2]->d_name,"..")==0 && (s4[s4.length()-1]=='.')) && !(strcmp(thefile[p-2]->d_name,".")==0))
        {
          strcat(path,"/");
          if((strcmp(thefile[p-2]->d_name,"..")==0))
          {
              s5=findpath(s1.top());
              strcpy(path,s5.c_str());
          }
          else
          strcat(path,thefile[p-2]->d_name);
        s1.push(path);
        ls(0,path);
        statusbar("NORMAL MODE",path);
       // level++;
        i=2;
        j=1;
        p=2;
        cout<<"\033["<<i<<";"<<j<<"H";        //cout<<thefile[i-1]->d_name;
        }
      }
      else if((thestat.st_mode & S_IFMT)!=S_IFBLK && (thestat.st_mode & S_IFMT)!=S_IFCHR && (thestat.st_mode & S_IFMT)!=S_IFIFO && (thestat.st_mode & S_IFMT)!=S_IFLNK && (thestat.st_mode & S_IFMT)!=S_IFSOCK)
      {
        string S1="/usr/bin/xdg-open ";
        //S1.append(" ");
       // cout<<S1;
        S1.append(thefile[p-2]->d_name);
        //cout<<S1;
        system(S1.c_str());
      }
      
    }
  } 
// tcsetattr(fileno(input),TCSANOW,&initial_settings);
//	fflush(stdin);
	//cout<<"----------------------------------------------------------------------------------------------------\n";
	//cout<<"NORMAL MODE\n"; */
//	cin >
}