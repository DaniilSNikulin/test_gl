#ifndef SCENE_H
#define SCENE_H

#include "glutils.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

class Scene
{
public:
  Scene() : m_animate(true) { }
	
	/**
	   Load textures, initialize shaders, etc.
	*/
	virtual void initScene() = 0;

	/**
	   This is called prior to every frame.  Use this
	   to update your animation.
	*/
	virtual void update( float t ) = 0;
  
  virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    printf("key code = %d (%c)\n", key, (char)key);
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
  }

	/**
	   Draw your scene.
	*/
	virtual void render() = 0;

	/**
	   Called when screen is resized
	*/
	virtual void resize(int, int) = 0;
    
	void animate( bool value ) { m_animate = value; }
  bool animating() const { return m_animate; }
    
protected:
	bool m_animate;
};

#endif // SCENE_H
