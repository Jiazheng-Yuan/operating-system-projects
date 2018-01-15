
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    /* Write the name of the file that will be mapped */
    strcpy(filename, "shakespeare.txt");

    char *filename1 = (char *) vm_map(nullptr, 2);

    strcpy(filename1, "shakespeare.txt");

    char *filename2 = (char *) vm_map(nullptr, 0);

    strcpy(filename2, "data1.bin");
    /* Map a page from the specified file */
    char *p2 = (char *) vm_map (filename2, 2);

    /* Print the first speech from the file */
    for (unsigned int i=0; i<2561; i++) {
	   cout << p2[i];
    }
    cout << endl;

    char *p1 = (char *) vm_map (filename1, 0);

    /* Print the first speech from the file */
    for (unsigned int i=0; i<2561; i++) {
       cout << p1[i];
    }
    cout << endl;
}
