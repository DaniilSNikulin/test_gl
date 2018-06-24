#ifndef SCENEBASIC_H
#define SCENEBASIC_H

#include "scene.hpp"
#include "glslprogram.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

class SceneBasic : public Scene
{
public:
    SceneBasic();

    void initScene() override;
    void update( float t ) override;
    void render() override;
    void resize(int, int) override;

private:
  GLSLProgram prog;
  int width, height;
  GLuint vaoHandle;

  void compileAndLinkShader();
};

#endif // SCENEBASIC_H
