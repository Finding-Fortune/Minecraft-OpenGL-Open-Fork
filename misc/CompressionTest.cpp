#include <iostream>



int main()
{
    // 6 Bits
    unsigned int posX = 25;
    // 6 Bits
    unsigned int posY = 63;
    // 6 Bits
    unsigned int posZ = 31;
    // 3 Bits
    unsigned int faceID = 5; // Which face is this? back is 0, front is 1, left is 2, right is 3, top is 4, bottom is 5
    // 5 Bits
    unsigned int texID = 23; // Which texture is this? 0-31

                                    // Shift 6 bits since we have 6 bits for x
                                               // Shift 12 bits since we have 6 bits for y
                                                              // Shift 18 bits since we have 6 bits for z
                                                                            // Shift 21 bits since we have 3 bits for faceID
    unsigned int finalNum = (posX | posY << 6 | posZ << 12 | faceID << 18 | texID << 21);
    // unsigned int finalNum = (posX | posY << 6 | posZ << 12);
    // Stored the position 25, 63, 31 in a unsigned int
    std::cout << "Final Compressed Num: " << finalNum << std::endl;

    std::cout << "Uncompressed X: " << (finalNum & 63) << std::endl; // Prints 25
    std::cout << "Uncompressed Y: " << ((finalNum >> 6) & 63) << std::endl; // Prints 63
    std::cout << "Uncompressed Z: " << ((finalNum >> 12) & 63) << std::endl; // Prints 31 
    std::cout << "Uncompressed FaceID: " << ((finalNum >> 18) & 7) << std::endl; // Prints 5
    std::cout << "Uncompressed TexID: " << ((finalNum >> 21) & 31) << std::endl; // Prints 23

    return 0;
}