#version 410

layout (location = 0) in vec3 VertexPosition;

out vec3 LightIntensity;

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
};
uniform MaterialInfo Material;

struct LightInfo {
  vec3 La;       // Ambient light intensity
};
uniform LightInfo Light;


uniform mat4 MVP;


void main()
{
    LightIntensity = Light.La * Material.Ka;
    gl_Position = MVP * vec4(VertexPosition,1.0);
}
