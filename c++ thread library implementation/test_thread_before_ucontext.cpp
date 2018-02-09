#include "thread.h"
#include <iostream>

using namespace std;

void thread_func(void* a){
	cout << "thread before ucontext" << endl;
}

void start(void* a){
	thread t ((thread_startfunc_t) thread_func, (void *) 1);
}

int main(){
	cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}