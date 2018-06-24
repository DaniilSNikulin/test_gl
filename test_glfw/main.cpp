#include "scene.hpp"
#include "scenebasic.hpp"
#include "glutils.hpp"

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <iostream>

// Function prototypes
GLFWwindow* init();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
  GLFWwindow* window = init();
	if (!window) {
		return -1;
	}

  Scene * scene = new SceneBasic();
  GLUtils::dumpGLInfo();
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  scene->initScene();
  
  while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
  {
    GLUtils::checkForOpenGLError(__FILE__, __LINE__);
    scene->update(float(glfwGetTime()));
    scene->render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}


GLFWwindow* init()
{
	if (!glfwInit()) {
		std::cerr << "bad init glfw\n!";
		return nullptr;
	}

	// Select OpenGL 4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	glfwWindowHint(GLFW_DEPTH_BITS, 16);

	std::string title = "Chapter 01 -- ";
	GLFWwindow* window = glfwCreateWindow(500, 500, title.c_str(), NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, key_callback);

	glewExperimental = true;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "Failed to init GLEW !" << std::endl << glewGetErrorString(err) << std::endl;
		glfwTerminate();
		return nullptr;
	}
	
	return window;
}


// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  std::cout << key << std::endl;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
}
