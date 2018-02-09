#include "thread.h"
#include <iostream>
#include <cstdlib>

using namespace std;

// as long as 
// join func run before
// i finish after join

thread* t;
mutex m1;

void join_func(void* a){
	//cout << "join func run" << endl;
	printf("join func run\n");
}

void join_thread(void* a){
	intptr_t i = (intptr_t) a;
	//cout << i << " join" << endl;
	printf("%d join \n", (int) i);
	t->join();
	printf("%d finish after join\n", (int) i);
}


void start(void* a){
	t = new thread((thread_startfunc_t) join_func, (void *) 1);
	for (intptr_t i = 0; i < 4; i++){
		thread t1 ((thread_startfunc_t) join_thread, (void *) i);
	}
}

int main(){
	cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}