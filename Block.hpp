#pragma once

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <glm/glm.hpp>
#include <glad/glad.h>



// The indexes that correspond to each block face
typedef enum BlockFaces {
    Back_Face   = 0,
    Front_Face  = 1,
    Left_Face   = 2,
    Right_Face  = 3,
    Top_Face    = 4,
    Bottom_Face = 5
} BlockFaces;



struct Block_Configuration
{
    GLint Block_TypeID = -1;
    GLboolean Transparent = false;
    GLboolean IsFoliage = false;
};



// Struct used to determine the ambient occlusion for each face of a block
struct aoBlockFace
{
    // Booleans to determine where ambient occlusion
    // should be done
    GLboolean aoTopLeft = false;
    GLboolean aoTopRight = false;
    GLboolean aoBottomLeft = false;
    GLboolean aoBottomRight = false;
};



class Block
{
public:
    glm::vec3 position; // Position of the block
    GLint blockTypeID = -1; // What type of block is this? By default Air

    // A struct to determine the Ambient Occlusion values
    // for each face of the block
    aoBlockFace topFace;
    aoBlockFace bottomFace;
    aoBlockFace leftFace;
    aoBlockFace rightFace;
    aoBlockFace frontFace;
    aoBlockFace backFace;

    // Blank constuctor
    Block(){};
};

// Reference for our textures
inline json blocks;

