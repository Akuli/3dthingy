// the 3d function graph being presented in the program

#ifndef GRAPH_H
#define GRAPH_H

#include "vecmat.h"

// returns height of graph
double graph_y(double x, double z);

// returns a vector perpendicular to the graph at (x,y,z), where y = graph_y(x,z)
struct Vec3 graph_perp_vector(double x, double z);


#endif   // GRAPH_H
