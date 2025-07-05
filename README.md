# My Shell

This project is a custom shell implementation in C. The shell supports basic command execution, directory traversal, process management, and more. The source files are organized in the `codes` directory.

## Features

- **Custom Commands**: Implemented custom commands like `hop`, `reveal`, `log`, `seek`, `proclore`.
- **Directory Traversal**: Easily navigate directories with custom commands.
- **Process Management**: Manage processes with custom and built-in commands.
- **Signal Handling**: Handles signals to manage processes and cleanup resources.
- **Colored Output**: Outputs file and directory names in colored text for easier identification.

## Files and Directories

- `main.c`: The main entry point for the shell program.
- `split.c`: Utility functions for string splitting and command parsing.
- `log.c`: Handles logging and history management.
- `process.c`: Manages processes and signal handling.
- `hop.c`: Implements the `hop` command for directory traversal.
- `reveal.c`: Implements the `reveal` command for listing files and directories.
- `proclore.c`: Implements process-related commands.
- `seek.c`: Implements the `seek` command for file and directory search.

## Limitations and Assumptions

The shell implementation makes certain assumptions and has specific limitations based on defined constants:

- **Maximum Sizes**:
  - `MAX_SIZE = 4097`: Maximum size for buffers used to store paths, usernames, system names, etc.
  - `max_comm_size = 100`: Maximum number of commands that can be handled at once.
  - `BUFFER_SIZE = 1024`: Buffer size for input and output operations.
  - `concatenated_commands[256 * 100]`: Buffer for storing concatenated commands.
  - `log_path[MAX_SIZE]`: Buffer for storing the log file path.

- **Logging**:
  - `no_of_logs = 15`: Maximum number of logs stored in memory.
  - `commands_logged = 0`: Counter for the number of commands logged during a session.

- **Background Processes**:
  - `MAX_BG_PROCESSES = 1024`: Maximum number of background processes that the shell can track simultaneously.
  - `bg_process_count = 0`: Counter for tracking the current number of background processes.

- **Memory Allocation**:
  - Dynamically allocates memory for background flags using `malloc`. The implementation assumes successful memory allocation without errors.

- **Foreground Process Time Tracking**:
  - The shell only tracks and adds to the total time slice for foreground processes whose time slice exceeds 2 seconds. However, it prints the total time that all foreground processes have accumulated.


- **Signal Handling**:
  - Utilizes `struct sigaction` and handles signals using the `SA_RESTART` and `SA_NOCLDSTOP` flags.

- **Libraries**:
  - The shell relies on standard C libraries like `unistd.h`, `signal.h`, `stdlib.h`, `stdio.h`, and custom header files like `split.h`.


### Compilation

To compile the project, run the following command in the root directory:

```bash
make

After compilation, you can run the shell using
make run

Alternatively, you can directly run the executable
./a.out

To remove the compiled files and the executable, run
make clean
```

