Pipex is a command-line utility designed to emulate the behavior of Unix shell, involving redirection and pipes.
It allows users to execute a sequence of shell commands while redirecting input and output streams between files and commands.
The output of the pipex program mirrors that of the UNIX shell when executing input in the following format:

< file1 command1 | command2 > file2

Key features of pipex include:

* Handling of input and output files: pipex can take input from one file and redirect output to another file, similar to the '<' and '>' operators in Unix shell commands.
* Command execution: pipex executes two shell commands in sequence, passing the output of the first command as input to the second command, similar to the '|' operator in Unix shell commands.
* Error handling: pipex includes robust error handling to detect and handle various error conditions, such as invalid command syntax, file I/O errors, and command execution failures.
* Bonus features: In addition to the mandatory functionality, pipex offers bonus features such as support for multiple pipes and the 'here_doc' feature, allowing input to be provided directly from the command line.


Error Handling:

Pipex incorporates robust error handling to ensure the program behaves predictably and gracefully in case of unexpected scenarios. Here are some key aspects of the error handling implemented:

* Thorough Error Checking: pipex extensively checks for errors throughout its execution, ensuring that the program does not terminate unexpectedly due to segmentation faults, bus errors, double frees, or other undefined behaviors.
* Error Messages: In the event of an error, pipex provides informative error messages to help users diagnose and address issues. These messages are designed to be clear and actionable, aiding users in resolving problems efficiently.
* Memory Management: All heap-allocated memory is properly managed and freed when necessary to prevent memory leaks. Pipex aims to maintain efficient memory usage throughout its execution.
* File Descriptor Management: pipex meticulously manages file descriptors throughout its execution to prevent leaks. It ensures that all file descriptors opened during program execution are properly closed before the program exits.


MANDATORY PART

The mandatory part of pipex handles exactly two commands.

Usage

To compile the mandatory part, use make or make all.
The program takes exactly 5 arguments, including exactly two commands.

This:

./pipex infile "ls -l" "wc -l" outfile

behaves like:

< infile ls -l | wc -l > outfile


BONUS PART

The bonus part of pipex extends functionality to handle multiple commands and supports here_doc for input.

Usage

To compile the bonus part, use make bonus.

With Input File:
The program takes 5 or more arguments, including two or more commands.

This:

./pipex_bonus infile cmd1 cmd2 cmd3 ... outfile

behaves like:

< infile cmd1 | cmd2 | cmd3 ... > outfile

With Here_doc:
The program takes 6 or more arguments, including two or more commands.

This:

./pipex_bonus here_doc LIMITER cmd1 cmd2 cmd3 ... outfile

behaves like:

cmd1 << LIMITER | cmd2 | cmd3 ... >> outfile
