/*
* mesh class
* Allow explicit look up vertices of a face
*
* A face must consists of 3 vertices (triangle)
* and its normal vectors and colorId of a mesh
*/
#pragma once
 
class mesh
{
public:
	
	mesh(int ind1, int ind2, int ind3);
	mesh(mesh* m);
	~mesh(void);
	void setColor(int);
	void printmesh(int i, FILE* a);

	int ind1, ind2, ind3;
	float normalX, normalY, normalZ;
	int colorId;
};

