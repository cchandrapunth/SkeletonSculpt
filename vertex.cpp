#include "StdAfx.h"
#include "log.h"
#include "vertex.h"
#include <deque>

vertex::vertex(float a, float b, float c)
{
	max = 20;
	x = a;
	y = b;
	z = c;

	faceId = (int *) malloc(sizeof(int)*max);
	nface = 0;

	vnormx = 0;
	vnormy = 0;
	vnormz = 0;
	
}

//copy
vertex::vertex(vertex* v){
	max = 20;

	x = v->x;
	y = v->y;
	z = v->z;

	faceId = (int *) malloc(sizeof(int)*max);
	nface = 0;

	for(int i=0; i< v->nface; i++){
		addFaceId(v->faceId[i]);
	}

	vnormx = v->vnormx;
	vnormy = v->vnormy;
	vnormz = v->vnormz;
}

vertex::~vertex(){
}

void vertex::clearFaceId(){
	nface = 0;
}

//auto-incremented number of face
int vertex::addFaceId(int i){
	if(nface >= max-1) {
		printf("unable to add face because the vertex had %d face\n", max);
		return -1;
	}
	else{
		bool newface = true;
		//check repeat 
		for(int k=0; k< nface; k++){
			if(faceId[k] == i) {
				newface = false;
				break;
			}
			
		}

		if(newface){
			//add
			faceId[nface] = i;
			nface++;
		}
	}
	return 1;
}

void vertex::removeFaceId(int id){
		if(nface <1) {
			printf("unable to remove face from empty list\n");
		}else{
		
			//search for id
			int find = -1;
			for(int i=0; i< nface; i++){
				if(faceId[i] == id) find = i;
			}

			//find the faceid to be removed
			if(find >=0){
				for(int i=find; i < nface-1; i++){
					faceId[i] = faceId[i+1];
				}
				nface--;
			}
		}

}


void vertex::printv(int i, FILE* a){
	fprintf(a, " %d vx:%.5f, vy:%.5f, vz:%.5f\n",i, x, y, z);
	fprintf(a, "vnorm: %f \t%f \t%f\n", vnormx, vnormy, vnormz);
}

void vertex::printface(FILE* a){
	fprintf(a, "nface= %d \t| faceId ", nface);
	for(int i=0; i< nface; i++){
		fprintf(a, "%d ", faceId[i]);
	}
	fprintf(a, "\n");
}


