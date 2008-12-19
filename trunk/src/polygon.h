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


polygons_t* initializePolygons( cairo_surface_t* original ); 

void drawPolygons( cairo_surface_t* surface, polygons_t* polygons );

void evolvePolygons( polygons_t* polygons );

polygons_t* copyPolygons( polygons_t* polygons );

#endif
