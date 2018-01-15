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
    char *f2 = (char *) vm_map(nullptr, 0);

    filename[4095] = 's';
    f2[0] = 'h';
    f2[1] = 'a';
    f2[2] = 'k';
    f2[3] = 'e';
    f2[4] = 's';
    f2[5] = 'p';
    f2[6] = 'e';
    f2[7] = 'a';
    f2[8] = 'r';
    f2[9] = 'e';
    f2[10] = '.';
    f2[11] = 't';
    f2[12] = 'x';
    f2[13] = 't';
    f2[14] = '\0';
    //strcpy(filename + 4095, "shakespeare.txt");

    char *p = (char *) vm_map (filename + 4095, 0);

    cout << f2[0] << endl;

    /* Print the first speech from the file */
    for (unsigned int i=0; i<2561; i++) {
	   cout << p[i];
    }
    cout << endl;
}
