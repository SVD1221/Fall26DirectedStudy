#include "scripts/Voro_2d.h"
#include "scripts/CA_2d.h"

#include "/usr/local/include/voro++/voro++.hh"

#include <ctime>
#include <iostream>

// g++ Voro_CA.cpp scripts/*.cpp -o Voro_CA -lvoro++ -fopenmp -w

int main()
{
    // Create a Voronoi container in a 10x10 domain
    std::vector<int> domain_size = {10, 10};
    bool periodic = false;
    bool boundary_is_neighbor = false;
    int boundary_state = 0;
    int num_seeds = 250;
    int random_seed = 12;
    // int random_seed = time(0);

    // define cycle lengths
    int ca_cycle = 1;
    int lloyd_cycle = 2;
    int iterations = 50;

    voro::container_2d con = create_Voro_Container(domain_size, num_seeds, periodic);
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

    vector<int> initial_state = createRandomVec(0, 1, num_seeds, random_seed);

    // Create a dynamic Voro CA under Lloyd's Algo
    CASystem ca(rule, con, num_seeds, boundary_is_neighbor, boundary_state);
    print_Dynamic_Voro_CA_Evolution(initial_state, ca, iterations, ca_cycle, lloyd_cycle, random_seed);

    // Create a static Voro CA
    // voro::container_2d final_con = run_Lloyd(con, num_seeds, 20);
    // CASystem ca(rule, final_con, num_seeds, boundary_is_neighbor, boundary_state);
    // print_CA_Evolution(initial_state, ca, 25, "output/static_Voro_CA/ca_evolution.txt");
    // print_Voro_Cells(final_con, "output/static_Voro_CA/voro_seeds.txt");

    // Run Lloyd's algorithm and output the animation
    // int lloyd_iterations = 30;
    // print_Lloyd_Evolution(con, num_seeds, lloyd_iterations);

    return 0;
}
