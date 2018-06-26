#ifndef SCENETWOVIEW_H
#define SCENETWOVIEW_H

#include "scene.hpp"
#include "glslprogram.hpp"
#include "vbo/vbotorus.hpp"
#include "vbo/vboteapot.hpp"
#include "vbo/vbopoints.hpp"

#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;

enum class ViewerType {
  PUBLISHER = 0,
  SUBSCRIBER = 1
};

class SceneTwoView : public Scene
{
public:
  SceneTwoView();

  void initScene();
  void update(float t);
  void render();
  void resize(int, int);
  virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) override;

private:
  GLSLProgram publisher, subscriber;

  vec3 publisherPos;
  vec3 subscriberPos;
  ViewerType actor;
  bool dirtyFrame;

  int width, height;
  GLuint fbo;
  GLuint depthBuf, worldPosTex;

  VBOTorus *torus;
  VBOTeapot *teapot;
  VBOPoints *points;

  mat4 model;
  mat4 view;
  mat4 projection;

  void setMatrices(GLSLProgram & prog);
  void compileAndLinkShader();
  void setupFBO();
  void createGBufTex(GLenum, GLenum, GLuint &);
  void pass1();
  void pass2();
};

#endif // SceneTwoView_H
