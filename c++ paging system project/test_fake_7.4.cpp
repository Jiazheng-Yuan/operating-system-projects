#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filename, "data1.bin");

    char *p = (char *) vm_map(filename, 0);
    
    fork();
    vm_yield();

    p[0] = '\?';

    char *d1 = (char *) vm_map(nullptr, 0);
    fork();
    vm_yield();
    d1[0] = '1';
    fork();
    vm_yield();
    p[0] = 'e';

    p[0] = 'c';

    return 0;
}
