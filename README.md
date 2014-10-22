RSHELL
====

Licensing Information: READ LICENSE
---
Project source can be downloaded from https://github.com/khuan013/rshell.git
---

Author & Contributor List
----------

Kenneth Huang

All known bugs and fixes can be sent to khuan013@ucr.edu

File List
--------
```
.:
Makefile

LICENSE

README.md

./src

./tests
```
```
/src:

rshell.cpp
```
```
/tests:
exec.script
```

How to run file
---------------
1. `cd` into `rshell` directory
2. Call `make`
3. `cd` into `bin`
4. Call `rshell`

Rshell runs instead of bash, type `exit` to quit the program.

Overview
------------
This program was created to be a simple command shell.
It can take in multiple commands using the connectors, `&&` `||` and `;`.
Uses `wait()`, `fork()` and  `execvp()` to call functions.

Parses the input by breaking it up into a string vector, and then determines
which elements should be sent to execvp.

Connectors work based on the most recent command. If the command before an &&
did not succeed, no further commands will be executed. If a command before an
|| does succeed, no further commands will be executed.

Also displays the user and hostname of the machine the user is logged into.

Bugs and Limitations
----------
* When running the program in a script on local machine,
the getlogin() function causes an error.
* Does not handle quotation marks or parantheses.
