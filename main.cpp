/*


 ██████   ██████ ██████████   ███████████    █████████
░░██████ ██████ ░░███░░░░███ ░░███░░░░░███  ███░░░░░███
 ░███░█████░███  ░███   ░░███ ░███    ░███ ███     ░░░
 ░███░░███ ░███  ░███    ░███ ░██████████ ░███
 ░███ ░░░  ░███  ░███    ░███ ░███░░░░░███░███    █████
 ░███      ░███  ░███    ███  ░███    ░███░░███  ░░███
 █████     █████ ██████████   ███████████  ░░█████████
░░░░░     ░░░░░ ░░░░░░░░░░   ░░░░░░░░░░░    ░░░░░░░░░


This is MDBG(mri - Debugger)
This is a small project that i always wanted to make. I hope you guys will love it.
It's still in Development phase.
*/





#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <unistd.h>
#include <sys/ptrace.h>
#include <linenoise.h>
#include <vector>
#include <sys/wait.h>
#include <sstream>

// Look at the Private part of this class to understand it better.
/*
	We take the data addr, where we have to put breakdown. And instruction there is overwritten by int3 which traps the program.
*/
class breakpointx{
public:
	breakpoint(pid_t pid,std::intptr_t addr):m_pid(pid),m_addr(addr){}
	void enable();
	void disable();
	auto is_enabled() const -> bool { return m_enabled; }
	auto get_address() const -> std::intptr_t { return m_addr;}

private:
	pid_t m_pid;
	std::intptr_t m_addr;                      // TYpe of integer type.
	bool m_enabled;
	uint8_t m_saved_data; //overwritten data.
};

void breakpoint::enable(){
	auto data = ptrace(PTRACE_PEEKDATA,m_pid,m_addr,nullptr);
	
	m_saved_data = static_cast<uint8_t>(data & 0xff);
	uint64_t interrupt3 = 0xcc;
	uint64_t data_with_int3 = ((data & ~0xff)|interrut3);
	
	ptrace(PTRACE_POKEDATA,m_pid,m_addr,data_with_int3);

	m_enabled = true;
}

void breakpoint::disable(){
	auto data = ptrace(PTRACE_PEEKDATA,m_pid,m_addr,nullptr);
	auto restored_data = ((data & ~oxff)|m_save_data);



}

class debugger{

public:
	debugger(char * prog_name, pid_t pid):m_prog_name(prog_name),m_pid(pid){}
	void run();
	void handle_command(const std::string& line);
	void continue_execution();
protected:
	std::string m_prog_name;
	pid_t m_pid;
};

std::vector<std::string> split (const std::string& line, char del){
	std::vector<std::string> out;
	std::stringstream src {line};		// Source
	std::string dest;			// Destination.
	while(std::getline(src,dest,del)){
		out.push_back(dest);
	}
	return out;
}




void debugger::continue_execution(){
	ptrace(PTRACE_CONT,m_pid,nullptr,nullptr);
	int wait_status;
	int options = 0;
	waitpid(m_pid,&wait_status,options);
}


bool is_prefix(const std::string& command, const std::string& prop){
	if(command.size()!=prop.size()) return false;
	return (std::equal(command.begin(),command.end(),prop.begin()));
}

void debugger::handle_command(const std::string& line){
	std::vector<std::string> args = split(line, ' ');
	std::string command = args[0];

	if(is_prefix(command,"continue")){
		continue_execution();
	}
	else if(is_prefix(command,"break"))
	else{
		std::cerr << "Unknown command\n";
	}
}


void debugger::run(){
	int wait_status;
	int option = 0;
	waitpid(m_pid,&wait_status,option);

	char *line = nullptr;
	while((line = linenoise("[DBG]: "))!= nullptr){
		linenoiseHistoryAdd(line);
		linenoiseFree(line);
		handle_command(line);
	}

}



int print_usage(char *file){
	std::string prog{file};                       // std::string provides constructor to get this job done, most probably copy constructor.
	std::cerr << "[USAGE]: "<<prog<<" <file to debug>\n"; 
//	printf("[USAGE] %s <file to debug>\n",file);
	return -1;
}

int main(int argc, char *argv[]){
	if(argc<2){
		if(print_usage(argv[0]) == -1){
			return -1;
		}
	}

	char* prog = argv[1];
	pid_t pid = fork();
	
	if(pid == 0){
		// we are in child process.
		ptrace(PTRACE_TRACEME,0,nullptr,nullptr);
		execl(prog,prog,(char *)nullptr);
	}
	else if(pid == -1){
		//couldn't make any process.
	}
	else if(pid >= 1){
		// we are in parent process.
	}

}
