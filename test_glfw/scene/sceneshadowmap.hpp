#ifndef SCENESHADOWMAP_H
#define SCENESHADOWMAP_H

#include "scene.hpp"
#include "glslprogram.hpp"
#include "vbo/vboplane.hpp"
#include "vbo/vbocube.hpp"
#include "vbo/vbotorus.hpp"
#include "vbo/vboteapot.hpp"
#include "frustum.hpp"


#include <glm/glm.hpp>
using glm::mat4;
using glm::vec4;
using glm::vec3;


enum class ActorType {
  PUBLISHER = 0,
  SUBSCRIBER = 1
};


class SceneShadowMap : public Scene
{
public:
  SceneShadowMap();

  void initScene();
  void update(float t);
  void render();
  void resize(int, int);
  virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) override;

private:
  GLSLProgram prog;
  GLuint shadowFBO, pass1Index, pass2Index;

  VBOTeapot *teapot;
  VBOPlane *plane;
  VBOTorus *torus;

  int width, height;
  const int shadowMapWidth, shadowMapHeight;

  vec3 lightPos;
  vec3 cameraPos;
  vec3 viewerPos;
  ActorType actor;

  mat4 model, view, projection;
  mat4 lightPV;
  mat4 shadowBias;

  Frustum *lightFrustum;

  void setMatrices();
  void compileAndLinkShader();
  void setupFBO();
  void drawScene();
  void spitOutDepthBuffer();
};

#endif // SCENESHADOWMAP_H
