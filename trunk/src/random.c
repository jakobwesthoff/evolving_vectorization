/* 
 * This file is part of Evolving vectorization
 * Copyright (C) 2008  Jakob Westhoff
 * 
 * Evolving vectorization is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3 of the License. 
 * 
 * Evolving vectorization is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License along with
 * arbit; if not, write to the Free Software Foundation, Inc., 51 Franklin St,
 * Fifth Floor, Boston, MA  02110-1301  USA 
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "random.h"

void rand_seed() 
{
    srand( time( 0 ) );
}

int rand_between( int start, int end ) 
{
    int randval = (int)(rand_double_between( start, end ));
    return randval;
}

double rand_double_between( int start, int end ) 
{
    return ( ( rand_double() * (double)( end - start + 1 ) ) + (double)(start) );
}

double rand_double() 
{
    return ( (double)rand() / ( (double)(RAND_MAX) + (double)(1) ) );
}
