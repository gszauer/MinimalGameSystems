#ifndef _H_OGL33LOADER_
#define _H_OGL33LOADER_

// TODO: Make sure these are not double defed in other loaders

#ifdef _WIN64
typedef signed   long long int khronos_intptr_t;
typedef unsigned long long int khronos_uintptr_t;
typedef signed   long long int khronos_ssize_t;
typedef unsigned long long int khronos_usize_t;
#else
typedef signed   long  int     khronos_intptr_t;
typedef unsigned long  int     khronos_uintptr_t;
typedef signed   long  int     khronos_ssize_t;
typedef unsigned long  int     khronos_usize_t;
#endif

typedef __int64                 khronos_int64_t;
typedef unsigned __int64        khronos_uint64_t;

typedef float GLfloat;

typedef unsigned int GLuint;
typedef khronos_uint64_t GLuint64;
typedef int GLsizei;
typedef unsigned int GLenum;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
typedef int GLint;
typedef char GLchar;
typedef khronos_int64_t GLint64;

// TODO: namespace ALL of thse to avoid collisions with actual types

typedef void (*PFNGLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
extern PFNGLGENBUFFERSPROC glGenBuffers;

typedef void (*PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint* buffers);
extern PFNGLDELETEBUFFERSPROC glDeleteBuffers;

typedef void (*PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
extern PFNGLBUFFERDATAPROC glBufferData;

typedef void (*PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
extern PFNGLBINDBUFFERPROC glBindBuffer;

typedef void (*PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const void* data);
extern PFNGLBUFFERSUBDATAPROC glBufferSubData;

typedef void (*PFNGLGENTEXTURESPROC)(GLsizei n, GLuint* textures);
extern PFNGLGENTEXTURESPROC glGenTextures;

typedef void (*PFNGLDELETETEXTURESPROC)(GLsizei n, const GLuint* textures);
extern PFNGLDELETETEXTURESPROC glDeleteTextures;

typedef void (*PFNGLGENERATEMIPMAPPROC)(GLenum target);
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

typedef void (*PFNGLBINDTEXTUREPROC)(GLenum target, GLuint texture);
extern PFNGLBINDTEXTUREPROC glBindTexture;

typedef void (*PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
extern PFNGLTEXIMAGE2DPROC glTexImage2D;

typedef void (*PFNGLTEXSUBIMAGE2DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);
extern PFNGLTEXSUBIMAGE2DPROC glTexSubImage2D;

typedef void (*PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
extern PFNGLTEXPARAMETERIPROC glTexParameteri;

typedef void (*PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
extern PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;

typedef void (*PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);
extern PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;

typedef void (*PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
extern PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;

typedef GLenum(*PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
extern PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;

typedef GLuint(* PFNGLCREATEPROGRAMPROC)(void);
extern PFNGLCREATEPROGRAMPROC glCreateProgram;

typedef GLuint(* PFNGLCREATESHADERPROC)(GLenum type);
extern PFNGLCREATESHADERPROC glCreateShader;

typedef void (* PFNGLDELETEPROGRAMPROC)(GLuint program);
extern PFNGLDELETEPROGRAMPROC glDeleteProgram;

typedef void (* PFNGLUSEPROGRAMPROC)(GLuint program);
extern PFNGLUSEPROGRAMPROC glUseProgram;

typedef void (* PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;

typedef void (* PFNGLGETACTIVEATTRIBPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
extern PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;

typedef GLint(* PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar* name);
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;

typedef void (* PFNGLGETACTIVEUNIFORMPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei* length, GLint* size, GLenum* type, GLchar* name);
extern PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;

typedef void (* PFNGLDELETESHADERPROC)(GLuint shader);
extern PFNGLDELETESHADERPROC glDeleteShader;

typedef void (* PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
extern PFNGLSHADERSOURCEPROC glShaderSource;

typedef void (* PFNGLCOMPILESHADERPROC)(GLuint shader);
extern PFNGLCOMPILESHADERPROC glCompileShader;

typedef void (* PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
extern PFNGLGETSHADERIVPROC glGetShaderiv;

typedef void (* PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;

typedef void (* PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
extern PFNGLATTACHSHADERPROC glAttachShader;

typedef void (* PFNGLLINKPROGRAMPROC)(GLuint program);
extern PFNGLLINKPROGRAMPROC glLinkProgram;

typedef void (* PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
extern PFNGLGETPROGRAMIVPROC glGetProgramiv;

typedef void (* PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;

typedef void (* PFNGLGENQUERIESPROC)(GLsizei n, GLuint* ids);
extern PFNGLGENQUERIESPROC glGenQueries;

typedef void (* PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint* ids);
extern PFNGLDELETEQUERIESPROC glDeleteQueries;

typedef void (* PFNGLQUERYCOUNTERPROC)(GLuint id, GLenum target);
extern PFNGLQUERYCOUNTERPROC glQueryCounter;

typedef void (* PFNGLGETQUERYOBJECTIVPROC)(GLuint id, GLenum pname, GLint* params);
extern PFNGLGETQUERYOBJECTIVPROC glGetQueryObjectiv;

typedef void (* PFNGLGETQUERYOBJECTUI64VPROC)(GLuint id, GLenum pname, GLuint64* params);
extern PFNGLGETQUERYOBJECTUI64VPROC glGetQueryObjectui64v;

typedef void (* PFNGLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
extern PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;

typedef void (* PFNGLDELETEVERTEXARRAYSPROC)(GLsizei n, const GLuint* arrays);
extern PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;

typedef void (* PFNGLBINDVERTEXARRAYPROC)(GLuint array);
extern PFNGLBINDVERTEXARRAYPROC glBindVertexArray;

typedef void (* PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
extern PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;

#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_STREAM_COPY 0x88E2
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STATIC_COPY 0x88E6
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_DYNAMIC_COPY 0x88EA
#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_TEXTURE_2D 0x0DE1
#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_STENCIL 0x84F9
#define GL_RGB 0x1907
#define GL_RGBA 0x1908
#define GL_RED 0x1903
#define GL_UNSIGNED_BYTE 0x1401
#define GL_FLOAT 0x1406
#define GL_RGB32F 0x8815
#define GL_RGBA32F 0x8814
#define GL_R32F 0x822E
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_FRAMEBUFFER 0x8D40
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_DOUBLE 0x140A

#define GL_FLOAT 0x1406
#define GL_INT 0x1404
#define GL_UNSIGNED_INT 0x1405

#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52

#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55

#define GL_UNSIGNED_INT_VEC2 0x8DC6
#define GL_UNSIGNED_INT_VEC3 0x8DC7
#define GL_UNSIGNED_INT_VEC4 0x8DC8

#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C

#define GL_FLOAT_MAT2x3 0x8B65
#define GL_FLOAT_MAT2x4 0x8B66
#define GL_FLOAT_MAT3x2 0x8B67
#define GL_FLOAT_MAT3x4 0x8B68
#define GL_FLOAT_MAT4x2 0x8B69
#define GL_FLOAT_MAT4x3 0x8B6A

#define GL_SAMPLER_1D 0x8B5D
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_3D 0x8B5F

#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_LINK_STATUS 0x8B82
#define GL_TIMESTAMP 0x8E28
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_QUERY_RESULT 0x8866
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#define GL_COLOR_ATTACHMENT3 0x8CE3
#define GL_COLOR_ATTACHMENT4 0x8CE4
#define GL_COLOR_ATTACHMENT5 0x8CE5
#define GL_COLOR_ATTACHMENT6 0x8CE6
#define GL_COLOR_ATTACHMENT7 0x8CE7
#define GL_COLOR_ATTACHMENT8 0x8CE8
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A

#endif // !_H_OGL33LOADER_
