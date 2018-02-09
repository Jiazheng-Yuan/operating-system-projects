#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int MAX_NUM = 2;
int current_num_products = 0;
int total_num = 5;

mutex product_num_lock;
cv consumer_cv;
cv producer_cv;

void produce(void *a){
	//intptr_t id = (intptr_t) a;

	
	while (1){		
		product_num_lock.lock();
		cout << "producer lock" << endl;
		while (current_num_products == MAX_NUM){
			cout << "full" <<endl;
			producer_cv.wait(product_num_lock);
		}
		cout << "producer produce" << endl;
		current_num_products++;
		total_num--;
		cout << "producer signal" << endl;
		consumer_cv.signal();
		cout << "producer unlock" << endl;
		product_num_lock.unlock();
		//cout<<"total num is :"<<total_num<<endl;
		if (total_num == 0){
			break;
		}
	}
	
}

void consume(void *a){
	//intptr_t id = (intptr_t) a;

	while (1){

		product_num_lock.lock();
		cout << "consumer lock" << endl;
		while (current_num_products == 0){
			cout << "empty" <<endl;
			consumer_cv.wait(product_num_lock);
			
		}
		cout << "consumer consume" << endl;
		current_num_products--;
		cout << "consumer signal" << endl;
		producer_cv.signal();
		cout << "consumer unlock" << endl;
		product_num_lock.unlock();
	}
	
}

void start(void *a){
	thread t2 ( (thread_startfunc_t) consume, (void *) 1);
	thread t1 ( (thread_startfunc_t) produce, (void *) 0);
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}
