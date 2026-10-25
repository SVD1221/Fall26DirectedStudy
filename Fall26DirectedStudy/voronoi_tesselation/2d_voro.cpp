#include "/usr/local/include/voro++/voro++.hh"
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>

// To compile without warnings:
// g++ 2d_voro.cpp -lvoro++ -fopenmp -w -o 2d_voro

/**
 * Runs Lloyd's algorithm and outputs a series of files into output/lloyd
 */
void animate_Lloyd(voro::container_2d &con, int num_seeds, int iterations)
{
    std::string filename = "output/lloyd/cellset" + std::to_string(0) + ".gnu";
    con.print_custom("%i %x %y %.8P", filename.c_str());

    // Initialize the current and next containers
    voro::container_2d con_curr(0,10, 0,10, 10,10, false, false, num_seeds);
    voro::container_2d con_next(0,10, 0,10, 10,10, false, false, num_seeds);

    // Insert points from con into con_curr
    for(auto cli = con.begin(); cli < con.end(); cli++) {
        double xp, yp;
        con.pos(cli, xp, yp);
        con_curr.put(con.pid(cli), xp, yp);
    }
    
    // Initiate Lloyd's Algo
    for(int i = 0; i < iterations; i++) {

        // Initialize a cell with the geometry of the current container
        voro::voronoicell_2d c(con_curr);

        // Iterate through every particle storing their locations and IDs
        for(auto cli = con_curr.begin(); cli < con_curr.end(); cli++) {
            if(con_curr.compute_cell(c, cli)) {
                double xc, yc;
                c.centroid(xc, yc);

                double xp, yp;
                con_curr.pos(cli, xp, yp);

                double X = xp + xc;
                double Y = yp + yc;

                int pid = con_curr.pid(cli);
                
                con_next.put(pid, X, Y);
            }
        }

        filename = "output/lloyd/cellset" + std::to_string(i+1) + ".gnu";
        con_next.print_custom("%i %x %y %.8P", filename.c_str());
        
        // Empty the current container and place the next container's seeds inside
        con_curr.clear();
        for(auto cli = con_next.begin(); cli < con_next.end(); cli++) {
            double xp, yp;
            con_next.pos(cli, xp, yp);
            con_curr.put(con_next.pid(cli), xp, yp);
        }
        con_next.clear();
    }
}


int main() 
{
    int N = 100;
    int lloyd_iterations = 30;
    std::vector<int> states(N);

    // Initialize the container class to be the unit square, with
	// non-periodic boundary conditions. Divide it into a 10 by 10 grid,
	// with an initial memory allocation of N particles per grid square.
	voro::container_2d con(0,10, 0,10, 10,10, false, false, N);

    // Add a few random points
    for(int i=0; i<N; i++) {
        double x = 10.0*rand()/RAND_MAX;
        double y = 10.0*rand()/RAND_MAX;
        con.put(i, x, y);
        states[i] = rand() % 2;
    }

    // Output for the domain and V-cells in gnuplot format
    con.draw_domain_gnuplot("output/domain.gnu");
    con.draw_cells_gnuplot("output/cells.gnu");
    // Output for the centroid of each cell
    con.print_custom("%i %C", "output/centroids.txt");
    // Output for the cell IDs, seed locations, and vertices of each cell
    con.print_custom("%i %x %y %.8P", "output/cellID_loc_vert.txt");
    // Output for Lloyd's algo
    animate_Lloyd(con, N, lloyd_iterations);

    return 0;
}
