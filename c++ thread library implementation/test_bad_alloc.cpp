#include "thread.h"
#include <iostream>

int num = 1000000;

void thread_func(void* a){
	//intptr_t i = (intptr_t) a;
	//std::cout << i << std::endl;
	return;
}

void createThread(void *a){
	for (intptr_t i = 0; i < num; i++) {
		try{
			thread t ((thread_startfunc_t) thread_func, (void *) i);
			thread::yield();
		} catch (std::bad_alloc& BadAlloc) {
			std::cout << "allocate too much memory" << std::endl;
			exit(0);
		}
	}
}

int main(){
	cpu::boot(1, (thread_startfunc_t) createThread, (void *) 0, false, false, 0);
}
