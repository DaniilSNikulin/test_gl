#include "scenetwoview.hpp"
#include "glutils.hpp"

#include <cstdio>
#include <cstdlib>
#include <iostream>
using std::endl;
using std::cerr;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/polar_coordinates.hpp>


SceneTwoView::SceneTwoView()
  : publisherPos(0.0f, 1.65*6.25f, 1.65*8.5f)
  , subscriberPos(11.5f, 7.0f, 3.0f)
  , actor(ViewerType::SUBSCRIBER)
  , dirtyFrame(true)
  , width(800)
  , height(600)
  , projection(glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f))
{}


void SceneTwoView::initScene()
{
  compileAndLinkShader();

  glEnable(GL_DEPTH_TEST);

  float c = 1.5f;
  torus = new VBOTorus(0.7f * c, 0.3f * c, 50, 50);
  teapot = new VBOTeapot(14, mat4(1.0));
  points = new VBOPoints();

  glBindVertexArray(0);
  setupFBO();
}

void SceneTwoView::createGBufTex(GLenum texUnit, GLenum format, GLuint &texid) {
  glActiveTexture(texUnit);
  glGenTextures(1, &texid);
  glBindTexture(GL_TEXTURE_2D, texid);
  glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void SceneTwoView::setupFBO()
{
  // Create and bind the FBO
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  // The depth buffer
  glGenRenderbuffers(1, &depthBuf);
  glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

  // Create the textures for position, normal and color
  createGBufTex(GL_TEXTURE3, GL_RGB32F, worldPosTex);  // World Position

  // Attach the textures to the framebuffer
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, worldPosTex, 0);

  GLenum drawBuffers[] = { GL_NONE, GL_COLOR_ATTACHMENT0};
  glDrawBuffers(2, drawBuffers);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneTwoView::update(float t)
{
}

bool eq(float a, float b) {
  float diff = abs(a - b);
  if (diff < 1e-5) {
    return true;
  }
  return false;
}

void SceneTwoView::render()
{
  pass1();

  if (dirtyFrame) {
    dirtyFrame = false;
    int size = width * height;
    float * buffer = new float[size * 3];
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindTexture(GL_TEXTURE_2D, worldPosTex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, buffer);

    std::vector<float> vertex;
    vertex.reserve(size);
    for (int i = 0; i < size; ++i) {
      float x = buffer[3 * i];
      float y = buffer[3 * i + 1];
      float z = buffer[3 * i + 2];
      if (!eq(x, 0.3f) || !eq(y, 0.3f) || !eq(z, 0.3f)) {
        vertex.push_back(x);
        vertex.push_back(y);
        vertex.push_back(z);
      }
    }
    delete[] buffer;
    points->initVertex(std::move(vertex));
  }

  pass2();
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


void SceneTwoView::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  printf("key code = %d (%c)\n", key, (char)key);
  vec3 & pos = actor == ViewerType::PUBLISHER ? publisherPos : subscriberPos;
  int flag = 0;
  if (actor == ViewerType::PUBLISHER) {
    flag = 1;
  }

  if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_H) {
    pos = glm::rotate(glm::radians(-5.f), vec3(0.f, 1.f, 0.f)) * vec4(pos, 1.0);
    ++flag;
  }
  else if (key == GLFW_KEY_L) {
    pos = glm::rotate(glm::radians(5.f), vec3(0.f, 1.f, 0.f)) * vec4(pos, 1.0);
    ++flag;
  } else if (key == GLFW_KEY_J) {
    pos = up_down_translate(pos, -2);
    ++flag;
  }
  else if (key == GLFW_KEY_K) {
    pos = up_down_translate(pos, 2);
    ++flag;
  }
  else if (key == GLFW_KEY_UP) {
    pos *= 0.95;
    ++flag;
  }
  else if (key == GLFW_KEY_DOWN) {
    pos *= 1.05;
    ++flag;
  }
  else if (key == GLFW_KEY_C && action == GLFW_PRESS)
    actor = static_cast<ViewerType>((static_cast<int>(actor)+1) % 2);

  if (flag >= 2) {
    dirtyFrame = true;
  }
}


void SceneTwoView::pass1()
{
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  view = glm::lookAt(publisherPos, vec3(0.0f, 0.0f, .0f), vec3(0.0f, 1.0f, 0.0f));

  model = mat4(1.0f);
  model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
  model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
  setMatrices(publisher);
  teapot->render();

  model = mat4(1.0f);
  model *= glm::translate(vec3(1.0f, 1.0f, 3.0f));
  model *= glm::rotate(glm::radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
  setMatrices(publisher);
  torus->render();

  glFinish();
}

void SceneTwoView::pass2()
{
  // Revert to default framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindVertexArray(0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  view = glm::lookAt(subscriberPos, vec3(0.0f, 0.0f, .0f), vec3(0.0f, 1.0f, 0.0f));
  model = mat4(1.0);
  subscriber.use();
  setMatrices(subscriber);

  // Render the quad
  points->render();
  publisher.use();
}

void SceneTwoView::setMatrices(GLSLProgram & prog)
{
  mat4 mv = view * model;
  prog.setUniform("ModelMatrix", model);
  prog.setUniform("MVP", projection * mv);
}

void SceneTwoView::resize(int w, int h)
{
  glViewport(0, 0, w, h);
  width = w;
  height = h;
  projection = glm::perspective(glm::radians(60.0f), (float)width / height, 0.3f, 100.0f);
}

void SceneTwoView::compileAndLinkShader()
{
  try {
    publisher.compileShader("shader/sourcepoints.vert", GLSLShaderType::VERTEX);
    publisher.compileShader("shader/sourcepoints.frag", GLSLShaderType::FRAGMENT);
    publisher.link();
    publisher.use();

    subscriber.compileShader("shader/points.vert", GLSLShaderType::VERTEX);
    subscriber.compileShader("shader/points.frag", GLSLShaderType::FRAGMENT);
    subscriber.link();
  }
  catch (GLSLProgramException &e) {
    cerr << e.what() << endl;
    exit(EXIT_FAILURE);
  }
}
