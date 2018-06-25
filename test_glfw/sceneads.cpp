#include "sceneads.hpp"
#include "glutils.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::cerr;
using std::endl;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


SceneADS::SceneADS()
  : cameraPosition(0.0f, 0.0f, 2.0f)
  , worldLight(5.0f, 5.0f, 2.0f, 1.0f)
{}

void SceneADS::initScene()
{
	compileAndLinkShader();

	glEnable(GL_DEPTH_TEST);
	torus = new VBOTorus(0.7f, 0.3f, 50, 50);

	model = mat4(1.0f);
	projection = mat4(1.0f);
  view = glm::lookAt(cameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

	prog.setUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	prog.setUniform("Light.Ld", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Light.Position", view * worldLight );
	prog.setUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
	prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
	prog.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
	prog.setUniform("Light.Ls", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Material.Shininess", 100.0f);
}

void SceneADS::update( float t ) { }

void SceneADS::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = mat4(1.0f);
  view = glm::lookAt(cameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
  
	setMatrices();
	torus->render();
}

void SceneADS::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  printf("key code = %d (%c)\n", key, (char)key);
  if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_H )
    cameraPosition = glm::rotate(glm::radians(-5.f), vec3(0.f, 1.f, 0.f)) * vec4(cameraPosition, 1.0);
  else if (key == GLFW_KEY_L)
    cameraPosition = glm::rotate(glm::radians(5.f), vec3(0.f, 1.f, 0.f)) * vec4(cameraPosition, 1.0);
  else if (key == GLFW_KEY_J)
    cameraPosition = glm::rotate(glm::radians(-5.f), vec3(1.f, 0.f, 0.f)) * vec4(cameraPosition, 1.0);
  else if (key == GLFW_KEY_K)
    cameraPosition = glm::rotate(glm::radians(5.f), vec3(1.f, 0.f, 0.f)) * vec4(cameraPosition, 1.0);
  else if (key == GLFW_KEY_UP)
    cameraPosition *= 0.95;
  else if (key == GLFW_KEY_DOWN)
    cameraPosition *= 1.05;
}


void SceneADS::setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix", mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
	prog.setUniform("MVP", projection * mv);
  prog.setUniform("Light.Position", view * worldLight);
}

void SceneADS::resize(int w, int h)
{
	glViewport(0,0,w,h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(70.0f), (float)w/h, 0.3f, 100.0f);
}

void SceneADS::compileAndLinkShader()
{
	try {
		prog.compileShader("shader/phong.vert");
		prog.compileShader("shader/phong.frag");
		prog.link();
		prog.use();
	} catch(GLSLProgramException & e) {
		cerr << e.what() << endl;
		exit( EXIT_FAILURE );
	}
}
