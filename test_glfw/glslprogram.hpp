#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H

#define GLEW_STATIC
#include <GL/glew.h>

#include <stdexcept>
#include <string>
#include <map>
using std::string;

#include <glm/glm.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;


struct GLSLProgramException : public std::runtime_error {
  GLSLProgramException(string msg) : std::runtime_error(msg) {}
};

enum class GLSLShaderType {
  VERTEX = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER,
  GEOMETRY = GL_GEOMETRY_SHADER,
  TESS_CONTROL = GL_TESS_CONTROL_SHADER,
  TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
  COMPUTE = GL_COMPUTE_SHADER
};


class GLSLProgram
{
public:
  GLSLProgram();
  ~GLSLProgram();

  GLSLProgram(const GLSLProgram &) = delete;
  GLSLProgram(GLSLProgram &&) = delete;
  GLSLProgram & operator=(const GLSLProgram &) = delete;
  GLSLProgram & operator=(GLSLProgram &&) = delete;

  void   compileShader(const char *fileName);

  void   link();
  void   validate();
  void   use();

  int    getHandle() const;
  bool   isLinked() const;

  void   bindAttribLocation(GLuint location, const char * name);
  void   bindFragDataLocation(GLuint location, const char * name);

  void   setUniform(const char *name, float x, float y, float z);
  void   setUniform(const char *name, const vec2 & v);
  void   setUniform(const char *name, const vec3 & v);
  void   setUniform(const char *name, const vec4 & v);
  void   setUniform(const char *name, const mat4 & m);
  void   setUniform(const char *name, const mat3 & m);
  void   setUniform(const char *name, float val);
  void   setUniform(const char *name, int val);
  void   setUniform(const char *name, bool val);
  void   setUniform(const char *name, GLuint val);

  void   printActiveUniforms();
  void   printActiveUniformBlocks();
  void   printActiveAttribs();

private:
  int  handle;
  bool linked;
  std::map<string, int> uniformLocations;

  void compileShader(const char * fileName, GLSLShaderType type);
  void compileShader(const string & source, GLSLShaderType type,
                     const char *fileName = nullptr);

  GLint  getUniformLocation(const char * name);

  static const char * getTypeString(GLenum type);
  static bool fileExists(const string & fileName);
  static GLSLShaderType getShaderType(const char * filename);
  static string getExtension(const char * fileName);
  static string readFile(const char * fileName);
};

#endif // GLSLPROGRAM_H
