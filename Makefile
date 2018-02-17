

all: ltrace

ltrace: source/ltrace.cc 
	g++ -o ltrace source/ltrace.cc -std=gnu++11 

# breakpoint: source/breakpoint.cc 
# 	g++ -o breakpoint source/breakpoint.cc -std=gnu++11 

# callsite: callsite.cc 
# 	g++ -o callsite source/callsite.cc -std=gnu++11 

# disassembler: source/disassembler.cc 
# 	g++ -o disassembler source/disassembler.cc -std=gnu++11 

clean:
	rm -f ltrace# breakpoint disassembler callsite