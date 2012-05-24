#include "stdafx.h"
#include "mode.h" 


/******************
mode 1- sculpting
mode 2- painting 
mode 3- selection 
*******************/

static int modeid = 1; //defalt -sculpt
static int stateid = 2; //defalt - render

void set_mode(int i){
	modeid = i;
}

bool is_mode(int i){
	return i == modeid ? true : false;
}

/******************
state 1- SELECT
state 2- RENDER
******************/
void set_state(int i){
	stateid = i;
}

bool is_state(int i){
	return i == stateid ? true : false;
}


