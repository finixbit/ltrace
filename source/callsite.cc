#include <algorithm> // find
#include "symbols.h"
#include "relocations.h"
#include "functions.h"
#include "disassembler.h"
#include "memory_common.h"
using namespace sdb;

std::unordered_map<std::intptr_t, Function> Function::m_functions_map;

std::unordered_map<std::string, uint8_t> Function::m_branch_type_table = {
  {"jo",  0x70},
  {"jno", 0x71},  {"jb",  0x72}, {"jnae", 0x72},  {"jc",  0x72},  {"jnb", 0x73},
  {"jae", 0x73},  {"jnc", 0x73}, {"jz",   0x74},  {"je",  0x74},  {"jnz", 0x75},
  {"jne", 0x75},  {"jbe", 0x76}, {"jna",  0x76},  {"jnbe",0x77},  {"ja",  0x77},
  {"js",  0x78},  {"jns", 0x79}, {"jp",   0x7a},  {"jpe", 0x7a},  {"jnp", 0x7b},
  {"jpo", 0x7b},  {"jl",  0x7c}, {"jnge", 0x7c},  {"jnl", 0x7d},  {"jge", 0x7d},
  {"jle", 0x7e},  {"jng", 0x7e}, {"jnle", 0x7f},  {"jg",  0x7f},  {"jmp", 0xeb},
  {"jmp", 0xe9},  {"jmpf",0xea}, {"",     0}
};


bool Function::get_function(std::intptr_t fn_address, Function &fn) {
  if(!Function::m_functions_map.count(fn_address))
    return false;

  fn = Function::m_functions_map[fn_address];
  return true;
}

void Function::generate_function(
  std::intptr_t fn_address, 
  cs_insn *disassembled_ins, size_t &total_ins, size_t start_idx) {
  
  auto &sym = Symbol::m_symbols_map[fn_address];

  Function fn;
  fn.m_fn_address = sym.symbol_value;
  fn.m_fn_name = sym.symbol_name;
  fn.m_fn_filename = sym.symbol_filename;

  // set The first instruction is a leader.
  fn.m_fn_block_leaders.push_back(fn.m_fn_address);
  
  for (size_t j = start_idx; j < total_ins; j++) {
    auto address = disassembled_ins[j].address; 
    auto insn = disassembled_ins[j];
    auto next_insn = disassembled_ins[j+1];

    fn.m_fn_insn[address] = insn;

    if(address != fn.m_fn_address) { 
      if(Symbol::m_symbols_map.count((std::intptr_t)insn.address)) {
        fn.m_fn_insn.erase(address); // remove ins from fn asm insn
        fn.m_fn_return_address = disassembled_ins[j-1].address;
        break;
      }
    }

    if(std::string(insn.mnemonic) == "ret") {
      fn.m_fn_return_address = address;
      break;
    }    

    if(Function::m_branch_type_table.count(insn.mnemonic)) {
      Function::generate_branchsite(
        fn, insn, disassembled_ins[j+1], disassembled_ins[j-1], true);
    }

    Function::generate_callsite(fn, insn, next_insn);
  }
  
  Function::m_functions_map[fn.m_fn_address] = fn; 
  
  fn.print_function_insn(false);
  fn.m_fn_insn.clear(); // clear function asm insns
}

void Function::cleanup_functions() {
}

void Function::print_function_insn(bool print) {
  if(!print)
    return;

  printf(
    "\n\n= %s (0x%" PRIx64 ")\n", m_fn_name.c_str(), m_fn_address);
  
  for(auto &insn: m_fn_insn) {
    printf("0x%" PRIx64 ":\t%s\t\t%s\n", 
      insn.second.address, insn.second.mnemonic, insn.second.op_str);
  }
}

void Function::generate_branchsite(Function &fn, 
  cs_insn &insn, cs_insn &next_insn, cs_insn &prev_insn, bool print) {
  bool resolved_branch = false;
  try {
    // The target of a conditional or an unconditional goto/jump 
    // instruction is a leader.
    std::intptr_t ptr = std::stol(std::string(insn.op_str), nullptr, 0);
    resolved_branch = true;
    
    if(std::find(
      fn.m_fn_block_leaders.begin(), 
      fn.m_fn_block_leaders.end(), ptr) == fn.m_fn_block_leaders.end()) {

      if(!Symbol::m_symbols_map.count(ptr))
        fn.m_fn_block_leaders.push_back(ptr);
    }    
  } catch(...) {}

  try {
    std::intptr_t ptr = (std::intptr_t)next_insn.address;

    // The instruction that immediately follows a conditional 
    // goto/jump instruction is a leader
    if(std::find(
      fn.m_fn_block_leaders.begin(), 
      fn.m_fn_block_leaders.end(), ptr) == fn.m_fn_block_leaders.end()) {

      if((!Symbol::m_symbols_map.count(ptr)) &&
         (next_insn.mnemonic != "nop")) {
        fn.m_fn_block_leaders.push_back(ptr);
      } 
    }
  } catch(...) {}
}

void Function::generate_callsite(
  Function &fn, cs_insn &insn, cs_insn &next_insn, bool print) {
  
  if(std::string(insn.mnemonic) == "call") {
    callsite_t cs;
    cs.cs_type = "unresolved";
    cs.cs_address = insn.address;
    cs.cs_name = std::string(insn.op_str);
    
    try {
      std::intptr_t ptr = std::stol(std::string(insn.op_str), nullptr, 0);
      cs.cs_location = ptr;

      if(Symbol::m_symbols_map.count(ptr)) {
        auto &sym = Symbol::m_symbols_map[ptr];
        cs.cs_type = "symbol";
        cs.cs_name = sym.symbol_name;
      }

      if(Relocation::m_relocations_map.count(ptr)) {
        auto &reloc = Relocation::m_relocations_map[ptr];
        cs.cs_type = "reloc";
        cs.cs_name = reloc.reloc_sym_name;
      }

      cs.cs_memory_fn = false;
      if((cs.cs_type == "reloc") || (cs.cs_type == "symbol")) {
        if(is_memory_function(cs.cs_name))
          cs.cs_memory_fn = true;
          cs.cs_return_address = next_insn.address;
      }
    } catch(...) {}

    fn.m_fn_callsites[cs.cs_address] = cs;

    if(print)
      printf("0x%" PRIx64 ":\t%s\t\t%s(%s, %s)\n", insn.address, 
        insn.mnemonic, insn.op_str, cs.cs_name.c_str(), cs.cs_type.c_str());
  }
}

void Function::print_function_blocks_leaders() {
  for(auto& bnch: m_fn_block_leaders) {
    printf("0x%" PRIx64 ":\t%s\n", bnch, m_fn_name.c_str());
  }
}

intptr_t Function::string_to_intptr(std::string str) {
  return (intptr_t)std::stoi(str);
}