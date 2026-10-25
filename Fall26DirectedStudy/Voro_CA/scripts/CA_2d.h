// Prevent multiple inclusions
#ifndef CA_2D_H
#define CA_2D_H

// Package inclusions
#include <vector>
#include <fstream>

using namespace std;

// Declare functions, classes, or variables

vector<int> createRandomVec(int min, int max, int size, int seed);

void writeTo(ofstream *file, vector<int> *vec);

vector<int> createAdjustments(bool top, bool bottom, bool left, bool right, int x, int y, vector<int> shape);

void convolve2D(vector<int> *pConvolent, vector<int> data, vector<int> shape, vector<int> kernel, vector<int> k_shape);

struct Rule
{
    vector<vector<float>> stateRules;
    vector<int> neighborhood;
    int neighWidth;
    int neighHeight;

    Rule(vector<vector<float>> stateRules, vector<int> neighborhood, int neighWidth, int neighHeight);
}; 

class CASystem
{
    private:
        vector<int> shape;
        vector<int> kernel;
        vector<int> kernel_shape;
        Rule rule;

    public:
        CASystem(vector<int> shape, Rule rule);
        
        vector<int> applyCA(vector<int> state);

        vector<int> countNeighbors(vector<int> state);

        vector<int> getShape();
};

void print_CA_Evolution(vector<int> initial_state, CASystem ca, int iterations, string filename);

#endif // CA_2D_H