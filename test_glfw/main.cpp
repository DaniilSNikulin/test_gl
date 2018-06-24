#include "scene.hpp"
#include "scenebasic.hpp"
#include "sceneads.hpp"
#include "glutils.hpp"

#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>

// Function prototypes
GLFWwindow* init(int, int);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
  const int width = 800;
  const int height = 600;
  GLFWwindow* window = init(width, height);
	if (!window) {
		return -1;
	}

  Scene * scene = new SceneADS();
  GLUtils::dumpGLInfo();
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glDebugMessageCallback(GLUtils::debugCallback, NULL);
  glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
  glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0,
                       GL_DEBUG_SEVERITY_NOTIFICATION, -1, "Start debugging");
  scene->initScene();
  scene->resize(width, height);
  
  while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
  {
    GLUtils::checkForOpenGLError(__FILE__, __LINE__);
    scene->update(float(glfwGetTime()));
    scene->render();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}


GLFWwindow* init(int width, int height)
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

//	glfwWindowHint(GLFW_DEPTH_BITS, 16);

	std::string title = "Chapter 02 -- ";
	GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
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
