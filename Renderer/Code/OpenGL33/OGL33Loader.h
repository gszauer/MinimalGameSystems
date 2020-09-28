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
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;

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

typedef void (* PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;

typedef void (* PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;

typedef void (* PFNGLACTIVETEXTUREPROC)(GLenum texture);
extern PFNGLACTIVETEXTUREPROC glActiveTexture;

typedef void (* PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
extern PFNGLUNIFORM1IPROC glUniform1i;

typedef void (* PFNGLVERTEXATTRIBIPOINTERPROC)(GLuint index, GLint size, GLenum type, GLsizei stride, const void* pointer);
extern PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer;

typedef void (* PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
extern PFNGLUNIFORM1FPROC glUniform1f;

typedef void (* PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat* value);
extern PFNGLUNIFORM2FVPROC glUniform2fv;

typedef void (* PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat* value);
extern PFNGLUNIFORM3FVPROC glUniform3fv;

typedef void (* PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat* value);
extern PFNGLUNIFORM4FVPROC glUniform4fv;

typedef void (* PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint* value);
extern PFNGLUNIFORM2IVPROC glUniform2iv;

typedef void (* PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint* value);
extern PFNGLUNIFORM3IVPROC glUniform3iv;

typedef void (* PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint* value);
extern PFNGLUNIFORM4IVPROC glUniform4iv;

typedef void (* PFNGLUNIFORM2UIPROC)(GLint location, GLuint v0, GLuint v1);
extern PFNGLUNIFORM2UIPROC glUniform2ui;

typedef void (* PFNGLUNIFORM2UIVPROC)(GLint location, GLsizei count, const GLuint* value);
extern PFNGLUNIFORM2UIVPROC glUniform2uiv;

typedef void (* PFNGLUNIFORM3UIVPROC)(GLint location, GLsizei count, const GLuint* value);
extern PFNGLUNIFORM3UIVPROC glUniform3uiv;

typedef void (* PFNGLUNIFORM4UIVPROC)(GLint location, GLsizei count, const GLuint* value);
extern PFNGLUNIFORM4UIVPROC glUniform4uiv;

typedef void (* PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX2FVPROC glUniformMatrix2fv;

typedef void (* PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv;

typedef void (* PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;

typedef void (* PFNGLUNIFORMMATRIX2X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX2X3FVPROC glUniformMatrix2x3fv;

typedef void (* PFNGLUNIFORMMATRIX3X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX3X2FVPROC glUniformMatrix3x2fv;

typedef void (* PFNGLUNIFORMMATRIX2X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX2X4FVPROC glUniformMatrix2x4fv;

typedef void (* PFNGLUNIFORMMATRIX4X2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX4X2FVPROC glUniformMatrix4x2fv;

typedef void (* PFNGLUNIFORMMATRIX3X4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX3X4FVPROC glUniformMatrix3x4fv;

typedef void (* PFNGLUNIFORMMATRIX4X3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
extern PFNGLUNIFORMMATRIX4X3FVPROC glUniformMatrix4x3fv;

typedef void (* PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;

typedef void (*PFNGLCLEARPROC)(GLbitfield mask);
extern PFNGLCLEARPROC glClear;

typedef void (* PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
extern PFNGLPOLYGONMODEPROC glPolygonMode;

typedef void (* PFNGLCULLFACEPROC)(GLenum mode);
extern PFNGLCULLFACEPROC glCullFace;

typedef void (* PFNGLDISABLEPROC)(GLenum cap);
extern PFNGLDISABLEPROC glDisable;

typedef void (* PFNGLENABLEPROC)(GLenum cap);
extern PFNGLENABLEPROC glEnable;

typedef void (* PFNGLFRONTFACEPROC)(GLenum mode);
extern PFNGLFRONTFACEPROC glFrontFace;

typedef void (* PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
extern PFNGLSCISSORPROC glScissor;

typedef void (* PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
extern PFNGLBLENDFUNCPROC glBlendFunc;

typedef void (* PFNGLLINEWIDTHPROC)(GLfloat width);
extern PFNGLLINEWIDTHPROC glLineWidth;

typedef void (* PFNGLPOINTSIZEPROC)(GLfloat size);
extern PFNGLPOINTSIZEPROC glPointSize;

typedef void (* PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
extern PFNGLVIEWPORTPROC glViewport;

typedef void (* PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const void* indices, GLsizei instancecount);
extern PFNGLDRAWELEMENTSINSTANCEDPROC glDrawElementsInstanced;

typedef void (* PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
extern PFNGLDRAWARRAYSINSTANCEDPROC glDrawArraysInstanced;

typedef GLboolean(* PFNGLISENABLEDPROC)(GLenum cap);
extern PFNGLISENABLEDPROC glIsEnabled;

typedef void (* PFNGLGETINTEGERVPROC)(GLenum pname, GLint* data);
extern PFNGLGETINTEGERVPROC glGetIntegerv;

typedef void (* PFNGLGETFLOATVPROC)(GLenum pname, GLfloat* data);
extern PFNGLGETFLOATVPROC glGetFloatv;

typedef const char* (__stdcall* PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
extern PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT;

typedef int (__stdcall* PFNWGLSWAPINTERVALEXTPROC) (int interval);
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

typedef int (__stdcall* PFNWGLGETSWAPINTERVALEXTPROC) (void);
extern PFNWGLGETSWAPINTERVALEXTPROC wglGetSwapIntervalEXT;

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
#define GL_RGB32F 0x8815
#define GL_RGBA32F 0x8814
#define GL_R32F 0x822E
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_FRAMEBUFFER 0x8D40
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_DOUBLE 0x140A
#define GL_FALSE 0
#define GL_TRUE 1
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
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_CULL_FACE 0x0B44
#define GL_SCISSOR_TEST 0x0C11
#define GL_BLEND 0x0BE2
#define GL_VIEWPORT 0x0BA2
#define GL_POLYGON_MODE 0x0B40
#define GL_CULL_FACE_MODE 0x0B45
#define GL_FRONT_FACE 0x0B46
#define GL_SCISSOR_BOX 0x0C10
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_LINE_WIDTH 0x0B21
#define GL_POINT_SIZE 0x0B11

#endif // !_H_OGL33LOADER_
