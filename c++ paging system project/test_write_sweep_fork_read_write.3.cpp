#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    char* dummy1 = (char *) vm_map(nullptr, 0);
    dummy1[0] = 'd';

    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filename, "shakespeare.txt");

    char* evictFirst = (char *) vm_map(nullptr, 0);
    evictFirst[0] = 'e';

    // filename page state
    /*
        resident = 1
        ref = 0
        read = 0
        write = 0
        dirty = 1
    */

    if (!fork()){
        // exec child
    } else {
        cout << filename[0] << endl; // vm_fault read
        filename[0] = 'h'; // vm_fault write
    }
    return 0;
}
