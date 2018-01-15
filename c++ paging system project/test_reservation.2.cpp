#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main(){
	for (int i = 0; i !=513;i++){
		char * filename = (char*)vm_map(nullptr,0);
		strcpy(filename,"hello");
	}
	//cout<<filename<<endl;
	//cout<<"fork value:"<<fork();
	return 0;
}