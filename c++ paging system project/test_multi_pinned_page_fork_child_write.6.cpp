#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using namespace std;

int main()
{
    /* Allocate swap-backed page from the arena */
    char *f1 = (char *) vm_map(nullptr, 0);
    char *f2 = (char *) vm_map(nullptr, 0);
    char *f3 = (char *) vm_map(nullptr, 0);
    if (!fork()){
        f3[0] = 'c';
    }
    cout << f1[0] << endl;
    cout << f2[0] << endl;
    return 0;
}

