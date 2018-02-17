
#include <iostream>
#include <cstring>
#include <sstream>
#include <unistd.h> /* fork() / execvp */
#include <linux/types.h> // pid_t
#include <sys/ptrace.h> /* ptrace */
#include <sys/prctl.h> // prctl
#include <signal.h> // SIGHUP


// #include "breakpoints.h"
// #include "disassembler.h"
// #include "functions.h"

class Debugger {
    public:
        Debugger (std::string &program_path): m_program_path{program_path} {   
            
        }
        void run_debuggee(char* cmd[]);   
        void run_debugger(pid_t child_pid);

    private:
        std::string m_program_path;
        std::string m_program_argv;
        std::string m_proc_program_path; 
        pid_t m_child_pid;
};

int main(int argc, char* argv[]) {
    char usage_banner[] = "usage: ./sdb [<cmd>]\n";
    if(argc < 2) {
        std::cerr << usage_banner;
        return -1;
    }

    std::string program = (std::string)argv[1];
    Debugger sdb_debugger(program);

    auto child_pid = fork();
    switch(child_pid) {
        case -1: 
            std::cerr << "error forking\n"; 
            break;
        case  0: 
            sdb_debugger.run_debuggee(argv+1);
            break;
        default: 
            sdb_debugger.run_debugger(child_pid);
            break;
    }
    return 0;
}

void Debugger::run_debuggee(char* cmd[]) {
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        std::cerr << "Ptrace Error.\n";
        return;
    }
    
    long ptrace_opts;
    ptrace_opts = PTRACE_O_TRACECLONE|PTRACE_O_TRACEFORK|PTRACE_O_TRACEEXEC|PTRACE_O_TRACEEXIT;
    ptrace(PTRACE_SETOPTIONS, 0, 0, ptrace_opts);

    prctl(PR_SET_PDEATHSIG, SIGHUP);
    execvp(cmd[0], cmd);
}

void Debugger::run_debugger(pid_t child_pid) {
    m_child_pid = child_pid;
    m_proc_program_path = "/proc/" + std::to_string(m_child_pid) + "exe";

    // Breakpoint::wait_for_signal(m_child_pid);  

    // Section::get_sections(m_mem_program);

    // Relocation::get_relocations(m_mem_program, 
    //    Section::m_plt_vma_address, Section::m_plt_entry_size);    
    
    std::cout << "Setting Breakpoints ..." << std::endl;
    //Breakpoint::set_breakpoints(m_child_pid);

    std::cout << "Continue Execution  ..." << std::endl;
    //Breakpoint::continue_execution(m_child_pid);
}

