#include ".\simulator.h"
#include "Exception.h"

#include <fstream>

using namespace std;

#include <windows.h>		// Header File For Windows

#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>
#include <math.h>



std::string ToString(float intdata)
{
    std::stringstream Stream;
    std::string ReturnValue;

    Stream << intdata;
    Stream >> ReturnValue;

    return ReturnValue;
}

std::string ToString(int intdata)
{
    std::stringstream Stream;
    std::string ReturnValue;

    Stream << intdata;
    Stream >> ReturnValue;

    return ReturnValue;
}


Simulator::Simulator(void)
{
	// kolory materialu beldow itd
	colors.push_back(vector3(0.37890625f, 0.28515625f, 0.13671875f));
	colors.push_back(vector3(0.0f, 0.8f, 0.2f));
	colors.push_back(vector3(0.2f, 0.0f, 0.0f));

	stuff.Init(0.1f , vector3(150.0f,150.0f,50.0f), 20, colors, 5);
	center.x = -150.0/2;
	center.y = -150.0/2;
	center.z = 0;
	drill.changeDrill(8, ToolType::Sphere, 40);
	errors.clear();
}

void Simulator::InitMat(float _Epsilon, vector3 _Size, float _BottomHeight, int _step)
{
	colors.clear();
	colors.push_back(vector3(0.37890625f, 0.28515625f, 0.13671875f));
	colors.push_back(vector3(0.0f, 0.8f, 0.2f));
	colors.push_back(vector3(0.2f, 0.0f, 0.0f));

	stuff.Init(_Epsilon , _Size, _BottomHeight, colors, 5);
	center.x = -_Size[0]/2;
	center.y = -_Size[1]/2;
	center.z = 0;
	drill.changeDrill(8, ToolType::Sphere, _Size[2]-10);
	errors.clear();
}

Simulator::~Simulator(void)
{
}

void Simulator::StartMachine(HWND hWnd)
{ 
	drill.setVisible();
	int i=150;
	while ( i > stuff.Size.z+20 ) {
		drill.moveDrill(vector3(0.0, 0.0, (float)i)); 
		InvalidateRect(hWnd, 0, false);
		Sleep(90);
		i-=2;
	}
}

void Simulator::StopMachine(HWND hWnd)
{
	int i=(int)stuff.Size.z+20;
	while ( i < 150 ) {
		drill.moveDrill(vector3(0.0, 0.0, (float)i)); 
		InvalidateRect(hWnd, 0, false);
		Sleep(90);
		i+=2;
	}
	drill.unsetVisible();
}

void Simulator::Draw()
{
	glPushMatrix();
	glTranslatef(center.x,center.y,center.z);
	stuff.Draw();
	drill.Draw();
	glPopMatrix();
}

// This function is used to load the program for milling machinie
bool Simulator::LoadPath(char * pathname)
{
	string line;

	if ( pathname == 0 )
		return false;

	pathlines.clear();
	ifstream infile (pathname);
	summary = 0;

	vector<string> res;
	if ( SplitString(pathname, string("."), res) == 1 ) {
		string str = res[1];
		if ( str[0] == 'k' ) {
			int k = (int)parseFloat(string("")+str[1]+str[2]);
			drill.changeDrill((float)k/2.0f, ToolType::Sphere, 40.0);
		} else {
			int k = (int)parseFloat(string("")+str[1]+str[2]);
			drill.changeDrill((float)k/2.0f, ToolType::Cylinder, 40.0);
		}
	}

	vector3 old(0.0,0.0,0.0);	
	string wsp[]={string("Z"), string("Y"), string("X")};

	float x=0.0f,y=0.0f,z=0.0f;
	while (getline(infile,line,'\n'))
	{
		try
		{
			for ( int i=0;i<3;++i)
			{
				vector< string > temp1;
				if ( SplitString(line, wsp[i], temp1) == 1 ) {
					line = temp1[0];
					switch (i) {
						case 0:
							z = parseFloat(temp1[1]);
							break;
						case 1:
							y = parseFloat(temp1[1]);
							break;
						case 2:
							x = parseFloat(temp1[1]);
							break;
					}
				}
			}
			pathlines.push_back(vector3(x, y, z));
			summary++;
		} catch (NumException e) {
			MessageBox(0, TEXT("Parse Error"), TEXT("info"), 0);
			return false;
		}
	}

	/*
	while (getline(infile,line,'\r\n'))
	{
		
		vector< string > d1;
		X="X";
		try 
		{
		if ( SplitString(line, X, d1) == 1 )
		{
			vector< string > d2;
			X="Y";
			if ( SplitString(d1[1], X, d2) == 1 )
			{
				float x = parseFloat(d2[0]);
				vector< string > d3;
				X="Z";
				if ( SplitString(d2[1], X, d3) == 1 )
				{
					float y = parseFloat(d3[0]);
					float z = parseFloat(d3[1]);
					pathlines.push_back(vector3(x, y, z));
				} else {
					float y = parseFloat(d3[0]);
				}
			}
		} // else ignore
		else 
		{
			vector< string > d2;
			X="Y";
			if ( SplitString(d1[1], X, d2) == 1 )
			{
				float x = old.x;
				vector< string > d3;
				X="Z";
				if ( SplitString(d2[1], X, d3) == 1 )
				{
					float y = parseFloat(d3[0]);
					float z = parseFloat(d3[1]);
					pathlines.push_back(vector3(x, y, z));
				}
			} else {
				float x = old.x;
				vector< string > d3;
				X="Z";
				if ( SplitString(d2[1], X, d3) == 1 )
				{
					float y = parseFloat(d3[0]);
					float z = parseFloat(d3[1]);
					pathlines.push_back(vector3(x, y, z));
				}
			}
		}
		} catch (NumException e) {
			MessageBox(0, TEXT("Parse Error"), TEXT("info"), 0);
			return false;
		}
	}
		current=pathlines.begin();
	counter=1;
	return true;
	*/

	current=pathlines.begin();
	counter=1;
	return true;
}

