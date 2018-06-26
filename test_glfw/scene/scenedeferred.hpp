#ifndef SCENEDEFERRED_H
#define SCENEDEFERRED_H

#include "scene.hpp"
#include "glslprogram.hpp"
#include "vbo/vboplane.hpp"
#include "vbo/vbotorus.hpp"
#include "vbo/vboteapot.hpp"

#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;

class SceneDeferred : public Scene
{
public:
  SceneDeferred();

  void initScene();
  void update(float t);
  void render();
  void resize(int, int);

private:
  GLSLProgram prog;

  int width, height;
  GLuint deferredFBO;
  GLuint quad, pass1Index, pass2Index;
  GLuint depthBuf, posTex, worldPosTex, normTex, colorTex;

  VBOPlane *plane;
  VBOTorus *torus;
  VBOTeapot *teapot;

  mat4 model;
  mat4 view;
  mat4 projection;
  float angle, tPrev, rotSpeed;

  void setMatrices();
  void compileAndLinkShader();
  void setupFBO();
  void createGBufTex(GLenum, GLenum, GLuint &);
  void pass1();
  void pass2();
};

#endif // SCENEDEFERRED_H
