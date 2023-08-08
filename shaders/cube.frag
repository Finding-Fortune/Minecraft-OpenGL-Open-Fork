#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Get our texture coordinate mappings
in vec2 TexCoord;
// Import normal vectors
in vec3 Normal;
// Import the vertex's position
in vec3 VertexPosition; 
// Texture ID (block type)
in float TexID;
// Block face
in float BlockFaceID;
// Color
in float AmbientOcclusionIntensity;
// Fog
in float FogIntensity;

vec3 skyColour = vec3(0.54f, 0.81f, 0.94f);

// Get the texture unit from when we bound our texture
uniform sampler2DArray ourTexture;	
// Gets the color of the light
uniform vec4 lightColor;
// Gets the light's position
uniform vec3 lightPosition;
// Gets the position of the camera
uniform vec3 cameraPosition;

void main()
{
	// Create texture
	vec4 Texture = texture(ourTexture, vec3(TexCoord.xy, TexID));

	// Normalize the normal vector
	vec3 normal = normalize(Normal);
	// Get the light direction vector by subtracting
	// current position by the light position and normalizing it
	vec3 lightDirection = normalize(lightPosition - VertexPosition);

	// Diffuse lighting
	// Get the dot product of the light direction and the normal
	// Get max of 0 because this can output negative values we don't want
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// Ambient lighting
	float ambient = 0.85f;

	// Specular lighting
	float specularLight = 0.5f; // Maximum intensity of the specular light
	vec3 viewDirection = normalize(cameraPosition - VertexPosition);
	vec3 reflectionDirection = max(reflect(-lightDirection, normal), 0.0f);
	// Amount of specular amount looking at 
	// a certain direction with our camera
	float specularAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0), 16);
	float specular = specularAmount * specularLight;

	// Depending on the face lighting will be different
	// float faceLight = 0.0f;

	// switch (int(BlockFaceID)) {
	// 	case 0: // 0 for Back face
	// 		faceLight = 0.3f * 1.0f;
	// 		break;
	// 	case 1: // 1 for Front face
	// 		faceLight = 0.3f * 1.0f;
	// 		break;
	// 	case 2: // 2 for Left face
	// 		faceLight = 0.2f * 1.0f;
	// 		break;
	// 	case 3: // 3 for Right face
	// 		faceLight = 0.2f * 1.0f;
	// 		break;
	// 	case 4: // 4 for Top face
	// 		faceLight = 0.4f * 1.0f;
	// 		break;
	// 	case 5: // 5 for Bottom face
	// 		faceLight = 0.1f * 1.0f;
	// 		break;
	// }

	// Our final color with phong lightings
	// For some reason faceLight causes visual distortion on windows so getting rid of that
	vec4 FinalColor = Texture * lightColor * (min(diffuse + ambient + specular, 0.6f) + AmbientOcclusionIntensity + (0.05*BlockFaceID));// + faceLight + AmbientOcclusionIntensity);
	FinalColor = mix(vec4(skyColour, 1.0), FinalColor, FogIntensity);

	// Add transparency if needed
	// if(int(TexID) == 5) // water
	// {
	// 	FragColor = vec4(FinalColor.rgb, 0.4);
	// }
	// else if(int(TexID) == 4 || int(TexID) == 8) // Leaves or Ice
	// {
	// 	FragColor = vec4(FinalColor.rgb, 0.7);
	// }
	// else
	// {
		// FragColor = vec4(vec3(0.4*ambient), 1.0f);
		FragColor = vec4(FinalColor.rgb, 1.0);
	// }
}