#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#include <cairo.h>

#include "random.h"
#include "polygon.h"


static unsigned int quadratic_error( cairo_surface_t* original, cairo_surface_t* destination );
static void initialize_new_render_surface( cairo_surface_t* input, cairo_surface_t** render_surface );
static void show_usage();



int main( int argc, char** argv ) 
{
    // Drawing and original cairo surfaces
    cairo_surface_t* input_surface  = NULL;
    cairo_surface_t* render_surface = NULL;

    // Polygon structs
    polygons_t* polygons      = NULL;
    polygons_t* best_polygons = NULL;

    // Iteration counters
    unsigned int iteration  = 0;
    unsigned int benefitial = 0;

    // Fitness levels
    unsigned int current_fitness = 0xffffffff;
    unsigned int best_fitness    = 0xffffffff;

    // Default simulated annealing values
    double temperature = 1000.0;
    double alpha       = 0.99999;
    double epsilon     = 0.01;

    // Default writeout values
    int svg_write_iterations = 10000;
    int png_write_iterations = 1000;

    // Input file and output directory
    char* input_file;
    char* output_directory;

    // Read commandline arguments
    {
        extern char *optarg;
        extern int optind, optopt;
        int c;
        while( ( c = getopt( argc, argv, "t:a:e:s:p:" ) ) != -1 ) 
        {
            switch( c ) 
            {
                case '?':
                    show_usage();
                    exit( EXIT_FAILURE );
                break;
                case 'p':
                    png_write_iterations = atoi( optarg );
                    printf( "png_write_iterations=%u\n", png_write_iterations );
                break;
                case 's':
                    svg_write_iterations = atoi( optarg );
                    printf( "svg_write_iterations=%u\n", svg_write_iterations );
                break;
                case 't':
                    temperature = strtod( optarg, NULL );
                    printf( "temperature=%f\n", temperature );
                break;
                case 'e':
                    epsilon = strtod( optarg, NULL );
                    printf( "epsilon=%f\n", epsilon );
                break;
                case 'a':
                    alpha = strtod( optarg, NULL );
                    printf( "alpha=%f\n", alpha );
                break;
            }
        }
    }
    
    if ( argc - optind < 2 ) 
    {
        show_usage();
        exit( EXIT_FAILURE );
    }

    input_file       = argv[optind];      
    output_directory = argv[optind + 1];

    // Seed the random number generator
    rand_seed();

    // Load the original image for comparison
    input_surface = cairo_image_surface_create_from_png( input_file );
    if ( cairo_surface_status( input_surface ) != CAIRO_STATUS_SUCCESS ) 
    {
        printf( "Could not create input surface.\n" );
        exit( EXIT_FAILURE );
    }    
        
    // Create random polygon structure and initialize all needed values
    polygons = initialize_polygons( input_surface );
    best_polygons = copy_polygons( polygons );
    initialize_new_render_surface( input_surface, &render_surface );
    draw_polygons( render_surface, polygons );
    current_fitness = quadratic_error( input_surface, render_surface );
    best_fitness    = current_fitness;
    printf( "%u/%u/%f (%u)", benefitial, 0, temperature, best_fitness );
    
    // Start simulated annealing cycle to try finding the optimal polygon
    // approximation of the image
    while( 1 ) 
    {
        unsigned int new_fitness;
        polygons_t* new_polygons;       
        
        // Write newline before any other status message
        if ( ( png_write_iterations != 0 && iteration % png_write_iterations == 0 )
          || ( svg_write_iterations != 0 && iteration % svg_write_iterations == 0 ) ) 
        {
            printf( "\n" );
        }
    
        // Write output png every x evolutions
        if ( png_write_iterations != 0 && iteration % png_write_iterations == 0 ) 
        {
            char* filename = malloc( sizeof( char ) * ( strlen( output_directory ) + 32 ) );

            initialize_new_render_surface( input_surface, &render_surface );
            draw_polygons( render_surface, best_polygons );
            
            sprintf( filename, "%s/%010u.png", output_directory, iteration );
            cairo_surface_write_to_png( render_surface, filename );

            printf( "PNG: %s written.\n", filename );
            free( filename );
        }

        // Write a svg every x iterations
        if ( svg_write_iterations != 0 && iteration % svg_write_iterations == 0 ) 
        {
            char* filename = malloc( sizeof( char ) * ( strlen( output_directory ) + 32 ) );
            sprintf( filename, "%s/%010u.svg", output_directory, iteration );

            draw_polygons_to_svg( best_polygons, filename );

            printf( "SVG: %s written.\n", filename );
            free( filename );
        }

        // Create new evolution
        initialize_new_render_surface( input_surface, &render_surface );
        new_polygons = copy_polygons( polygons );
        evolve_polygons( new_polygons );
        draw_polygons( render_surface, new_polygons );
        new_fitness = quadratic_error( input_surface, render_surface );
    
        // Store polygons with the best fitness found so far
        if ( new_fitness < best_fitness ) 
        {
            free( best_polygons );
            best_polygons = copy_polygons( new_polygons );
            best_fitness = new_fitness;
        }

        // If the new evolution is better than the old one the old one will die
        // and the new one will be taken further
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

        printf( "\r%u/%u/%f (%u)            ", benefitial, iteration, temperature, best_fitness );

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

    // Render the best state found so far to png and svg
    {
        char* filename = malloc( sizeof( char ) * strlen( output_directory ) + 16 );

        initialize_new_render_surface( input_surface, &render_surface );
        draw_polygons( render_surface, best_polygons );
        sprintf( filename, "%s/final.png", output_directory );
        cairo_surface_write_to_png( render_surface, filename );
        printf( "PNG: %s written.\n", filename );

        sprintf( filename, "%s/final.svg", output_directory );
        draw_polygons_to_svg( best_polygons, filename );
        printf( "SVG: %s written.\n", filename );

        free( filename );        
    }
    
    // Free the allocated memory
    if ( polygons != NULL  )
        free( polygons );
    if ( best_polygons != NULL  )
        free( best_polygons );

    cairo_surface_destroy( render_surface );
    cairo_surface_destroy( input_surface );
}

static void show_usage() 
{
    printf( "Evolving Vectorizer\n" );
    printf( "(c) Jakob Westhoff <jakob@php.net>\n" );
    printf( "Usage:\n" );
    printf( "   evolver [options] <inputfile> <outputdirectory>\n" );
    printf( "Options:\n" );
    printf( "   -p <int>:   Save current state as png every <number>\n\
               iterations (Default: 1000) (0 to disable)\n" );
    printf( "   -s <int>:   Save current state as svg every <number>\n\
               iterations (Default: 10000) (0 to disable)\n" );        
    printf( "   -t <float>: Initial temperature for simulated\n\
               annealing (Default: 1000.0)\n" );
    printf( "   -e <float>: Epsilon value to stop the annealing at\n\
               (Default: 0.01)\n" );
    printf( "   -a <float>: Alpha to cool temperature by every\n\
               iteration (Default: 0.99999)\n" );
}

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

static void initialize_new_render_surface( cairo_surface_t* input, cairo_surface_t** render_surface ) 
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
