// print ping pong

#include <iostream>
#include "thread.h"
#include<fstream>
using namespace std;

mutex mutex1;
cv ping_cv;
cv pong_cv;
bool print_ping = true;
//ofstream myfile;


void ping(void *a){
	for (int i = 0; i < 5; i++){
		mutex1.lock();
		cout<<"ping get lock"<<endl;
		while (!print_ping){
		cout<<"ping wait"<<endl;
			ping_cv.wait(mutex1);
			cout<<"ping wake up"<<endl;
		}
		cout<< "ping\n";
		print_ping = false;
		cout<<"ping signal pong"<<endl;
		pong_cv.signal();
		cout<<"ping unlocking"<<endl;
		mutex1.unlock();
	}
}

void pong(void *a){
	
	for (int i = 0; i < 5; i++){
		mutex1.lock();
		cout<<"pong get the lock"<<endl;
		while (print_ping){
		cout<<"pong wait"<<endl;
			pong_cv.wait(mutex1);
			cout<<"pong wake up"<<endl;
		}
		cout << "pong\n";
		print_ping = true;
			cout<<"pong signal ping"<<endl;
		ping_cv.signal();
			cout<<"pong unlocking"<<endl;
		mutex1.unlock();
	}
}

void parent(void *a){
	//myfile.open("result.txt");
	
	thread ping_thread ((thread_startfunc_t) ping, (void *) "ping");
	//cout<<"dchbjhcjdhnkcnkdncdnc,mdnc,dnc,dn,cnd,cn,dnc,n,cn,"<<endl;
	thread pong_thread ((thread_startfunc_t) pong, (void *) "pong");
}

int main()
{
    cpu::boot(1, (thread_startfunc_t) parent, (void *) 100, false, false, 0);
}
