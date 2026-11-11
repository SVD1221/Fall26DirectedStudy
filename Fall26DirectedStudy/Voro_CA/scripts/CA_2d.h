// Prevent multiple inclusions
#ifndef CA_2D_H
#define CA_2D_H

// Package inclusions
#include "Voro_2d.h"
#include "/usr/local/include/voro++/voro++.hh"
#include <vector>
#include <fstream>

using namespace std;

// Declare functions, classes, or variables

vector<int> createRandomVec(int min, int max, int size, int seed);

void writeTo(ofstream *file, vector<int> *vec);

struct Rule
{
    vector<vector<vector<float>>> stateRules;

    Rule(vector<vector<vector<float>>> stateRules);
}; 

class CASystem
{
    private:
        int num_seeds;
        bool include_boundary;
        int boundary_state;
        Rule rule;
        voro::container_2d &voro_cells;

    public:
        CASystem(Rule rule, voro::container_2d &voro_cells, int num_seeds, bool include_boundary, int boundary_state);

        voro::container_2d getCells();

        void updateCells(voro::container_2d &new_voro_cells);
        
        vector<int> applyCA(vector<int> &state);

        int getNumSeeds();
};

void print_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, string filename);

void print_Dynamic_Voro_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, int ca_cycle, int lloyd_cycle, int random_seed);

#endif // CA_2D_H