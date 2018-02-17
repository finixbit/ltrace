#ifndef H_SDB_FUNCTIONS
#define H_SDB_FUNCTIONS

#include <vector> // vector
#include <unordered_map> // unordered_map
#include <map> // map
#include <capstone/capstone.h>


class Callsite {
    public:
        void print_function_insn(bool print = false);
        void print_function_blocks_leaders();
        static intptr_t string_to_intptr(std::string str);

        static void cleanup_functions();        
        static bool get_function(std::intptr_t fn_address, Function &fn); 
        static void generate_function(
            std::intptr_t fn_address, 
            cs_insn *disassembled_ins, size_t &total_ins, size_t start_idx);

        static void generate_callsite(
            Function &fn, 
            cs_insn &insn, cs_insn &next_insn, bool print = false);
        
        static void generate_branchsite(
            Function &fn, cs_insn &insn, 
            cs_insn &next_insn, cs_insn &prev_insn, bool print = false);
        
        // cs_address: instruction_here
        std::intptr_t m_cs_address = 0; 

        // reloc      (call free)
        // symbol     (call symbol)
        // unresolved (call eax/%rbp+8)
        std::string m_cs_type;

        // next instruction following cs_address
        std::intptr_t m_cs_return_address = 0; 

        // reloc|symbol|register
        std::string m_cs_name;

        static std::unordered_map<std::intptr_t, Callsite> m_callsite_map;
};


#endif