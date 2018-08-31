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
#include <vector>
#define commandmodeline 37
using namespace std;

stack <string> stack1,stack2;

char buf[1000],path[1000]=".",pop[1000];
int curser_row=2,curser_column=1,file_num=2,total_files,level=0;

DIR *point_to_directory;
struct dirent **point_to_file;
struct stat file_stat;
//struct passwd *tf; 
//struct group *gf;

string findpath(string tempstr)
{
    int i=tempstr.length()-1;
    while(tempstr[i]!='/')
          i--;
    tempstr[i]='\0';
    return tempstr;
}

void statusbar(string tempstr,const char current_path[])
{   
    string newpath;
    newpath=current_path;
    newpath[0]='~';
    int i=34,j=1;
    cout<<"\033["<<i<<";"<<j<<"H";
    for(int i=0;i<143;i++)
        cout<<"=";
    cout<<"\n";
    cout<<"\033[5;7m"<<tempstr<<"\033[0m";
    cout<<"  \033[5;7mPath:"<<newpath<<"\033[0m";
    for(int i=0;i<66;i++)
        cout<<" ";
    cout<<"\n";
    for(int i=0;i<143;i++)
        cout<<"=";
}

void ls(int startline,const char path[])
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
    if(lastline>33)
        lastline=startline+33;
    if(lastline>total_files)
        lastline=total_files;

   cout<<"\n";
    for(i=startline;i<lastline;i++)
    {   
        sprintf(buf,"%s/%s",path, point_to_file[i]->d_name);
        
        stat(buf, &file_stat);

        printf("%-25s ", point_to_file[i]->d_name); 
       // cout<<c<<" ";
        printf("%2.1f KB  ",(file_stat.st_size)/1024.0);
           
        printf( (file_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf( (file_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf( (file_stat.st_mode & S_IXUSR) ? "x" : "-");
        printf( (file_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf( (file_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf( (file_stat.st_mode & S_IXGRP) ? "x" : "-");
        printf( (file_stat.st_mode & S_IROTH) ? "r" : "-");
        printf( (file_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf( (file_stat.st_mode & S_IXOTH) ? "x" : "-");
       // char temparr[100]
        strcpy(temparr,ctime(&file_stat.st_mtime));
        temparr[24]='\0';
        printf(" %s ", temparr);
        if(i<lastline-1)
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
    gotoNonCanon();
   // path=".";
    stack1.push(path);
    ls(0,path);
    statusbar("NORMAL MODE",path);
    curser_row=2,curser_column=1;
    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
  while(1)
  {
    char buffer[128];
    read(0,buffer,128);

    if(buffer[0]==58) //colon
    {
        statusbar("COMMAND MODE",path);
      int flag=1,curser=0;
      //cout<<"in command mode"<<flush;
      //i=37,j=1;
      cout<<"\033["<<commandmodeline<<";1H> "<<flush;
      curser_column=3;
      while(flag)
      { 
            char command[500],temp[]="                                                                                                                   ";
            read(0,buffer,128);
            
            if(buffer[0]==27 && buffer[1]!=91)//esc key
            {
              //  cout<<"\nescap pressed: coming to normal mode"<<flush;
                ls(0,path);
                statusbar("NORMAL MODE",path);
                curser_row=2;
                curser_column=1;
                file_num=2;
                cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                flag=0;
            }
            else if(buffer[0]!=27 && buffer[0]!=127 && buffer[0]!=10)//any key data
            {
                command[curser++]=buffer[0];
                cout<<buffer[0]<<flush;
                cout<<"\033["<<commandmodeline<<";"<<++curser_column<<"H"<<flush;
            }
            else if(buffer[0]==127 || buffer[0]==8) /// back space
            {
                if(curser_column>3)
                {
                    command[--curser]=' ';
                    command[curser+1]='\0';
                    //curser--;
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    cout<<command<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<--curser_column<<"H"<<flush;
                }
                  
            }
            else if(buffer[0]==10)//enter key
            {
              //cout<<"in command mode:";
            std:: vector<string> v;
              command[curser]='\0';
              char s[1000];
                int j=0,i;
                for(i=0;command[i]!='\0';i++)
                    {
                        if(command[i]==' ')
                        {
                            if(command[i-1]==92)
                                s[j++]=command[i];
                            else
                            {
                                s[j]='\0';
                               //cout<<s1<<": ";
                                v.push_back(s);
                               // s1.clear(); 
                                j=0;    
                            }
                        }
                        else if(command[i]!=92)
                        {
                                s[j++]=command[i];
                        }
                    }
                        s[j]=command[i];
                        v.push_back(s);
                    cout<<"\n";
                    for(int i=0;i<v.size();i++)
                        cout<<v[i]<<"|"<<flush;
                    v.clear();
                
                if()
                {
                    
                }
              

                sleep(3);
                //cout<<temp<<flush;
                
                cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                cout<<"\e[0J"<<flush;
                cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                cout<<"> "<<flush;
                cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                curser=0,curser_column=3;


            }

            buffer[0]=buffer[1]=buffer[2]=0;    
      } 
    }
    else if(buffer[0]==27) //escape
    {
      if(buffer[1]==91)
      {
        if(buffer[2]==65)
        {
            if(curser_row>2)
                    { 
                        curser_row--;
                        file_num--;
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                    }
                    else if(curser_row==2 && file_num>2)
                    {
                        file_num--;
                        ls(file_num-curser_row,path);
                        statusbar("NORMAL MODE",path);
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                    } 
            //cout<<"up arrow pressed\n";
        }
        if(buffer[2]==66)
        {
            if(file_num<total_files+1 && curser_row<33)
                    {
                        curser_row++;
                        file_num++;
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                    }
                    else if(file_num<total_files+1 && curser_row==33)
                    {
                        file_num++;
                        ls(file_num-curser_row,path);
                        statusbar("NORMAL MODE",path);
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                    }
           // cout<<"down arrow pressed\n";
        }
        if(buffer[2]==67)
        {
            if(!stack2.empty())
                    {
                        string S3;              
                        S3=stack2.top();
                        stack2.pop();
                        stack1.push(S3);
                    
                        strcpy (path,S3.c_str());
                        ls(0,path);
                        statusbar("NORMAL MODE",path);
                        curser_row=file_num=2;
                        curser_column=1;
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;      
                    }
         //   cout<<"right arrow pressed\n";
        }
        if(buffer[2]==68)
        {
            if(stack1.size()!=1)
                  {
                      string S3;
                      S3=stack1.top();
                      stack2.push(S3);
                      stack1.pop();

                      S3=stack1.top();
                      strcpy (path,S3.c_str());

                      ls(0,path);
                      statusbar("NORMAL MODE",path);
                      curser_row=file_num=2;
                      curser_column=1;
                      cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;  
                  }
        //  cout<<"left arrow pressed\n";
        }   
      }
    }
    else if(buffer[0]==10) //enter
    {
        string s4,s5,s6;
            stat(point_to_file[file_num-2]->d_name, &file_stat);
            
            if((file_stat.st_mode & S_IFMT)==S_IFDIR)
            {// cout<<"this is dir";
                s4=stack1.top();
                if(!(strcmp(point_to_file[file_num-2]->d_name,"..")==0 && (s4[s4.length()-1]=='.')) && !(strcmp(point_to_file[file_num-2]->d_name,".")==0))
                {
                    strcat(path,"/");
                    
                    if((strcmp(point_to_file[file_num-2]->d_name,"..")==0))
                    {
                        s5=findpath(stack1.top());
                        strcpy(path,s5.c_str());
                    }
                    else
                        strcat(path,point_to_file[file_num-2]->d_name);
                    
                    stack1.push(path);
                    ls(0,path);
                    statusbar("NORMAL MODE",path);
                    curser_row=2;
                    curser_column=1;
                    file_num=2;
                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;        //cout<<point_to_file[i-1]->d_name;
                }
            }

            else if((file_stat.st_mode & S_IFMT)!=S_IFBLK && (file_stat.st_mode & S_IFMT)!=S_IFCHR && (file_stat.st_mode & S_IFMT)!=S_IFIFO && (file_stat.st_mode & S_IFMT)!=S_IFLNK && (file_stat.st_mode & S_IFMT)!=S_IFSOCK)
            {
                string S1="/usr/bin/xdg-open ";
                S1.append(point_to_file[file_num-2]->d_name);
                system(S1.c_str());
            }
      //cout<<"enter pressed\n";
    }
    else if(buffer[0]==72 || buffer[0]==104 && !(strcmp(stack1.top().c_str(),".")==0)) //home
    {
        strcpy(path,".");
            ls(0,path);
            statusbar("NORMAL MODE",path);
            stack1.push(".");
            curser_row=2;
            curser_column=1;
            file_num=2;
            cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
     // cout<<"home key pressed\n";
    }
    else if(buffer[0]==8 || buffer[0]==127 && !(strcmp(stack1.top().c_str(),".")==0)) //bacspace
    {
        string S1=findpath(stack1.top());
            strcpy(path,S1.c_str());
            ls(0,path);
            statusbar("NORMAL MODE",path);
            stack1.push(path);
            curser_row=2;
            curser_column=1;
            file_num=2;
            cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
      //  cout<<"backspace pressed\n";
    }
    buffer[0]=buffer[1]=buffer[2]=0;
  /*char ch,pop[10000],path[1000]=".";
    

    while(1)
    {
        ch = getchar();
        if((ch == 'h' || ch == 'H')  && !(strcmp(stack1.top().c_str(),".")==0))
        {
            strcpy(path,".");
            ls(0,path);
            statusbar("NORMAL MODE",path);
            stack1.push(".");
            curser_row=2;
            curser_column=1;
            file_num=2;
            cout<<"\033["<<curser_row<<";"<<curser_column<<"H";
        }
        else if(ch == 127 && !(strcmp(stack1.top().c_str(),".")==0))
        {
            string S1=findpath(stack1.top());
            strcpy(path,S1.c_str());
            ls(0,path);
            statusbar("NORMAL MODE",path);
            stack1.push(path);
            curser_row=2;
            curser_column=1;
            file_num=2;
            cout<<"\033["<<curser_row<<";"<<curser_column<<"H";    
        }
        else if (ch == '\033') 
        { // if the first value is esc
            getchar(); // skip the [
            switch(getchar()) 
            { // the real value
                case 'A':
                    if(curser_row>2)
                    { 
                        curser_row--;
                        file_num--;
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H";
                    }
                    else if(curser_row==2 && file_num>2)
                    {
                        file_num--;
                        ls(file_num-curser_row,path);
                        statusbar("NORMAL MODE",path);
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H";
                    } 
                    break;  
                case 'B':
                    if(file_num<total_files+1 && curser_row<33)
                    {
                        curser_row++;
                        file_num++;
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H";
                    }
                    else if(file_num<total_files+1 && curser_row==33)
                    {
                        file_num++;
                        ls(file_num-curser_row,path);
                        statusbar("NORMAL MODE",path);
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H";
                    }
                    break;
                case 'C':
                    if(!stack2.empty())
                    {
                        string S3;              
                        S3=stack2.top();
                        stack2.pop();
                        stack1.push(S3);
                    
                        strcpy (path,S3.c_str());
                        ls(0,path);
                        statusbar("NORMAL MODE",path);
                        curser_row=file_num=2;
                        curser_column=1;
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H";      
                    }
                    break;
                case 'D':
                  if(stack1.size()!=1)
                  {
                      string S3;
                      S3=stack1.top();
                      stack2.push(S3);
                      stack1.pop();

                      S3=stack1.top();
                      strcpy (path,S3.c_str());

                      ls(0,path);
                      statusbar("NORMAL MODE",path);
                      curser_row=file_num=2;
                      curser_column=1;
                      cout<<"\033["<<curser_row<<";"<<curser_column<<"H";  
                  }
                  break;
            }
        }
        else if(ch == '\n')
        {
            string s4,s5,s6;
            stat(point_to_file[file_num-2]->d_name, &file_stat);
            
            if((file_stat.st_mode & S_IFMT)==S_IFDIR)
            {// cout<<"this is dir";
                s4=stack1.top();
                if(!(strcmp(point_to_file[file_num-2]->d_name,"..")==0 && (s4[s4.length()-1]=='.')) && !(strcmp(point_to_file[file_num-2]->d_name,".")==0))
                {
                    strcat(path,"/");
                    
                    if((strcmp(point_to_file[file_num-2]->d_name,"..")==0))
                    {
                        s5=findpath(stack1.top());
                        strcpy(path,s5.c_str());
                    }
                    else
                        strcat(path,point_to_file[file_num-2]->d_name);
                    
                    stack1.push(path);
                    ls(0,path);
                    statusbar("NORMAL MODE",path);
                    curser_row=2;
                    curser_column=1;
                    file_num=2;
                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H";        //cout<<point_to_file[i-1]->d_name;
                }
            }

            else if((file_stat.st_mode & S_IFMT)!=S_IFBLK && (file_stat.st_mode & S_IFMT)!=S_IFCHR && (file_stat.st_mode & S_IFMT)!=S_IFIFO && (file_stat.st_mode & S_IFMT)!=S_IFLNK && (file_stat.st_mode & S_IFMT)!=S_IFSOCK)
            {
                string S1="/usr/bin/xdg-open ";
                S1.append(point_to_file[file_num-2]->d_name);
                system(S1.c_str());
            }
        }*/
    } 
}