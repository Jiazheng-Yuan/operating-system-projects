#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{   fork();
    vm_yield();
    fork();
    vm_yield();fork();
    vm_yield();fork();
    vm_yield();
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);
    strcpy(filename, "shakespeare.txt");

    char *filenameFake = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filenameFake, "shakespe");

    if (!fork()){
        char *p1 = (char*) vm_map(filename, 0);
        cout << (void *) p1 << endl;

        for (unsigned int i=0; i<2561; i++) {
           cout << p1[i];
        }

    } 
    char* p = (char *) vm_map(filenameFake, 0);
    cout << filenameFake << endl;

    cout << p[0] << endl;
    

    return 0;
}