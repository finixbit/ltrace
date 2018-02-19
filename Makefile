

all: callsite disassembler breakpoint ltrace 

ltrace: source/ltrace.cc 
	g++ -o ltrace source/ltrace.cc callsite.o disassembler.o breakpoint.o -std=gnu++11 -lcapstone

breakpoint: source/breakpoint.cc 
	g++ -o breakpoint.o -c source/breakpoint.cc -std=gnu++11 

callsite: source/callsite.cc 
	g++ -o callsite.o -c source/callsite.cc -std=gnu++11 

disassembler: source/disassembler.cc 
	g++ -o disassembler.o -c source/disassembler.cc -std=gnu++11 -lcapstone

clean:
	rm -f ltrace disassembler.o callsite.o breakpoint.o