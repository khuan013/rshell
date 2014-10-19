OPTIONS = -Wall -Werror -ansi -pedantic

all: rshell

rshell:
	mkdir -p bin
	cd src && g++ $(OPTIONS) -o rshell rshell.cpp &&\
	mv rshell ../bin

clean:
	rm -rf bin

new: clean all
