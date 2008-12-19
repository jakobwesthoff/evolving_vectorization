#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>

#include "polygon.h"

static int rand_between( int start, int end ) 
{
    return ( ( rand() % ( end - start + 1 ) ) + start );
}

polygons_t* copyPolygons( polygons_t* polygons )
{
    polygons_t* copy = malloc( sizeof( polygons_t ) * sizeof( char ) );
    memcpy( copy, polygons, sizeof( polygons_t ) * sizeof( char ) );
    return copy;
}

void drawPolygons( cairo_surface_t* surface, polygons_t* polygons ) 
{
    int i,j;
    cairo_t* cr = cairo_create( surface );
    
    for( i=0; i<POLYGON_COUNT; ++i ) 
    {
        cairo_save( cr );
        
        cairo_set_source_rgba( cr,
            (double)(polygons->polygon[i].color[0]) / 255.0,
            (double)(polygons->polygon[i].color[1]) / 255.0,
            (double)(polygons->polygon[i].color[2]) / 255.0,
            (double)(polygons->polygon[i].color[3]) / 255.0
        );
/*        printf( "moveto: %i, %i\n", 
            polygons->polygon[i].vertex[POLYGON_VERTICES - 1].x,
            polygons->polygon[i].vertex[POLYGON_VERTICES - 1].y
        );
*/
        cairo_move_to( cr, 
            polygons->polygon[i].vertex[POLYGON_VERTICES - 1].x,
            polygons->polygon[i].vertex[POLYGON_VERTICES - 1].y
        );
        
        for( j=0; j<POLYGON_VERTICES; ++j ) 
        {
/*            printf( "lineto: %i, %i\n",
                polygons->polygon[i].vertex[j].x,
                polygons->polygon[i].vertex[j].y
            );
*/
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

void evolvePolygons( polygons_t* polygons ) 
{
    int polygon_number = rand_between( 0, POLYGON_COUNT - 1 );
    // Change vertices or color
    if( rand_between( 0, 1 ) == 1 ) 
    {
        int vertex_number = rand_between( 0, POLYGON_VERTICES - 1 );
        polygons->polygon[polygon_number].vertex[vertex_number].x = rand_between( 0, polygons->original_width - 1 );
        polygons->polygon[polygon_number].vertex[vertex_number].y = rand_between( 0, polygons->original_height - 1 );
    }
    else 
    {
        int color_number = rand_between( 0, 3 );
        polygons->polygon[polygon_number].color[color_number] = rand_between( 0, 255 );
    }
}

polygons_t* initializePolygons( cairo_surface_t* original ) 
{
    int i,j;
    polygons_t* polygons = malloc( sizeof( polygons_t ) * sizeof( char ) );

    polygons->original_width = cairo_image_surface_get_width( original );
    polygons->original_height = cairo_image_surface_get_height( original );
    
    for( i=0; i<POLYGON_COUNT; ++i ) 
    {
        for( j=0; j<POLYGON_VERTICES; ++j ) 
        {
            polygons->polygon[i].vertex[j].x = rand_between( 0, polygons->original_width - 1 );
            polygons->polygon[i].vertex[j].y = rand_between( 0, polygons->original_height - 1 );
        }
        for( j=0; j<4; ++j ) 
        {
            polygons->polygon[i].color[j] = rand_between( 0, 255 );
        }
    }

    return polygons;
}
