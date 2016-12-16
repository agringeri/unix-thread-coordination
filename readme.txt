Anthony Gringeri
acgringeri
CS 3013 A16, Project 3, due Friday, September 30, 2016

This program is a basic implementation of thread coordination and resource sharing using threads and semaphores.

Contained is the program 'graph' which takes a text file as an argument as configuration.

When the program is run, a thread is assigned to each node of the configuration file, and computation is carried out given the parameters of the text file. 

To compile, a Makefile is included. Use the command 'make'.

To run, use './graph exampleconfiguration.txt'.

A script named 'proj3test.scr' is included which contains a sample session of a few different configuration files, both valid and invalid.

Additional comments:
	One known problem with this program is its inability to deal with configuration files with any empty lines, even at the end of the file. If an empty line is encountered, the program prints the error message that the configuration file has an incorrect format. To fix, simply delete any empty lines within the text file and ensure that there are no empty lines at the end of the file. Other than that, no known problems exist.
	Only part one of the project is completed.


