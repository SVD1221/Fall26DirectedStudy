#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <chrono>

// TO USE MULTIPLE THREADS RUN: g++ -o 2d_ca 2d_ca.cpp -fopenmp

using namespace std;


float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}

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

void convolve2D(vector<int> *pConvolent, vector<int> data, vector<int> shape, vector<int> kernel, vector<int> k_shape, bool periodic = true)
{
    int radius = k_shape[0]/2;
    int k_center = k_shape[0]*k_shape[1]/2;

    // pragma here produces similar results
    for (int x = 0; x < shape[1]; x++){ //~max(shape)
        #pragma omp parallel for
        for (int y = 0; y < shape[0]; y++){ // ~max(shape)
            int sum = 0;
            int center = shape[1]*y + x;
            for (int r = 1; r < radius+1; r++){ // << min(shape)
                // top (left & right) neighs
                for (int i = -radius; i < radius+1; i++){
                    vector<int> xy_adjust = createAdjustments(true,false,true,true,x+i,y-r,shape);
                    sum += kernel[k_center - k_shape[1]*r + i]*data[center - shape[1]*r + i + xy_adjust[0] + xy_adjust[1]];
                }
                // bottom (left & right) neighs
                for (int i = -radius; i < radius+1; i++){
                    vector<int> xy_adjust = createAdjustments(false,true,true,true,x+i,y+r,shape);
                    sum += kernel[k_center + k_shape[1]*r + i]*data[center + shape[1]*r + i + xy_adjust[0] + xy_adjust[1]];
                }
                // left center neighs
                for (int i = -radius; i < 0; i++){
                    int x_adjust = createAdjustments(false,false,true,false,x+i,y,shape)[0];
                    sum += kernel[k_center + i]*data[center + i + x_adjust];
                }
                // right center neighs
                for (int i = 1; i <= radius; i++){
                    int x_adjust = createAdjustments(false,false,false,true,x+i,y,shape)[0];
                    sum += kernel[k_center + i]*data[center + i + x_adjust];
                }
            }
            (*pConvolent)[center] = sum;
        }
    }
}

//// Rule
// stateRules hold the rules for that index/state of the system and should line up with the rule types
// ruleTypes: neighbor count = 0, random chance/looking at neighbors = 1
//  0 - next state for each possible count (vector<int>)
//  1 - chance of that state to become any other state AND chance of that state to become another state if that state is in the neighborhood (vector<floats>)
struct Rule
{
    vector<vector<float>> stateRules;
    vector<int> ruleType;
    vector<int> neighborhood;
    int neighWidth;
    int neighHeight;
};               
                        
class CASystem
{
    private:
        vector<int> shape;
        vector<int> kernel;
        vector<int> kernel_shape;
        Rule rule;

    public:
        CASystem(vector<int> shape, Rule rule)
        : shape(shape), rule(rule), kernel(rule.neighborhood), kernel_shape({rule.neighHeight, rule.neighWidth})
        {}
        
        vector<int> applyCA(vector<int> state)
        {
            vector<int> next_state(shape[0]*shape[1], 0);
            vector<int> neighbors = countNeighbors(state);

            // adding pragma here doesn't improve performance
            // #pragma omp parallel for
            for (int x = 0; x < shape[1]; x++){ //~max(shape)
                // adding pragma here doesn't improve performance
                // #pragma omp parallel for
                for (int y = 0; y < shape[0]; y++){ //~max(shape)
                    int pos = shape[1]*y + x;

                    for (int s = 0; s < rule.stateRules.size(); s++){ // number of states
                        if (rule.ruleType[s] == 0){
                            //something with randomness/looking
                            if (state[pos] == s){
                                next_state[pos] = randomCheck(rule.stateRules[s], state, x, y);
                            }
                        }
                        else if (rule.ruleType[s] == 1){
                            if (state[pos] == s){
                                next_state[pos] = rule.stateRules[s][neighbors[pos]];
                            }
                        }
                    }
                }
            }
            return next_state;
        }

