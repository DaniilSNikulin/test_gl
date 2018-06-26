#pragma once

#include "glslprogram.hpp"

#include <glm/glm.hpp>
using glm::mat4;
using glm::vec3;

#include <vector>


class VBOPoints
{
public:
  VBOPoints();

  void render();
  void initVertex(std::vector<float> && source);

private:
  unsigned int vao;
  unsigned int vbo;
  std::vector<float> vertex;
};
