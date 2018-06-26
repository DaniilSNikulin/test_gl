#version 430

layout( location = 0 ) in vec3 VertexPosition;

out vec3 WorldPosition;

uniform mat4 ModelMatrix;
uniform mat4 MVP;

void main()
{
    WorldPosition = vec3( ModelMatrix * vec4(VertexPosition,1.0) );
    gl_Position = MVP * vec4(VertexPosition,1.0);
}
