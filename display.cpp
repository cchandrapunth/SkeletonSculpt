#include "stdafx.h"
#include <GL/glut.h>
#include "XnPoint3D.h"
#include "mode.h"
#include "picking.h"
#include "gesture.h"
#include "drawmodel.h"
#include "hand_history.h"
#include "vertex.h"
#include "vmmodel.h"
#include "display.h"
#include "paint.h"

//resolution
static int g_nXRes = 640, g_nYRes = 480;
static int h=800, w= 800;
static float cursorX, cursorY; 

bool stateGrabR = false;		//0- not grab, 1 - already in grab
bool stateGrabL = false;		//0- not grab, 1 - already in grab
bool switchHand = false;		//right hand perform rotation

bool static BACK_BUFF;		//display back buffer

void activate_rotate(){
	switchHand = !switchHand;
}

bool isRotate(){
	return switchHand;
}

/* Select from 3 available modes and process accordingly */
void mode_selection(XnPoint3D* handPointList, hand_h* rhand){

	//-------------------sculpting--------------------------
	if(is_mode(1)){
	
		checkRCursor(1, rhand);						//right
		//if(hasTwoHands()) checkLCursor(lhand);	//left
		

		//SELECTION
		if(is_state(1)){	
			float* cursor = getCursor();
			cursorX = cursor[0];
			cursorY = cursor[1];

			drawPickVMModel();
			processPick(cursorX, cursorY);
			set_state(2);	
		}
		//RENDER
		else {
			draw_hand(handPointList);	//draw every hands
			
			//show back buffer?
			if(!BACK_BUFF){
				drawVMModel();
			}
			else{ 
				drawPickVMModel();
			}
			set_state(1);
			glutSwapBuffers();
		}
	}
	
	//-------------------paint----------------------------
	else if(is_mode(2)) {
		checkRCursor(2, rhand); 

		//SELECTION
		if(is_state(1)){
			//update cursor for paint effect
			float* cursor = getCursor();
			cursorX = cursor[0];
			cursorY = cursor[1];

			drawPickVMModel();
			processPick(cursorX, cursorY);
			set_state(2);
		}
		//RENDER
		else {
			//////////////////////////////////////////////////** in gesture////////////////
			draw_hand(handPointList);

			
			if(!BACK_BUFF)
				drawVMModel();
			else drawPickVMModel();

			//back to select other mesh
			set_state(1);
			glutSwapBuffers();
		}
	}

	//------------------ Selection ------------------------
	else if(is_mode(3)) {
		checkRCursor(3, rhand);
		
		 if(is_state(1)){
			drawPickVMModel();

			set_state(2);
		}
		else {
			//////////////////////////////////////////////////** in gesture////////////////
			draw_hand(handPointList);
			if(!BACK_BUFF)
				drawVMModel();
			else drawPickVMModel();
			drawVMModel();
			glutSwapBuffers();
		}
		
	}

}


//RIGHT HAND:: treat grab as a mouse click. 
void checkRCursor(int func, hand_h* rhand){
	
	if(isGrab()) {
		rhand->storeHand(getPalm());		//keep the hand movement history

		//first time grab gesture occurs for right hand
		if(!stateGrabR) {	
			//adjust with width and height of the screen
			float* cursor = getCursor();
			cursorX = cursor[0];
			cursorY = cursor[1];
			set_state(1); 
			stateGrabR = true;

		}
		//still in grab gesture
		else{
			//sculpt
			if(func == 1) {
				
				//select a mesh once
				//we don't need this for painting
				set_state(2);
				
				if(switchHand){
					commitScene(rhand->gettranslateX(), rhand->gettranslateY(), rhand->gettranslateZ());
					recalNormal();
				}
				else{
					//grab group of mesh
					if(sListContain(getSelection()) >= 0 ){
						interpolate(getsList(), rhand->gettranslateX(), 
							rhand->gettranslateY(), rhand->gettranslateZ(), getRotX(), getRotY());
						recalNormal();
					}
					//grab one mesh
					else if(getSelection() >= 0 && getSelection() < getFaceListSize()){
						interpolate(getSelection(), rhand->gettranslateX(), 
							rhand->gettranslateY(), rhand->gettranslateZ(), getRotX(), getRotY());
						recalNormal();
					}
				}
			}
			//paint
			else if(func ==2){

					if(getSelection() >=0 && getSelection() < getFaceListSize()){
						//printf("selection ->%d\n", getSelection());
						paintMesh(getSelection(), getBrushColor());
					}		
			}

			//selection?
			else if(func ==3){
				
			}
		}
	}
	else{
		//just release
		if(stateGrabR){
			//selection list
			//if(selection && getSelection() > 0 && getSelection() < getFaceListSize()){
			//	store_selection(getSelection());
			//}

			stateGrabR = false; 
			rhand->clearHandList();
			setNullSelection(); //show no mesh response when hand released

			//undo
			if(func == 1) copy_vmmodel(); 
		}
	}


}

//LEFT HAND:: handle rotation
void checkLCursor(hand_h* lhand){
	
	if((isLGrab() && !switchHand) || (switchHand && isGrab())) {
		XnPoint3D point;
		if(switchHand)  point = getPalm();
		else	 point = getLPalm();
		lhand->storeHand(point);		//keep the hand movement history
				
		//first time grab gesture occurs for left hand
		if(!stateGrabL) {
			set_state(1); 
			stateGrabL = true;
		}
		//still in grab gesture
		else{
			commitScene(lhand->gettranslateX(), lhand->gettranslateY(), lhand->gettranslateZ());
			recalNormal();
		}
	}
	else{
		//just release
		if(stateGrabL){
			stateGrabL = false; 
			lhand->clearHandList();
		}
	}
	
}

void preview_scene(){
	commitScene(2, 0, 0);
	drawVMModel();
	glutSwapBuffers();
}


void set_cursor(float x, float y){
	cursorX = x;
	cursorY = y;
}
void switch_buffer(){
	BACK_BUFF = !BACK_BUFF;
}
bool get_buffer(){
	return BACK_BUFF;
}

float* getCursor(){
	
	float* c = (float*) malloc(sizeof(float)*2);
	c[0] = getPalm().X;
	c[1] = getPalm().Y;
	return c;
	
	return 0;
}



