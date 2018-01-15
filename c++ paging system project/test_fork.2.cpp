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

    if (!fork()){

        char *p = (char *) vm_map(filename, 0);

        for (unsigned int i=0; i<2561; i++) {
           cout << p[i];
        } 
        cout << endl; 

        strcpy(filename, "data4.bin");

        cout << "new filename" << endl;
        for (unsigned int i = 0; i != 9; ++i){
            //cout << filename[i];
            filename[i] = 'a';
        }
        cout << endl;

    } else {
        vm_yield();
        for (unsigned int i = 0; i != 3; ++i){
            cout << filename[i];
        }
        cout << endl;
    }
    

    cout << endl;
}
