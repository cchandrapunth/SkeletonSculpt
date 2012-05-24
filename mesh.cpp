#include "StdAfx.h"
#include "mesh.h"
#include "log.h"

//constructor
mesh::mesh(int id1, int id2, int id3)
{
	ind1 = id1;
	ind2 = id2;
	ind3 = id3;
	colorId  =2;	//default color is red

	normalX = 0; normalY = 0; normalZ = 0;
}

mesh::mesh(mesh* m){
	ind1 = m->ind1;
	ind2 = m->ind2;
	ind3 = m->ind3;
	colorId = m->colorId;

	normalX = m->normalX;
	normalY = m->normalY;
	normalZ = m->normalZ;
}

mesh::~mesh(){
}


void mesh::setColor(int id){
	colorId = id;
}

void mesh::printmesh(int i, FILE* a){
	fprintf(a, "%d ind %d, %d, %d \t| normal %.5f, %.5f, %.5f \t| colorId = %d\n",i,  ind1, ind2, ind3, normalX, normalY, normalZ, colorId);
}
