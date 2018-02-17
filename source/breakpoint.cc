#include <iostream>
#include <algorithm> // find
#include <sys/ptrace.h> /* ptrace */
#include <sys/wait.h> /* wait */
#include <inttypes.h> /* PRIx64 */
#include "breakpoints.h"
//#include "functions.h"
using namespace sdb;


std::unordered_map<std::intptr_t, Breakpoint> Breakpoint::m_breakpoints_map;

// void Breakpoint::add_function_breakpoint(
//   pid_t &child_pid, std::intptr_t fn_address, std::intptr_t fn_return_address) {

//     if(Breakpoint::m_breakpoints_map.count(fn_address)) {
//       auto &bp = Breakpoint::m_breakpoints_map[fn_address];
//       bp.m_is_fn_call_address = true;
//       // printf("Set breakpoint 0x%" PRIx64 ": (%d)%s\n", 
//       //             fn_address, fn_address, "funtcion.call");

//     } else {
//       Breakpoint bp;
//       bp.m_bp_address = fn_address;
//       bp.m_is_fn_call_address = true;

//       bp.enable_breakpoint(child_pid);
//       Breakpoint::m_breakpoints_map[fn_address] = bp;
//       // printf("Set breakpoint 0x%" PRIx64 ": (%d)%s\n", 
//       //          fn_address, fn_address, "funtcion.call");
//     }

//     if(Breakpoint::m_breakpoints_map.count(fn_return_address)) {
//       auto &bp = Breakpoint::m_breakpoints_map[fn_return_address];
//       bp.m_is_fn_return_address = true;
//       bp.m_return_caller_address = fn_address;
//       // printf("Set Breakpoint 0x%" PRIx64 ": (%d)%s\n", 
//       //   fn_return_address, fn_return_address, "funtcion.ret");

//     } else {
//       Breakpoint bp;
//       bp.m_bp_address = fn_return_address;
//       bp.m_is_fn_return_address = true;
//       bp.m_return_caller_address = fn_address;
      
//       bp.enable_breakpoint(child_pid);
//       Breakpoint::m_breakpoints_map[fn_return_address] = bp;
//       //printf("Set Breakpoint 0x%" PRIx64 ": (%d)%s\n", 
//       //  fn_return_address, fn_return_address, "funtcion.ret");
//     }    
// }

// void Breakpoint::add_callsite_breakpoint(
//   pid_t &child_pid, std::intptr_t fn_address, 
//   std::intptr_t cs_address, std::intptr_t cs_return_address) {
  
//   if(Breakpoint::m_breakpoints_map.count(cs_address)) {
//       auto &bp = Breakpoint::m_breakpoints_map[cs_address];
//       bp.m_is_cs_call_address = true;
//       bp.m_cs_caller_address = fn_address;

//   } else {
//       Breakpoint bp;
//       bp.m_bp_address = cs_address;
//       bp.m_is_cs_call_address = true;
//       bp.m_cs_caller_address = fn_address;
      
//       bp.enable_breakpoint(child_pid);
//       Breakpoint::m_breakpoints_map[cs_address] = bp;
//       //printf("Setting breakpoint 0x%" PRIx64 ": %s\n", 
//       //        cs_address, "callsite.call");
//   } 

//   if(Breakpoint::m_breakpoints_map.count(cs_return_address)) {
//       auto &bp = Breakpoint::m_breakpoints_map[cs_return_address];
//       bp.m_is_cs_return_address = true;
//       bp.m_cs_fn_address = fn_address;
//       bp.m_cs_return_caller_address = cs_address;

//   } else {
//       Breakpoint bp;
//       bp.m_bp_address = cs_return_address;
//       bp.m_is_cs_return_address = true;
//       bp.m_cs_return_caller_address = cs_address;
//       bp.m_cs_fn_address = fn_address;
      
//       bp.enable_breakpoint(child_pid);
//       Breakpoint::m_breakpoints_map[cs_return_address] = bp;
//   //     printf("Setting breakpoint 0x%" PRIx64 ": %s\n", 
//   //                     cs_return_address, "callsite.ret");
//   }   
// }

void Breakpoint::set_breakpoints(pid_t &child_pid) {
  // for(auto &fn_map: Function::m_functions_map) {
  //   auto &fn = fn_map.second;
  //   bool fn_is_breakpoint = false;

  //   Breakpoint::add_function_breakpoint(
  //     child_pid, fn.m_fn_address, fn.m_fn_return_address);

  //   for(auto &cs_map: fn.m_fn_callsites) {
  //     Breakpoint::add_callsite_breakpoint(child_pid, 
  //       fn.m_fn_address, cs_map.second.cs_address, cs_map.second.cs_return_address);
  //   }
  // }
}

void Breakpoint::continue_execution(pid_t &child_pid) {
    while(1) { 
        ptrace(PTRACE_CONT, child_pid, nullptr, nullptr);
        Breakpoint::wait_for_signal(child_pid);
        Breakpoint::step_over_breakpoint(child_pid);
    }
}

void Breakpoint::wait_for_signal(pid_t &child_pid) {
    int wait_status = 0;
    auto options = 0;
    auto i = waitpid(child_pid, &wait_status, options);
    
    if(WIFEXITED(wait_status) || (WIFSIGNALED(wait_status) && WTERMSIG(wait_status) == SIGKILL)) {
      std::cout << "[+] process " << child_pid << " terminated" << std::endl;
      return;
    }
}

