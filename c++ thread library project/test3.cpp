// print ping pong

#include <iostream>
#include "thread.h"

using namespace std;

mutex mutex1;
cv ping_cv;
cv pong_cv;
bool print_ping = true;


void ping(void *a){
	for (int i = 0; i < 5; i++){
		mutex1.lock();
		while (!print_ping){
			ping_cv.wait(mutex1);
		}
		cout << "ping\n";
		print_ping = false;
		pong_cv.signal();
		mutex1.unlock();
	}
}

void pong(void *a){
	for (int i = 0; i < 5; i++){
		mutex1.lock();
		while (print_ping){
			pong_cv.wait(mutex1);
		}
		cout << "pong\n";
		print_ping = true;
		ping_cv.signal();
		mutex1.unlock();
	}
}

void parent(void *a){
	thread ping_thread ((thread_startfunc_t) ping, (void *) "ping");
	thread pong_thread ((thread_startfunc_t) pong, (void *) "pong");
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}
