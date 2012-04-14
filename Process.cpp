#include "Process.hpp"
#include <stdlib.h> 
#include <sys/wait.h> 
using namespace std; 

Process::Process(const stf::vector<char*> args, bool verbose = false)
{
    if(verbose)
    {
        cerr<<"Process: Constructor verbose mode on, you will now see real time statuses."<<endl; 
        verbose = true; 
    }
    else 
    {
        verbose = false; 
    }
    
    m_pid = fork(); 
    if(m_pid==0)
    {
        //Child Process 
        const char **argsC = new const char* [args.size()];
        int i; 
        for(i=0;i<args.size();i++)
        {
            if(verbose)
            {
                cerr<<"Process: Copying argument"<<i<<": "; 
                if(args[i]==NULL) 
                {                
                    cerr<<"NULL"<<endl; 
                }
                else 
                {
                    cerr<<args[i]<<endl; 
                }
            }                
        }
        execvp(args[0],(char **) argsC);
        delete argsC;     
    }      
}

Process::~Process()
{
    if(verbose)
    {
        cerr<<"Process: Destructor verbose mode on" <<endl; 
        cerr<<"Process:Killing Process" << endl;
    }
    kill(m_pid, SIGTERM); 
} 




