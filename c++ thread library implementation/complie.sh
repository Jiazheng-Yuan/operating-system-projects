#!/bin/bash
filename=$1
g++ impl.cpp -c -g -std=c++11
g++ mutex.cpp -c -g -std=c++11 
g++ cv.cpp -c -g -std=c++11 
g++ thread.cpp -c -g -std=c++11 
g++ cpu.cpp -c -g -std=c++11
g++ -g -Wall $filename.cpp thread.o mutex.o cv.o libcpu.o cpu.o impl.o -ldl -pthread -std=c++11 -o $filename
