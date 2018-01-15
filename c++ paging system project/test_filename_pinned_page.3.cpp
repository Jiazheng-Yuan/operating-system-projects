#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *filename = (char *) vm_map(nullptr, 0);

    char *p = (char *) vm_map(filename, 0);

    cout << p[1] << endl;
    
    return 0;
}
