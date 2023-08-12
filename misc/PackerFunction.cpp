/** @file PackerFunction.cpp
 * An attempt to make a data packer that can handle arbitrarily sized input integers.
 * @author frozen_dude
 * @copyright This example is released in the public domain without implicit or explicit warranty.
 */

#include <stdexcept>
#include <initializer_list>
#include <utility> // std::pair

/** Pack one uint somewhere in another.
 * The function that does the actual packing for pack_data.
 * @param data the data to be packed.
 * @param pos offset in the output where to put the data, right-to-left from 0.
 * @param size the amount of bits from the data to use.
 * @return partially packed data, logical or with other data to stitch them together.
 */
const unsigned int pack_uint(const unsigned int &data, const char pos, const char size) {
    return (data & ((1 << size) - 1)) << pos;
}

/**
 * Helper type definition.
 */
typedef std::pair<const unsigned int, char> packing_var;

/** Arbitrary size data packer.
 * Packs several unsigned integers into one unsigned integer,
 * working in reverse order, least significant bits first.
 * You feed it a list of pairs.
 * @n
 * Example:
 * @code
 * unsigned int x = pack_data( { {40,8}, {10,8}, {1,1}, {230,15} } );
 * @endcode
 * @throws range_error if the unsigned int is too small for the input.
 * @param var_list list of items to pack, with size in bits as second parameter.
 * @return packed data as an unsigned int.
 */
const unsigned int pack_data(std::initializer_list<packing_var> var_list) {
    unsigned int value = 0;
    char pos = 0;
    for (auto item : var_list) {
        if (pos + item.second > sizeof(unsigned int)*8)
            throw new std::range_error("Data is too big to fit inside an unsigned int.");
        value |= pack_uint(item.first, pos, item.second);
        pos += item.second;
    }
    return value;
}
