// Prevent multiple inclusions
#ifndef CA_2D_H
#define CA_2D_H

// Package inclusions
#include "/usr/local/include/voro++/voro++.hh"
#include <vector>
#include <fstream>

using namespace std;

// Declare functions, classes, or variables

vector<int> createRandomVec(int min, int max, int size, int seed);

void writeTo(ofstream *file, vector<int> *vec);

vector<int> createAdjustments(bool top, bool bottom, bool left, bool right, int x, int y, vector<int> shape);

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
        Rule rule;
        voro::container_2d &voro_cells;

    public:
        CASystem(Rule rule, voro::container_2d &voro_cells, int num_seeds, bool include_boundary);
        
        vector<int> applyCA(vector<int> &state);

        int getNumSeeds();
};

void print_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, string filename);

#endif // CA_2D_H