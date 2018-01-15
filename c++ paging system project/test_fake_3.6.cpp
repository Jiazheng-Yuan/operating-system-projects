#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main(){ 
    char *filename = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);
    strcpy(filename, "shakespeare.txt");

    char *filenameFake = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filenameFake, "data1.bin"); 
    cout<< filename[0]<<endl;
    fork();
    vm_yield();
    cout<< filenameFake[0]<<endl;
    fork();
    vm_yield();
    cout<< filenameFake[0]<<endl;
    fork();
    vm_yield();
    cout<< filenameFake[0]<<endl;
    char* filename4 = (char *) vm_map(filename,0);
    strcpy(filename4,"caonima");


    return 0;
}