#include <iostream>
#include <cstdlib>
#include "thread.h"

using namespace std;

int MAX_NUM = 10;
int current_num_products = 0;
int total_num = 100;

mutex product_num_lock;
cv consumer_cv;
cv producer_cv;

void produce(void *a){
	int *id = (int *) a;

	product_num_lock.lock();
	while (total_num > 0){		

		while (current_num_products == MAX_NUM){
			producer_cv.wait(product_num_lock);
		}

		cout << "~~~~~~~producer: " << id << " produces one product"<< endl;
		current_num_products++;
		total_num--;

		if (current_num_products == 1){
			consumer_cv.signal();
		}
	}
	product_num_lock.unlock();
}

void consume(void *a){
	int *id = (int *) a;

	product_num_lock.lock();
	while (total_num > 0){

		while (current_num_products == 0){
			consumer_cv.wait(product_num_lock);
		}

		cout << "************consumer: " << id << " consumes one product"<< endl;
		current_num_products--;

		if (current_num_products == MAX_NUM - 1){
			producer_cv.signal();
		}
	}
	product_num_lock.unlock();
}

void start(void *a){
	for (int i = 0; i < 3; i++){
		thread t ( (thread_startfunc_t) produce, (void *) i);
	}
	for (int i = 0; i < 4; i++){
		thread t ( (thread_startfunc_t) consume, (void *) i);
	}
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) start, (void *) 100, false, false, 0);
}