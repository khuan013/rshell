RSHELL
===

Licensing Information: READ LICENSE
---
Project source can be downloaded from https://github.com/khuan013/rshell.git
----

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
4. call `rshell`

Rshell runs instead of bash, type `exit` to quit the program.

Overview
------------
This program was created to be a simple command shell.
It can take in multiple commands using the connectors, `&&` `||` and `;`.
Uses `wait()`, `fork()` and  `execvp()` to call functions.

Bugs
----------
To be discovered.
