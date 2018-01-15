pager.exe: impl.o libvm_pager.o
	g++ -g -Wall -o vm_pager.cpp libvm_pager.o impl.o -std=c++11

impl.o:
	g++ impl.cpp -c -g -std=c++11


clean:
	rm pager.exe impl.o