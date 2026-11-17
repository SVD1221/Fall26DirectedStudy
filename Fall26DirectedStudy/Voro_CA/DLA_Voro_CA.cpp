#include "scripts/Voro_2d.h"
#include "scripts/CA_2d.h"

#include "/usr/local/include/voro++/voro++.hh"

#include <iostream>

// g++ DLA_Voro_CA.cpp scripts/*.cpp -o DLA_Voro_CA -lvoro++ -fopenmp -w

void swap_values(vector<int> &vec, int a, int b)
{
    int temp = vec[a];
    vec[a] = vec[b];
    vec[b] = temp;
}

vector<int> assignRandomValues(int num_fixed, int num_mobile, int num_seeds, int random_seed, int min, int max, bool constant, int constant_val)
{
    srand(random_seed);

    vector<int> rand_vec(num_seeds, 0);
    int num_particles = num_fixed+num_mobile;

    for (int i = 0; i < num_fixed; i++) {
        rand_vec[i] = -(rand() % (-min) + 1);
    }

    for (int i = num_fixed; i < num_particles; i++) {
        if (constant){
            rand_vec[i] = constant_val;
        }
        else{
            rand_vec[i] = rand() % max + 1;
        }
    }

    for (int i = 0; i < 2*num_particles; i++) {
        swap_values(rand_vec, rand() % num_seeds, rand() % num_seeds);
    }

    return rand_vec;
}

int main()
{
    // Create a Voronoi container in a 10x10 domain
    std::vector<int> domain_size = {10, 10};
    bool periodic = true;
    int num_seeds = 2000;
    int random_seed = 1794769999;

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

    // // 1 fixed - 1 mobile
    // int rule_min = -1;
    // int rule_max = 1;
    // DLARule rule({
    //     // state = 1 (i.e. the mobile state)
    //     {-1, 1}
    // }, rule_min, rule_max);

    // 2 fixed - 3 mobile
    int rule_min = -2;
    int rule_max = 3;
    DLARule rule({
        // state = 1 (i.e. the first type of mobile state)
        {2, -1, 1, 1, 1},
        // state = 2 (i.e. the second type of mobile state)
        {-2, 1, 2, 2, 2},
        // state = 3 (i.e. the third type of mobile state)
        {2, 1, 3, 3, 3}
    }, rule_min, rule_max);

    vector<int> initial_state = assignRandomValues(6, 750, num_seeds, random_seed, rule_min, rule_max, true, rule_max);

    // // Static Voro CA with Lloyd iterations
    voro::container_2d final_con = run_Lloyd(con, num_seeds, 30);
    DLASystem dla = DLASystem(rule, final_con, num_seeds);
    print_DLA_Evolution(initial_state, dla, iterations, "output/DLA/dla_evolution.txt");
    print_Voro_Cells(final_con, "output/DLA/dla_seeds.txt");

    // Static Voro CA with random seeds
    // DLASystem dla = DLASystem(rule, con, num_seeds);
    // print_DLA_Evolution(initial_state, dla, iterations, "output/DLA/dla_evolution.txt");
    // print_Voro_Cells(con, "output/DLA/dla_seeds.txt");

    return 0;
}