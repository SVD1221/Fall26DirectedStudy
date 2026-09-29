#include <iostream>
#include <string>

using namespace std;

struct Wood {
    string color;
};

void paintWood1(Wood wood, string color)
{
    cout << &wood << "\n";
    wood.color = color;
}

void paintWood2(Wood *pWood, string color)
{
    cout << pWood << "\n";
    (*pWood).color = color;
}

int main()
{
    // standard variable assignment
    int value = 10;
    // create a pointer of the same type assigns the variable's address
    int *pValue = &value;

    // the reference operator (&) and the pointer output the same address
    // cout << &value << "\n";
    // cout << pValue << "\n";

    // the dereference operator (*) and the variable output the same value 
    // cout << value << "\n";
    // cout << *pValue << "\n";

    // the pointer is stored in memory and can be treated like any other variable
    int **pPValue = &pValue;
    // cout << pPValue << "\n";
    // cout << &pValue << "\n";

    Wood oak = Wood{"brown"};
    // cout << &oak << "\n";
    // cout << oak.color << "\n";
    // the object 'oak' will remain unchanged by paintWood1() because 'oak' 
    // is passed by value (i.e. a copy of 'oak' is used in paintWood1() rather 
    // than the original 'oak' object)
    // paintWood1(oak, "red");
    // cout << oak.color << "\n";
    // the object 'oak' will be changed by paintWood2() because 'oak' 
    // is passed by reference (i.e. the address of 'oak' is used in paintWood2()
    // then dereferenced to access the original 'oak' object)
    // paintWood2(&oak, "red");
    // cout << oak.color << "\n";
}
