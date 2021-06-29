#pragma once

#ifndef __UTILS__
#define __UTILS__

#include<stdio.h>
#include <algorithm>


#include <gl/glew.h>
#include <Console.h>


using namespace std;

/***********************************************************************************************/
template <typename T> string tostr(const T& t) {
	ostringstream os;
	os << t;
	return os.str();
}

inline void printGlError()
{
	GLenum err = glGetError();
	if (GLEW_OK != err)
	{
		cout << "GL ERROR:" << err << " => " << gluErrorString(err) << endl;
	}
}

#endif