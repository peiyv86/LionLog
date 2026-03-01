#include<iostream>
#include<string>
#include<vector>
#include<zlib.h>
#include<cstring>

void find_line(const std::string& filename,const std::string& keyword)
{
    gzFile file=gzopen(filename.c_str(),"rb");
    if(!file)
    {
        std::cerr<<"Failed to open: "<<filename <<'\n';
        return ;
    }
    char buffer[4096];
    
    while(gzgets(file,buffer,sizeof(buffer))!=NULL)
    {
        std::string line(buffer);
        if(line.find(keyword)!=std::string::npos)
        {
            std::cout<<'['<<filename<<']'<<line;
        }
    }
    gzclose(file);
}

int main(int argc , char* argv[])
{
    if(argc<3)
    {   
        std::cout << "\033[32m";
        std::cout<<"Usage: lion-q <keyword> <file1> [file2 ...]\n";
        std::cout << "\033[0m";
        return 1;
    }
    std::string keyword =argv[1];
    for(int i=2;i<argc;i++)
    {
        find_line(argv[i],keyword);
    }
    return 0;
}