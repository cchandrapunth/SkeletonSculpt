#include "stdafx.h"
#include <stdio.h>
#include "XnPoint3D.h"
#include <deque>
#include <math.h>
#include "vertex.h"

#include "hand_history.h";

//helper for the real time effect 
//allow the model to react to movement of the hand during grab gesture
hand_h::hand_h(){
	current = *new XnPoint3D();
	prev = *new XnPoint3D();
}
hand_h::~hand_h(){
}


//list of palm position when grab occurs
//keep the newest element at the front. maintain size at MAX_HANDLIST
void hand_h::storeHand(XnPoint3D p){
	prev = current;
	current = p;
}

//clear the list after released
void hand_h::clearHandList(){
	current = *new XnPoint3D();
	prev = *new XnPoint3D();	
}

//movement in x-axis
float hand_h::gettranslateX(){
	return prev.X -current.X;
}

//movement in y-axis
float hand_h::gettranslateY(){
	return prev.Y -current.Y;
}

float hand_h::gettranslateZ(){
	return prev.Z -current.Z;
	
}

