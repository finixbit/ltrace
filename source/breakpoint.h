#ifndef H_SDB_BREAKPOINT
#define H_SDB_BREAKPOINT

#include <vector> // vector
#include <unordered_map> // unordered_map
#include <signal.h> // siginfo_t
#include <sys/user.h> /* user_regs_struct */
#include <capstone/capstone.h>


class Breakpoint {
    public:
        // static void add_function_breakpoint(
        //     pid_t &child_pid, std::intptr_t fn_address, std::intptr_t fn_return_address);

        // static void add_callsite_breakpoint(
        //     pid_t &child_pid, 
        //     std::intptr_t fn_address, std::intptr_t cs_address, std::intptr_t cs_return_address);

        static void set_breakpoints(pid_t &child_pid);
        static void continue_execution(pid_t &child_pid);
        static void wait_for_signal(pid_t &child_pid);
        static void step_over_breakpoint(pid_t &child_pid);
        static siginfo_t get_signal_info(pid_t &child_pid);

        static uint64_t get_program_counter(pid_t &child_pid);
        static void set_program_counter(pid_t &child_pid, uint64_t &pc);

        static user_regs_struct get_registers(pid_t &child_pid);
        static void set_registers(pid_t &child_pid, user_regs_struct &regs);
        static void print_registers(user_regs_struct &regs);
        
        void enable_breakpoint(pid_t &child_pid);
        void disable_breakpoint(pid_t &child_pid);

        std::intptr_t m_bp_address = 0;
        uint8_t m_original_data = 0;
        bool m_enabled = false;

        std::intptr_t m_cs_caller_address = 0;      // function calling from

        bool m_is_cs_return_address = false;
        std::intptr_t m_cs_return_caller_address = 0; // address for its m_cs_caller_address

        bool m_is_caller_address = false;           // used only when symbol is not found

        static std::unordered_map<std::intptr_t, Breakpoint> m_breakpoints_map;

    private:
        pid_t m_child_pid;
};


#endif