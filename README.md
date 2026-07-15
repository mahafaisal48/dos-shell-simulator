# DOS Shell Simulator

A C++ simulation of a classic DOS-style command shell: a virtual file system with directories and files, a modal text editor (inspired by vi), and print job scheduling with both FCFS and priority queues.

## Features

### Virtual File System (`FileSystem` / `Node`)
- Tree-structured directories and files (`V:\` as root, like a DOS drive)
- Commands: `CD`, `DIR`, `PWD`, `MKDIR`, `RMDIR`, `CREATE`, `DEL`, `COPY`, `MOVE`, `RENAME`, `TREE`, `FORMAT`, `ATTRIB`, `FIND`, `FINDF`, `FINDSTR`, `CONVERT`, `PROMPT`, `VER`, `HELP`
- Supports both relative paths and absolute `V:\path\to\file` paths
- Full tree persistence — saves/loads the entire directory structure and file metadata to `tree.txt` between sessions

### Text Editor (`Text_Editor`)
- Modal editing inspired by vi: **NORMAL**, **INSERT**, and **COMMAND** modes
- Normal mode navigation: `h`/`j`/`k`/`l` (and arrow keys), `i` to insert, `x` to delete, `0`/`$` for line start/end
- Command mode: `:wq` (save & quit), `:q` (quit, prompts to save if modified), `:q!` (force quit), `/text` and `?text` (search)
- Find-and-replace via `:%s/old/new/`
- A built-in **processing menu** (Ctrl+P) for word/character analysis: find, replace first/all, add prefix/postfix, word count, largest/smallest word, average word length, paragraph/sentence count, special character count

### Document Model (`Document` / `Para` / `Line` / `letter`)
- Custom hierarchical text representation: a document is a list of paragraphs, each a list of lines, each a linked list of individual characters (`letter`)
- Supports cursor-based navigation and editing (insert, delete, backspace, join lines, new line) directly on this structure

### Print Queue Simulation (`print` / `normalPrintQueue` / `priorityPrintQueue`)
- **Normal queue**: FCFS print job scheduling
- **Priority queue**: higher-priority jobs print first, with tie-breaking by arrival time; supports both preemptive insertion during an active print job and standard queuing
- Commands: `PRINT`, `PPRINT`, `QUEUE`, `PQUEUE`

## Known Limitations
- **Windows-only.** Uses `<conio.h>`, `<windows.h>`, `_getch()`, `SetConsoleCursorPosition`, `system("cls")`, and `ctime_s` for console control and timestamps — porting to Linux/macOS would require replacing these with POSIX/ANSI equivalents.

## Design Notes
- `COPY` duplicates a file's tree metadata (attributes, character/line counts) rather than its underlying text content — copying creates a new entry that starts as an empty document.
```
Type `HELP` at the prompt for a full command list. State (file tree + metadata) persists to `tree.txt` in the working directory and reloads automatically on the next run.
