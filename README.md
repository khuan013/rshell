RSHELL
====

Licensing Information: MIT 
---
Project source can be downloaded from https://github.com/khuan013/rshell.git
---

Author & Contributor List
----------

Kenneth Huang

All known bugs and fixes can be sent to khuan013@ucr.edu

How to run files
---------------
1. `cd rshell`
2. `make all`
3. `bin/rshell`
4. `bin/ls`
5. `bin/cp`


Rshell runs instead of bash, type `exit` to quit the program.

Overview
------------

rshell
---

This program was created to be a simple command shell.
It can take in multiple commands using the connectors, `&&` `||` and `;`.
Uses `wait()`, `fork()` and  `execvp()` to call functions.

Parses the input by breaking it up into a string vector, and then determines
which elements should be sent to execv.

Connectors work based on the most recent command. If the command before an &&
did not succeed, no further commands will be executed. If a command before an
|| does succeed, no further commands will be executed.

**IO Redirection**

Rshell supports: 
* input redirection with `<`
* output redirection with `>` and `>>`
* piping with `|`
* redirecting from string with `<<<`
* output redirection with file descriptors by placing a number before `>` or `>>`

Like bash, if chaining together multiple input redirections or multiple output redirections, only the last one will be used.

Rshell searches through the user's current PATH environment variable for commands. Rshell no longer exits when the user typoes `^C`.

Rshell now supports `cd` which allows the user to change the current working directory.

ls
---

This program will output the contents of a directory. If no directory
is passed as an argument, it will output the `.` directory. Also displays different colors for different types of files.

ls accepts 3 flags

1. -a Will show hidden files.
2. -l Outputs the log of each file, including permissions, last modified and file size.
3. -R Will output the contents of folders within the current one recursively.

cp
---

This program will copy the contents of a text file into a created file.

It does this in 3 ways.

1. get and put functions to copy one character at a time
2. read() and write() functions to copy one character at a time
3. read() and write() to copy one buffer at a time.

This program takes up to 3 arguments in this order.

1. Name of text file
2. Name of copied file to be created
3. "-a" flag (optional)

By default it will copy by the 3rd way. If run with the -a flag, all 3
ways will be attempted and timed.

Bugs and Limitations
----------

rshell
---
* Does not error check number of `<` or `>`. For example, `ls >>> hello` will result in an error.
* Does not check for regular expressions.

ls
---
* Output is not formatted the same way. 
* Not sorted case insensitive.

cp
---
* Only able to copy text files.
* The -a flag must be the 3rd argument list.
