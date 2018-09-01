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
#include <string>
#include <fstream>
#include <fstream>
#include <iostream>
#include <ftw.h>
#include <sys/stat.h>
#define commandmodeline 37
using namespace std;

stack <string> stack1,stack2;

char buf[1000],path[1000]=".",pop[1000];
int curser_row=2,curser_column=1,file_num=2,total_files,level=0;
string dst_root;
DIR *point_to_directory;
struct dirent **point_to_file;
struct stat file_stat;
//struct passwd *tf; 
//struct group *gf;
int copy_file(const char* src_path, const struct stat* sb, int typeflag) {
    std::string dst_path = dst_root + src_path;
    switch(typeflag) {
    case FTW_D:
        mkdir(dst_path.c_str(), sb->st_mode);
        break;
    case FTW_F:
        std::ifstream  src(src_path, std::ios::binary);
        std::ofstream  dst(dst_path, std::ios::binary);
        dst << src.rdbuf();
    }
    return 0;
}

int copy_directory(const char* src_root)
{
    return ftw(src_root, copy_file,20);
//extern "C" int copy_file(const char*, const struct stat, int);
}
bool copyFile(const char *SRC, const char* DEST)
{
    std::ifstream src(SRC, std::ios::binary);
    std::ofstream dest(DEST, std::ios::binary);
    dest << src.rdbuf();
    stat(SRC, &file_stat);
    chmod(DEST, file_stat.st_mode);
    return src && dest;
}
string makefullpath(string str1)
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
    else if(str1[0]=='.')
    {
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
string findname(string str1)
{
  int i=str1.length()-1;
  string str2;
  while(str1[i]!='/')
        --i;
     // s1[i]='\0';
  //  cout<<i;  
    str2=str1.substr(i+1,str1.length()-i-1); 
   return str2;
}
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
                   // for(int i=0;i<v.size();i++)
                    //    cout<<v[i]<<"|"<<flush;
                
                 //   cout<<v[2]<<flush;
                if(v[0]=="copy")
                {
                    
                    for(int i=1;i<v.size()-1;i++)
                    {   
                        string destination_path=makefullpath(v[v.size()-1]);
                        string source_path=makefullpath(v[i]);
                        string temp;
                        if(!stat(source_path.c_str(), &file_stat))
                        {

                            if((file_stat.st_mode & S_IFMT)==S_IFDIR) //for directory
                            {
                                if(!stat(destination_path.c_str(), &file_stat))
                                {
                                    
                                    destination_path.append("/");
                                    temp.append(destination_path);
                                    temp.append(findname(source_path));
                                    source_path.append("/");
                                    //cout<<temp<<flush;
                                    if(stat(temp.c_str(), &file_stat))
                                    {
                                        dst_root.clear();
                                        dst_root=destination_path;
                                        copy_directory(source_path.c_str());
                                        cout<<"Directory successfully copied!  "<<flush;    
                                    }
                                    else
                                        cout<<"Destination already exists!  "<<flush;    
                                    
                                }
                                else
                                    cout<<"Destination path is not valid! "<<flush;
                            }
                            else
                            {   
                                if(!stat(destination_path.c_str(), &file_stat))
                                {
                               // stat(source_path.c_str(), &file_stat);
                                    destination_path.append("/");
                                    destination_path.append(findname(source_path));
                                    if(stat(destination_path.c_str(), &file_stat))
                                    {
                                        if(copyFile(source_path.c_str(),destination_path.c_str()))
                                        {
                                            cout<<"Dile copied successfully! "<<flush;
                                        }
                                        else
                                            cout<<"Dile does not exists or source or destination path is not valid!! "<<flush;    
                                    }
                                    else
                                        cout<<"FIle already exists! "<<flush;
                                      
                                }
                                else
                                    cout<<"Destination path is not valid! "<<flush; 
                            }
                        }
                        else
                            cout<<"File or directory does not exists or source path is not valid! "<<flush;
                        
                       // cout<<"source:"<<source_path<<"  destination:"<<destination_path<<flush;
                        //cout<<chmod(str2, the_stat.st_mode);
                        //cout<<makefullpath(v[i])<<"|"<<flush;
                    }
                }
                else if(v[0]=="move")
                {

                }
                else if(v[0]=="rename")
                {   
                        string destination_path=makefullpath(v[2]);
                        string source_path=makefullpath(v[1]);
                        string temp1=findpath(source_path);
                        string temp2=findpath(destination_path);
                        if(!stat(source_path.c_str(), &file_stat) && (temp1==temp2))
                        {
                            if(!rename(source_path.c_str(),destination_path.c_str()))
                               cout<<"File renamed successfully"<<flush;
                        }
                        else
                            cout<<"File name does not exists or source path or destination is not valid"<<flush;
                        
                       // cout<<"source:"<<source_path<<"  destination:"<<destination_path<<flush;
                        //cout<<chmod(str2, the_stat.st_mode);
                        //cout<<makefullpath(v[i])<<"|"<<flush;
                }
                  //  
                else if(v[0]=="create_file")
                {

                }
                else if(v[0]=="create_dir")
                {

                }
                else if(v[0]=="delelte_file")
                {

                }
                else if(v[0]=="delete_dir")
                {

                }
                else if(v[0]=="goto")
                {

                }
                else if(v[0]=="search")
                {

                }
                else if(v[0]=="snapshot")
                {

                }
                v.clear();
                sleep(4);
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