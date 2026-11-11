#include "CA_2d.h"

#include "Voro_2d.h"

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

Rule::Rule(vector<vector<vector<float>>> stateRules) : stateRules(stateRules) {}

CASystem::CASystem(Rule rule, voro::container_2d &voro_cells, int num_seeds, bool include_boundary, int boundary_state)
        : rule(rule), voro_cells(voro_cells), num_seeds(num_seeds), include_boundary(include_boundary), boundary_state(boundary_state) {}

voro::container_2d CASystem::getCells()
{
    return voro_cells;
}

void CASystem::updateCells(voro::container_2d &new_voro_cells)
{
    voro_cells.clear();

    voronoicell_2d c(new_voro_cells);

    for(auto cli = new_voro_cells.begin(); cli < new_voro_cells.end(); cli++) {
            double xp, yp;
            new_voro_cells.pos(cli, xp, yp);
            voro_cells.put(new_voro_cells.pid(cli), xp, yp);
        }

    new_voro_cells.clear();
}

vector<int> CASystem::applyCA(vector<int> &state)
{
    vector<int> next_state(num_seeds, 0);

    // stateRules[N][O][M] - N corresponds to the total neighbors, O is the state of the cell, and M is the alive neighbors
    // Neighbors = n = [[..., 4, 5], [6], [7, 8, ...]] --> N = [0, 1, 2] (i.e. if n <=5, N = 0; if n=6, N = 1; if n>=7, N = 2)

    voro::voronoicell_neighbor_2d c(voro_cells);
    for(auto cli = voro_cells.begin(); cli < voro_cells.end(); cli++)
    {
        if (voro_cells.compute_cell(c, cli)) {
            vector<int> neighbor_ids;
            c.neighbors(neighbor_ids);
            int c_id = voro_cells.pid(cli);

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
                M += boundary_state*(neighbor_ids.size() - n);
                n = neighbor_ids.size();
            }
            int N = clamp(n - 5, 0, 2);
            M = clamp(M, 0, 7);

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

void print_Dynamic_Voro_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, int ca_cycle, int lloyd_cycle, int random_seed)
{
    voro::container_2d con_curr = ca.getCells();
    voro::container_2d con_next(0,10, 0,10, 10,10, false, false, ca.getNumSeeds());

    std::string filename = "output/dynamic_Voro_CA/cellset" + std::to_string(0) + ".gnu";
    con_curr.print_custom("%i %x %y %.8P", filename.c_str());                           // print initial state

    ofstream file;
    file.open("output/dynamic_Voro_CA/dynamic_evolution.txt");
    file << random_seed << endl;
    file << ca.getNumSeeds() << endl;
    file << ca_cycle << endl;
    file << lloyd_cycle << endl;
    vector<int> curr_state = initial_state;

    for (int i = 0; i < iterations; i++) {
        writeTo(&file, &curr_state);                                                        // print current states
        file << endl;
        if (i % ca_cycle == ca_cycle-1){
            curr_state = ca.applyCA(curr_state);                                            // apply CA rules once
        }

        filename = "output/dynamic_Voro_CA/cellset" + std::to_string(i+1) + ".gnu";
        if (i % lloyd_cycle == lloyd_cycle-1){
            voro::container_2d con_next = run_Lloyd(con_curr, ca.getNumSeeds(), 1);                                                  // update con_next
            con_next.print_custom("%i %x %y %.8P", filename.c_str());                       // print con_next
            ca.updateCells(con_next);                                                       // update con_curr to con_next and clear con_next
        }
        else{
            con_curr.print_custom("%i %x %y %.8P", filename.c_str());                       // print con_curr
        }
    }
    writeTo(&file, &curr_state);
    file.close();
}
