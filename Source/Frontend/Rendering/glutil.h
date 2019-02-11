#ifndef _GLUTIL_H_
#define _GLUTIL_H_

#include "Debug.h"

#define GL_NO_CALL

#define GL(CALL) \
{ \
	CALL; \
	GLenum err = glGetError(); \
	if (err) \
	{ \
		Debug::Print("[GL]: %s; at %s:%u: %s (0x%04x)\n", #CALL, __FILE__, __LINE__, \
		glewGetErrorString(err), err); \
		Debug::Break(); \
	} \
} 

#endif