        int randomCheck(vector<float> state_space, vector<int> state, int x, int y)
        {
            int half = state_space.size()/2;
            int default_state;
            int k_center = kernel_shape[0]*kernel_shape[1]/2;
            int radius = kernel_shape[0]/2;
            int pos = shape[1]*y + x;
            for (int j = 0; j < state_space.size(); j++){
                if (state_space[j] == -1){
                    default_state = j;
                    continue;
                }

                if (state_space[j] > 0){
                    bool is_adjacent = true;

                    if (j >= half){
                        //check adj cells from kernel
                        is_adjacent = false;
                        for (int r = 1; r < radius+1; r++){
                            // top (left & right) neighs
                            for (int i = -radius; i < radius+1; i++){
                                vector<int> xy_adjust = createAdjustments(true,false,true,true,x+i,y-r,shape);
                                if(kernel[k_center - kernel_shape[1]*r + i] > 0 && j - half == state[pos - shape[1]*r + i + xy_adjust[0] + xy_adjust[1]]){
                                    is_adjacent = true;
                                    break;
                                }
                            }
                            // bottom (left & right) neighs
                            for (int i = -radius; i < radius+1; i++){
                                vector<int> xy_adjust = createAdjustments(false,true,true,true,x+i,y+r,shape);
                                if(kernel[k_center + kernel_shape[1]*r + i] > 0 && j - half == state[pos + shape[1]*r + i + xy_adjust[0] + xy_adjust[1]]){
                                    is_adjacent = true;
                                    break;
                                }
                            }
                            // left center neighs
                            for (int i = -radius; i < 0; i++){
                                int x_adjust = createAdjustments(false,false,true,false,x+i,y,shape)[0];
                                if(kernel[k_center + i] > 0 && j - half == state[pos + i + x_adjust]){
                                    is_adjacent = true;
                                    break;
                                }
                            }
                            // right center neighs
                            for (int i = 1; i <= radius; i++){
                                int x_adjust = createAdjustments(false,false,false,true,x+i,y,shape)[0];
                                if(kernel[k_center + i] > 0 && j - half == state[pos + i + x_adjust]){
                                    is_adjacent = true;
                                    break;
                                }
                            }
                            if(is_adjacent){
                                break;
                            }
                        }
                    }
                    
                    if (is_adjacent){
                        //check prob
                        if (randomFloat() < state_space[j]){
                            if (j < half){
                                return j;
                            }
                            return j - half;
                        }
                    }
                }
            }

            return default_state;
        }

        vector<int> countNeighbors(vector<int> state)
        {
            int size = shape[0]*shape[1];
            vector<int> convolent(size);

            convolve2D(&convolent, state, shape, kernel, kernel_shape, true);

            return convolent;
        }
};

const Rule GoL = {  {   {0, 0, 0, 1, 0, 0, 0, 0, 0},
                        {0, 0, 1, 1, 0, 0, 0, 0, 0} },
                    {   1, 
                        1   },
                    {   1, 1, 1,
                        1, 0, 1,
                        1, 1, 1   },
                    3, 3};

const Rule Sparkle = {  {   {0, 0, 1, 0, 1, 0, 0, 0, 0}, 
                            {0, 0, 1, 0, 0, 0, 0, 0, 0} },
                        {   1,
                            1   },
                        {   0, 1, 0,
                            1, 0, 1,
                            0, 1, 0   },
                        3, 3};

const Rule Fizzle = {   {   {0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, 
                            {0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0} }, 
                        {   1, 
                            1   },
                        {   1, 2, 1,
                            2, 0, 2,
                            1, 2, 1  },
                        3, 3};

//forest fire model
//  1) empty(0) cells have a CHANCE of spawning a tree(1) from adjacent trees(1)
//  2) tree(1) cells have a CHANCE of being struck by lightning and burning(2)
//  3) burning(2) cells WILL spread to nearby trees(1)
//  4) empty(0) cells have a CHANCE to be struck by lightning and burn(2)
//  5) burning(2) cells hava a CHANCE to burn out, leaving the cell empty(0)

const Rule ForestFire = {   {   {-1, 0.001, 0.00001, 0, 0.05, 0},
                                {0, -1, 0.00001, 0, 0, 0.75},
                                {0.3, 0, -1, 0, 0, 0}   },
                            {   0,
                                0,
                                0   },
                            {   0, 1, 0,
                                1, 0, 1,
                                0, 1, 0   },
                            3, 3};

int main()
{
    //// Start Clock
    auto start = chrono::steady_clock::now();

    int iterations = 150;
    vector<int> shape = {100, 100}; //{height, width}
    vector<int> initial_state = createRandomVec(0, 1, shape[0]*shape[1], 1);
    CASystem ca = CASystem(shape, GoL);
    // CASystem ca = CASystem(shape, ForestFire);

    //// Writes to file (shape \\newline data)
    ofstream file;
    file.open("output_files/2dca_output.txt");
    file << shape[0] << " " << shape[1] << endl;
    vector<int> curr_state = initial_state;

    for (int i = 0; i < iterations; i++){
        writeTo(&file, &curr_state);
        file << endl;
        curr_state = ca.applyCA(curr_state);
    }
    writeTo(&file, &curr_state);

    file.close();

    //// Stop Clock
    auto end = chrono::steady_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "Program finished in " << ms << " ms\n";

    return 0;
}
