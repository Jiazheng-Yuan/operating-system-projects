#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);
    strcpy(filename, "shakespeare.txt");

    char *filename1 = (char *) vm_map(nullptr, 0);
    strcpy(filename1, "shakespeare.txt");
    
    char *ssss = (char *) vm_map(nullptr, 0);
    strcpy(ssss, "s");

    cout << filename1[0] << endl;
    cout << ssss[0] << endl;
    // filename state
    /*
        resident = 0
        ref = 0
        read = 0
        write = 0
        dirty = 0
    */
    cout << filename[0] << endl;
    // filename state
    /*
        resident = 1
        ref = 1
        read = 1
        write = 0
        dirty = 0
    */

    if (!fork()){
        filename[0] = 'h';
    } else {
        vm_yield();
        // filename state
        /*
            resident = 1
            ref = 0
            read = 0
            write = 0
            dirty = 0
        */
        cout << filename[0] << endl;

    }

    return 0;
}

