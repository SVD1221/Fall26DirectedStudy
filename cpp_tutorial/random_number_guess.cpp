#include <random>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

int main()
{
    random_device rnd_mch;
    mt19937 gen(rnd_mch());

    int min = 1;
    int max = 12;
    int sample_size = 100000;
    int size = max - min + 1;

    uniform_int_distribution<> dist(min, max);

    //store the randomly generated values
    vector<int> rnd_counts(size, 0);
    for (int i = 0; i < sample_size; i++)
    {
        int rnd_num = dist(gen);
        rnd_counts[rnd_num-min]++;
        // cout << rnd_num << " ";
    }
    // cout << endl;

    //print each number in the range of values
    for (int i = 0; i < size; i++)
    {
        cout << min + i << "\t";
    }
    cout << endl;

    //print the count of each number generated
    for (int i = 0; i < size; i++)
    {
        cout << rnd_counts[i] << "\t";
    }
    cout << endl;

    //collect the fequencies in a distribution vector
    vector<float> sample_distribution(size, 0);
    for (int i = 0; i < size; i++)
    {
        float percent = rnd_counts[i] / (float)sample_size;
        sample_distribution[i] = percent;
        cout << percent << "\t";
    }
    cout << endl;

    //compare to the true distibution with mean-squared error
    vector<float> true_distribution(size, 1/(float)size);
    float mse;
    for (int i = 0; i < size; i++)
    {
        mse = mse + pow(true_distribution[i] - sample_distribution[i], 2);
    }
    mse = pow(mse, 0.5);
    cout << "The mean-squared error is " << mse << "." << endl;

    return 0;
}
