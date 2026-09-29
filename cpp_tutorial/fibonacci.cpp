#include <iostream>
#include <vector>

using namespace std;

void printVec(vector<int> vec)
{
    for (int i = 0; i < vec.size(); i++)
    {
        cout << vec[i] << " ";
    }
    cout << endl;
}

int main() 
{
    vector<int> seq;
    //Initialize the sequence
    seq = {-3, 1};
    printVec(seq);

    //Use the recursive definition of the Fibonacci sequence 
    //to generate more terms
    while (seq.size() < 30)
    {
        seq.push_back(seq[seq.size() - 1] + seq[seq.size() - 2]);
    }
    printVec(seq);

    float ratio = ((float)seq[seq.size() - 1])/ seq[seq.size() - 2];
    cout << ratio << endl;
}
