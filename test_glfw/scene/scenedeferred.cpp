#include "scenedeferred.hpp"
#include "glutils.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::endl;
using std::cerr;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>

SceneDeferred::SceneDeferred()
  : width(800)
  , height(600)
  , angle(0.0f)
  , tPrev(0.0f)
  , rotSpeed(glm::pi<float>() / 8.0f)
{}


void SceneDeferred::initScene()
{
  compileAndLinkShader();

  glEnable(GL_DEPTH_TEST);

  float c = 1.5f;
  plane = new VBOPlane(50.0f, 50.0f, 1, 1);
  torus = new VBOTorus(0.7f * c, 0.3f * c, 50, 50);
  teapot = new VBOTeapot(14, mat4(1.0));

  angle = glm::pi<float>() / 2.0f;

  // Array for quad
  GLfloat verts[] = {
    -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f
  };
  GLfloat tc[] = {
    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
  };

  // Set up the buffers
  unsigned int handle[2];
  glGenBuffers(2, handle);

  glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), verts, GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
  glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), tc, GL_STATIC_DRAW);

  // Set up the vertex array object
  glGenVertexArrays(1, &quad);
  glBindVertexArray(quad);

  glBindBuffer(GL_ARRAY_BUFFER, handle[0]);
  glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
  glEnableVertexAttribArray(0);  // Vertex position

  glBindBuffer(GL_ARRAY_BUFFER, handle[1]);
  glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0)));
  glEnableVertexAttribArray(2);  // Texture coordinates

  glBindVertexArray(0);

  setupFBO();

  // Set up the subroutine indexes
  GLuint programHandle = prog.getHandle();
  pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass1");
  pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "pass2");

  prog.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));
}

void SceneDeferred::createGBufTex(GLenum texUnit, GLenum format, GLuint &texid) {
  glActiveTexture(texUnit);
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void SceneDeferred::setupFBO()
{
  // Create and bind the FBO
  glGenFramebuffers(1, &deferredFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);

  // The depth buffer
  glGenRenderbuffers(1, &depthBuf);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  // Create the textures for position, normal and color
  createGBufTex(GL_TEXTURE0, GL_RGB32F, posTex);  // Position
  createGBufTex(GL_TEXTURE1, GL_RGB32F, normTex); // Normal
  createGBufTex(GL_TEXTURE2, GL_RGB8, colorTex);  // Color
  createGBufTex(GL_TEXTURE3, GL_RGB32F, worldPosTex);  // World Position

  // Attach the textures to the framebuffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, posTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, colorTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, worldPosTex, 0);

  GLenum drawBuffers[] = {
    GL_NONE,
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3 };
  glDrawBuffers(5, drawBuffers);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneDeferred::update(float t)
{
  float deltaT = t - tPrev;
  if (tPrev == 0.0f) deltaT = 0.0f;
  tPrev = t;

  angle += rotSpeed * deltaT;
  if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
}


static bool eq(float a, float b) {
  float diff = abs(a - b);
  if (diff < 1e-5) {
    return true;
  }
  return false;
}

void SceneDeferred::render()
{
  pass1();

//  static int cnt = 0;
//  cnt++;
//  if (cnt == 10) {
//    int size = width * height;
//    printf("%d, %d\n", width, height);
//    float * buffer = new float[size * 3];
//    glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
//    glBindTexture(GL_TEXTURE_2D, posTex);
//    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, buffer);
//
//    FILE * pFile = fopen("myfile.csv", "w");
//    fprintf(pFile, "x,y,z\n");
//    for (int i = 0; i < size; ++i) {
//      float x = buffer[3 * i];
//      float y = buffer[3 * i + 1];
//      float z = buffer[3 * i + 2];
//      if (!eq(x, 0.3) || !eq(y, 0.3) || !eq(z, 0.3)) {
////        fprintf(pFile, "%7.3f, %7.3f, %7.3f\n", x, y, z);
//      }
//    }
//    fclose(pFile);
//  }

  pass2();
}

void SceneDeferred::pass1()
{
  glBindFramebuffer(GL_FRAMEBUFFER, deferredFBO);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);

  view = glm::lookAt(vec3(7.0f * cos(angle), 4.0f, 7.0f * sin(angle)), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
  projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);

  prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
  prog.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);

  model = mat4(1.0f);
  model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
  model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
  setMatrices();
  teapot->render();

  //prog.setUniform("Material.Kd", 0.4f, 0.4f, 0.4f);
  //model = mat4(1.0f);
  //model *= glm::translate(vec3(0.0f, -0.75f, 0.0f));
  //setMatrices();
  //plane->render();

  prog.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
  prog.setUniform("Material.Kd", 0.9f, 0.5f, 0.2f);
  model = mat4(1.0f);
  model *= glm::translate(vec3(1.0f, 1.0f, 3.0f));
  model *= glm::rotate(glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
  setMatrices();
  torus->render();

  glFinish();
}

void SceneDeferred::pass2()
{
  // Revert to default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//  glDisable(GL_DEPTH_TEST);

  view = mat4(1.0);
  model = mat4(1.0);
  projection = mat4(1.0);
  setMatrices();

  // Render the quad
  glBindVertexArray(quad);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SceneDeferred::setMatrices()
{
  mat4 mv = view * model;
  prog.setUniform("ModelMatrix", model);
  prog.setUniform("ModelViewMatrix", mv);
  prog.setUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
  prog.setUniform("MVP", projection * mv);
}

void SceneDeferred::resize(int w, int h)
{
  glViewport(0, 0, w, h);
  width = w;
  height = h;
}

void SceneDeferred::compileAndLinkShader()
{
  try {
    prog.compileShader("shader/deferred.vert", GLSLShaderType::VERTEX);
    prog.compileShader("shader/deferred.frag", GLSLShaderType::FRAGMENT);
    prog.link();
    prog.use();
  }
  catch (GLSLProgramException &e) {
    cerr << e.what() << endl;
    exit(EXIT_FAILURE);
  }
}
