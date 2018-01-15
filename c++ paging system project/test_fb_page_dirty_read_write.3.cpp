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

    p[0] = '\?';
    //cout<<filename[0]<<endl;
    //if (!fork()){ // child
        char *d1 = (char *) vm_map(nullptr, 0);
        strcpy(d1, "data1.bin");
        cout<<p[0]<<endl;
        char* d2 = (char *) vm_map(d1, 0);
        d2[0] = 'a';
        // evict and write data1.bin to disk
    //}

    return 0;
}
