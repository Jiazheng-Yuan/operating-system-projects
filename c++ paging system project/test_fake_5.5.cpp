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
    strcpy(filename1, "data1.bin");
    strcpy(filename2, "data1.bin");
    fork();
    vm_yield();
    char *p1 = (char *) vm_map(filename1, 0);
    char *p2 = (char *) vm_map(filename2, 0);
    
    fork();
    vm_yield();
    cout<<p1[0]<<endl;
    cout<<p2[0]<<endl;


    strcpy(filename, "can you see me?");

    char *filename3= (char *) vm_map(nullptr, 100);
    strcpy(filename3, "whatever");
    fork();
    vm_yield();
    cout << filename[0] << endl;
    return 0;
}