#ifndef _EXCEPTION
#define _EXCEPTION

#include <windows.h>		// Header File For Windows

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
using namespace std;

#pragma once

class Exception
{
	string msg;
public:
	Exception(string _msg):msg(_msg) { }
	virtual ~Exception(void);
	void ShowMessage() { MessageBox(0, msg.c_str(), TEXT("Exception !!!"), 0); }
};

class NumException:public Exception
{
public:
	NumException():Exception("Parse Error") { }
};

struct ColorType{
	enum _ColorType { cNormal=0, cError=1, cDrill=2 };
};

struct ToolType{
	static enum _ToolType { Sphere=0, Cylinder=1 };
};


struct TRECT
{
	TRECT() {}
	TRECT(int xmin, int xmax, int ymin, int ymax) {
		indXMin = xmin;
		indXMax = xmax;
		indYMin = ymin;
		indYMax = ymax;
	}
	int indXMin, indXMax, indYMin, indYMax;
};

float parseFloat(string str);

int SplitString(const string& input, const string& delimiter, vector<string>& results);


#endif