#include "thread.h"
#include <iostream>

using namespace std;

void thread_func(void* a){
	//intptr_t num = (intptr_t) a;
	cout << "ucontext before thread" << endl;
}

void start(void* a){
	thread t ((thread_startfunc_t) thread_func, (void *) 1);
	thread::yield();
}

int main(){
	cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}