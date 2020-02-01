# DomSet-Lossy

This folder contains a code used to check the effectiveness of lossy reduction rules for Dominating Set on sparse graphs.

The main folder is dividing into the following four sub-folders.


1. Random Graph Generator
2. Data_files
3. Exact Algorithm
1. Basic

====================

1. Basic

This folder contains codes written in c and the main file can be compiled using the following command.

$ gcc -lm main.c -o main

The executable demand a data_file which has the following format-
First-line -- Number of graphs in the file
Second-line -- Number of times the program needs to run on the graph
Third line -- Number of configurations.
Fourth line onwards -- Each line contains a path to file which contains graphs in .mtx format.

 One can run the program with the following command.
$ ./main datafiles

=====================

2. Exact Algorithm

This folder contains code written in C++ using CPLEX (v$12.8.0.0$) Concert Technology for C++ to compute almost the exact dominating set for a given graph.

$ make
$ ./main datafiles

=====================

3. Random Graph Generator

This folder contains code written in Python (v3.5) using Networkx (v$1.11$). Run the following command to generate various kinds of random graphs.

$ python main.py

=====================

4. Data_files

This folder has a collection of data files generated and/or collected to run the above algorithm.

Due to space constraints, we can not upload all instances on Github. The readers can find the instances we have generated here [1] in tar.gz. format.


[1] https://www.dropbox.com/s/jgkay8dshq2jzrl/Instances.tar.gz?dl=0
