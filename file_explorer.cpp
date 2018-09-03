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
#include <iostream>
#include <ftw.h>
#include <sys/ioctl.h>

using namespace std;


    
stack <string> stack1,stack2;
vector <string> Path,Name;

char buf[1000],path[1000]=".",pop[1000];
int curser_row=2,curser_column=1,file_num=2,total_files,level=0,search_flag=0,total_rows,total_columns,listend,commandmodeline;
string dst_root;

DIR *point_to_directory;
struct dirent **point_to_file;
struct stat file_stat;
struct winsize win;

void snapsht(const char path[],const char destination[])
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
void printlist(int startline)
{   
    int lastline;
    lastline=total_files;

    if(lastline>listend)
        lastline=startline+listend;
    if(lastline>total_files)
        lastline=total_files;

    for(int i=startline;i<lastline;i++)
        cout<<Name[i]<<"  "<<Path[i]<<"\n"<<flush;
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
int copy_file(const char* src_path, const struct stat* sb, int typeflag) 
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
            dst << src.rdbuf();
    }
    return 0;
}
int removedir(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
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
string findname(string str1)
{
    int i=str1.length()-1;
    string str2;
    while(str1[i]!='/')
        --i;

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
    int i=listend+1,j=1;

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
    if(lastline>listend)
        lastline=startline+listend;
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
    ioctl(STDIN_FILENO, TIOCGWINSZ, &win);
    total_rows=win.ws_row;
    total_columns=win.ws_col;
    listend=total_rows-5;
    commandmodeline=total_rows-1;
    gotoNonCanon();

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

        cout<<"\033["<<commandmodeline<<";1H> "<<flush;
        curser_column=3;
      
        while(flag)
        { 
            char command[500];
            read(0,buffer,128);
            
            if(buffer[0]==27 && buffer[1]!=91)//esc key
            {
              //  cout<<"\nescap pressed: coming to normal mode"<<flush;
                ls(0,stack1.top().c_str());
                statusbar("NORMAL MODE",stack1.top().c_str());
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
                                        cout<<"Directory already exists!  "<<flush;    
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
                    }

                    v.clear();
                    sleep(2);
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    cout<<"\e[0J"<<flush;
                    ls(0,stack1.top().c_str());
                    statusbar("COMMAND MODE",path);
                    // curser_row=2;
                    // curser_column=1;
                    // file_num=2;
                    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                    cout<<"> "<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    curser=0,curser_column=3;
                }
                else if(v[0]=="move")
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
                                        rmdir(source_path.c_str());
                                        cout<<"Directory successfully moved!  "<<flush;    
                                    }
                                    else
                                        cout<<"Directory already exists!  "<<flush;    
                                    
                                }
                                else
                                    cout<<"Destination path is not valid! "<<flush;
                            }
                            else
                            {   
                                if(!stat(destination_path.c_str(), &file_stat)) //for file
                                {
                               // stat(source_path.c_str(), &file_stat);
                                    destination_path.append("/");
                                    destination_path.append(findname(source_path));
                                    if(stat(destination_path.c_str(), &file_stat))
                                    {
                                        if(copyFile(source_path.c_str(),destination_path.c_str()))
                                        {
                                            remove( source_path.c_str());
                                            cout<<"File moved successfully! "<<flush;
                                        }
                                        else
                                            cout<<"File does not exists or source or destination path is not valid!! "<<flush;    
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

                    v.clear();
                    sleep(2);
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    cout<<"\e[0J"<<flush;
                    ls(0,stack1.top().c_str());
                    statusbar("COMMAND MODE",path);
                    // curser_row=2;
                    // curser_column=1;
                    // file_num=2;
                    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                    cout<<"> "<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    curser=0,curser_column=3;
                }
                else if(v[0]=="rename")
                {   
                        string destination_path=makefullpath(v[2]);
                        string source_path=makefullpath(v[1]);
                        // string temp1=findpath(source_path);
                        // string temp2=findpath(destination_path);
                        if(!stat(source_path.c_str(), &file_stat))
                        {
                            if(!rename(source_path.c_str(),destination_path.c_str()))
                               cout<<"File renamed successfully"<<flush;
                        }
                        else
                            cout<<"File name does not exists or source path or destination is not valid"<<flush;
                        
                        // cout<<"source:"<<source_path<<"  destination:"<<destination_path<<flush;
                        //cout<<chmod(str2, the_stat.st_mode);
                        //cout<<makefullpath(v[i])<<"|"<<flush;
                        v.clear();
                        sleep(2);
                        cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                        cout<<"\e[0J"<<flush;
                        ls(0,stack1.top().c_str());
                        statusbar("COMMAND MODE",path);
                        // curser_row=2;
                        // curser_column=1;
                        // file_num=2;
                        cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                        cout<<"> "<<flush;
                        cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                        curser=0,curser_column=3;
                }
                  //  
                else if(v[0]=="create_file")
                {
                        string destination_path=makefullpath(v[2]);
                        string filename=v[1];
                        string temp=destination_path;
                        temp.append("/");
                        temp.append(filename);
                        //cout<<filename<<" "<<destination_path<<flush;
                        if(!stat(destination_path.c_str(), &file_stat))
                        {
                            if(stat(temp.c_str(), &file_stat))
                            {
                                ofstream file {temp.c_str()};
                                cout<<"File is created!!"<<flush;    
                            }
                            else
                                cout<<"File is already exists!!"<<flush;   
                            
                        }
                        else
                            cout<<"Destination path is not valid!!"<<flush;
                        v.clear();
                        sleep(2);
                        cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                        cout<<"\e[0J"<<flush;
                        ls(0,stack1.top().c_str());
                        statusbar("COMMAND MODE",path);
                        // curser_row=2;
                        // curser_column=1;
                        // file_num=2;
                        cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                        cout<<"> "<<flush;
                        cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                        curser=0,curser_column=3;

                }
                else if(v[0]=="create_dir")
                {
                    string destination_path=makefullpath(v[2]);
                    string filename=v[1];
                    string temp=destination_path;
                    temp.append("/");
                    temp.append(filename);
                        //cout<<filename<<" "<<destination_path<<flush;
                        if(!stat(destination_path.c_str(), &file_stat))
                        {
                            if(stat(temp.c_str(), &file_stat) )
                            {
                               if(mkdir(temp.c_str(),0777)==-1)//creating a directory
                                {
                                    cerr<<"error: ";
                                    exit(1);
                                }
                                cout<<"Directory is created!!"<<flush;    
                            }
                            else
                                cout<<"directory is already exists!!"<<flush;   
                            
                        }
                        else
                            cout<<"Destination path is not valid!!"<<flush;
                        v.clear();
                        sleep(2);
                        cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                        cout<<"\e[0J"<<flush;
                        ls(0,stack1.top().c_str());
                        statusbar("COMMAND MODE",path);
                        // curser_row=2;
                        // curser_column=1;
                        // file_num=2;
                        cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                        cout<<"> "<<flush;
                        cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                        curser=0,curser_column=3;
                }
                else if(v[0]=="delete_file")
                {
                    //cout<<makefullpath("../eae/eae")<<flush;
                    string destination_path=makefullpath(v[1]);
                    
                    if(!stat(destination_path.c_str(), &file_stat))
                    {
                            remove( destination_path.c_str());
                            cout<<"File removed successfully!!"<<flush;
                    }
                    else
                        cout<<"File is not exits or Path is not valid!!"<<flush;
                    v.clear();
                    sleep(2);
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    cout<<"\e[0J"<<flush;
                    ls(0,stack1.top().c_str());
                    statusbar("COMMAND MODE",path);
                    // curser_row=2;
                    // curser_column=1;
                    // file_num=2;
                    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                    cout<<"> "<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    curser=0,curser_column=3;

                }
                else if(v[0]=="delete_dir")
                {
                    string destination_path=makefullpath(v[1]);
                    
                    if(!stat(destination_path.c_str(), &file_stat))
                    {
                            rmdir(destination_path.c_str());
                            cout<<"directory removed successfully!!"<<flush;
                    }
                    else
                        cout<<"Directory is not exits or Path is not valid!!"<<flush;
                    v.clear();
                    sleep(2);
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    cout<<"\e[0J"<<flush;
                    ls(0,stack1.top().c_str());
                    statusbar("COMMAND MODE",path);
                    // curser_row=2;
                    // curser_column=1;
                    // file_num=2;
                    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                    cout<<"> "<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    curser=0,curser_column=3;
                }
                else if(v[0]=="goto")
                {
                    string destination_path=makefullpath(v[1]);
                    if(!stat(destination_path.c_str(), &file_stat))
                    {
                        strcpy(path,destination_path.c_str());
                        stack1.push(path);
                        ls(0,path);
                        statusbar("NORMAL MODE",path);
                        curser_row=2;
                        curser_column=1;
                        file_num=2;
                        cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                        flag=0;

                    }
                    else
                        cout<<"Directory Path is not valid!!"<<flush;

                }
                else if(v[0]=="search")
                {
                    string search_file_name=v[1];
                    dst_root.clear();
                    dst_root=search_file_name;
                    Name.clear();
                    Path.clear();
                    search();
                    
                    cout<<"\033[3J"<<flush;
                    cout<<"\033[H\033[J"<<flush;
                    cout<<"\n"<<flush;
                    total_files=Name.size();
                    printlist(0);
                    statusbar("NORMAL MODE",path);
                    curser_row=2;
                    curser_column=1;
                    file_num=2;
                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                    flag=0;
                    v.clear();
                    search_flag=1;
                    // sleep(2);
                  //  curser=0,curser_column=3;
                }
                else if(v[0]=="snapshot")
                {
                    string dumpfile=makefullpath(v[2]);
                    string folder=makefullpath(v[1]);

                    if(!stat(folder.c_str(), &file_stat))
                    {
                        if(stat(dumpfile.c_str(), &file_stat))
                        {
                            snapsht(folder.c_str(),dumpfile.c_str());
                            cout<<"snapshoted!!!!!"<<flush;
                        }
                        else
                            cout<<"dumpfile is already exists!!!!!"<<flush;

                    }
                    else
                        cout<<"Source path or Directory is not valid"<<flush;

                    v.clear();
                    sleep(2);
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    cout<<"\e[0J"<<flush;
                    ls(0,stack1.top().c_str());
                    statusbar("COMMAND MODE",path);
                    // curser_row=2;
                    // curser_column=1;
                    // file_num=2;
                    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                    cout<<"> "<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    curser=0,curser_column=3;
                }
                else
                {
                    cout<<"Wrong operation!!"<<flush;
                    v.clear();
                    sleep(2);
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    cout<<"\e[0J"<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<1<<"H"<<flush;
                    cout<<"> "<<flush;
                    cout<<"\033["<<commandmodeline<<";"<<3<<"H"<<flush;
                    curser=0,curser_column=3;
                }

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
            if(search_flag==0)
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
            }
            else
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
                                    printlist(file_num-curser_row);
                                    statusbar("NORMAL MODE",path);
                                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                                }
            }
            //cout<<"up arrow pressed\n";
        }
        if(buffer[2]==66)
        {
            if(search_flag==0)
            {
                if(file_num<total_files+1 && curser_row<listend)
                                {
                                    curser_row++;
                                    file_num++;
                                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                                }
                                else if(file_num<total_files+1 && curser_row==listend)
                                {
                                    file_num++;
                                    ls(file_num-curser_row,path);
                                    statusbar("NORMAL MODE",path);
                                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                                }
            }
            else
            {
                if(file_num<total_files+1 && curser_row<listend)
                                {
                                    curser_row++;
                                    file_num++;
                                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                                }
                                else if(file_num<total_files+1 && curser_row==listend)
                                {
                                    file_num++;
                                    printlist(file_num-curser_row);
                                    statusbar("NORMAL MODE",path);
                                    cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                                }
            }
           // cout<<"down arrow pressed\n";
        }
        if(buffer[2]==67)
        {
            if(search_flag==0)
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
            }
         //   cout<<"right arrow pressed\n";
        }
        if(buffer[2]==68)
        {
            if(search_flag==0)
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
            }
            else
            {
                
                                  ls(0,stack1.top().c_str());
                                  statusbar("NORMAL MODE",path);
                                  curser_row=file_num=2;
                                  curser_column=1;
                                  cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                                  search_flag=0;  
            }
        //  cout<<"left arrow pressed\n";
        }   
      }
    }
    else if(buffer[0]==10) //enter
    {
            if(search_flag==0)
            {
                string s4,s5,s6;
                        stat(point_to_file[file_num-2]->d_name, &file_stat);
                        
                        if((file_stat.st_mode & S_IFMT)==S_IFDIR)
                        {   cout<<"this is dir"<<flush;
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
            
                            string S1="/usr/bin/xdg-open";
                           // S1.append(point_to_file[file_num-2]->d_name);
                            string temppath=stack1.top();
                            temppath.append("/");
                            temppath.append(point_to_file[file_num-2]->d_name);

                            int c_pid = fork();
            
                            if (c_pid == 0)
                            {
                               execl(S1.c_str(),"xdg-open",temppath.c_str(),NULL);
                            }
                            
                            
                           // system(S1.c_str());
                        }
            }
            else
            {
                        stat(Path[file_num-2].c_str(), &file_stat);
                        
                        if((file_stat.st_mode & S_IFMT)==S_IFDIR)
                        {    cout<<"this is dir";
                                strcpy(path,Path[file_num-2].c_str());
                                stack1.push(path);
                                ls(0,path);
                                statusbar("NORMAL MODE",path);
                                curser_row=2;
                                curser_column=1;
                                file_num=2;
                                cout<<"\033["<<curser_row<<";"<<curser_column<<"H"<<flush;
                                search_flag=0;      //cout<<point_to_file[i-1]->d_name;
                        }
            
                        else if((file_stat.st_mode & S_IFMT)!=S_IFBLK && (file_stat.st_mode & S_IFMT)!=S_IFCHR && (file_stat.st_mode & S_IFMT)!=S_IFIFO && (file_stat.st_mode & S_IFMT)!=S_IFLNK && (file_stat.st_mode & S_IFMT)!=S_IFSOCK)
                        {
            
                            string S1="/usr/bin/xdg-open";
                           // S1.append(point_to_file[file_num-2]->d_name);
                          //  cout<<"vvvvvvvvvvvvvvvv:"<<Path[file_num-2]<<flush;
                            int c_pid = fork();
            
                            if (c_pid == 0)
                            {
                               execl(S1.c_str(),"xdg-open",Path[file_num-2].c_str(),NULL);
                            }
                            
                            
                           // system(S1.c_str());
                        }
            }
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
            search_flag=0;
     // cout<<"home key pressed\n";
    }
    else if(buffer[0]==8 || buffer[0]==127 && !(strcmp(stack1.top().c_str(),".")==0)) //bacspace
    {
        if(search_flag==0)
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
                    search_flag=0;
        }
      //  cout<<"backspace pressed\n";
    }
    buffer[0]=buffer[1]=buffer[2]=0;
    
    } 
}