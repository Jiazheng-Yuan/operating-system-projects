g++ impl.cpp -c -g -std=c++11

g++ -g -Wall vm_pager.cpp -o pager libvm_pager.o impl.o -std=c++11