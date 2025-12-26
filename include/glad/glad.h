/**
 * =============================================================================
 * GLAD - OpenGL Loader (Minimal Header)
 * =============================================================================
 * This is a simplified GLAD header for educational purposes.
 * In a production environment, you would generate this from:
 * https://glad.dav1d.de/ with OpenGL 3.3 Core Profile
 * 
 * GLAD loads OpenGL function pointers at runtime, which is necessary because
 * OpenGL is a specification, and the actual implementations are provided by
 * GPU drivers. The function addresses vary between systems and drivers.
 * =============================================================================
 */

#ifndef GLAD_H
#define GLAD_H

#ifdef __cplusplus
extern "C" {
#endif

// Platform detection for proper calling conventions
#if defined(_WIN32) && !defined(__CYGWIN__)
    #define APIENTRY __stdcall
#else
    #define APIENTRY
#endif

#ifndef APIENTRYP
    #define APIENTRYP APIENTRY *
#endif

#ifndef GLAPIENTRY
    #define GLAPIENTRY APIENTRY
#endif

#ifndef GLAPI
    #define GLAPI extern
#endif

// Include KHR platform header for type definitions
#include <KHR/khrplatform.h>

// =============================================================================
// OpenGL Type Definitions
// =============================================================================
// These types ensure consistent sizes across platforms
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef void GLvoid;
typedef khronos_int8_t GLbyte;
typedef khronos_uint8_t GLubyte;
typedef khronos_int16_t GLshort;
typedef khronos_uint16_t GLushort;
typedef int GLint;
typedef unsigned int GLuint;
typedef khronos_int32_t GLclampx;
typedef int GLsizei;
typedef khronos_float_t GLfloat;
typedef khronos_float_t GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void *GLeglClientBufferEXT;
typedef void *GLeglImageOES;
typedef char GLchar;
typedef char GLcharARB;
typedef khronos_intptr_t GLintptr;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_int64_t GLint64;
typedef khronos_uint64_t GLuint64;

// =============================================================================
// OpenGL Constants
// =============================================================================
// These are the most commonly used OpenGL constants

// Boolean values
#define GL_FALSE 0
#define GL_TRUE 1

// Data types
#define GL_BYTE 0x1400
#define GL_UNSIGNED_BYTE 0x1401
#define GL_SHORT 0x1402
#define GL_UNSIGNED_SHORT 0x1403
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405
#define GL_FLOAT 0x1406

// Primitive types (how vertices are interpreted)
#define GL_POINTS 0x0000
#define GL_LINES 0x0001
#define GL_LINE_LOOP 0x0002
#define GL_LINE_STRIP 0x0003
#define GL_TRIANGLES 0x0004
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLE_FAN 0x0006

// Buffer bit masks (for glClear)
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000

// Enable/Disable capabilities
#define GL_DEPTH_TEST 0x0B71
#define GL_BLEND 0x0BE2
#define GL_CULL_FACE 0x0B44
#define GL_SCISSOR_TEST 0x0C11

// Blend functions
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303

// Face culling
#define GL_FRONT 0x0404
#define GL_BACK 0x0405
#define GL_FRONT_AND_BACK 0x0408
#define GL_CW 0x0900
#define GL_CCW 0x0901

// Depth functions
#define GL_NEVER 0x0200
#define GL_LESS 0x0201
#define GL_EQUAL 0x0202
#define GL_LEQUAL 0x0203
#define GL_GREATER 0x0204
#define GL_NOTEQUAL 0x0205
#define GL_GEQUAL 0x0206
#define GL_ALWAYS 0x0207

// Shader types
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

// Shader parameters
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84

// Buffer types
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893

// Buffer usage hints
#define GL_STREAM_DRAW 0x88E0
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8

// Texture targets
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_CUBE_MAP 0x8513

// Texture parameters
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_NEAREST 0x2600
#define GL_LINEAR 0x2601
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_REPEAT 0x2901
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_MIRRORED_REPEAT 0x8370

// Texture units
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3

// Pixel formats
#define GL_RED 0x1903
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1

// Error codes
#define GL_NO_ERROR 0
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_VALUE 0x0501
#define GL_INVALID_OPERATION 0x0502
#define GL_OUT_OF_MEMORY 0x0505

// =============================================================================
// GLAD Initialization
// =============================================================================
/**
 * Initialize GLAD and load all OpenGL function pointers.
 * 
 * @param load Function pointer to the loader function (use glfwGetProcAddress)
 * @return 1 on success, 0 on failure
 * 
 * Example usage:
 *   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
 *       // Failed to initialize GLAD
 *   }
 */
typedef void* (*GLADloadproc)(const char* name);
GLAPI int gladLoadGLLoader(GLADloadproc load);

// Version check
GLAPI int GLAD_GL_VERSION_3_3;

// =============================================================================
// OpenGL Function Pointers
// =============================================================================
// These are declared as extern so they can be defined in glad.c and used
// throughout the application. Each function pointer is loaded at runtime
// by gladLoadGLLoader.

// State management
typedef void (APIENTRYP PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (APIENTRYP PFNGLCLEARPROC)(GLbitfield mask);
typedef void (APIENTRYP PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLENABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLDISABLEPROC)(GLenum cap);
typedef void (APIENTRYP PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (APIENTRYP PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (APIENTRYP PFNGLCULLFACEPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLFRONTFACEPROC)(GLenum mode);
typedef void (APIENTRYP PFNGLDEPTHMASKPROC)(GLboolean flag);
typedef GLenum (APIENTRYP PFNGLGETERRORPROC)(void);
typedef const GLubyte* (APIENTRYP PFNGLGETSTRINGPROC)(GLenum name);

GLAPI PFNGLCLEARCOLORPROC glClearColor;
GLAPI PFNGLCLEARPROC glClear;
GLAPI PFNGLVIEWPORTPROC glViewport;
GLAPI PFNGLENABLEPROC glEnable;
GLAPI PFNGLDISABLEPROC glDisable;
GLAPI PFNGLBLENDFUNCPROC glBlendFunc;
GLAPI PFNGLDEPTHFUNCPROC glDepthFunc;
GLAPI PFNGLCULLFACEPROC glCullFace;
GLAPI PFNGLFRONTFACEPROC glFrontFace;
GLAPI PFNGLDEPTHMASKPROC glDepthMask;
GLAPI PFNGLGETERRORPROC glGetError;
GLAPI PFNGLGETSTRINGPROC glGetString;

// Shader functions
typedef GLuint (APIENTRYP PFNGLCREATESHADERPROC)(GLenum type);
typedef void (APIENTRYP PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const* string, const GLint* length);
typedef void (APIENTRYP PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef void (APIENTRYP PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
typedef void (APIENTRYP PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (APIENTRYP PFNGLDELETESHADERPROC)(GLuint shader);
typedef GLuint (APIENTRYP PFNGLCREATEPROGRAMPROC)(void);
typedef void (APIENTRYP PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (APIENTRYP PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
typedef void (APIENTRYP PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
typedef void (APIENTRYP PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (APIENTRYP PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);

GLAPI PFNGLCREATESHADERPROC glCreateShader;
GLAPI PFNGLSHADERSOURCEPROC glShaderSource;
GLAPI PFNGLCOMPILESHADERPROC glCompileShader;
GLAPI PFNGLGETSHADERIVPROC glGetShaderiv;
GLAPI PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
GLAPI PFNGLDELETESHADERPROC glDeleteShader;
GLAPI PFNGLCREATEPROGRAMPROC glCreateProgram;
GLAPI PFNGLATTACHSHADERPROC glAttachShader;
GLAPI PFNGLLINKPROGRAMPROC glLinkProgram;
GLAPI PFNGLGETPROGRAMIVPROC glGetProgramiv;
GLAPI PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
GLAPI PFNGLUSEPROGRAMPROC glUseProgram;
GLAPI PFNGLDELETEPROGRAMPROC glDeleteProgram;
GLAPI PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;

// Uniform functions (for passing data to shaders)
typedef void (APIENTRYP PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (APIENTRYP PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
typedef void (APIENTRYP PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

GLAPI PFNGLUNIFORM1IPROC glUniform1i;
GLAPI PFNGLUNIFORM1FPROC glUniform1f;
GLAPI PFNGLUNIFORM2FPROC glUniform2f;
GLAPI PFNGLUNIFORM3FPROC glUniform3f;
GLAPI PFNGLUNIFORM4FPROC glUniform4f;
GLAPI PFNGLUNIFORM3FVPROC glUniform3fv;
GLAPI PFNGLUNIFORM4FVPROC glUniform4fv;
GLAPI PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;
GLAPI PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

// Vertex Array Object (VAO) functions
typedef void (APIENTRYP PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
typedef void (APIENTRYP PFNGLBINDVERTEXARRAYPROC)(GLuint array);
typedef void (APIENTRYP PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);

GLAPI PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
GLAPI PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
GLAPI PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

// Buffer Object functions
typedef void (APIENTRYP PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
typedef void (APIENTRYP PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
typedef void (APIENTRYP PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);

GLAPI PFNGLGENBUFFERSPROC glGenBuffers;
GLAPI PFNGLBINDBUFFERPROC glBindBuffer;
GLAPI PFNGLBUFFERDATAPROC glBufferData;
GLAPI PFNGLBUFFERSUBDATAPROC glBufferSubData;
GLAPI PFNGLDELETEBUFFERSPROC glDeleteBuffers;

// Vertex attribute functions
typedef void (APIENTRYP PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
typedef void (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);

GLAPI PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
GLAPI PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
GLAPI PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

// Drawing functions
typedef void (APIENTRYP PFNGLDRAWARRAYSPROC)(GLenum mode, GLint first, GLsizei count);
typedef void (APIENTRYP PFNGLDRAWELEMENTSPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices);

GLAPI PFNGLDRAWARRAYSPROC glDrawArrays;
GLAPI PFNGLDRAWELEMENTSPROC glDrawElements;

// Texture functions
typedef void (APIENTRYP PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
typedef void (APIENTRYP PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
typedef void (APIENTRYP PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
typedef void (APIENTRYP PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (APIENTRYP PFNGLGENERATEMIPMAPPROC)(GLenum target);
typedef void (APIENTRYP PFNGLACTIVETEXTUREPROC)(GLenum texture);
typedef void (APIENTRYP PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);

GLAPI PFNGLGENTEXTURESPROC glGenTextures;
GLAPI PFNGLBINDTEXTUREPROC glBindTexture;
GLAPI PFNGLTEXIMAGE2DPROC glTexImage2D;
GLAPI PFNGLTEXPARAMETERIPROC glTexParameteri;
GLAPI PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
GLAPI PFNGLACTIVETEXTUREPROC glActiveTexture;
GLAPI PFNGLDELETETEXTURESPROC glDeleteTextures;

// Polygon mode (for wireframe rendering)
typedef void (APIENTRYP PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
GLAPI PFNGLPOLYGONMODEPROC glPolygonMode;

#define GL_LINE 0x1B01
#define GL_FILL 0x1B02

#ifdef __cplusplus
}
#endif

#endif // GLAD_H
