//
// Created by frozen on 2023-08-12.
//

#include "PackedVectorData.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

void print_data(PackedVectorData &data) {
    cout.flags(ios::hex | ios::right);
    cout << "Data: " << setw(sizeof(unsigned int)*2) << setfill('0') << data << endl;
    cout.flags(ios::dec | ios::left);
    cout.fill(' ');
    cout << "     X: " << setw(10) << data.x;
    cout << "     Y: " << setw(10) << data.y;
    cout << "     Z: " << setw(10) << data.z << endl;
    cout << "faceID: " << setw(6) << data.faceID;
    switch (data.faceID) {
        case PackedVectorData::FaceID::top:
            cout << "(top)   "; break;
        case PackedVectorData::FaceID::bottom:
            cout << "(bottom)"; break;
        case PackedVectorData::FaceID::front:
            cout << "(front) "; break;
        case PackedVectorData::FaceID::back:
            cout << "(back)  "; break;
        case PackedVectorData::FaceID::left:
            cout << "(left)  "; break;
        case PackedVectorData::FaceID::right:
            cout << "(right) "; break;
        default:
            cout << "(error) ";
    }
    cout << " texID: " << setw(10) << data.texID;
    cout << " extra: " << setw(10) << data.extra << endl;
}

int main() {
    PackedVectorData data = 11353512;
    print_data(data);
    data = {20, 30, 40, 2};
    print_data(data);
    data.x = 10;
    print_data(data);
}