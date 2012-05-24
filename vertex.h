/*
* vertex class 
* Allow explicit look up face surrounding a vertex
* Every face can have maximum 5 faces (faceId)
* 
*/
#pragma once
class vertex
{
public:
	//coordinates
	float x, y, z;	

	//faces
	int max;
	int *faceId;
	int nface;

	float vnormx, vnormy, vnormz;

	//functions
	vertex(float, float, float);
	vertex(vertex* v);

	~vertex();
	int addFaceId(int);
	void removeFaceId(int i);
	void clearFaceId();

	//debug
	void printv(int i, FILE* a);
	void printface(FILE* a);
};

