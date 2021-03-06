#include "scenebasic.hpp"
#include "glutils.hpp"

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using std::ifstream;
using std::string;


SceneBasic::SceneBasic() : width(0), height(0)
{}

void SceneBasic::initScene()
{
  compileAndLinkShader();

  /////////////////// Create the VBO ////////////////////
  float positionData[] = {
      -0.8f, -0.8f, 0.0f,
        0.8f, -0.8f, 0.0f,
        0.0f,  0.8f, 0.0f };
  float colorData[] = {
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f };

  // Create and populate the buffer objects
  GLuint vboHandles[2];
  glGenBuffers(2, vboHandles);
  GLuint positionBufferHandle = vboHandles[0];
  GLuint colorBufferHandle = vboHandles[1];

  glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), positionData, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
  glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), colorData, GL_STATIC_DRAW);

  // Create and set-up the vertex array object
  glGenVertexArrays( 1, &vaoHandle );
  glBindVertexArray(vaoHandle);

  glEnableVertexAttribArray(0);  // Vertex position
  glEnableVertexAttribArray(1);  // Vertex color

  glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );

  glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL );
  glBindVertexArray(0);
}


void SceneBasic::update( float t )
{}

void SceneBasic::render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(vaoHandle);
    glDrawArrays(GL_TRIANGLES, 0, 3 );
    glBindVertexArray(0);
}

void SceneBasic::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
}


void SceneBasic::compileAndLinkShader()
{
  using namespace std;
  try {
    prog.compileShader("shader/basic.vert");
    prog.compileShader("shader/basic.frag");
    prog.link();
    prog.use();
  }
  catch (GLSLProgramException & e) {
    cerr << e.what() << endl;
    exit(EXIT_FAILURE);
  }
}