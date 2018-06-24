#ifndef GLUTILS_H
#define GLUTILS_H

#define GLEW_STATIC
#include <GL/glew.h>

#ifdef _WIN32
#include <windows.h>
#else
#define APIENTRY
#endif

namespace GLUtils
{
    int checkForOpenGLError(const char *, int);
    
    void dumpGLInfo(bool dumpExtensions = false);
    
    void APIENTRY debugCallback( GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar * msg, const void * param );
}

#endif // GLUTILS_H
