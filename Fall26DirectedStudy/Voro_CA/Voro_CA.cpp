#include "scripts/Voro_2d.h"
#include "scripts/CA_2d.h"

#include "/usr/local/include/voro++/voro++.hh"

#include <ctime>
#include <iostream>

//TODO: Get thing to print next generation

// To compile without warnings:
// g++ Voro_CA.cpp scripts/*.cpp -o Voro_CA -lvoro++ -fopenmp -w

int main()
{
    // Create a Voronoi container in a 10x10 domain
    std::vector<int> domain_size = {10, 10};
    int num_seeds = 200;
    int random_seed = 12;
    // int random_seed = time(0);

    voro::container_2d con = create_Voro_Container(domain_size, num_seeds);
    vector<int> states(num_seeds, 0);

    // Add random points
    srand(random_seed);

    for(int i=0; i<num_seeds; i++) {
        double x = 10.0*rand()/RAND_MAX;
        double y = 10.0*rand()/RAND_MAX;
        con.put(i, x, y);
        states[i] = rand() % 2;
    }

    Rule rule({
        // N=0 (n<=5)
        {{0, 0, 0, 1, 0, 0},
         {0, 0, 0, 1, 1, 0}
        },
        // N=1 (n=6)
        {{0, 0, 1, 0, 0, 0, 0},
         {0, 0, 0, 1, 1, 1, 0}
        },
        // N=2 (n>=7)
        {{0, 0, 0, 1, 0, 0, 0, 0},
         {0, 0, 1, 1, 1, 0, 0, 0}
        }
    });

    CASystem ca(rule, con, num_seeds, true);

    vector<int> initial_state = createRandomVec(0, 1, num_seeds, random_seed);

    // Create a static Voro_CA
    print_CA_Evolution(initial_state, ca, 25, "output/Voro_CA_evolution.txt");
    print_Voro_Cells(con, "output/cellID_loc_vert.txt");

    // // Run Lloyd's algorithm and output the animation
    // int lloyd_iterations = 30;
    // print_Lloyd_Evolution(con, num_seeds, lloyd_iterations);

    return 0;
}
