#version 330 core

// Positions/Coordinates
layout (location = 0) in uint packedVertexData;

// Output textures to the Fragment Shader
out vec2 TexCoord;
// Output normal vectors to the Fragment Shader
out vec3 Normal;
// Current position of our vertices 
// used to calculate direction of light
out vec3 VertexPosition;
// Output texture ID to the Fragment Shader
out float TexID;
// Block face
out float BlockFaceID;
// Ambient occlusion intensity
out float AmbientOcclusionIntensity;
// Fog intensity
out float FogIntensity;

// Fog variables
const float fogDensity = 0.001;
const float fogGradient = 4.0;

// Imports the camera matrix from the main function
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

// Uniform for chunk offset
uniform vec3 chunkOffset;

// The size of our blocks
uniform float blockSize;

vec2 texCoords[4] = vec2[4](
    vec2(0.0f, 0.0f), // Bottom left
    vec2(1.0f, 1.0f), // Top right
    vec2(1.0f, 0.0f), // Bottom right
    vec2(0.0f, 1.0f)  // Top left
);

vec3 backFaceNormals[4] = vec3[4](
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f, -1.0f)
);

vec3 frontFaceNormals[4] = vec3[4](
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 1.0f)
);

vec3 leftFaceNormals[4] = vec3[4](
	vec3(-1.0f, 0.0f, 0.0f),
	vec3(-1.0f, 0.0f, 0.0f),
	vec3( 0.0f, 0.0f, 1.0f),
	vec3(-1.0f, 0.0f, 0.0f)
);

vec3 rightFaceNormals[4] = vec3[4](
	vec3(0.0f, 0.0f,  1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f,  1.0f)
);

vec3 topFaceNormals[4] = vec3[4](
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f,  1.0f),
	vec3(0.0f, 0.0f, -1.0f),
	vec3(0.0f, 0.0f,  1.0f)
);

vec3 bottomFaceNormals[4] = vec3[4](
	vec3( 0.0f, 0.0f,  1.0f),
	vec3( 0.0f, 0.0f, -1.0f),
	vec3( 0.0f, 0.0f,  1.0f),
	vec3(-1.0f, 0.0f,  0.0f)
);


vec3 backFacePositions[4] = vec3[4](
	vec3(0.0f, 0.0f, 0.0f), // Bottom left
	vec3(1.0f, 1.0f, 0.0f), // Top Right
	vec3(1.0f, 0.0f, 0.0f), // Top Left
	vec3(0.0f, 1.0f, 0.0f)  // Bottom Right
);

vec3 frontFacePositions[4] = vec3[4](
	vec3(0.0f, 0.0f, 1.0f), // Bottom left
	vec3(1.0f, 1.0f, 1.0f), // Top Right
	vec3(0.0f, 1.0f, 1.0f), // Top Left
	vec3(1.0f, 0.0f, 1.0f)  // Bottom Right
);

vec3 leftFacePositions[4] = vec3[4](
	vec3(0.0f, 1.0f, 1.0f), // Top right
	vec3(0.0f, 0.0f, 0.0f), // Bottom left
	vec3(0.0f, 0.0f, 1.0f), // Top left
	vec3(0.0f, 1.0f, 0.0f)  // Bottom right
);

vec3 rightFacePositions[4] = vec3[4](
	vec3(1.0f, 1.0f, 1.0f), // Top right
	vec3(1.0f, 0.0f, 0.0f), // Bottom left
	vec3(1.0f, 1.0f, 0.0f), // Bottom right
	vec3(1.0f, 0.0f, 1.0f)  // Top left
);

vec3 topFacePositions[4] = vec3[4](
	vec3(0.0f, 1.0f, 0.0f),
	vec3(1.0f, 1.0f, 1.0f),
	vec3(1.0f, 1.0f, 0.0f),
	vec3(0.0f, 1.0f, 1.0f)
);

vec3 bottomFacePositions[4] = vec3[4](
	vec3(0.0f, 0.0f, 1.0f),
	vec3(1.0f, 0.0f, 0.0f),
	vec3(1.0f, 0.0f, 1.0f),
	vec3(0.0f, 0.0f, 0.0f)
);

