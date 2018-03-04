#ifndef _STUFF
#define _STUFF

#include <vector>
#include <algorithm>
#include "mtxlib.h"
#include "vector2d.h"
#include "Exception.h"

using namespace std;

#pragma once

class Stuff
{
	friend class Simulator;
protected:
	vector2d< float >		Heights;
	vector2d< int >			Colors;
	vector< vector3 >		StuffColors;
	vector2d< vector3 >		vertices;
	vector2d< vector3 >		normals;
	vector2d< vector3 >		colors;

	vector3				Size;

	int					poczX;
	int					poczY;
	int					step;

	float				Epsilon;
	float				BottonHeight;

public:
	int					x,y;

	float getEpsilon() const { return Epsilon; }
	void Init(float _Epsilon, vector3 _Size, float _BottomHeight, vector< vector3 > _StuffColors, int _step);
	void Recreate() { Init(Epsilon, Size, BottonHeight, StuffColors, step); }
	void Update(TRECT & rect);
	void Draw();
	Stuff(void);
	~Stuff(void);
};

#endif