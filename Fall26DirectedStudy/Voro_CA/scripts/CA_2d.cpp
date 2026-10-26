#include "CA_2d.h"

#include "/usr/local/include/voro++/voro++.hh"
#include <vector>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iostream>

using namespace std;

vector<int> createRandomVec(int min, int max, int size, int seed)
{  
    vector<int> rand_vec(size);
    srand(seed);

    for (int i = 0; i < size; i++)
    {
        rand_vec[i] = min + rand() % (max - min + 1);
    }

    return rand_vec;
}

void writeTo(ofstream *file, vector<int> *vec)
{
    for (int i = 0; i < (*vec).size(); i++){
        *file << (*vec)[i] << " ";
    }
}

vector<int> createAdjustments(bool top, bool bottom, bool left, bool right, int x, int y, vector<int> shape)
{
    vector<int> xy_adjust(2);

    if (top){
        xy_adjust[1] += (y < 0) ? shape[0]*shape[1]: 0;
    }
    if (bottom){
        xy_adjust[1] += (y >= shape[0]) ? -shape[0]*shape[1]: 0;
    }
    if(left){
        xy_adjust[0] += (x < 0) ? shape[1]: 0;
    }
    if(right){
        xy_adjust[0] += (x >= shape[1]) ? -shape[1]: 0;
    }

    return xy_adjust;
}

Rule::Rule(vector<vector<vector<float>>> stateRules) : stateRules(stateRules) {}

CASystem::CASystem(Rule rule, voro::container_2d &voro_cells, int num_seeds, bool include_boundary)
        : rule(rule), voro_cells(voro_cells), num_seeds(num_seeds), include_boundary(include_boundary) {}

vector<int> CASystem::applyCA(vector<int> &state)
{
    vector<int> next_state(num_seeds, 0);

    // stateRules[N][O][M] - N corresponds to the total neighbors, O is the state of the cell, and M is the alive neighbors
    // Neighbors = n = [[..., 4, 5], [6], [7, 8, ...]] --> N = [0, 1, 2] (i.e. if n <=5, N = 0; if n=6, N = 1; if n>=7, N = 2)

    // voro::voronoicell_2d c(voro_cells);
    voro::voronoicell_neighbor_2d c(voro_cells);
    for(auto cli = voro_cells.begin(); cli < voro_cells.end(); cli++)
    {
        if (voro_cells.compute_cell(c, cli)) {
            vector<int> neighbor_ids;
            c.neighbors(neighbor_ids);
            int c_id = voro_cells.pid(cli);

            // Debug: Check the cell ID and its neighbors
            // cout << "Cell ID: " << c_id << ", Neighbors: ";
            // for (int id : neighbor_ids) {
            //     cout << id << " ";
            // }
            // cout << endl;

            int O = state[c_id];
            int M = 0;
            int n = 0;
            for (int id : neighbor_ids) {
                if (id >= 0){
                    M += state[id];
                    n++;
                }
            }
            if (include_boundary){
                n = neighbor_ids.size();
            }
            int N = clamp(n - 5, 0, 2);
            M = clamp(M, 0, 7);

            // Debug: Print the values of N, O, and M
            // cout << "Cell ID: " << c_id << ", n: " << n << ", N: " << N << ", O: " << O << ", M: " << M << endl;

            next_state[c_id] = rule.stateRules[N][O][M];
        }
    }

    return next_state;
}

int CASystem::getNumSeeds() 
{
    return num_seeds;
}

void print_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, string filename)
{
    ofstream file;
    file.open(filename);
    file << ca.getNumSeeds() << endl;
    vector<int> curr_state = initial_state;

    for (int i = 0; i < iterations; i++) {
        writeTo(&file, &curr_state);
        file << endl;
        curr_state = ca.applyCA(curr_state);
    }
    writeTo(&file, &curr_state);
    file.close();
}
