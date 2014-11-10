OPTIONS = -Wall -Werror -ansi -pedantic

all: rshell ls cp

rshell:
	mkdir -p bin
	cd src && g++ $(OPTIONS) -o rshell rshell.cpp &&\
	mv rshell ../bin

ls:
	mkdir -p bin
	cd src && g++ $(OPTIONS) -o ls ls.cpp &&\
	mv ls ../bin

cp:
	mkdir -p bin
	cd src && g++ $(OPTIONS) -o cp cp.cpp && \
		mv cp ../bin

clean:
	rm -rf bin

new: clean all
