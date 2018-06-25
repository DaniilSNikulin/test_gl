#include "sceneshadowmap.hpp"
#include "glutils.hpp"

#include <cstdio>
#include <iostream>
#include <cstdlib>
using std::cerr;
using std::endl;

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>



SceneShadowMap::SceneShadowMap()
 : width(800)
 , height(600)
 , shadowMapWidth(512)
 , shadowMapHeight(512)
 , lightPos(0.0f, 1.65*6.25f, 1.65*8.5f)
 , cameraPos(11.5f, 7.0f, .0f)
 , actor(ActorType::CAMERA)
{}


void SceneShadowMap::initScene()
{
  compileAndLinkShader();

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

  glEnable(GL_DEPTH_TEST);

  teapot = new VBOTeapot(14, mat4(1.0f));
  plane = new VBOPlane(40.0f, 40.0f, 2, 2);
  float scale = 2.0f;
  torus = new VBOTorus(0.7f * scale, 0.3f * scale, 50, 50);

  // Set up the framebuffer object
  setupFBO();

  GLuint programHandle = prog.getHandle();
  pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
  pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

  shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
                    vec4(0.0f, 0.5f, 0.0f, 0.0f),
                    vec4(0.0f, 0.0f, 0.5f, 0.0f),
                    vec4(0.5f, 0.5f, 0.5f, 1.0f)
                    );

  lightFrustum = new Frustum(Projection::PERSPECTIVE);
  prog.setUniform("Light.Intensity", vec3(0.85f));
  prog.setUniform("ShadowMap", 0);
}


void SceneShadowMap::update(float t)
{}


void SceneShadowMap::render()
{
  lightFrustum->orient(lightPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
  lightFrustum->setPerspective(50.0f, 1.0f, 1.0f, 50.0f);
  lightPV = shadowBias * lightFrustum->getProjectionMatrix() * lightFrustum->getViewMatrix();

  prog.use();
  // Pass 1 (shadow map generation)
  view = lightFrustum->getViewMatrix();
  projection = lightFrustum->getProjectionMatrix();
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
  glClear(GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, shadowMapWidth, shadowMapHeight);
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  drawScene();
  glFlush();
  glFinish();
  //spitOutDepthBuffer(); // This is just used to get an image of the depth buffer

  // Pass 2 (render)
  view = glm::lookAt(cameraPos, vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));
  prog.setUniform("Light.Position", view * vec4(lightFrustum->getOrigin(), 1.0f));
  projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.1f, 100.0f);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, width, height);
  glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
  glDisable(GL_CULL_FACE);
  drawScene();
}


vec3 up_down_translate(vec3 pos, float degree) {
  static float back(1);
  float length = glm::length(pos);
  vec3 polarPos= glm::polar(pos);
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


void SceneShadowMap::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
  printf("key code = %d (%c)\n", key, (char)key);
  vec3 & pos = actor == ActorType::CAMERA ? cameraPos : lightPos;
  if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_H)
    pos = glm::rotate(glm::radians(-5.f), vec3(0.f, 1.f, 0.f)) * vec4(pos, 1.0);
  else if (key == GLFW_KEY_L)
    pos = glm::rotate(glm::radians(5.f), vec3(0.f, 1.f, 0.f)) * vec4(pos, 1.0);
  else if (key == GLFW_KEY_J)
    pos = up_down_translate(pos, -2);
  else if (key == GLFW_KEY_K)
    pos = up_down_translate(pos, 2);
  else if (key == GLFW_KEY_UP)
    pos *= 0.95;
  else if (key == GLFW_KEY_DOWN)
    pos *= 1.05;
  else if (key == GLFW_KEY_C && action == GLFW_PRESS)
    actor = static_cast<ActorType>((static_cast<int>(actor)+1) % 2);
  printf("%d\n", (int)actor);
}


// ------ private block ------- //


