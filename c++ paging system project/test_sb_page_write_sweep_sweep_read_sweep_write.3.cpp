#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *target = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(target, "shakespeare.txt");
    char *filename = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filename, "data1.bin");

    char *f2 = (char *) vm_map(nullptr, 0);
    strcpy(f2, "whatever");
    // target [0 0 0 0 0]
    // filename [1 0 0 0 1]
    // f2 [1 1 1 1 1]
    cout << target[0] << endl;
    // filename [0 0 0 0 0]
    // f2 [1 1 1 1 1]
    // target [1 1 1 0 0]
    cout << filename[0] << endl;
    // f2 [0 0 0 0 0]
    // target [1 0 0 0 0]
    // filename [1 1 1 0 0]
    target[0] = '\?';
    // target [1 1 1 1 1]
    return 0;
}