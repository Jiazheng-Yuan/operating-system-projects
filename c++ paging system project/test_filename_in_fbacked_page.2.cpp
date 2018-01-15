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

    char *p = (char *) vm_map(filename, 0);

    //strcpy(p, "data1.bin");
    
    p[0] = 'd';
    p[1] = 'a';
    p[2] = 't';
    p[3] = 'a';
    p[4] = '1';
    p[5] = '.';
    p[6] = 'b';
    p[7] = 'i';
    p[8] = 'n';
    
    char *p2 = (char *) vm_map(p, 0);

    for (unsigned int i=0; i<15; i++) {
	   cout << p2[i];
    }

    cout << endl;
}
