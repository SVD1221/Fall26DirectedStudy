#include "scripts/Voro_2d.h"
#include "scripts/CA_2d.h"

#include "/usr/local/include/voro++/voro++.hh"

// To compile without warnings:
// g++ Voro_CA.cpp scripts/*.cpp -o Voro_CA -lvoro++ -fopenmp -w

int main()
{
    // Create a Voronoi container with 100 seeds in a 10x10 domain
    std::vector<int> domain_size = {10, 10};
    int num_seeds = 100;
    voro::container_2d con = create_Voro_Container(domain_size, num_seeds);

    // Print the Voronoi cells to a file
    print_Voro_Cells(con, "output/cellID_loc_vert.txt");

    // Run Lloyd's algorithm and output the animation
    int lloyd_iterations = 30;
    print_Lloyd_Evolution(con, num_seeds, lloyd_iterations);

    return 0;
}
