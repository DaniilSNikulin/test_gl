#pragma once

#include "scene.hpp"
#include "glslprogram.hpp"

#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;

#include <vector>


class ScenePoints :  public Scene
{
public:
  ScenePoints();

  void initScene();
  void update(float t);
  void render();
  void resize(int, int);

  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) override;

private:
  GLSLProgram prog;

  int width, height;
  unsigned int vaoHandle;
  std::vector<float> vertex;
  std::vector<float> normal;

  vec3 cameraPosition;
  vec4 worldLight;

  mat4 model;
  mat4 view;
  mat4 projection;

  void setMatrices();
  void compileAndLinkShader();
};
