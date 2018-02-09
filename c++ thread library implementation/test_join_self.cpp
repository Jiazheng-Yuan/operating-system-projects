#include "thread.h"
#include <iostream>

thread* t;
bool finish = false;
mutex mutex1;
cv cv1;

void self_join(void *a){
	mutex1.lock();
	finish = true;
	(*t).join();
}

void create_t(void *a){
	mutex1.lock();
	thread t2 ((thread_startfunc_t) self_join, (void *) 1);
	t = &t2;
	while (!finish){
		cv1.wait(mutex1);
	}
	mutex1.unlock();
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) create_t, (void *) 100, false, false, 0);
}