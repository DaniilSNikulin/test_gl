#include "vbopoints.hpp"

#include <string>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/polar_coordinates.hpp>

using namespace std;

VBOPoints::VBOPoints()
{
  // Create and populate the buffer objects
  GLuint vboHandles[1];
  glGenBuffers(1, vboHandles);
  vbo = vboHandles[0];

  //string fileName = "myfile.txt";
  //FILE * pFile = fopen(fileName.c_str(), "r");
  //float x, y, z;
  //while (true) {
  //  int code = fscanf(pFile, "%f, %f, %f\n", &x, &y, &z);
  //  if (code == 0 || code == EOF) {
  //    break;
  //  }
  //  vertex.push_back(x);
  //  vertex.push_back(y);
  //  vertex.push_back(z);
  //}
}


void VBOPoints::initVertex(std::vector<float> && source)
{
  vertex = source;

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

  // Create and set-up the vertex array object
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glEnableVertexAttribArray(0);  // Vertex position
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
  glBindVertexArray(0);
}


void VBOPoints::render()
{
  glBindVertexArray(vao);
  glDrawArrays(GL_POINTS, 0, vertex.size());
  glBindVertexArray(0);
}
