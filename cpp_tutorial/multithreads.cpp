#include <iostream>
#include <thread>
#include <vector>

using namespace std;

void funct(vector<int> *a, vector<int> i)
{
    (*a)[i[0]] = (-1)*((*a)[i[0]]);
}

int main()
{
    vector<int> a = {1, -3};
    vector<int> i1 = {0};
    vector<int> i2 = {1};
    thread t1(funct, &a, i1);
    thread t2(funct, &a, i2);

    t1.join();
    t2.join();

    cout << a[0] << " " << a[1] << endl;

    return 0;
}
