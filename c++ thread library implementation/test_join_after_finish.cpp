#include "thread.h"
#include <iostream>

using namespace std;

void join_func(void* a){
	cout << "run join_func" << endl;
}

void create_thread(void* a){
	cout << "wait for t" << endl;
	thread t ((thread_startfunc_t) join_func, (void *) 1);
	thread::yield();
	cout << "come back from join_func" << endl;
	t.join();
	cout << "after t finished" << endl;
}

int main(){
	cpu::boot(1, (thread_startfunc_t) create_thread, (void *) 100, false, false, 0);
}