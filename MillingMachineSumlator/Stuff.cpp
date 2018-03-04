#include ".\stuff.h"
#include "Exception.h"


#include <windows.h>		// Header File For Windows

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>


Stuff::Stuff(void)
{
	Epsilon = 1.0;
	BottonHeight = 10;
	Size = vector3(150,150,50);
	x = static_cast< int >(Size[0]/Epsilon) + 1;
	y = static_cast< int >(Size[1]/Epsilon) + 1;
}

Stuff::~Stuff(void)
{
}

void Stuff::Init(float _Epsilon, vector3 _Size, float _BottomHeight, vector< vector3 > _StuffColors, int _step)
{
	Heights.clear();
	Colors.clear();

	if ( Epsilon <= 0 )
		throw Exception("in class Stuff, Init method, Epsilion was 0");

	if ( _Size.x <= 0 || _Size.y <= 0 || _Size.z <= 0 )
		throw Exception("in class Stuff, Init method, Size is Inapropriate");

	if ( _BottomHeight <= 0 )
		throw Exception("in class Stuff, Init method, BottomHeight <= 0");

	if ( _StuffColors.size() != 3 )
		throw Exception("in class Stuff, Init method, bad _StuffColors size");

	step = _step;
	Epsilon = _Epsilon;
	BottonHeight = _BottomHeight;
	Size = _Size;
	this->x = static_cast< int >(Size.x/Epsilon) + 1;
	this->y = static_cast< int >(Size.y/Epsilon) + 1;
	StuffColors = _StuffColors;

	// Initialize vector with stuff;
	Heights.assign(x, y, Size.z);
	Colors.assign(x, y, ColorType::cNormal);


	int indexXLast = x - 1;
	int indexYLast = y - 1;

	int ileKrokc = (indexXLast / step) + 1;
	int ileKrokr = (indexYLast / step) + 1;

	int resztaKrokc = indexXLast % step;
	int resztaKrokr = indexYLast % step;

	poczX  = resztaKrokc / 2;
	poczY  = resztaKrokr / 2;

	vertices.resize(ileKrokc, ileKrokc);
	normals.assign(ileKrokc, ileKrokc, vector3(0.0,0.0,1.0));

	TRECT	indexRect(0,indexXLast,0,indexYLast);


	indexRect.indXMin = indexRect.indXMin / step ;
	indexRect.indYMin = indexRect.indYMin / step ;
	
	indexRect.indXMax = indexRect.indXMax / step + 1;
	indexRect.indYMax = indexRect.indYMax / step + 1;

	if (indexRect.indYMax > (int)vertices.r-1)
		--indexRect.indYMax ;

	if (indexRect.indXMax> (int)vertices.c-1)
		--indexRect.indXMax ;

	
	TRECT dRect = indexRect;
	
	dRect.indXMin = poczX + dRect.indXMin * step;
	dRect.indYMin = poczY + dRect.indYMin * step;

	dRect.indXMax = poczX + dRect.indXMax * step;
	dRect.indYMax = poczY + dRect.indYMax * step;

	for (int sy = dRect.indYMin,j = indexRect.indYMin; sy<= dRect.indYMax ; sy += step, ++j) 
		for (int sx = dRect.indXMin,i = indexRect.indXMin; sx <= dRect.indXMax ; sx += step, ++i)
			vertices[j][i] = vector3(sx * Epsilon,sy * Epsilon,Heights[sx][sy]);

}

