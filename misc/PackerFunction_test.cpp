//
// Created by frozen on 2023-08-12.
//

#include <iostream>
#include <iomanip>
#include "PackerFunction.cpp"

using namespace std;

int main() {
    unsigned int x = pack_data({{16,8},{32,8},{64,12},{8,4}});
    cout.flags(ios::hex | ios::right);
    cout.fill('0');
    cout << setw(sizeof(unsigned int)*2) << x << endl;
    return 0;
}
