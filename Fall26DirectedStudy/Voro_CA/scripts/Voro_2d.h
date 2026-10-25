// Prevent multiple inclusions
#ifndef VORO_2D_H
#define VORO_2D_H

// Package inclusions
#include "/usr/local/include/voro++/voro++.hh"
#include <vector>
#include <fstream>

using namespace voro;

// Declare functions, classes, or variables
void print_Lloyd_Evolution(container_2d &con, int num_seeds, int iterations);

container_2d create_Voro_Container(vector<int> domain_size, int num_seeds);

void print_Voro_Cells(container_2d &con, string filename);

#endif // VORO_2D_H