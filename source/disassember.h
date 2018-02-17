#ifndef H_SDB_DISASSEMBER
#define H_SDB_DISASSEMBER

#include <vector> // vector
#include <map> // map
#include <unordered_map> // unordered_map
#include <link.h> /* ElfW */
#include <capstone/capstone.h> //cs_insn


class Disassembler {
    public:
        std::unordered_map<std::intptr_t, callsite_t> disassemble_ins(
            uint8_t code, int32_t size, std::intptr_t code_entry, bool print_ins);

        void disassemble_ins(
            uint8_t code, int32_t size, std::intptr_t code_entry, bool print_ins);
        
    private:
        void print_disassembled_ins(cs_insn &disassembled_ins);
        std::map<std::intptr_t, cs_insn> m_disassembled_ins;

};

#endif