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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>

#include "random.h"
#include "polygon.h"

polygons_t* copy_polygons( polygons_t* polygons )
{
    polygons_t* copy = allocate_polygon_structure( polygons->count );
    copy->original_width   = polygons->original_width;
    copy->original_height = polygons->original_height;
    memcpy( copy->polygon, polygons->polygon, sizeof( polygon_t ) * sizeof( char ) * polygons->count );    
    return copy;
}

void draw_polygons( cairo_surface_t* surface, polygons_t* polygons ) 
{
    int i,j;
    cairo_t* cr = cairo_create( surface );
    
    for( i=0; i<polygons->count; ++i ) 
    {
        cairo_save( cr );
        
        cairo_set_source_rgba( cr,
            (double)(polygons->polygon[i].color[0]) / 255.0,
            (double)(polygons->polygon[i].color[1]) / 255.0,
            (double)(polygons->polygon[i].color[2]) / 255.0,
            (double)(polygons->polygon[i].color[3]) / 255.0
        );
        cairo_move_to( cr, 
            polygons->polygon[i].vertex[POLYGON_VERTICES - 1].x,
            polygons->polygon[i].vertex[POLYGON_VERTICES - 1].y
        );
        
        for( j=0; j<POLYGON_VERTICES; ++j ) 
        {
            cairo_line_to( cr,
                polygons->polygon[i].vertex[j].x,
                polygons->polygon[i].vertex[j].y
            );
        }
        
        cairo_fill( cr );

        cairo_restore( cr );
    }
    
    cairo_destroy( cr );
}

void draw_polygons_to_svg( polygons_t* polygons, char* filename ) 
{
    cairo_surface_t* svg_surface = (cairo_surface_t*)cairo_svg_surface_create( filename, 
        (double)(polygons->original_width),
        (double)(polygons->original_height)
    );

    if ( cairo_surface_status( svg_surface ) != CAIRO_STATUS_SUCCESS ) 
    {
        printf( "Could not create svg surface.\n" );
        exit( EXIT_FAILURE );
    }

    draw_polygons( svg_surface, polygons );

    cairo_surface_destroy( svg_surface );
}

void evolve_polygons( polygons_t* polygons ) 
{
    int polygon_number = rand_between( 0, polygons->count - 1 );
    // Change vertices or color
    if( rand_between( 0, 1 ) == 1 ) 
    {
        int vertex_number = rand_between( 0, POLYGON_VERTICES - 1 );
        polygons->polygon[polygon_number].vertex[vertex_number].x = rand_between( 0, polygons->original_width );
        polygons->polygon[polygon_number].vertex[vertex_number].y = rand_between( 0, polygons->original_height );
    }
    else 
    {
        int color_number = rand_between( 0, 3 );
        int color        = rand_between( 0, 255 );
        // We don not want to create completely transparent polygons
        if ( color_number == 3 && color == 0 ) 
        {
            color = 1;
        }
        polygons->polygon[polygon_number].color[color_number] = color;
    }
}

polygons_t* initialize_polygons( cairo_surface_t* original, int count ) 
{
    int i,j;
    polygons_t* polygons = allocate_polygon_structure( count );

    polygons->original_width = cairo_image_surface_get_width( original );
    polygons->original_height = cairo_image_surface_get_height( original );
    
    for( i=0; i<count; ++i ) 
    {
        for( j=0; j<POLYGON_VERTICES; ++j ) 
        {
            polygons->polygon[i].vertex[j].x = rand_between( 0, polygons->original_width );
            polygons->polygon[i].vertex[j].y = rand_between( 0, polygons->original_height );
        }
        for( j=0; j<4; ++j ) 
        {
            polygons->polygon[i].color[j] = rand_between( 0, 255 );
        }
    }

    return polygons;
}

static polygons_t* allocate_polygon_structure( int count ) 
{
    polygons_t* p = malloc( sizeof( polygons_t ) * sizeof( char ) );
    p->polygon = malloc( sizeof( polygon_t ) * sizeof( char ) * count );
    p->count = count;
    return p;
}

void free_polygons( polygons_t* polygons ) 
{
    free( polygons->polygon );
    free( polygons );
}