void SceneShadowMap::drawScene()
{
  vec3 color = vec3(0.5f, 0.7f, 0.3f);
  prog.setUniform("Material.Ka", color * 0.05f);
  prog.setUniform("Material.Kd", color);
  prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
  prog.setUniform("Material.Shininess", 150.0f);
  model = mat4(1.0f);
  model *= glm::translate(vec3(-2.0f, 0.0f, 0.0f));
  model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
  setMatrices();
  teapot->render();

  color = vec3(0.7f, 0.5f, 0.3f);
  prog.setUniform("Material.Ka", color * 0.05f);
  prog.setUniform("Material.Kd", color);
  prog.setUniform("Material.Ks", vec3(0.9f, 0.9f, 0.9f));
  prog.setUniform("Material.Shininess", 150.0f);
  model = mat4(1.0f);
  model *= glm::translate(vec3(0.0f, 2.0f, 5.0f));
  model *= glm::rotate(glm::radians(-45.0f), vec3(1.0f, 0.0f, 0.0f));
  setMatrices();
  torus->render();

  prog.setUniform("Material.Kd", 0.25f, 0.25f, 0.25f);
  prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
  prog.setUniform("Material.Ka", 0.05f, 0.05f, 0.05f);
  prog.setUniform("Material.Shininess", 1.0f);
  model = mat4(1.0f);
  model *= glm::translate(vec3(0.0f, 0.0f, 0.0f));
  setMatrices();
  plane->render();
}


void SceneShadowMap::spitOutDepthBuffer()
{
  int size = shadowMapWidth * shadowMapHeight;
  float * buffer = new float[size];
  unsigned char * imgBuffer = new unsigned char[size * 4];

  glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, buffer);

  for (int i = 0; i < shadowMapHeight; i++) {
    for (int j = 0; j < shadowMapWidth; j++)
    {
      int imgIdx = 4 * ((i*shadowMapWidth) + j);
      int bufIdx = ((shadowMapHeight - i - 1) * shadowMapWidth) + j;

      // This is just to make a more visible image.  Scale so that
      // the range (minVal, 1.0) maps to (0.0, 1.0).  This probably should
      // be tweaked for different light configurations.
      float minVal = 0.88f;
      float scale = (buffer[bufIdx] - minVal) / (1.0f - minVal);
      unsigned char val = (unsigned char)(scale * 255);
      imgBuffer[imgIdx] = val;
      imgBuffer[imgIdx + 1] = val;
      imgBuffer[imgIdx + 2] = val;
      imgBuffer[imgIdx + 3] = 0xff;
    }
  }

  delete[] buffer;
  delete[] imgBuffer;
  exit(1);
}

void SceneShadowMap::setupFBO()
{
  GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
  // The depth buffer texture
  GLuint depthTex;
  glGenTextures(1, &depthTex);
  glBindTexture(GL_TEXTURE_2D, depthTex);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

  // Assign the depth buffer texture to texture channel 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthTex);

  // Create and set up the FBO
  glGenFramebuffers(1, &shadowFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D, depthTex, 0);

  GLenum drawBuffers[] = { GL_NONE };
  glDrawBuffers(1, drawBuffers);

  GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (result == GL_FRAMEBUFFER_COMPLETE) {
    printf("Framebuffer is complete.\n");
  }
  else {
    printf("Framebuffer is not complete.\n");
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SceneShadowMap::setMatrices()
{
  mat4 mv = view * model;
  prog.setUniform("ModelViewMatrix", mv);
  prog.setUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
  prog.setUniform("MVP", projection * mv);
  prog.setUniform("ShadowMatrix", lightPV * model);
}

void SceneShadowMap::resize(int w, int h)
{
  glViewport(0, 0, w, h);
  width = w;
  height = h;
}

void SceneShadowMap::compileAndLinkShader()
{
  try {
    prog.compileShader("shader/shadowmap.vert", GLSLShaderType::VERTEX);
    prog.compileShader("shader/shadowmap.frag", GLSLShaderType::FRAGMENT);
    prog.link();
    prog.use();
  }
  catch (GLSLProgramException &e) {
    cerr << e.what() << endl;
    exit(EXIT_FAILURE);
  }
}
