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
    char *filename1 = (char *) vm_map(nullptr, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filename1, "data1.bin");
    if(!fork()){
        char *filename2 = (char *) vm_map(filename, 0);
    //char *filename2 = (char *) vm_map(nullptr, 0);

        cout<<filename2[0]<<endl;
        cout<<filename1[0] <<endl;

    }

    
    
    return 0;
}