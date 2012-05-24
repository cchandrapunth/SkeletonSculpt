
//essentially keep track of the model transformation 
//call from translatePoly in model.cpp
//anable undo/redo 

#include "stdafx.h"
#include <deque>

#include "undo.h"

#include "mesh.h"
#include "vertex.h"
#include "vmmodel.h"

//should be changable by the users
#define maxAct 30
#define maxControl 10

using namespace std;
deque<model_state> actionList;
deque<matrix_state> controlList;

static int rotateSceneX =0;
static int rotateSceneY =0;
static float zoomSceneZ = 0;

void addMatrix(int x, int y, float z){
	rotateSceneX += x;
	if(rotateSceneX > 360) rotateSceneX -=360; 

	rotateSceneY += y;
	if(rotateSceneY > 360) rotateSceneY -=360; 

	zoomSceneZ +=z;
	if(zoomSceneZ > 300) zoomSceneZ = 300;
	else if(zoomSceneZ < -300) zoomSceneZ = -300;
}

int getMatX() {return rotateSceneX;}
int getMatY() {return rotateSceneY;}
float getMatZ() {return zoomSceneZ;}

//prepare for undo
void pushMatrix(){
	matrix_state* ms= (matrix_state*) malloc (sizeof(matrix_state));
	ms->rollX = rotateSceneX; 
	ms->rollY = rotateSceneY;
	ms->zoomZ = zoomSceneZ;

	if(!controlList.empty()){
		ms->rollX += controlList.front().rollX;
		if(ms->rollX > 360) ms->rollX -=360;
		ms->rollY += controlList.front().rollY;
		if(ms->rollY > 360) ms->rollY -=360;
		ms->zoomZ += controlList.front().zoomZ;
	}

	controlList.push_front(*ms);
	if(controlList.size() > maxControl){
		controlList.pop_back();
	}

	rotateSceneX = 0;
	rotateSceneY = 0;
	zoomSceneZ = 0;
}

//useless
int restoreMatX(){
	if(controlList.empty()) return 0;
	return controlList.front().rollX;
}

int restoreMatY(){
	if(controlList.empty()) return 0;
	return controlList.front().rollY;
}

float restoreMatZ(){
	if(controlList.empty()) return 0;
	return controlList.front().zoomZ;
}

