#include "Sky.hpp"
#include "WorldConstants.hpp"

#include <iostream>



Sky::Sky()
{
    
}



void Sky::SetSkyColor()
{
    // Specify the color of the background
    glClearColor(World::skyColor.x, World::skyColor.y, World::skyColor.z, World::skyColor.w);
    // Clean the back buffer and depth buffer 
    // and assign the new color to back buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


