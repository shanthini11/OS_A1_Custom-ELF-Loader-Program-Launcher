This project implements a minimal custom ELF loader and a user-level program launcher, built as part of our Operating Systems course held in the Monsoon'2025 semester. It demonstrates low-level systems concepts: loading an ELF into memory, setting up execution context, and transferring control to user code:without exec() ommand

What was the main goal here to achieve?
- Reading and parsing an ELF executable
- Loading its loadable segments into memory
- Preparing the stack (argc, argv)
- Jumping to its entry point to execute it
- Returning back to the launcher after execution

How does the Loader and program launcher work side by side?
Step 1: Parsing the ELF Header
- loader.c uses the ELF header to verify:
    ->File type (32-bit, little-endian ELF)
    ->Program header offset
    ->Number of program headers

Step 2: Reading Program Headers
For each loadable segment (PT_LOAD) we then allocated memory,copied the segment contents to memory and applied zero-padding for .bss portions. This mimics how the kernel maps segments during program loading.

Step 3: Stack Setup
The loader prepares the user stack where it does these commands of(Push argc, Push argv[], Push a null sentinel) It then sets the stack pointer to the region again

Step 4: Jumping to the Entry Point
The loader then gets:
- entry = elf_header->e_entry; (and then jumps to this function pointer with this particular command:((void(*)()) entry)();)
- After this,execution begins inside the loaded program (fib).

Step 5: Returning to Launcher
Once user code finishes, control returns to launch.c, which prints the required status messages

--------------------------------------------------------------------------------------
Credits towards making this assignment: Manojna Kamaram Reddy and Shanthini Muralidhar
Please note this was done as an assignment under course regulations, please use it as a referal guide