void Stuff::Draw()
{
	glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,40);
	
	for (unsigned y=0;y<vertices.r-1; ++y) {
		glBegin(GL_QUAD_STRIP);
		for (unsigned x=0;x<vertices.c; ++x) {
			unsigned iX =  x*step ;
			unsigned iY =  y*step ;
			glColor3fv(StuffColors[Colors[iX][iY]].v());
			glNormal3fv(normals[y][x].v());
			glVertex3fv(vertices[y][x].v());
			glColor3fv(StuffColors[Colors[iX][iY+1]].v());
			glNormal3fv(normals[y+1][x].v());
			glVertex3fv(vertices[y+1][x].v());
			
		}
		glEnd();	
	}
	size_t lX = vertices.r - 1;
	size_t lY = vertices.r - 1;
	glBegin(GL_QUAD_STRIP);
	glColor3fv(StuffColors[ColorType::cNormal].v());
	glNormal3f(0.0,-1.0,0.0);
	for (unsigned x=0;x<vertices.c; ++x) {
		glVertex3f(vertices[0][x].x,vertices[0][x].y,0.0);
		glVertex3fv(vertices[0][x].v());
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glNormal3f(0.0,1.0,0.0);
	
	for (unsigned x=0;x<vertices.c; ++x) {
		glVertex3f(vertices[lY][x].x,vertices[lY][x].y,0.0);
		glVertex3fv(vertices[lY][x].v());
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glNormal3f(-1.0,0.0,0.0);
	
	for (unsigned y=0;y<vertices.r; ++y) {
		glVertex3f(vertices[y][0].x,vertices[y][0].y,0.0);
		glVertex3fv(vertices[y][0].v());
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	glNormal3f(1.0,0.0,0.0);
	
	for (unsigned y=0;y<vertices.r; ++y) {
		glVertex3f(vertices[y][lX].x,vertices[y][lX].y,0.0);
		glVertex3fv(vertices[y][lX].v());
	}
	glEnd();
}

void Stuff::Update(TRECT & rect)
{
	int indexXLast = x - 1;
	int indexYLast = y - 1;

	int ileKrokc = (indexXLast / step) + 1;
	int ileKrokr = (indexYLast / step) + 1;

	int resztaKrokc = indexXLast % step;
	int resztaKrokr = indexYLast % step;

	poczX  = resztaKrokc / 2;
	poczY  = resztaKrokr / 2;

	TRECT	indexRect(0,indexXLast,0,indexYLast);

	indexRect.indXMin = indexRect.indXMin / step ;
	indexRect.indYMin = indexRect.indYMin / step ;
	
	indexRect.indXMax = indexRect.indXMax / step + 1;
	indexRect.indYMax = indexRect.indYMax / step + 1;

	if (indexRect.indYMax > (int)vertices.r-1)
		--indexRect.indYMax ;

	if (indexRect.indXMax> (int)vertices.c-1)
		--indexRect.indXMax ;

	
	TRECT dRect = indexRect;
	
	dRect.indXMin = poczX + dRect.indXMin * step;
	dRect.indYMin = poczY + dRect.indYMin * step;

	dRect.indXMax = poczX + dRect.indXMax * step;
	dRect.indYMax = poczY + dRect.indYMax * step;

	for (int y = dRect.indYMin,j=indexRect.indYMin;/*j <= indexRect.indYMax*/ y<=dRect.indYMax ; y+=step,++j) 
		for (int x=dRect.indXMin,i=indexRect.indXMin; /*i <= indexRect.indXMax */ x <= dRect.indXMax ; x+= step,++i) 
			vertices[j][i] = vector3(x * Epsilon, y * Epsilon , Heights[x][y] );

	int jMin = std::max<int>(1,indexRect.indYMin);
	int jMax = std::min<int>((int)normals.c,(int)indexRect.indYMax-1);

	int iMin = std::max<int>(1,indexRect.indXMin);
	int iMax = std::min<int>((int)normals.r,(int)indexRect.indXMax-1);

	for (int j=jMin;j <= jMax ; ++j) {
		for (int i=iMin; i<= iMax  ; ++i) 
		{
			const vector3& vm0 = vertices[j][i-1];
			const vector3& v00 = vertices[j][i];
			const vector3& v0m = vertices[j-1][i];
			const vector3& v0p = vertices[j+1][i];
			const vector3& vp0 = vertices[j][i+1];
			vector3 n1 = cross(v0p-v00,v00-vp0);
			vector3 n2 = cross(v0p-vm0,vm0-v00);
			vector3 n3 = cross(vm0-v0m,v0m-v00);
			vector3 n4 = cross(v0m-vp0,vp0-v00);
			
			vector3 na = (n1 + n2);
			vector3 nb = (n1 + n2);
			vector3 nc = (na + nb);
			normals[j][i]= n2.normalize();
			
		}
		normals[j][normals.r-1]=normals[j][normals.r-2];
	}


}
