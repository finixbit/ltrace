#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <unistd.h> /* fork() / execvp */
#include <linux/types.h> // pid_t
#include <sys/ptrace.h> /* ptrace */
#include <sys/prctl.h> // prctl
#include <signal.h> // SIGHUP
#include "ltrace.h"
#include "elf-parser/elf_parser.h"
#include "callsite.h"
using namespace elf_parser;


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
    m_proc_program_path = "/proc/" + std::to_string(m_child_pid) + "/exe";
    wait_for_signal(m_child_pid);  

    auto relocs = get_relocations();
    m_relocs = relocations_to_unordered_map(relocs);

    uint8_t* text_code;
    std::intptr_t text_code_entry;
    long text_code_size;

    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)get_memory_map();
    text_code_entry = ehdr->e_entry;

    auto segs = get_segments();
    for(auto &seg: segs) {
        if(seg.segment_type == "LOAD") {
            text_code = &get_memory_map()[text_code_entry - seg.segment_virtaddr];
            text_code_size = seg.segment_memsize;
            break;
        }
    }
    
    disassemble_callsites(
        text_code, text_code_size, text_code_entry, false);

    // set breakpoints
    for(auto &cs_map: Callsite::m_callsites_map) {
        auto &cs = cs_map.second;

        if((!cs.m_cs_target_resolved) || (cs.m_cs_target_address == 0))
            continue;

        if(!m_relocs.count(cs.m_cs_target_address))
            continue;
        else
            cs.m_cs_name = \
                m_relocs[cs.m_cs_target_address].relocation_symbol_name;
        
        set_callsite_breakpoint(
          child_pid, cs.m_cs_address, cs.m_cs_return_address);
    }

    // continue program execution
    continue_execution(m_child_pid);
}


RELOC_MAP Debugger::relocations_to_unordered_map(std::vector<relocation_t> &rels) {
    RELOC_MAP rmap;
    for(auto &rel: rels) {
        rmap[rel.relocation_plt_address] = rel;
    }
    return rmap;
}