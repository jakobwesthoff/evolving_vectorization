#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cairo.h>

#include "random.h"
#include "polygon.h"

polygons_t* copy_polygons( polygons_t* polygons )
{
    polygons_t* copy = malloc( sizeof( polygons_t ) * sizeof( char ) );
    memcpy( copy, polygons, sizeof( polygons_t ) * sizeof( char ) );
    return copy;
}

void draw_polygons( cairo_surface_t* surface, polygons_t* polygons ) 
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
        cairo_move_to( cr, 
            polygons->polygon[i].vertex[polygons->polygon[i].vertex_count - 1].x,
            polygons->polygon[i].vertex[polygons->polygon[i].vertex_count - 1].y
        );
        
        for( j=0; j<polygons->polygon[i].vertex_count; ++j )             
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
    int polygon_number = rand_between( 0, POLYGON_COUNT - 1 );
    // Change vertices, color or vertex_count
    int mutation = rand_between( 0, 2 );
    if( mutation == 0 ) 
    {
        // Change vertex position
        int vertex_number = rand_between( 0, polygons->polygon[polygon_number].vertex_count - 1 );
        polygons->polygon[polygon_number].vertex[vertex_number].x = rand_between( 0, polygons->original_width - 1 );
        polygons->polygon[polygon_number].vertex[vertex_number].y = rand_between( 0, polygons->original_height - 1 );
    }
    else if( mutation == 1 )
    {
        // Change color value
        int color_number = rand_between( 0, 3 );
        polygons->polygon[polygon_number].color[color_number] = rand_between( 0, 255 );
    }
    else 
    {
        // Add or remove vertice
        if ( rand_between( 0, 1 ) == 1 ) 
        {
            ++(polygons->polygon[polygon_number].vertex_count);
            polygons->polygon[polygon_number].vertex[polygons->polygon[polygon_number].vertex_count - 1].x = rand_between( 0, polygons->original_width - 1 );
            polygons->polygon[polygon_number].vertex[polygons->polygon[polygon_number].vertex_count - 1].y = rand_between( 0, polygons->original_height - 1 );
        }
        else 
        {
            if ( polygons->polygon[polygon_number].vertex_count > POLYGON_VERTICES ) 
            {
                --(polygons->polygon[polygon_number].vertex_count);
            }
        }
    }
}

polygons_t* initialize_polygons( cairo_surface_t* original ) 
{
    int i,j;
    polygons_t* polygons = malloc( sizeof( polygons_t ) * sizeof( char ) );

    polygons->original_width = cairo_image_surface_get_width( original );
    polygons->original_height = cairo_image_surface_get_height( original );


    
    for( i=0; i<POLYGON_COUNT; ++i ) 
    {
        int new_vertex_count = rand_between( POLYGON_VERTICES, MAX_POLYGON_VERTICES - 1 );
        for( j=0; j<new_vertex_count; ++j ) 
        {
            polygons->polygon[i].vertex[j].x = rand_between( 0, polygons->original_width - 1 );
            polygons->polygon[i].vertex[j].y = rand_between( 0, polygons->original_height - 1 );
        }
        polygons->polygon[i].vertex_count = new_vertex_count;

        for( j=0; j<4; ++j ) 
        {
            polygons->polygon[i].color[j] = rand_between( 0, 255 );
        }        
    }

    return polygons;
}
