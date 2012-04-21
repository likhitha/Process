#include "Process.hpp"
using namespace std; 

Process::Process(const std::vector<char*>& args, bool verbose) :
	m_name(args[0]),
	m_pid((pid_t)NULL),
	m_writepipe {-1,-1},
	m_readpipe {-1,-1}
{
	// Pipe used to write from parent to
	// child process
	// Parent ==> Child
	if(pipe(m_writepipe) == -1)
	{
		perror("Error Process pipe");
	}
	
	// Pipe used to read from child to 
	// parent process
	// Parent <== Child
	if(pipe(m_readpipe) == -1)
	{
		perror("Error Process pipe");
		exit(EXIT_FAILURE);
	}
	
	m_pid = fork();
	
	if(m_pid < 0)
	{
		perror("Error Process fork");
		exit(EXIT_FAILURE);
	}
	else if(m_pid == 0)
	{
		// Child Process

		// Closing the write part of the
		// Parent ==> Child pipe
		close(m_writepipe[1]);
		
		// Closing the read part of the
		// Parent <== Child pipe
		close(m_readpipe[0]);
	
		// Ideally you would want to include some sort of
		// of the following code to write to the parent
		// process
		//
		// write(m_readpipe[1], ...);
		//
		// and some sort of the following code to read from
		// the parent process
		//
		// while(read(m_writepipe[0], ...) > 0)	
		
		execvp(args[0], const_cast<char**>(&args[0]));
		
		// Closes pipes between the Parent
		// and Child process
		// (from the Child prespective)
		// Assuming this will be called
		// when the kill command is
		// issued in the ~Process(). 
		close(m_writepipe[0]);
		close(m_readpipe[1]);

		perror("Error Process execvp");
		_exit(EXIT_FAILURE);
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
			cout << "Process " << 
				    m_name << 
		         ": forked m_pid " << 	
				     m_pid << endl;
		}
	}
};

Process::~Process()
{
	int status;

	if(verbose)
    	{
        	cout << "Process: Destructor verbose mode on" << endl; 
        	cout << "Process: Killing Process" << endl;
    	}
    	
	// Close the two pipes between the 
	// parent and child process 
	// (from the parent process prespective)
	close(m_readpipe[0]);
	close(m_writepipe[1]);
	
	// Kill the child process
	kill(m_pid, SIGTERM);
	
	pid_t pid = waitpid(m_pid, &status, 0);
	
	if(pid < 0)
	{	
		perror("Error ~Process waitpid");
		exit(EXIT_FAILURE);
	}		 
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
}
