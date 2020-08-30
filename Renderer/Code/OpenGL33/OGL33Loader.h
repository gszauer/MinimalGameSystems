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

typedef unsigned int GLuint;
typedef int GLsizei;
typedef unsigned int GLenum;
typedef khronos_ssize_t GLsizeiptr;
typedef khronos_intptr_t GLintptr;
typedef int GLint;

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

#endif // !_H_OGL33LOADER_
