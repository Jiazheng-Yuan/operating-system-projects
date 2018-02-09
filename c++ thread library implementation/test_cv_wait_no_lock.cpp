#include "thread.h"
#include<stdexcept>
#include <iostream>

mutex mutex1;
cv cv1;

void waitLock(void* a){
	try {
		cv1.wait(mutex1);
	} catch (std::runtime_error& e){
		std::cout << "cv wait no lock"<< std::endl;
		exit(0);
	}
}

void lock_first(void* a){
	mutex1.lock();
	thread t ((thread_startfunc_t) waitLock, (void *) 1);
}

int main(){
	cpu::boot(1, (thread_startfunc_t) lock_first, (void *) 100, false, false, 0);
}