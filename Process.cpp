#include "Process.hpp"
using namespace std; 

Process::Process(const std::vector<char*>& args, bool verbose) :
	m_name(args[0]),
	m_pid((pid_t)NULL),
	m_writepipe {-1,-1},
	m_readpipe {-1,-1}
{

    if(args[1]==0)
    {
        throw "Not enough arguments!";
    }
	// Pipe used to write from parent to
	// child process
	// Parent ==> Child
	if(pipe(m_writepipe) == -1)
	{
        perror("pipe");
		throw std::string("Pipe");
	}
	
	// Pipe used to read from child to 
	// parent process
	// Parent <== Child
	if(pipe(m_readpipe) == -1)
	{
        perror("pipe");
		throw std::string("Pipe");
	}
	
	m_pid = fork();
	
	if(m_pid < 0)
	{
		perror("Process fork");
        throw std::string("Process fork");
	}
	else if(m_pid == 0)
	{
		// Child Process

		// Closing the write part of the
		// Parent ==> Child pipe
		close(PARENT_WRITE);
		
		// Closing the read part of the
		// Parent <== Child pipe
		close(PARENT_READ);

        dup2(CHILD_WRITE,1); close(CHILD_WRITE);
        dup2(CHILD_READ,0); close(CHILD_READ);
		
        std::vector<const char*> args;
        std::transform(argss.begin(),argss.end(), std::back_inserter(args), [](std::string s)
        {
            return s.c_str();
        } );
        args.push_back( NULL );
        execvp(args[0], const_cast<char**>(&args[0]));
        perror("Process execvp");
        throw std::string("Error Process execvp");
	}
	else
	{
		// Parent Process
		
		// Closing the write part of the
		// Parent <== Child pipe
		close(m_readpipe[1]);
		
		// Close the read part of the
		// Parent ==> Child pipe
		close(m_writepipe[0]);

		if (verbose)
		{
			cout << "Process " <<m_name << ": forked m_pid " <<m_pid << endl;
		}
	}
};

Process::~Process()
{

    if (verbose)
        std::cerr << "Process " << m_name << ": Entering ~Process()" << std::endl;
    int status;
    pid_t pid = waitpid(m_pid, &status, 0);
    if (pid < 0)
    {
        perror("~Process waitpid");
        throw std::string("Error ~Process waitpid");
    }

    if (verbose)
        std::cerr << "Process " << m_name << ": Leaving ~Process()" << std::endl;
    	
	// Close the two pipes between the 
	// parent and child process 
	// (from the parent process prespective)
	close(PARENT_READ );
	close(PARENT_WRITE);
	
	// Kill the child process
	kill(m_pid, SIGTERM);	 
} 

void Process::write(const std::string& str)
{
	write(m_writepipe[0], str.c_str(), str.length());
}

std::string Process::read(void)
{
	string buffer = "test";
	while(read(m_readpipe[1], buffer.c_str(), 1) > 0); 
	return buffer;
    //For further error handling, we could check if the buffer size is less than the allocated space and returnt he errro code EFAULT
    //We could check for bad file descriptors 
    //If its an unsuitable file, we could return an EINVAL 
}