// This is the order that vertices must be rendered
// because of the winding order required for backface culling
int indices[6] = int[6]( 0, 1, 2, 1, 0, 3 );

void main()
{
	// Unpack vertex data
	uint x         = (packedVertexData) 	  & 63u; // 6 bits, x position in chunk
	uint y         = (packedVertexData >> 6)  & 63u; // 6 bits, y position in chunk
	uint z         = (packedVertexData >> 12) & 63u; // 6 bits, z position in chunk
	uint aFaceID   = (packedVertexData >> 18) & 7u;	 // 3 bits, what face in the cube this is
	uint aVertexID = (packedVertexData >> 21) & 7u;	 // 3 bits, which of 6 face vertices is this
	uint aTexID    = (packedVertexData >> 24) & 31u; // 5 bits, which texture to use
	uint aoEnabled = (packedVertexData >> 29) & 1u;  // 1 bit, whether ambient occlusion is toggled

	// Adjust the offset for this chunk by the block size
	vec3 aPos = vec3(x, y, z);				

	// This is the final position for the vertex in world coordinates
	VertexPosition = aPos * blockSize + chunkOffset;

	// Set our normal vectors and adjust vertex position
	switch(aFaceID) {
		case 0u: // 0 is Index for Back face
			Normal = backFaceNormals[indices[aVertexID]] * blockSize;
			VertexPosition += backFacePositions[indices[aVertexID]] * blockSize;
			TexCoord = texCoords[indices[aVertexID]] * blockSize;
			break;
		case 1u: // 1 is Index for Front face
			Normal = frontFaceNormals[indices[aVertexID]] * blockSize;
			VertexPosition += frontFacePositions[indices[aVertexID]] * blockSize;
			TexCoord = texCoords[indices[aVertexID]] * blockSize;
			break;
		case 2u: // 2 is Index for Left face
			Normal = leftFaceNormals[indices[aVertexID]] * blockSize;
			VertexPosition += leftFacePositions[indices[aVertexID]] * blockSize;
			TexCoord = texCoords[indices[aVertexID]] * blockSize;
			break;
		case 3u: // 3 is Index for Right face
			Normal = rightFaceNormals[indices[aVertexID]] * blockSize;
			VertexPosition += rightFacePositions[indices[aVertexID]] * blockSize;
			TexCoord = texCoords[indices[aVertexID]] * blockSize;
			break;
		case 4u: // 4 is Index for Top face
			Normal = topFaceNormals[indices[aVertexID]] * blockSize;
			VertexPosition += topFacePositions[indices[aVertexID]] * blockSize;
			TexCoord = texCoords[indices[aVertexID]] * blockSize;
			break;
		case 5u: // 5 is Index for Bottom face
			Normal = bottomFaceNormals[indices[aVertexID]] * blockSize;
			VertexPosition += bottomFacePositions[indices[aVertexID]] * blockSize;
			TexCoord = texCoords[indices[aVertexID]] * blockSize;
			break;
	}

	// Outputs the positions/coordinates of all vertices
	gl_Position = projectionMatrix * viewMatrix * vec4(VertexPosition, 1.0f);
	// Set our normal vectors from vertex data, adjust it by vertex position
	Normal += VertexPosition; 
	// Set our texture ID from vertex data
	TexID = aTexID; 
	// Block face
	BlockFaceID = aFaceID;
	if(aoEnabled == 1u) // This is the working code for top face, uncomment later
		AmbientOcclusionIntensity = -0.6f;
	else
		AmbientOcclusionIntensity = 0.0f;

	// Calculate Fog
	vec4 positionRelativeToCamera = viewMatrix * vec4(VertexPosition, 1.0f);
	float distance = length(positionRelativeToCamera.xyz);
	FogIntensity = exp(-pow((distance*fogDensity), fogGradient));
	FogIntensity = clamp(FogIntensity, 0.0, 1.0); // Clamp fog visibility between 0 and 1
}