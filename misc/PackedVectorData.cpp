/** @file PackedVectorData.cpp
 * Class made from the example Finding Fortune made with this video:
 * https://youtu.be/d10MOYtNXB4
 * @author frozen_dude
 * @copyright This example is released in the public domain without implicit or explicit warranty.
 */

#include "PackedVectorData.hpp"
#include <initializer_list>

/** Assign from packed data.
 * Read packed data when passing an unsigned int as rvalue in an assignment.
 * @n
 * Example:
 * @code
 * myObject = 16416372;
 * cout << myObject.x << endl;
 * @endcode
 * @param packedData data packed into an unsigned int
 * @return returns this object.
 */
PackedVectorData &PackedVectorData::operator=(const unsigned int &packedData) {
    this->x = (packedData >> 0) & ((1 << 6) - 1);
    this->y = (packedData >> 6) & ((1 << 6) - 1);
    this->z = (packedData >> 12) & ((1 << 6) - 1);
    this->faceID = static_cast<PackedVectorData::FaceID>((packedData >> 18) & ((1 << 3) - 1));
    this->texID = (packedData >> 21) & ((1 << 5) - 1);
    this->extra = (packedData >> 26) & ((1 << 6) - 1);
    return *this;
}

/** Get packed data.
 * Return the packed data as an unsigned int when you treat the object as a normal integer.
 * @n
 * Example:
 * @code
 * PackedVectorData myObject = {10,16,22};
 * unsigned int x = myObject;
 *
 * cout << myObject << endl;
 * @endcode
 * @return unsigned int with packed data
 */
PackedVectorData::operator unsigned int() const {
    return ((x)     &((1<< 6 )-1))\
    |      ((y)     &((1<< 6 )-1))<<6\
    |      ((z)     &((1<< 6 )-1))<<12\
    |      ((faceID)&((1<< 3 )-1))<<18\
    |      ((texID) &((1<< 5 )-1))<<21\
    |      ((extra) &((1<< 6 )-1))<<26;
}

/** Assign from an initializer list.
 * Assign values in order when assigning from an initializer list.
 * @attention If the initializer list is smaller than the member count, the undefined members are left untouched.
 * @n
 * Example:
 * @code
 * PackedVectorData myObject;
 * myObject = {50, 40, 30, 3, 2};
 * @endcode
 * @param values initializer_list with {x, y, z, faceID, texID, extra}
 * @return returns this object.
 */
PackedVectorData &PackedVectorData::operator=(const std::initializer_list<unsigned int> &values) {
    unsigned int *vars[] = {&(this->x),&(this->y),&(this->z),reinterpret_cast<unsigned int *>(&(this->faceID)),&(this->texID),&(this->extra)};
    int i = 0;
    for ( auto item : values ) {
        *vars[i++] = item;
        if (i >= 6) break;
    }
    return *this;
}

/** Constructor for initializer list.
 * Constructor for initializer list.
 * @n
 * Example:
 * @code
 * PackedVectorData myObject({1,2,3,4,5,6});
 * PackedVectorData otherObject = {1,2,3,4,5,6};
 * @endcode
 * @param values initializer list.
 */
PackedVectorData::PackedVectorData(const std::initializer_list<unsigned int> &values) {
    *this = 0;
    *this = values;
}

/** Empty constructor.
 * Constructor used when no parameters are given upon creation, makes all values 0.
 */
PackedVectorData::PackedVectorData() {
    *this = 0;
}

/** Packed data constructor.
 * Constructor used when an unsigned int is given as a parameter for the object construction.
 * @n
 * Example:
 * @code
 * PackedVectorData myObject(164362346);
 * PackedVectorData otherObject = 36161523;
 * @endcode
 * @param packedData unsigned it with the packed data.
 */
PackedVectorData::PackedVectorData(const unsigned int packedData) {
    *this = packedData;
}
