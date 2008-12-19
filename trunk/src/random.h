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

#ifdef RANDOM_H
#define RANDOM_H

void rand_seed();

int rand_between( int start, int end );

double rand_double();

#endif
