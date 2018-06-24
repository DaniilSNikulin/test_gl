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
private:
	GLSLProgram prog;

	int width, height;
	VBOTorus *torus;
	float angle;

	mat4 model;
	mat4 view;
	mat4 projection;

	void setMatrices();
	void compileAndLinkShader();

public:
	SceneADS();

	void initScene();
	void update( float t );
	void render();
	void resize(int, int);
};

#endif // SCENEADS_H
