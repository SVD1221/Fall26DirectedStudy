#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <unordered_set>

using namespace std;

// g++ DLA_CA.cpp -o DLA_CA

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

vector<vector<int>> createRandom2DPoints(int num_points, int width, int height, int random_seed)
{
    srand(random_seed);

    vector<vector<int>> points(num_points, vector<int>(2));

    int i = 0;
    unordered_set<int> used;
    while (i < num_points)
    {
        int x = rand() % width;
        int y = rand() % height;

        int idx = y * width + x;

        if (used.insert(idx).second)
        {
            points[i] = {x, y};
            i += 1;
        }
    }

    return points;
}

vector<int> assignRandomValues(vector<vector<int>> points, int num_fixed, int width, int height, int random_seed, int min, int max, bool constant, int constant_val)
{
    srand(random_seed);

    vector<int> rand_vec(width*height, 0);

    for (int i = 0; i < num_fixed; i++)
    {
        int index = points[i][1]*width + points[i][0];
        rand_vec[index] = -(rand() % (-min) + 1);
    }

    for (int i = num_fixed; i < points.size(); i++)
    {
        int index = points[i][1]*width + points[i][0];
        if (constant){
            rand_vec[index] = constant_val;
        }
        else{
            rand_vec[index] = rand() % max + 1;
        }
    }

    return rand_vec;
}

// Rule
/**
 * Represents the rules for the limited diffusion aggregation (DLA) model.
 * stateRules[M][I] - M corresponds type of mobile cell, I corresponds to the interaction of that cell with its neighbors (negative left-most and positive right-most priority).
 *        e.g. _|_-1_|_1_|_2_|                  e.g. _|_-1_|_1_|
 *             1|_-1_|_1_|_2_|                       1|_-1_|_1_|
 *             2|_-1_|_2_|_2_|
 *        (Two-mobile cell example)             (Standard DLA example)
 */
struct Rule
{
    vector<vector<float>> stateRules;
    int min;
    int max;

    Rule(vector<vector<float>> stateRules, int min, int max): stateRules(stateRules), min(min), max(max) {}
};
                        
class DLASystem
{
    private:
        Rule rule;
        vector<vector<int>> particles; // particle locations
        int num_particles;
        int width;
        int height;

    public:
        DLASystem(Rule rule, vector<vector<int>> particles, vector<int> shape)
            : rule(rule), particles(particles), num_particles(particles.size()), width(shape[1]), height(shape[0])
        {}

        vector<int> applyCA(vector<int> &state)
        {
            vector<int> next_state(width * height, 0);
            vector<bool> occupied(width * height, false);

            for (int i = 0; i < num_particles; i++) {
                int x = particles[i][0];
                int y = particles[i][1];

                int curr_index = y * width + x;
                int curr_state = state[curr_index];

                if (curr_state < 0) { // fixed cells don't change state
                    next_state[curr_index] = curr_state;
                    occupied[curr_index] = true;
                    continue;
                }

                vector<int> neighbor_indices(8);

                // periodic BCs
                neighbor_indices[0] = ((y - 1 + height) % height) * width + x;                        // top
                neighbor_indices[1] = ((y + 1) % height) * width + x;                                 // bottom
                neighbor_indices[2] = y * width + (x - 1 + width) % width;                            // left
                neighbor_indices[3] = y * width + (x + 1) % width;                                    // right
                neighbor_indices[4] = ((y - 1 + height) % height) * width + (x - 1 + width) % width;  // top-left
                neighbor_indices[5] = ((y - 1 + height) % height) * width + (x + 1) % width;          // top-right
                neighbor_indices[6] = ((y + 1) % height) * width + (x - 1 + width) % width;           // bottom-left
                neighbor_indices[7] = ((y + 1) % height) * width + (x + 1) % width;                   // bottom-right

                // Find the "smallest" neighbor
                int max = -100;
                int min = 100;

                for (int neighbor_index : neighbor_indices) {
                    if (state[neighbor_index] < min) {
                        min = state[neighbor_index];
                    }
                    if (state[neighbor_index] > max) {
                        max = state[neighbor_index];
                    }
                }

                if (min < 0) { // if one of your neighbors is fixed, freeze and change state according to stateRules
                    next_state[curr_index] = rule.stateRules[curr_state-1][min-rule.min];   // adjust indices to match their location in stateRules
                    int new_index = curr_index;
                    occupied[new_index] = true;
                }
                else if (max >= 0){ // if none of your neighbors are fixed, randomly move in emtpy cell direction and change state according to stateRules
                    int new_index = curr_index;
                    vector<int> empty_neighbors;

                    for (int neighbor_index : neighbor_indices) {
                        if (state[neighbor_index] == 0 && !occupied[neighbor_index]) {
                            empty_neighbors.push_back(neighbor_index);
                        }
                    }

                    if (!empty_neighbors.empty()) {
                        new_index = empty_neighbors[rand() % empty_neighbors.size()];
                    }

                    if (max == 0) {
                        next_state[new_index] = state[y * width + x];
                    }
                    else {
                        next_state[new_index] = rule.stateRules[curr_state - 1][max - rule.min - 1]; // adjust indices to match their location in stateRules
                    }  
                    particles[i][0] = new_index % width;
                    particles[i][1] = new_index / width;
                    occupied[new_index] = true;
                }
            }

            return next_state;
        }
};

int main()
{
    int iterations = 150;
    vector<int> grid_shape = {100, 100};
    float density = 0.25;
    int num_particles = int(grid_shape[0]*grid_shape[1] * density);

    // int rule_min = -1;
    // int rule_max = 1;
    // Rule rule({
    //     // state = 1 (i.e. the only type of mobile state)
    //     {-1, 1}
    // }, rule_min, rule_max);

    int rule_min = -2;
    int rule_max = 3;
    Rule rule({
        // state = 1 (i.e. the first type of mobile state)
        {2, -1, 1, 1, 1},
        // state = 2 (i.e. the second type of mobile state)
        {-2, 1, 2, 2, 2},
        // state = 2 (i.e. the third type of mobile state)
        {2, 1, 3, 3, 3}
    }, rule_min, rule_max);

    vector<vector<int>> particles = createRandom2DPoints(num_particles, grid_shape[0], grid_shape[1], 12);
    vector<int> initial_state = assignRandomValues(particles, 10, grid_shape[0], grid_shape[1], 12, 
                                                    rule_min, rule_max, true, 3);

    DLASystem dla = DLASystem(rule, particles, grid_shape);

    // Write to file
    ofstream file;
    file.open("output/DLA.txt");
    file << grid_shape[0] << " " << grid_shape[1] << " " << rule_min << " " << rule_max << endl;
    vector<int> curr_state = initial_state;

    for (int i = 0; i < iterations; i++){
        writeTo(&file, &curr_state);
        file << endl;
        curr_state = dla.applyCA(curr_state);
    }
    writeTo(&file, &curr_state);

    file.close();

    return 0;
}
