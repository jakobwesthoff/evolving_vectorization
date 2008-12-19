#include <stdlib.h>
#include <time.h>

#include "random.h"

void rand_seed() 
{
    srand( time( 0 ) );
}

int rand_between( int start, int end ) 
{
    return ( ( rand() % ( end - start + 1 ) ) + start );
}

double rand_double() 
{
    return ( (double)(rand()) / (double)RAND_MAX );
}
