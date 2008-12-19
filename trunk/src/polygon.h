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

#ifndef POLYGON_H
#define POLYGON_H

#define POLYGON_VERTICES 6
#define POLYGON_COUNT 50

typedef struct vertex 
{
    int x, y;
} vertex_t;

typedef struct polygon
{
    vertex_t vertex[POLYGON_VERTICES];
    int color[4]; // rgba
} polygon_t;

typedef struct polygons 
{
    polygon_t polygon[POLYGON_COUNT];
    int original_width, original_height;
} polygons_t;


polygons_t* initialize_polygons( cairo_surface_t* original ); 

void draw_polygons( cairo_surface_t* surface, polygons_t* polygons );
void draw_polygons_to_svg( polygons_t* polygons, char* filename );

void evolve_polygons( polygons_t* polygons );

polygons_t* copy_polygons( polygons_t* polygons );

#endif
