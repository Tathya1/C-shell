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

IMAGES FROM CHATGPT

![part2](https://github.com/user-attachments/assets/4adc9c3d-a1d1-412f-b0cc-ad4c2d643676)

![part3](https://github.com/user-attachments/assets/403ed896-0df9-4ee2-9afc-6f7f68653504)

![part4](https://github.com/user-attachments/assets/3fd32c3a-1343-4bab-86ae-44c1619f283b)

![part16](https://github.com/user-attachments/assets/33f1b662-b687-41b5-9276-1a428f341141)

![part17](https://github.com/user-attachments/assets/8cde6fb3-a3a0-45d9-8666-8fbc3ff37679)

![part18](https://github.com/user-attachments/assets/6a75bbd5-0dee-4805-a246-9c67b2ac5024)

![part19](https://github.com/user-attachments/assets/91bc654f-36bf-478c-b4a6-5617ed0594f1)

![part5](https://github.com/user-attachments/assets/3539e263-f769-48f8-9221-e04ed8e3e477)

![part6](https://github.com/user-attachments/assets/041ecdaa-5f41-47c5-a831-b5aacaec5afe)

![part7](https://github.com/user-attachments/assets/e9a92fbc-032b-476c-a720-25d65453afa7)

![part8](https://github.com/user-attachments/assets/4ec91638-ddc8-4373-86e9-38fb21e517a4)

![part9](https://github.com/user-attachments/assets/e3b3be39-2727-4622-828b-bec7e473019a)

![part10](https://github.com/user-attachments/assets/dfafccbc-82cc-4a19-8101-555022ca122c)

![part11](https://github.com/user-attachments/assets/57e5188f-9e49-422f-9fcf-c69f1fc8808c)

![part12](https://github.com/user-attachments/assets/fe13569d-eac8-4b37-92bb-b19ec667a0a9)

![part13](https://github.com/user-attachments/assets/c24ba8fc-f512-456a-ae65-359137ad0328)

![part14](https://github.com/user-attachments/assets/0fad42be-feb1-46a1-89a1-3ca52e46023d)

![part15](https://github.com/user-attachments/assets/111f2315-2c92-4cd8-ba01-e139f3ef3429)