void Breakpoint::step_over_breakpoint(pid_t &child_pid) {
  // - 1 because execution will go past the breakpoint
  // auto possible_breakpoint_location = Breakpoint::get_program_counter(child_pid) - 1;

  // if (m_breakpoints_map.count(possible_breakpoint_location)) {
  //   auto& bp = m_breakpoints_map[possible_breakpoint_location];

  //   if (bp.m_enabled) {
  //     //get_breakpoint_info(bp);
  //     printf("bp @ 0x%" PRIx64 " ", bp.m_bp_address);
  //     if(bp.m_is_fn_call_address) {
  //       auto &fn = Function::m_functions_map[bp.m_bp_address];
  //       std::cout << " fn_call@" << fn.m_fn_name;
  //     }

  //     if(bp.m_is_fn_return_address) {
  //       auto &fn = Function::m_functions_map[bp.m_return_caller_address];
  //       std::cout << " fn_ret@" << fn.m_fn_name;
  //     }
       
  //     if(bp.m_is_cs_call_address) {
  //       auto &fn = Function::m_functions_map[bp.m_cs_caller_address];
  //       auto &cs = fn.m_fn_callsites[bp.m_bp_address];
  //       std::cout << " mem_call@" << cs.cs_name << "(" << fn.m_fn_name << ")";
  //     }

  //     if(bp.m_is_cs_return_address) {
  //       auto &fn = Function::m_functions_map[bp.m_cs_fn_address];
  //       auto &cs = fn.m_fn_callsites[bp.m_cs_return_caller_address];
  //       std::cout << " mem_ret@" << cs.cs_name << "(" << fn.m_fn_name << ")";
  //     }
  //     printf("\n");

  //     auto previous_instruction_address = possible_breakpoint_location;
  //     Breakpoint::set_program_counter(child_pid, previous_instruction_address);

  //     bp.disable_breakpoint(child_pid);
  //     ptrace(PTRACE_SINGLESTEP, child_pid, nullptr, nullptr);
  //     Breakpoint::wait_for_signal(child_pid);

  //     bp.enable_breakpoint(child_pid);
  //   }
  // } 
}

siginfo_t Breakpoint::get_signal_info(pid_t &child_pid) {
    siginfo_t info;
    ptrace(PTRACE_GETSIGINFO, child_pid, nullptr, &info);
    return info;
}

uint64_t Breakpoint::get_program_counter(pid_t &child_pid) {
    user_regs_struct regs = Breakpoint::get_registers(child_pid);
    return (uint64_t)regs.rip;
}

void Breakpoint::set_program_counter(pid_t &child_pid, uint64_t &pc) {
    user_regs_struct regs = Breakpoint::get_registers(child_pid);

    #ifdef __x86_64__
        regs.rip = pc;
    #else
        regs.eip = pc;
    #endif
    ptrace(PTRACE_SETREGS, child_pid, nullptr, &regs);
}

void Breakpoint::set_registers(pid_t &child_pid, user_regs_struct &regs) {
    user_regs_struct current_regs = Breakpoint::get_registers(child_pid);
    ptrace(PTRACE_SETREGS, child_pid, nullptr, &regs);
}

void Breakpoint::print_registers(user_regs_struct &regs) {
    printf("[+] rax 0x%lx\n", regs.rax);
    printf("[+] rbx 0x%lx\n", regs.rbx);
    printf("[+] rcx 0x%lx\n", regs.rcx);
    printf("[+] rdx 0x%lx\n", regs.rdx);
    printf("[+] rsi 0x%lx\n", regs.rsi);
    printf("[+] rdi 0x%lx\n", regs.rdi);
    printf("[+] rbp 0x%lx\n", regs.rbp);
    printf("[+] rsp 0x%lx\n", regs.rsp);
    printf("[+] rip 0x%lx\n", regs.rip);
}

user_regs_struct Breakpoint::get_registers(pid_t &child_pid) {
    struct user_regs_struct regs;
    long esp, eax, ebx, edx, ecx, esi, edi, eip;
    #ifdef __x86_64__
      esp = regs.rsp;
      eip = regs.rip;
      eax = regs.rax;
      ebx = regs.rbx;
      ecx = regs.rcx;
      edx = regs.rdx;
      esi = regs.rsi;
      edi = regs.rdi;
    #else
      esp = regs.esp;
      eip = regs.eip;
      eax = regs.eax;
      ebx = regs.ebx;
      ecx = regs.ecx;
      edx = regs.edx;
      esi = regs.esi;
      edi = regs.edi;
    #endif
    if(ptrace(PTRACE_GETREGS, child_pid, nullptr, &regs) == -1) {
        std::cout << "Error: PTRACE_GETREGS" << std::endl;
        exit(1);
    };
    return regs;
}

void Breakpoint::enable_breakpoint(pid_t &child_pid) {
    auto data = ptrace(PTRACE_PEEKDATA, child_pid, m_bp_address, nullptr);
    m_original_data = static_cast<uint8_t>(data & 0xff); //save bottom byte
    
    //set bottom byte to 0xcc
    uint64_t int3 = 0xcc;
    uint64_t data_with_int3 = ((data & ~0xff) | int3);
    ptrace(PTRACE_POKEDATA, child_pid, m_bp_address, data_with_int3);

    m_enabled = true;
}

void Breakpoint::disable_breakpoint(pid_t &child_pid) {
    auto data = ptrace(PTRACE_PEEKDATA, child_pid, m_bp_address, nullptr);

    //overwrite the low byte with the original data and write it back to memory.
    auto restored_data = ((data & ~0xff) | m_original_data);
    ptrace(PTRACE_POKEDATA, child_pid, m_bp_address, restored_data);

    m_enabled = false;
}


