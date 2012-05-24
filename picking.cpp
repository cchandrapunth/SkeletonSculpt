#include "stdafx.h"
#include <math.h>
#include <GL/glut.h>
#include <deque>

#include "picking.h"
#include "undo.h"

using namespace std;
static GLint model_display_list;

deque<int> sList;

void processPick(float cursorX, float cursorY){
	GLint viewport[4];
	GLubyte pixel[3];

	//ask for value of the viewport
	glGetIntegerv(GL_VIEWPORT, viewport);

	//read pixel under the curser
	glReadPixels(cursorX, viewport[3]-cursorY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, (void*) pixel);

	//printf("cursorX= %f, cursorY= %f\n", cursorX, viewport[3]-cursorY);
	printf("%d %d %d\n", pixel[0], pixel[1], pixel[2]);
	
	//0-255
	if(pixel[0] == 0 && pixel[1] == 0){
		pickMe = pixel[2];
		printf("You pick %d\n", pickMe);
	}
	//255- 65,535
	else if(pixel[0] == 0 && pixel[1] != 0){
		pickMe = (256*pixel[1] + pixel[2]+1);
		printf("You pick %d\n", pickMe);
	}
	else{
	   printf("You didn't click any rect!");
	   pickMe = -1;
	 }
	printf ("\n");

}

int getSelection(){
	return pickMe;
}
void setNullSelection(){
	pickMe = -1;
}

void picked(GLuint name, int sw){
	printf("my name = %d in %d\n", name, sw);
}

void printsList(){
	for(int k=0; k< sList.size(); k++){
			printf("> %d ", sList.at(k));
	}
	if(sList.size() > 0)	printf("\n");
	else printf("empty\n");
}
void store_selection(int i){
	
	int pos = sListContain(i);

	//remove if repeated
	if(sList.size() > 0 && pos >= 0){
		sList.erase(sList.begin() + pos);

	}else{
		int *x= (int *) malloc(sizeof(int));
		*x = i;
		sList.push_back(*x);
	}
	printsList();
}

void clearSelectionList(){
	sList.clear();
}

int sListContain(int i){
	//printsList();

	for(int a= 0; a< sList.size(); a++){
		if(sList.at(a) == i) {
			return a;
		}
	} 
	return -1;
}

int* getsList(){
	int* list = (int *) malloc(sizeof(int)*sList.size());
	for(int a= 0; a< sList.size(); a++){
		list[a] = sList.at(a);
	}
	return list;
}

int getsListSize(){
	return sList.size();
}