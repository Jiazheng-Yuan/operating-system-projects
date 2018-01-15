#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main(){ 
    char *filename = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);
    //strcpy(filename, "shakespeare.txt");

    char *filename1= (char *) vm_map(nullptr, 9);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    //strcpy(filenameFake, "data1.bin"); 
    char *filename2= (char *) vm_map(nullptr, 100);
    fork();
    vm_yield();
    strcpy(filename2, "data1.bin");
    fork();
    filename1[0] = 'c';
    filename[0] = 'd';
    
    char *filename3= (char *) vm_map(nullptr, 100);
    vm_yield();
    filename3[0] = 'e';
    char *filename4= (char *) vm_map(filename2, 0);
    cout<<filename4[0]<<endl;
    fork();
    vm_yield();
    return 0;
}