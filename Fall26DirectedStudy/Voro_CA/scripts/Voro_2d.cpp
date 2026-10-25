#include "Voro_2d.h"

#include "/usr/local/include/voro++/voro++.hh"
#include <vector>
#include <fstream>

using namespace voro;

void print_Lloyd_Evolution(container_2d &con, int num_seeds, int iterations)
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

container_2d create_Voro_Container(vector<int> domain_size, int num_seeds)
{
    return container_2d(0, domain_size[0], 0, domain_size[1], domain_size[0], domain_size[1], false, false, num_seeds);
}

void print_Voro_Cells(container_2d &con, string filename)
{
    con.print_custom("%i %x %y %.8P", filename.c_str());
}

void print_Voro_Centroids(container_2d &con, string filename)
{
    con.print_custom("%i %C", filename.c_str());
}
