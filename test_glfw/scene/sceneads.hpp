#ifndef SCENEADS_H
#define SCENEADS_H

#include "scene.hpp"
#include "glslprogram.hpp"
#include "vbo/vbotorus.hpp"

#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;


class SceneADS : public Scene
{
public:
	SceneADS();

	void initScene()      override;
  void update(float t)  override;
  void render()         override;
  void resize(int, int) override;

  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) override;

private:
  GLSLProgram prog;

  int width, height;
  VBOTorus *torus;

  vec3 cameraPosition;
  vec4 worldLight;

  mat4 model;
  mat4 view;
  mat4 projection;

  void setMatrices();
  void compileAndLinkShader();
};

#endif // SCENEADS_H
