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

    char* filename1 = (char *) vm_map(nullptr, 0);

    strcpy(filename1, "shakespeare.txt");

    if (!fork()){

        strcpy(filename, "data4.bin");

        cout << "new filename" << endl;
        for (unsigned int i = 0; i != 9; ++i){
            cout << filename[i];
        }
        cout << endl;

    } else {
        vm_yield();
        // should not trigger vm_fault when write
        for (unsigned int i = 0; i != 3; ++i){
            filename1[i] = '!';
        }
    }
    return 0;
}
