# ltrace
Simple Library calls tracer

# Required libraries
capstone 
finixbit/elf-parser

# Installation

```
$ sudo apt-get install libcapstone-dev
$ git clone --recursive https://github.com/finixbit/ltrace.git
$ cd ltrace
$ make
```

# Usage 

```
./ltrace <cmd>
```

# Example
```
./ltrace /bin/ps

breakpoint[0x402c0b]   call: __libc_start_main 
breakpoint[0x40350c]   call: look_up_our_self 
breakpoint[0x403511] return: look_up_our_self 
breakpoint[0x403520]   call: ioctl 
breakpoint[0x403525] return: ioctl 
breakpoint[0x40370f]   call: ioctl 
breakpoint[0x403714] return: ioctl 
breakpoint[0x40355a]   call: isatty 
breakpoint[0x40355f] return: isatty 
breakpoint[0x403572]   call: getenv 
breakpoint[0x403577] return: getenv 
breakpoint[0x40358a]   call: getenv 
breakpoint[0x40358f] return: getenv 
breakpoint[0x4030d6]   call: getenv 
breakpoint[0x4030db] return: getenv 
breakpoint[0x4031bd]   call: getenv 
breakpoint[0x4031c2] return: getenv 
breakpoint[0x4031e2]   call: getenv 
breakpoint[0x4031e7] return: getenv 
breakpoint[0x40311b]   call: __strncpy_chk 
breakpoint[0x403120] return: __strncpy_chk 
breakpoint[0x403127]   call: __strdup 
breakpoint[0x40312c] return: __strdup 
...<redacted>
```

