#include "log.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    tracef("just a test!\n");
    warnf("just a test!\n");
    errorf("just a test!\n");
    fatalf("just a test!\n");
    return 0;
}