TRECT Simulator::calcUpdateRect(vector3 & p0, vector3 & p1)  
{
	TRECT rect;
	
	float xmin = min<float>(p0.x, p1.x);
	float xmax = max<float>(p0.x, p1.x);
	float ymin = min<float>(p0.y, p1.y);
	float ymax = max<float>(p0.y, p1.y);


	xmin -= drill.getRadius();
	ymin -= drill.getRadius();
	xmax += drill.getRadius();
	ymax += drill.getRadius();
	
	rect.indXMin = (int) ( xmin / stuff.getEpsilon() - 2.0f );
	rect.indXMax = (int) ( xmax / stuff.getEpsilon() + 2.0f );
	rect.indYMin = (int) ( ymin / stuff.getEpsilon() - 2.0f );
	rect.indYMax = (int) ( ymax / stuff.getEpsilon() + 2.0f );
	

	if ( rect.indYMin < 0 ) 
		rect.indYMin = 0;
	if ( rect.indYMax > stuff.y-1 ) 
	 	rect.indYMax = stuff.y-1;
	if ( rect.indXMin < 0 ) 
		rect.indXMin = 0;
	if ( rect.indXMax > stuff.x-1 ) 
		rect.indXMax = stuff.x-1;
	
	return rect;
}

TRECT Simulator::calcUpdateRect(vector3 & p0)  
{
	TRECT rect;
	
	float xmin = p0.x - drill.getRadius();
	float ymin = p0.y - drill.getRadius();
	float xmax = p0.x + drill.getRadius();
	float ymax = p0.y + drill.getRadius();

	rect.indXMin = (int) ( xmin / stuff.getEpsilon() - 2.0f );
	rect.indXMax = (int) ( xmax / stuff.getEpsilon() + 2.0f );
	rect.indYMin = (int) ( ymin / stuff.getEpsilon() - 2.0f );
	rect.indYMax = (int) ( ymax / stuff.getEpsilon() + 2.0f );
	

	if ( rect.indYMin < 0 ) 
		rect.indYMin = 0;
	if ( rect.indYMax > stuff.y-1 ) 
	 	rect.indYMax = stuff.y-1;
	if ( rect.indXMin < 0 ) 
		rect.indXMin = 0;
	if ( rect.indXMax > stuff.x-1 ) 
		rect.indXMax = stuff.x-1;
	
	return rect;
}



