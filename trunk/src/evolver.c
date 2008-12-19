#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cairo.h>

#include "polygon.h"

static unsigned int quadraticError( cairo_surface_t* original, cairo_surface_t* destination ) 
{
    unsigned char *original_data, *destination_data;
    int i;
    unsigned int quadraticError = 0;        
    int size = cairo_image_surface_get_height( original )*cairo_image_surface_get_stride( original );

    original_data = cairo_image_surface_get_data( original );
    if ( original_data == NULL ) 
    {
        printf( "Could not access original image data during compare\n" );
        exit( EXIT_FAILURE );
    }
    destination_data = cairo_image_surface_get_data( destination );
    if ( destination_data == NULL ) 
    {
        printf( "Could not access destination image data during compare\n" );
        exit( EXIT_FAILURE );
    }

    for( i=0; i<size; ++i ) 
    {
        int difference = destination_data[i] - original_data[i];
        quadraticError += difference*difference;
    }
    return quadraticError;
}

void initialize_new_render_surface( cairo_surface_t* input, cairo_surface_t** render_surface ) 
{
    if ( *render_surface != NULL ) 
    {
        cairo_surface_destroy( *render_surface );
    }
    
    // Retrieve input metadata and create render_surface
    *render_surface = cairo_surface_create_similar( 
        input,
        CAIRO_CONTENT_COLOR_ALPHA,
        cairo_image_surface_get_width( input ),
        cairo_image_surface_get_height( input )
    );
    if ( cairo_surface_status( *render_surface ) != CAIRO_STATUS_SUCCESS ) 
    {
        printf( "Could not create render surface.\n" );
        exit( EXIT_FAILURE );
    }
}

int main( int argc, char** argv ) 
{
    int i;
    cairo_surface_t* input_surface  = NULL;
    cairo_surface_t* render_surface = NULL;
    polygons_t* polygons      = NULL;
    polygons_t* best_polygons = NULL;
    unsigned int benefitial    = 0;
    unsigned int current_error = 0xffffffff;
    unsigned int best_error    = 0xffffffff;

    if ( argc < 3 ) 
    {
        printf( "Usage:\n" );
        printf( "   evolver <inputfile> <outputdirectory>\n" );
        exit( EXIT_FAILURE );
    }

    // Load the original image for comparison
    input_surface = cairo_image_surface_create_from_png( argv[1] );
    if ( cairo_surface_status( input_surface ) != CAIRO_STATUS_SUCCESS ) 
    {
        printf( "Could not create input surface.\n" );
        exit( EXIT_FAILURE );
    }
        
    polygons = initializePolygons( input_surface );
    best_polygons = copyPolygons( polygons );
    initialize_new_render_surface( input_surface, &render_surface );
    drawPolygons( render_surface, polygons );
    current_error = quadraticError( input_surface, render_surface );
    best_error    = current_error;
    printf( "%u/%u (%u)", benefitial, 0, best_error );
    
    for ( i=0; i<1000000; ++i ) 
    {
        unsigned int new_error;
        polygons_t* new_polygons;

        // Write output images every 1000 evolutions
        if ( i % 1000 == 0 ) 
        {
            char* filename;

            initialize_new_render_surface( input_surface, &render_surface );
            drawPolygons( render_surface, best_polygons );
            
            filename = malloc( sizeof( char ) * ( strlen( argv[2] ) + 12 + 1 ) );
            sprintf( filename, "%s/%07u.png", argv[2], i );
            cairo_surface_write_to_png( render_surface, filename );

            printf( "\n%s written.\n", filename );
            free( filename );
        }

        initialize_new_render_surface( input_surface, &render_surface );
        new_polygons = copyPolygons( polygons );
        evolvePolygons( new_polygons );
        drawPolygons( render_surface, new_polygons );
        new_error = quadraticError( input_surface, render_surface );
    
        if ( new_error < best_error ) 
        {
            free( best_polygons );
            best_polygons = copyPolygons( new_polygons );
            best_error = new_error;
        }

        if ( new_error < current_error ) 
        {
            ++benefitial;
            free( polygons );
            polygons = new_polygons;
            current_error = new_error;
        }
        else 
        {
            free( new_polygons );
        }

        printf( "\r%u/%u (%u)                ", benefitial, i, best_error );
    }
    printf( "\n" );

    initialize_new_render_surface( input_surface, &render_surface );
    drawPolygons( render_surface, best_polygons );
    cairo_surface_write_to_png( render_surface, argv[2] );

    printf( "%s written.\n", argv[2] );
    
    // Free the allocated memory
    if ( polygons != NULL  )
        free( polygons );
    if ( best_polygons != NULL  )
        free( best_polygons );

    cairo_surface_destroy( render_surface );
    cairo_surface_destroy( input_surface );
}
