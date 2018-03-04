#include ".\drill.h"
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>
#include <math.h>
#include <algorithm>

using namespace std;

Drill::Drill(void):position(75.0f, 75.0f, 100.0f)
{
	toolType = ToolType::Sphere;
	Radius = 8.0f;
	quadrics = (GLUquadric *) gluNewQuadric();
	gluQuadricNormals(quadrics, GLU_SMOOTH);
}

Drill::~Drill(void)
{
	gluDeleteQuadric(quadrics);
}

void Drill::moveDrill(vector3 newPosition)
{
	position = newPosition;
}

void Drill::changeDrill(float _Radius, int _toolType, float _Hight)
{
	Radius = _Radius;
	toolType = _toolType;
	Hight = _Hight;
}

void Drill::Draw()
{
	if ( visible ) {
	glPushMatrix();
	glTranslatef(position.x, position.y, position.z);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 10);
	
	if( toolType == ToolType::Sphere)
	{
		/// Sphere color silver
		glColor3f(0.8984375f, 0.90625f, 0.9765625f);
		gluSphere(quadrics, Radius , 16, 16);
		// traslate cylinder and draw
		glTranslatef(0.0f, 0.0f, Radius / 2.0f);
		glColor3f(0.13671875f, 0.41796875f, 0.5546875f);
		gluCylinder(quadrics, Radius / 2.0,Radius / 2.0, 512.0, 16, 8);
	} else {
		// cylinder color
		glColor3f(0.8984375f, 0.90625f, 0.9765625f);
		gluCylinder(quadrics, Radius , Radius / 4.0, 17.0f, 16, 16);		
	
		glTranslatef(0.0f, 0.0f, Radius / 2.0f);
		glColor3f(0.13671875f, 0.41796875f, 0.5546875f);
		gluCylinder(quadrics, Radius /3.0f, Radius /3.0f, 1000.0, 16, 8);
	}
	glPopMatrix();
	}
}

int  Drill::countSphere(float x, float y, float & z, vector3 & p0, vector3 & p1)
{
	float r=Radius;
	float fx,fy,fz,b,c,sqr;
	float a;
	vector3 u = p1 - p0;
	u.normalize();
	a=u.z*u.z*u.x*u.x+u.z*u.z*u.y*u.y+(u.z*u.z-1)*(u.z*u.z-1);

	if(a==0.0f) {
			return 0;
	}
	
	fx=x-p0.x;
	fy=y-p0.y;
	
	b=2*u.z*(u.z*u.z+u.x*u.x+u.y*u.y-2)*(fx*u.x+fy*u.y);
	c=	fx*fx*(u.x*u.x-1)*(u.x*u.x-1)+
		fy*fy*u.x*u.x*u.y*u.y+
		2*fx*fy*(u.x*u.x-1)*u.y*u.x+
		fx*fx*u.x*u.x*u.y*u.y+
		fy*fy*(u.y*u.y-1)*(u.y*u.y-1)+
		2*fx*fy*u.x*u.y*(u.y*u.y-1)+
		fx*fx*u.z*u.z*u.x*u.x+
		fy*fy*u.y*u.y*u.z*u.z+
		2*fx*fy*u.z*u.z*u.x*u.y
		-r*r;
	sqr=b*b-4*a*c;
	if(sqr<0.0f) return 0;
	fz=(-b-sqrt(sqr))/(2*a);
	z=fz+p0.z;
	if((u.x*fx+u.y*fy+u.z*fz)<0.0||(u.x*fx+u.y*fy+u.z*fz) > ((vector3)(p1-p0)).length())
		return 0;
	return 1;
}

int  Drill::countCylindrict(float x, float y, float & z, vector3 & p0, vector3 & p1)
{
	float r=Radius;
	vector3 d = p1 - p0;
	float a=d.x*d.x+d.y*d.y;
	vector3 u = d;
	u.normalize();


	float b,sx,sy,delta,b2,s1,s2,za,zb;
	if(a==0.0f) {
		delta=r*r-(x-p0.x)*(x-p0.x)-(y-p0.y)*(y-p0.y);		//czy istnieje rozwiazanie na z?
		if(delta<0.0f) 
		return 0;
		z=min<float>(p0.z,p1.z);
		return 1;
	}
	sx=x-p1.x;
	sy=y-p1.y;
	float fx,fy;
	fx=x-p0.x;
	fy=y-p0.y;

	if (p0.z == p1.z) {
		if((u.x*fx+u.y*fy)< 0.0 || (u.x*fx+u.y*fy)*(u.x*fx+u.y*fy)>d.x*d.x+d.y*d.y )
			return 0;
		b=2*(sx*d.x+sy*d.y);
		b2=sy*d.x-sx*d.y;
		delta=4*(r*r*a-b2*b2);
		if(delta<0.0f) 
			return 0;
		z = p0.z;
		return 1;
	}
	
	b=2*(sx*d.x+sy*d.y);
	b2=sy*d.x-sx*d.y;
	delta=4*(r*r*a-b2*b2);
	if(delta<0.0f) 
		return 0;
	s1=(-b-sqrt(delta))/(2*a);
	s2=(-b+sqrt(delta))/(2*a);
	za=s1*(p0.z-p1.z)+p1.z;
	zb=s2*(p0.z-p1.z)+p1.z;
	z=min<float>(za,zb);
	if(z<min<float>(p0.z,p1.z) || z>max<float>(p0.z,p1.z))
		return 0;
	return 1;
}