void Simulator::MakeMoveUpdate(vector3 & start, vector3 & end)
{
	if ( start == end )
		return;
	TRECT updateRect=calcUpdateRect(start, end);
	TRECT r = calcUpdateRect(end);
	float z=0.0;
	vector3 cur;
	float zToolSquare;
	float zTool;


	switch ( drill.getToolType() )
	{
		case ToolType::Cylinder:
			{

				for(int iY=updateRect.indYMin;iY<=updateRect.indYMax;++iY)
					for(int iX = updateRect.indXMin; iX <= updateRect.indXMax ; ++iX) {
						if(drill.countCylindrict( iX*stuff.getEpsilon(),iY * stuff.getEpsilon(), z, start, end))
							if(z < stuff.Heights[iX][iY]) {
								int color = ColorType::cNormal;
								if(z<stuff.BottonHeight) {
									errors.push_back(string("Uderzone w podstawke g쿮boko쒏 w linii ")+ToString(counter)+string("\r\n"));
									color = ColorType::cError;
								}
								if(stuff.Heights[iX][iY]-z>drill.getHeight()) {
									errors.push_back(string("Uderzone czescia nieskrawajaca w linii ")+ToString(counter)+string("\r\n"));
									color = ColorType::cError;
								}
									
								stuff.Heights[iX][iY]=z;
								stuff.Colors[iX][iY] = color;
							}
					}



				for(int iY = r.indYMin; iY <= r.indYMax ; ++iY)	{
					for(int iX = r.indXMin; iX <= r.indXMax ; ++iX) {
						cur.x = iX * stuff.getEpsilon();
						cur.y = iY * stuff.getEpsilon();
						cur.z = stuff.Heights[iX][iY];

						zToolSquare = drill.getRadius()*drill.getRadius() - (cur.x - end.x) * (cur.x - end.x) - (cur.y - end.y)* (cur.y - end.y);
						if( zToolSquare <  0)	
							continue;
						int color = ColorType::cNormal;			
						

						if( cur.z > end.z ) {					
							if( stuff.Heights[iX][iY]-end.z> drill.getHeight() ) 
							{
								errors.push_back(string("Uderzone czescia nieskrawajaca w linii ")+ToString(counter)+string("\r\n"));
								color = ColorType::cError;
							}
							if( end.z<stuff.BottonHeight ) 
							{ 
								errors.push_back(string("Uderzone w podstawke g쿮boko쒏 w linii ")+ToString(counter)+string("\r\n"));
								color = ColorType::cError;
							}
							stuff.Heights[iX][iY] = end.z;
							stuff.Colors[iX][iY] = color;
						}
			
					}
				}


			}
			break;
		case ToolType::Sphere:
			{
				for(int iY=updateRect.indYMin;iY<=updateRect.indYMax;++iY)
					for(int iX = updateRect.indXMin; iX <= updateRect.indXMax ; ++iX) {
						if(drill.countSphere(iX*stuff.getEpsilon(), iY*stuff.getEpsilon(), z, start, end)) {
							int color = ColorType::cNormal;
							if( z < stuff.Heights[iX][iY] ) 
							{
								if( stuff.Heights[iX][iY] - z > drill.getHeight() ) 
								{
									errors.push_back(string("Uderzone czescia nieskrawajaca w linii ")+ToString(counter)+string("\r\n"));
									color = ColorType::cError;
								}
								if( z < stuff.BottonHeight ) {
									errors.push_back(string("Uderzone w podstawke g쿮boko쒏 w linii ")+ToString(counter)+string("\r\n"));
									color = ColorType::cError;
								}
								stuff.Heights[iX][iY] = z;
								stuff.Colors[iX][iY] = color;
							}
						}
					}

				for(int iY = r.indYMin; iY <= r.indYMax ; ++iY)	{
					for(int iX = r.indXMin; iX <= r.indXMax ; ++iX) {
						cur.x = iX * stuff.getEpsilon();
						cur.y = iY * stuff.getEpsilon();
						cur.z = stuff.Heights[iX][iY];

						zToolSquare = drill.getRadius()*drill.getRadius() - (cur.x - end.x) * (cur.x - end.x) - (cur.y - end.y)* (cur.y - end.y);
						if( zToolSquare <  0)	
							continue;
						int color = ColorType::cNormal;			
						

						zTool = sqrt(zToolSquare);
						float newZ = end.z - zTool;
						if(cur.z > newZ )				
						{
							if( stuff.Heights[iX][iY]-end.z > drill.getHeight() ) 
							{
								errors.push_back(string("Uderzone czescia nieskrawajaca w linii ")+ToString(counter)+string("\r\n"));
								color = ColorType::cError;
							}
							if( newZ < stuff.BottonHeight ) {
								errors.push_back(string("Uderzone w podstawke g쿮boko쒏 w linii ")+ToString(counter)+string("\r\n"));
								color = ColorType::cError;
							}
							stuff.Heights[iX][iY] = newZ;
							stuff.Colors[iX][iY] = color;
						}
					}
				}			



			}
			break;
	}
	stuff.Update(updateRect);
}

bool Simulator::Move()
{
	if ( pathlines.end() == current )
		return false;

	MakeMoveUpdate(drill.getPosition(), *(current) - center);


	drill.moveDrill(*(current) - center);

	++current;
	++counter;
	return true;
}
