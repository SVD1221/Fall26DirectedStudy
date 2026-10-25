#include "CA_2d.h"

#include <vector>
#include <fstream>

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

void convolve2D(vector<int> *pConvolent, vector<int> data, vector<int> shape, vector<int> kernel, vector<int> k_shape)
{
    int radius = k_shape[0]/2;
    int k_center = k_shape[0]*k_shape[1]/2;

    for (int x = 0; x < shape[1]; x++){ //~max(shape)
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

Rule::Rule(vector<vector<float>> stateRules, vector<int> neighborhood, int neighWidth, int neighHeight)
    : stateRules(stateRules), neighborhood(neighborhood), neighWidth(neighWidth), neighHeight(neighHeight)
    {}

CASystem::CASystem(vector<int> shape, Rule rule)
    : shape(shape), rule(rule), kernel(rule.neighborhood), kernel_shape({rule.neighHeight, rule.neighWidth})
    {}

vector<int> CASystem::applyCA(vector<int> state)
{
    vector<int> next_state(shape[0]*shape[1], 0);
    vector<int> neighbors = countNeighbors(state);

    for (int x = 0; x < shape[1]; x++){ //~max(shape)
        for (int y = 0; y < shape[0]; y++){ //~max(shape)
            int pos = shape[1]*y + x;
            next_state[pos] = rule.stateRules[0][neighbors[pos]];
            }
        }

    return next_state;
}

vector<int> CASystem::countNeighbors(vector<int> state)
{
    int size = shape[0]*shape[1];
    vector<int> convolent(size);

    convolve2D(&convolent, state, shape, kernel, kernel_shape);

    return convolent;
}

vector<int> CASystem::getShape()
{
    return shape;
}

void print_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, string filename)
{
    vector<int> shape = ca.getShape();
    ofstream file;
    file.open(filename);
    file << shape[0] << " " << shape[1] << endl;
    vector<int> curr_state = initial_state;

    for (int i = 0; i < iterations; i++){
        writeTo(&file, &curr_state);
        file << endl;
        curr_state = ca.applyCA(curr_state);
    }
    writeTo(&file, &curr_state);
    file.close();
}
