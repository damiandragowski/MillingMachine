#ifndef _SIMULATOR
#define _SIMULATOR
#include <windows.h>

#include <vector>
#include <list>
#include "stuff.h"
#include "drill.h"
#include "mtxlib.h"
#pragma once

class Simulator
{
	enum Errors { HitBaseError, FlatError, DrillError, InproperSideError };
	bool b_ispathloaded;
	Stuff	stuff; 
	list< vector3 >::iterator		current;
	int								counter;
	int								summary;
	
	list< vector3 >		pathlines;
	vector< vector3 >		colors;
	vector3				center;
public:
	list< string >					errors;
	Drill	drill;
	Simulator(void);
	~Simulator(void);
	void InitMat(float _Epsilon, vector3 _Size, float _BottomHeight, int _step);
	float getProc() { return 100.0f*((float)counter/(float)summary); }
	void Draw();
	bool Move();
	void MakeMoveUpdate(vector3 & start, vector3 & end);
	TRECT calcUpdateRect(vector3 & p0,vector3 & p1);
	TRECT calcUpdateRect(vector3 & p0);
	void StartMachine(HWND hWnd);
	void StopMachine(HWND hWnd); 
	// This function is used to load the program for milling machinie
	bool LoadPath(char * pathname);
};

#endif