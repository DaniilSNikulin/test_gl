#include "scenepoints.hpp"

#include <string>
#include <iostream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/polar_coordinates.hpp>

using namespace std;

ScenePoints::ScenePoints()
  : width(300)
  , height(250)
  , cameraPosition(3.*7.0f, 3 * 4.0f, 7.0f)
  , worldLight(5.0f, 5.0f, 2.0f, 1.0f)
{
  vertex.reserve(400 * 300);

  string fileName = "myfile.txt";
  FILE * pFile = fopen(fileName.c_str(), "r");
  float x, y, z;
  while (true) {
    int code = fscanf(pFile, "%f, %f, %f\n", &x, &y, &z);
    if (code == 0 || code == EOF) {
      break;
    }
    vertex.push_back(x);
    vertex.push_back(y);
    vertex.push_back(z);
  }
}

void ScenePoints::initScene()
{
  compileAndLinkShader();

  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, width, height);

  {
    // Create and populate the buffer objects
    GLuint vboHandles[2];
    glGenBuffers(1, vboHandles);
    GLuint positionBufferHandle = vboHandles[0];

    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], GL_STATIC_DRAW);

    // Create and set-up the vertex array object
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glEnableVertexAttribArray(0);  // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
    glBindVertexArray(0);
  }

  model = mat4(1.0f);
  projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
  view = glm::lookAt(cameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

  prog.setUniform("Material.Ka", 0.9f, 0.5f, 0.3f);
  prog.setUniform("Light.La", 0.4f, 0.4f, 0.4f);
}


void ScenePoints::update(float t) { }

void ScenePoints::render()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  view = glm::lookAt(cameraPosition, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

  setMatrices();

  glBindVertexArray(vaoHandle);
  glDrawArrays(GL_POINTS, 0, vertex.size());
  glBindVertexArray(0);
}

static vec3 up_down_translate(vec3 pos, float degree) {
  static float back(1);
  float length = glm::length(pos);
  vec3 polarPos = glm::polar(pos);
  polarPos.x += glm::radians(degree*back);
  if (glm::half_pi<float>() < abs(polarPos.x)) {
    back *= -1;
    polarPos.x -= glm::radians(5.f);
    polarPos.y = std::fmod(polarPos.y + glm::pi<float>(), glm::two_pi<float>());
  }
  pos = glm::euclidean(vec2(polarPos.x, polarPos.y));
  pos *= length;
  return pos;
}

void ScenePoints::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_H)
    cameraPosition = glm::rotate(glm::radians(-5.f), vec3(0.f, 1.f, 0.f)) * vec4(cameraPosition, 1.0);
  else if (key == GLFW_KEY_L)
    cameraPosition = glm::rotate(glm::radians(5.f), vec3(0.f, 1.f, 0.f)) * vec4(cameraPosition, 1.0);
  else if (key == GLFW_KEY_J)
    cameraPosition = up_down_translate(cameraPosition, -2);
  else if (key == GLFW_KEY_K)
    cameraPosition = up_down_translate(cameraPosition, 2);
  else if (key == GLFW_KEY_UP)
    cameraPosition *= 0.95;
  else if (key == GLFW_KEY_DOWN)
    cameraPosition *= 1.05;
}


void ScenePoints::setMatrices()
{
  mat4 mv = view * model;
//  prog.setUniform("ModelViewMatrix", mv);
  prog.setUniform("MVP", projection * mv);
}

void ScenePoints::resize(int w, int h)
{
  glViewport(0, 0, w, h);
  width = w;
  height = h;
  projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void ScenePoints::compileAndLinkShader()
{
  using namespace std;
  try {
    prog.compileShader("shader/points.vert");
    prog.compileShader("shader/points.frag");
    prog.link();
    prog.use();
  }
  catch (GLSLProgramException & e) {
    cerr << e.what() << endl;
    exit(EXIT_FAILURE);
  }
}
