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

    // filename state
    /*
        resident = 0
        ref = 0
        read = 0
        write = 0
        dirty = 0
    */

    if (!fork()){
        // child
        filename[0] = 'c';
    } else {
        vm_yield();
        cout << filename[0] << endl;
        if (!fork()){
            //child 
            filename[0] = 'c';
        } else {
            vm_yield();
            cout << filename[1] << endl;
        }
    }

    return 0;
}

