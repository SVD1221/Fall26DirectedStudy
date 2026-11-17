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

void print_Dynamic_Voro_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, int ca_cycle, int lloyd_cycle, int random_seed, string folder)
{
    voro::container_2d con_curr = ca.getCells();

    std::vector<int> domain = {con_curr.bx, con_curr.by};
    bool periodic = con_curr.x_prd;
    
    container_2d con_next(0,domain[0], 0,domain[1], domain[0],domain[1], periodic, periodic, ca.getNumSeeds());

    std::string filename = folder + "/cellset" + std::to_string(0) + ".gnu";
    con_curr.print_custom("%i %x %y %.8P", filename.c_str());                           // print initial state

    ofstream file;
    file.open(folder + "/dynamic_evolution.txt");
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

        filename = folder + "/cellset" + std::to_string(i+1) + ".gnu";
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

DLARule::DLARule(vector<vector<float>> stateRules, int min, int max): stateRules(stateRules), min(min), max(max) {}

DLASystem::DLASystem(DLARule rule, voro::container_2d &voro_cells, int num_seeds)
            : rule(rule), voro_cells(voro_cells), num_seeds(num_seeds) {}

vector<int> DLASystem::getValueRange()
{
    return {rule.min, rule.max};
}

voro::container_2d DLASystem::getCells()
{
    return voro_cells;
}

void DLASystem::updateCells(voro::container_2d &new_voro_cells)
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

vector<int> DLASystem::applyCA(vector<int> &state)
{

    vector<int> next_state(num_seeds, 0);
    vector<bool> occupied(num_seeds, false);

    // stateRules[O][K] - O corresponds to the state of the cell and K is the smallest neighbor 

    voro::voronoicell_neighbor_2d c(voro_cells);
    for(auto cli = voro_cells.begin(); cli < voro_cells.end(); cli++)
    {
        if (voro_cells.compute_cell(c, cli)) {
            vector<int> neighbor_ids;
            c.neighbors(neighbor_ids);
            int c_id = voro_cells.pid(cli);

            int O = state[c_id];
            
            if (O < 0){
                next_state[c_id] = O;
                occupied[c_id] = true;
                continue;
            }

            if (O == 0){
                continue;
            }
            
            int max = -100;
            int min = 100;
            for (int id : neighbor_ids) {
                if (state[id] < min) {
                    min = state[id];
                }
                if (state[id] > max) {
                    max = state[id];
                }
            }

            if (min < 0) { // if one of your neighbors is fixed, freeze and change state according to stateRules
                next_state[c_id] = rule.stateRules[O-1][min-rule.min];   // adjust indices to match their location in stateRules
                occupied[c_id] = true;
            }
            else if (max >= 0){ // if none of your neighbors are fixed, randomly move in emtpy cell and change state according to stateRules
                int new_id;
                vector<int> empty_neighbors;

                for (int id : neighbor_ids) {
                    if (state[id] == 0 && !occupied[id]) {
                        empty_neighbors.push_back(id);
                    }
                }

                if (!empty_neighbors.empty()) {
                    new_id = empty_neighbors[rand() % empty_neighbors.size()];
                }

                if (max == 0) {
                    next_state[new_id] = O;
                }
                else {
                    next_state[new_id] = rule.stateRules[O - 1][max - rule.min - 1]; // adjust indices to match their location in stateRules
                }

                occupied[new_id] = true;
            }

        }
    }

    return next_state;
}

int DLASystem::getNumSeeds() 
{
    return num_seeds;
}

void print_DLA_Evolution(vector<int> initial_state, DLASystem dla, int iterations, string filename)
{
    ofstream file;
    file.open(filename);
    vector<int> min_max = dla.getValueRange();
    file << dla.getNumSeeds() << " " << min_max[0] <<  " " << min_max[1] << endl;
    vector<int> curr_state = initial_state;

    for (int i = 0; i < iterations; i++) {
        writeTo(&file, &curr_state);
        file << endl;
        curr_state = dla.applyCA(curr_state);
    }
    writeTo(&file, &curr_state);
    file.close();
}

void print_Dynamic_DLA_Voro_CA_Evolution(vector<int> initial_state, DLASystem dla, int iterations, int ca_cycle, int lloyd_cycle, int random_seed, string folder)
{
    voro::container_2d con_curr = dla.getCells();

    std::vector<int> domain = {con_curr.bx, con_curr.by};
    bool periodic = con_curr.x_prd;
    
    container_2d con_next(0,domain[0], 0,domain[1], domain[0],domain[1], periodic, periodic, dla.getNumSeeds());

    std::string filename = folder + "/cellset" + std::to_string(0) + ".gnu";
    con_curr.print_custom("%i %x %y %.8P", filename.c_str());                           // print initial state

    ofstream file;
    file.open(folder + "/dynamic_evolution.txt");
    file << random_seed << endl;
    file << dla.getNumSeeds() << endl;
    file << ca_cycle << endl;
    file << lloyd_cycle << endl;
    vector<int> curr_state = initial_state;

    for (int i = 0; i < iterations; i++) {
        writeTo(&file, &curr_state);                                                        // print current states
        file << endl;
        if (i % ca_cycle == ca_cycle-1){
            curr_state = dla.applyCA(curr_state);                                            // apply CA rules once
        }

        filename = folder + "/cellset" + std::to_string(i+1) + ".gnu";
        if (i % lloyd_cycle == lloyd_cycle-1){
            voro::container_2d con_next = run_Lloyd(con_curr, dla.getNumSeeds(), 1);        // update con_next
            con_next.print_custom("%i %x %y %.8P", filename.c_str());                       // print con_next
            dla.updateCells(con_next);                                                      // update con_curr to con_next and clear con_next
        }
        else{
            con_curr.print_custom("%i %x %y %.8P", filename.c_str());                       // print con_curr
        }
    }
    writeTo(&file, &curr_state);
    file.close();
}
