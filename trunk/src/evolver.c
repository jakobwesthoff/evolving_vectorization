#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <cairo.h>

#include "random.h"
#include "polygon.h"

#define INITIAL_TEMPERATURE 500.0
#define EPSILON 0.001
#define ALPHA 0.95

static unsigned int quadratic_error( cairo_surface_t* original, cairo_surface_t* destination ) 
{
    unsigned char *original_data, *destination_data;
    int i;
    unsigned int quadratic_error = 0;        
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
        // Make sure there will be no overflow
        if ( quadratic_error + ( difference*difference ) < quadratic_error ) 
        {
            return 0xffffffff;
        }
        else 
        {
            quadratic_error += difference*difference;
        }
    }
    return quadratic_error;
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
    cairo_surface_t* input_surface  = NULL;
    cairo_surface_t* render_surface = NULL;
    polygons_t* polygons      = NULL;
    polygons_t* best_polygons = NULL;
    unsigned int iteration  = 0;
    unsigned int benefitial = 0;
    unsigned int current_fitness = 0xffffffff;
    unsigned int best_fitness    = 0xffffffff;
    double temperature = INITIAL_TEMPERATURE;
    double alpha       = ALPHA;
    double epsilon     = EPSILON;

    if ( argc < 3 ) 
    {
        printf( "Usage:\n" );
        printf( "   evolver <inputfile> <outputdirectory>\n" );
        exit( EXIT_FAILURE );
    }

    // Seed the random number generator
    rand_seed();

    // Load the original image for comparison
    input_surface = cairo_image_surface_create_from_png( argv[1] );
    if ( cairo_surface_status( input_surface ) != CAIRO_STATUS_SUCCESS ) 
    {
        printf( "Could not create input surface.\n" );
        exit( EXIT_FAILURE );
    }    
        
    polygons = initialize_polygons( input_surface );
    best_polygons = copy_polygons( polygons );
    initialize_new_render_surface( input_surface, &render_surface );
    draw_polygons( render_surface, polygons );
    current_fitness = quadratic_error( input_surface, render_surface );
    best_fitness    = current_fitness;
    printf( "%u/%u (%u)", benefitial, 0, best_fitness );
    
    while( 1 ) 
    {
        unsigned int new_fitness;
        polygons_t* new_polygons;       

        // Write output images every 1000 evolutions
        if ( iteration % 1000 == 0 ) 
        {
            char* filename;

            initialize_new_render_surface( input_surface, &render_surface );
            draw_polygons( render_surface, best_polygons );
            
            filename = malloc( sizeof( char ) * ( strlen( argv[2] ) + 15 + 1 ) );
            sprintf( filename, "%s/%10u.png", argv[2], iteration );
            cairo_surface_write_to_png( render_surface, filename );

            printf( "\n%s written.\n", filename );
            free( filename );
        }

        // Create new evolution
        initialize_new_render_surface( input_surface, &render_surface );
        new_polygons = copy_polygons( polygons );
        evolve_polygons( new_polygons );
        draw_polygons( render_surface, new_polygons );
        new_fitness = quadratic_error( input_surface, render_surface );
    
        if ( new_fitness < best_fitness ) 
        {
            free( best_polygons );
            best_polygons = copy_polygons( new_polygons );
            best_fitness = new_fitness;
        }

        if ( new_fitness < current_fitness ) 
        {
            ++benefitial;
            free( polygons );
            polygons = new_polygons;
            current_fitness = new_fitness;
        }
        else 
        {
            // Only change to worse evolution with falling probability based on
            // the iteration and the fitness difference
            // Small changes in fitness are more likely to be accepted.
            if( rand_double() < exp( -1 * ( (double)( new_fitness - current_fitness ) / temperature ) ) ) 
            {
                free( polygons );
                polygons = new_polygons;
                current_fitness = new_fitness;
            }
            else 
            {
                // Don't accept the new change
                free( new_polygons );
            }
        }

        printf( "\r%u/%u/%d (%u)                ", benefitial, iteration, temperature, best_fitness );

        // Check for abort condition
        if( temperature < epsilon ) 
        {
            break;
        }

        // Lower the temperature
        temperature *= alpha;

        ++iteration;
    }
    printf( "\n" );

    initialize_new_render_surface( input_surface, &render_surface );
    draw_polygons( render_surface, best_polygons );
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
