# this is makefile for MeMS

all: clean example 

example: example.c mems.h
	gcc -w -o example example.c
	./example
clean:
	rm -rf example