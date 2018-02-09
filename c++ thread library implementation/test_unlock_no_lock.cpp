#include "thread.h"
#include<stdexcept>
#include <iostream>

mutex mutex1;

void lock_second(void* a){
	try {
		mutex1.unlock();
	} catch (std::runtime_error& e){
		std::cout << "unlock without lock" << std::endl;
		exit(0);
	}
}

void lock_first(void* a){
	mutex1.lock();
	thread t ((thread_startfunc_t) lock_second, (void *) 1);
	thread::yield();
	mutex1.unlock();
}

int main(){
	cpu::boot(1, (thread_startfunc_t) lock_first, (void *) 100, false, false, 0);
}