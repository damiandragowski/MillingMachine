#ifndef _DRILL
#define _DRILL
#include "Exception.h"
#include "mtxlib.h"
#pragma once
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>

class Drill
{
private:
	int toolType;
	class GLUquadric * quadrics;
	float Radius;
	float Hight;
	vector3 position;
	bool visible;
public:
	void setVisible() { visible = true; }
	void unsetVisible() { visible = false; }
	Drill(void);
	~Drill(void);
	vector3 getPosition() const { return position; }
	int		getToolType() const { return toolType; }
	float getHeight() const { return Hight; }
	float getRadius() const { return Radius; }
	int   countCylindrict(float x, float y, float & z, vector3 & p0, vector3 & p1);
	int   countSphere(float x, float y, float & z, vector3 & p0, vector3 & p1);
	void changeDrill(float _Radius, int _toolType, float _Hight);
	void moveDrill(vector3 newPosition);
	void Draw();
};

#endif