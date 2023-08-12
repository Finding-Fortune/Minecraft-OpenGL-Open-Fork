/** @file PackedVectorData.hpp
 * Class made from the example Finding Fortune made with this video:
 * https://youtu.be/d10MOYtNXB4
 * @author frozen_dude
 * @copyright This example is released in the public domain without implicit or explicit warranty.
 */

#ifndef MINECRAFT_OPENGL_OPEN_FORK_PACKEDVECTORDATA_HPP
#define MINECRAFT_OPENGL_OPEN_FORK_PACKEDVECTORDATA_HPP

#include <initializer_list>

class PackedVectorData {
public:
    unsigned int x;
    unsigned int y;
    unsigned int z;
    /**
     * Which face is it?
     */
    enum FaceID : unsigned int {
        back = 0,
        front = 1,
        left = 2,
        right = 3,
        top = 4,
        bottom = 5
    } faceID;
    /**
     * Which texture is it? (0-31)
     */
    unsigned int texID;
    unsigned int extra;
    PackedVectorData & operator= (const unsigned int &packedData);
    PackedVectorData & operator= (const std::initializer_list<unsigned int> &values);
    operator unsigned int () const;
    PackedVectorData(const std::initializer_list<unsigned int> &values);
    PackedVectorData(const unsigned int packedData);
    PackedVectorData();
};


#endif //MINECRAFT_OPENGL_OPEN_FORK_PACKEDVECTORDATA_HPP
