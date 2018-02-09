#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int total_num = 20;
int sum = 0;
mutex mutex1;
cv one_cv;
cv two_cv;
bool ind = true;

void one(void *a){
	mutex1.lock();
	while(sum!=2){
		one_cv.wait(mutex1);
	}
	cout<<"one"<<endl;
	ind = false;
	two_cv.signal();
	mutex1.unlock();
}
void two(void* a){
	mutex1.lock();
	sum++;
		while(ind){
			one_cv.signal();
			two_cv.wait(mutex1);
		}
	cout<<"two"<<endl;
	mutex1.unlock();
}

void start(void *a){
	
	thread t2 ( (thread_startfunc_t) two, (void *) 1);
	thread t1 ( (thread_startfunc_t) one, (void *) 0);
	thread t3 ( (thread_startfunc_t) two, (void *) 1);
	
	
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}
