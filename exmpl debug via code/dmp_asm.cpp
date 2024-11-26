// dump_assembly.cpp
#include <stdio.h>
#include <stdlib.h>

void showAssemblyDumpWithBreakpoints(const char *targetProgram) {
    // Create the GDB command to load the target program, set breakpoints, and disassemble
    char command[512];

    snprintf(command, sizeof(command),
             "gdb -batch "
             "-ex 'file %s' "
             "-ex 'break main' "          // Set a breakpoint at the start of the main function
             "-ex 'break greet' "         // Set a breakpoint at the greet function
             "-ex 'run' "
             "-ex 'disassemble main' "
             "-ex 'disassemble greet' "   // Disassemble the greet function
             "-ex 'info breakpoints'",    // Display active breakpoints
             targetProgram);

    // Run the command and capture the output
    printf("Assembly dump with breakpoints for %s:\n\n", targetProgram);
    int result = system(command);
    
    if (result == -1) {
        perror("Error running GDB command");
    }
}

int main() {
    int result = system("gcc -g target.c -o target");

    const char *targetProgram = "./target";  // Path to the compiled target program

    showAssemblyDumpWithBreakpoints(targetProgram);

    return 0;
}
