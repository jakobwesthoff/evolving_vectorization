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


static int rand_between( int start, int end );


polygons_t* initialize_polygons( cairo_surface_t* original ); 

void draw_polygons( cairo_surface_t* surface, polygons_t* polygons );

void evolve_polygons( polygons_t* polygons );

polygons_t* copy_polygons( polygons_t* polygons );

#endif
