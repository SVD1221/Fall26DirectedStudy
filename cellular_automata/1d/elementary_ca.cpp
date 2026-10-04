//elementary cellular automata
// - convolutions for rules

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

vector<int> join(vector<int> a, vector<int> b)
{
    int a_size = a.size();
    vector<int> joint(a_size+b.size());
    for (int i = 0; i < joint.size(); i++) {
        if (i < a_size) {
            joint[i] = a[i];
        }
        else {
            joint[i] = b[i-a_size];
        }
    }

    return joint;
}

void printVec(vector<int> vec)
{
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i] << " ";
    }
    cout << endl;
}

void printAsDots(vector<int> vec, vector<char> bin)
{
    for (int i = 0; i < vec.size(); i++) {
        cout << bin[vec[i]] << " ";
    }
    cout << endl;
}

int dotProduct(vector<int> v, vector<int> u)
{
    int sum = 0;
    for (int i = 0; i < v.size(); i++) {
        sum += v[i]*u[i];
    }
    return sum;
}

vector<int> convolve1D(vector<int> x, vector<int> kernel, bool periodic = true)
{
    vector<int> y = x;
    if (periodic) {
        y = join({y[y.size()-1]}, join(y, {y[0]}));
    }
    vector<int> result(y.size() - 2);
    for (int i = 1; i < y.size() - 1; i++) {
        vector<int> sub_y(kernel.size());
        for (int j = 0; j < sub_y.size(); j++) {
            sub_y[j] = y[j+i-1];
        }
        result[i - 1] = dotProduct(sub_y, kernel);
    }

    return result;
}

vector<int> next(vector<int> prev_state, int rule, vector<int> kernel)
{
    const bitset<8> rule_bits(rule);
    vector<int> next_state(prev_state.size());
    vector<int> index_vec = convolve1D(prev_state, kernel);

    for (int i = 0; i < index_vec.size(); i++)
    {
        next_state[i] = rule_bits[index_vec[i]];
    }

    return next_state;
}

void writeTo(ofstream *file, vector<int> *vec)
{
    for (int i = 0; i < (*vec).size(); i++){
        *file << (*vec)[i] << " ";
    }
}

int main()
{
    ofstream file;
    file.open("output_files/eca_output.txt");

    int iterations = 30;
    vector<int> kernel = {4, 2, 1};
    vector<int> initial_state = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    vector<int> RULES = {37, 42, 53, 67, 81, 105, 149};

    file << initial_state.size() << endl;
    writeTo(&file, &RULES);
    file << endl;

    for (int i = 0; i < RULES.size(); i++) {
        const int rule = RULES[i];
        vector<int> curr_state = initial_state;

        //// Writes to file (width \n rules \n data)
        writeTo(&file, &curr_state);
        for (int i = 1; i < iterations - 1; i++){
            curr_state = next(curr_state, rule, kernel);
            writeTo(&file, &curr_state);
        }
        curr_state = next(curr_state, rule, kernel);
        writeTo(&file, &curr_state);
        if (i != RULES.size() -1){
            file << endl;
        }

        //// Writes to Terminal
        // const bitset<8> rule_bits(rule);
        // cout << "Rule: " << rule << " (" << rule_bits << ")" << endl;
        // for (int i = 0; i < iterations; i++){
        //     printAsDots(curr_state, {' ', '*'});
        //     curr_state = next(curr_state, rule, kernel);
        // }
    }

    file.close();

    return 0;
}
