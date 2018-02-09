#include <iostream>
#include "thread.h"
#include<fstream>
using namespace std;
//testing cv broadcast, signal, multiple thread on one cv and on multiple mutex;
mutex mutex1;
mutex mutex2;
cv one_cv;
cv three_cv;
bool one_two = true;
int three_four_five = 3;
int total = 20;
//ofstream myfile;


void one(void *a){
mutex1.lock();
while(1){
if(total <= 0)
return;
if(one_two){
	cout<<"this is a one"<<endl;
	one_two = false;
	total--;
	one_cv.broadcast();one_cv.wait(mutex1);
}
else{
	one_cv.wait(mutex1);
}
}

	
}

void two(void *a){
mutex1.lock();
while(1){
if(total <= 0)
return;
if(!one_two){
	cout<<"this is a two"<<endl;
	one_two = true;
	total--;
	one_cv.signal();one_cv.wait(mutex1);
}
else{
	one_cv.wait(mutex1);
}
}
	
}
void three(void* a){cout<<"this is a three"<<endl;
mutex2.lock();
while(1){if(total <= 0)
return;
	if(three_four_five == 3){
		cout<<"this is a three"<<endl;
		cout<<total<<endl;
		three_four_five++;
		total--;
		three_cv.broadcast();
		three_cv.wait(mutex2);
	}else{three_cv.broadcast();
	three_cv.wait(mutex2);
	}
}
}
void four(void* a){
mutex2.lock();
while(1){if(total <= 0)
return;
if(three_four_five == 4){
		cout<<"this is a four"<<endl;cout<<total<<endl;
		three_four_five--;
		total--;
		three_cv.broadcast();
		three_cv.wait(mutex2);
	}else{three_cv.broadcast();
	three_cv.wait(mutex2);
	}
}
}
void five(void* a){
mutex2.lock();
while(1){if(total <= 0)
return;
if(total%10 ==0){
cout<<"this is a five"<<endl;
total--;cout<<total<<endl;
}
three_cv.broadcast();
three_cv.wait(mutex2);
}
}
void randomly(void* a){
for(int i = 0; i < 10;i++){
	cout<<"this is random "<<i<<endl;
}
}
void parent(void *a){
	//myfile.open("result.txt");
	
	thread one_thread ((thread_startfunc_t) one, (void *) "one");
	//cout<<"dchbjhcjdhnkcnkdncdnc,mdnc,dnc,dn,cnd,cn,dnc,n,cn,"<<endl;
	thread two_thread ((thread_startfunc_t) two, (void *) "two");

	thread four_thread((thread_startfunc_t) four, (void *) "four");
	thread five_thread((thread_startfunc_t) five, (void *) "five");	
	thread three_thread((thread_startfunc_t) three, (void *) "three");
	thread random_thread((thread_startfunc_t) randomly, (void *) "three");
}

int main()
{
    cpu::boot(5, (thread_startfunc_t) parent, (void *) 100, false, true, 0);
}