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

    strcpy(filename, "shakespeare.txt");

    if (!fork()){ // child
        char *p1 = (char *) vm_map(filename, 0);
        cout << p1[15] << endl; // no vm_fault
    } else {
        char *p = (char *) vm_map(filename, 0);

        cout << p[0] << endl;
        cout << p[1] << endl;
        cout << p[2] << endl;
        cout << p[3] << endl;
        cout << p[4] << endl;
    }
    return 0;
}
