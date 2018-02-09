#include "thread.h"
#include <iostream>

/*
pong wait
pong wait
ping
pong
pong
*/

using namespace std;

bool print_ping = true;
mutex mu1;
cv pong_wait;
int num_pong = 0;

void ping(void *a){
	mu1.lock();
	cout << "ping" << endl;
	print_ping = false;
	//pong_wait.broadcast();
	pong_wait.signal();
	mu1.unlock();
}

void pong(void *a){
	mu1.lock();
	while (print_ping){
		cout << "pong wait" <<endl;
		pong_wait.wait(mu1);
	}
	cout << "pong" <<endl;
	num_pong++;
	if (num_pong == 2){
		print_ping = true;
		num_pong = 0;
	}
	mu1.unlock();
}

void start(void *a){
	
	thread pong1 ((thread_startfunc_t) pong, (void *) "pong");
	thread pong2 ((thread_startfunc_t) pong, (void *) "pong");
	
	thread ping1 ((thread_startfunc_t) ping, (void *) "ping");
}

int main(){
	cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}
