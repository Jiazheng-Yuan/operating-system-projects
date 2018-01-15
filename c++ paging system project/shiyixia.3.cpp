#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *dummy = (char *) vm_map(nullptr, 0);

    strcpy(dummy, "fake");

    char *filename = (char *) vm_map(nullptr, 0);

    strcpy(filename, "data1.bin");

    char *f = (char *) vm_map(nullptr, 0);
    f[0] = 't';

    char *p = (char *) vm_map(filename, 0);

    filename[0] = 'f'; // no vm_fault

    cout << p[0] << endl;
    return 0;
}