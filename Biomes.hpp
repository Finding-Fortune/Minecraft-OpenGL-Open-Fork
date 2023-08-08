#pragma once

#include "Block.hpp"

// The indexes that correspond to each biome type
typedef enum Biomes {
    Null_Biome     = -1,
    Plains_Biome   = 0,
    Desert_Biome   = 1,
    Forest_Biome   = 2,
    Snow_Biome     = 3,
    Mountain_Biome = 4,
    Swamp_Biome    = 5
} Biomes;



struct Biome_Configuration
{
    GLint Surface_Block = 0;
    GLint Subsurface_Block = 1;
    GLboolean HotTemperature = true;
    GLboolean AllowWater = false;
    GLint TreeFrequency = -1; // The lower the tree frequency the more trees. A frequency of -1 means no trees spawn
    GLfloat NoiseGain = 1.0f;
    GLfloat NoiseFrequency = 0.0005f;
};



const Biome_Configuration BiomeConfiguration[] = {
    { // Plains_Biome
         0//blocks["Grass_Top"]["index"]            // Surface Block
        ,1//blocks["Dirt_Top"]["index"]             // Subsurface Block
        ,true                               // Hot Temperature
        ,true                               // Allow Water
        ,500                                // Tree Frequency
        ,0.340f                             // Noise Gain
        ,0.0005f                            // Noise Frequency
    },
    { // Desert_Biome
         6//blocks["Sand_Block"]["index"]             // Surface Block
        ,6//blocks["Sand_Block"]["index"]             // Subsurface Block
        ,true                               // Hot Temperature
        ,false                              // Allow Water
        ,-1                                 // Tree Frequency
        ,0.540f                             // Noise Gain
        ,0.0007f                            // Noise Frequency
    },
    { // Forest_Biome
         0//blocks["Grass_Top"]["index"]            // Surface Block
        ,1//blocks["Dirt_Top"]["index"]            // Subsurface Block
        ,true                               // Hot Temperature
        ,false                              // Allow Water
        ,25                                 // Tree Frequency
        ,0.540f                             // Noise Gain
        ,0.0005f                            // Noise Frequency
    },
    { // Snow_Biome
        7//blocks["Snow_Block"]["index"]             // Surface Block
        ,7//blocks["Snow_Block"]["index"]             // Subsurface Block
        ,false                              // Hot Temperature
        ,true                               // Allow Water
        ,5500                               // Tree Frequency
        ,0.340f                             // Noise Gain
        ,0.0005f                            // Noise Frequency
    },
    { // Mountain_Biome
         0//blocks["Grass_Top"]["index"]            // Surface Block
        ,1//blocks["Dirt_Top"]["index"]             // Subsurface Block
        ,true                               // Hot Temperature
        ,false                              // Allow Water
        ,500                                // Tree Frequency
        ,20.540f                            // Noise Gain
        ,0.0009f                            // Noise Frequency
    },
    { // Swamp_Biome
         0//blocks["Grass_Top"]["index"]            // Surface Block
        ,1//blocks["Dirt_Top"]["index"]             // Subsurface Block
        ,true                               // Hot Temperature
        ,true                               // Allow Water
        ,500                                // Tree Frequency
        ,1.540f                             // Noise Gain
        ,0.0005f                            // Noise Frequency
    }
};


