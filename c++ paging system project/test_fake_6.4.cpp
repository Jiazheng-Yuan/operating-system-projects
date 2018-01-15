#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    fork();
    fork();

    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filename, "shakespeare.txt");

    char *p = (char *) vm_map(filename, 0);

    strcpy(p, "data1.bin");

    fork();
    
    char *p2 = (char *) vm_map(p, 0);

    cout << p2[0] << endl;

    return 0;
}
