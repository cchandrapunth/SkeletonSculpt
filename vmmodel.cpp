#include "stdafx.h"
#include <gl/glut.h>
#include <fstream>
#include "mesh.h"
#include "vertex.h"
#include <deque>
#include <set>
#include <map>
#include "vmmodel.h"
#include "log.h"
#include "picking.h"
#include "paint.h"

#define UNDORANGE 15
using namespace std;

/**color: check-1, red-2, blue-3, green-4, yellow-5, white-6 **/

static deque<mesh> faceList;
static deque<vertex> vertexList;

static deque<mesh> exfaceList;
static deque<vertex> exvertexList[UNDORANGE];
static map <int, int> twohalf;

int numMod = -1; 
int countMod = 0;

int selectedMesh;
int initColor = 3;	//initial color = green

bool debug = true;
bool LOAD_FROM_ORIGIN = true;
Log *pLog; 
FILE *mFile;
FILE *pFile3;

int getFaceListSize(){	return faceList.size();};
int getVertexListSize(){	return vertexList.size();};
float maxArea = 0.3;
float maxLen = 1.0;
int maxSubdivide = 4;
int countDivide = 0; 


void import_vm(){

	//pLog = new Log("vmmodel.log");
	mFile = fopen("vmmodel.txt", "w");	
	pFile3 = fopen("softselection.txt", "w");

	ifstream indata;
	if(LOAD_FROM_ORIGIN)
		indata.open("modelinput.txt");
	else 
		indata.open("vmmodeloutput.txt");
	if(!indata) {
		//pLog->Write("Error: input file couldn't be opened");
		exit(1);
	}

	vertexList.clear();
	faceList.clear();

	int nvertex, nmesh;
	indata >> nvertex;
	indata >> nmesh;

	//read in vertex data
	for(int i=0; i< nvertex; i++){
		float x, y, z;
		indata >> x;
		indata >> y;
		indata >> z;
	
		vertex *v = new vertex(x, y, z);
		vertexList.push_back(*v);
	}

	//read in mesh data
	for(int j=0; j< nmesh; j++){
		int id1, id2, id3, cid;
		indata >> id1;
		indata >> id2;
		indata >> id3;
		indata >> cid;

		mesh *m = new mesh(id1, id2, id3);
		//normal vector
		vertex* v = getFaceNormal(vertexList.at(id1), vertexList.at(id2), vertexList.at(id3));	
		m->normalX = v->x;
		m->normalY = v->y;
		m->normalZ = v->z;
		delete(v);

	
		m->setColor(cid);
		faceList.push_back(*m);

		//fill the lookup table for vertices 
		vertexList.at(id1).addFaceId(j);
		vertexList.at(id2).addFaceId(j);
		vertexList.at(id3).addFaceId(j);
	}
	
	if(LOAD_FROM_ORIGIN){
		//subDivide
		for(int k=0; k< 3; k++){
			subDivide(true);
		}
	}
	

	calVertexNormal();
	print_debug();

}

///DEbug
void print_debug(){
		//pLog->Write("load model complete");
		fprintf(mFile, "\n=================================================\n");
		fprintf(mFile, "\tvertexList :: nVertex = %d\n", vertexList.size());
		fprintf(mFile,"=================================================\n");
		for(int i=0; i< vertexList.size(); i++){
			vertexList.at(i).printv(i, mFile);
			vertexList.at(i).printface(mFile);
		}

		fprintf(mFile, "\n=================================================\n");
		fprintf(mFile, "\tfaceList :: nFace = %d\n", faceList.size());
		fprintf(mFile, "=================================================\n");
		for(int i=0; i< faceList.size(); i++){
			faceList.at(i).printmesh(i, mFile);
		}
}

void export_vm(){
	
	ofstream outdata;
	outdata.open("vmmodeloutput.txt");
	if(!outdata) {
		//pLog->Write("Error: output file couldn't be opened\n");
		exit(1);
	}

	int nVertex = vertexList.size();
	int nMesh = faceList.size();
	outdata << nVertex << " " << nMesh << endl;

	//export vertex
	for(int i=0; i< nVertex; i++){
		vertex v = vertexList.at(i);
		outdata << v.x << "\t" << v.y << "\t" << v.z << endl;
	}

	//export mesh
	for(int i=0; i< nMesh; i++){
		mesh m = faceList.at(i);
		outdata << m.ind1 << "\t" << m.ind2 << "\t" << m.ind3 << "\t" << m.colorId << endl;
	}
	
	outdata.close();
	print_debug();
}

//recalculate nornal in all 
void recalNormal(){
	for(int i=0; i< faceList.size(); i++){
		int ind1 = faceList.at(i).ind1;
		int ind2 = faceList.at(i).ind2;
		int ind3 = faceList.at(i).ind3;

		vertex *v = getFaceNormal(vertexList.at(ind1), vertexList.at(ind2), vertexList.at(ind3));
		faceList.at(i).normalX = v->x;
		faceList.at(i).normalY = v->y;
		faceList.at(i).normalZ = v->z;
		delete(v);
	}

	calVertexNormal();
}

//find normal of 3 vertices
//output pointer to norm vector
vertex* getFaceNormal(vertex vv1, vertex vv2, vertex vv3){

	vertex* v1 = new vertex(0, 0, 0);
	vertex* v2 = new vertex(0, 0, 0);

	v1->x = vv3.x - vv1.x;
	v1->y = vv3.y - vv1.y;
	v1->z = vv3.z - vv1.z;

	v2->x = vv2.x - vv1.x;
	v2->y = vv2.y - vv1.y;
	v2->z = vv2.z - vv1.z;

	vertex *norm = new vertex(0 ,0 ,0);
	//find normal using cross product
	norm->x = (v1->y * v2->z) - (v1->z * v2->y);
	norm->y = (v1->z * v2->x) - (v1->x* v2->z);
	norm->z = (v1->x * v2->y) - (v1->y * v2->x);	

	//declare on the stack
	delete(v1);
	delete(v2);
	return norm;
}

void calVertexNormal(){
	for(int i=0; i< vertexList.size(); i++){
		int numFace = vertexList.at(i).nface;

		float nx =0, ny =0, nz=0;
		for(int j=0; j< numFace; j++){
			int n = vertexList.at(i).faceId[j];

			nx += faceList.at(n).normalX;
			ny += faceList.at(n).normalY;
			nz += faceList.at(n).normalZ;
		}
		vertex *v = new vertex(nx, ny, nz);
		v = normalizeV(v);
		vertexList.at(i).vnormx = v->x;
		vertexList.at(i).vnormy = v->y;
		vertexList.at(i).vnormz = v->z;
		
		delete(v);
	}
}

//normalize
vertex* normalizeV(vertex* norm){
	float CombinedSquares = (norm->x * norm->x) +(norm->y * norm->y) +(norm->z * norm->z);
	float NormalisationFactor = sqrt(CombinedSquares);
		 norm->x = norm->x / NormalisationFactor;		
		 norm->y = norm->y / NormalisationFactor;
		 norm->z = norm->z / NormalisationFactor;
	return norm;
}


void setColorPaint(int id){
	int cid = faceList.at(id).colorId;
	//check-1, red-2, blue-3, green-4, yellow-5, white-6 
	//glBindTexture(GL_TEXTURE_2D,cid);

	setGLbrushColor(cid);
}

void drawMesh(int meshId, bool shade){

	mesh m = faceList.at(meshId);

	vertex v1 = vertexList.at(m.ind1);
	vertex v2 = vertexList.at(m.ind2);
	vertex v3 = vertexList.at(m.ind3);

	//float offsetx = -v1.vnormx/200;
	//float offsety = -v1.vnormy/200;
	//float offsetz = -v1.vnormz/200;
	
	glShadeModel(GL_SMOOTH);
	if(shade){
		setEffectColor(v1);
	}
	glNormal3f(-v1.vnormx, -v1.vnormy, -v1.vnormz);
	 glVertex3f(v1.x, v1.y, v1.z);

	if(shade){
		setEffectColor(v2);
	 }
	glNormal3f(-v2.vnormx, -v2.vnormy, -v2.vnormz);
	 glVertex3f(v2.x, v2.y,v2.z);

	if(shade){
		setEffectColor(v3);
	}
	glNormal3f(-v3.vnormx, -v3.vnormy, -v3.vnormz);
	glVertex3f(v3.x, v3.y, v3.z);
	
}

void drawMesh(int meshId){

	mesh m = faceList.at(meshId);
	vertex v1 = vertexList.at(m.ind1);
	vertex v2 = vertexList.at(m.ind2);
	vertex v3 = vertexList.at(m.ind3);

	glShadeModel(GL_SMOOTH);

	glNormal3f(-v1.vnormx, -v1.vnormy, -v1.vnormz);
	glVertex3f(v1.x, v1.y, v1.z);


	glNormal3f(-v2.vnormx, -v2.vnormy, -v2.vnormz);
	glVertex3f(v2.x, v2.y,v2.z);

	glNormal3f(-v3.vnormx, -v3.vnormy, -v3.vnormz);
	glVertex3f(v3.x, v3.y, v3.z);
	
}


//not use
int bsize =1;
void upBrush(){
	bsize+=1;
	printf("brush size  =%d\n", bsize);
}
void downBrush(){
	if(bsize > 2) bsize-=1;
	printf("brush size  =%d\n", bsize);
}

void paintMesh(int mid, int cid){
	//radius is up to bsize
	set<int> mSet;
	mSet.insert(mid);


	int ind = faceList.at(mid).ind1;
	int* list = vertexList.at(ind).faceId;
	for(int j=0; j< vertexList.at(ind).nface; j++){
		mSet.insert(list[j]);
	}

	ind = faceList.at(mid).ind2;
	list = vertexList.at(ind).faceId;
	for(int j=0; j< vertexList.at(ind).nface; j++){
		mSet.insert(list[j]);
	}

	ind = faceList.at(mid).ind3;
	list = vertexList.at(ind).faceId;
	for(int j=0; j< vertexList.at(ind).nface; j++){
		mSet.insert(list[j]);
	}


	for (set<int>::iterator it=mSet.begin(); it!=mSet.end(); it++){
		//printf("%d ",*it);
		faceList.at(*it).colorId = cid;
	}
	//printf("\n");
	mSet.clear();
}

/***************************************************
				subdivision
***************************************************/
bool sameVertex(vertex v1, vertex v2){
	if(v1.x - v2.x < 0.001 && v1.x - v2.x > -0.001 &&
		v1.y - v2.y < 0.001 && v1.y - v2.y > -0.001 &&
		v1.z - v2.z < 0.001 && v1.z - v2.z > -0.001){
			return true;
	}
	return false;
}

void reassignFaceId(){
	//assign faceId
	for(int i=0; i< vertexList.size(); i++){
		vertexList.at(i).clearFaceId();

		for(int j=0; j< faceList.size(); j++){
			if(faceList.at(j).ind1 == i || 
				faceList.at(j).ind2 == i ||
				faceList.at(j).ind3 == i){
					vertexList.at(i).addFaceId(j);
			}
		}
	}

}

//sub divide the whole model
void subDivide(bool do_normalize){
	
	if(countDivide < maxSubdivide){
		int size = getFaceListSize();
		for(int i=0; i< size; i++){
			subDivideMesh(i, do_normalize);
		}

		reassignFaceId();
		countDivide ++;
	}
}

vertex* findCenter(mesh m){
	vertex v1 = vertexList.at(m.ind1);
	vertex v2 = vertexList.at(m.ind2);
	vertex v3 = vertexList.at(m.ind3);

	vertex* v4 = new vertex((v1.x+v2.x+v3.x)/3, (v1.y+v2.y+v3.y)/3, (v1.z+v2.z+v3.z)/3);
	return v4;
}

void internalSubDivide(int meshId){
	mesh m = faceList.at(meshId);
	vertex* center = findCenter(m);

	int ind4 = vertexList.size();
	
	//new mesh
	mesh m2= new mesh(m.ind2, m.ind3, ind4);
	mesh m3= new mesh(m.ind3, m.ind1, ind4);
	//old mesh
	faceList.at(meshId).ind3 = ind4;

	//center = normalizeV(center);
	//assign faceID for new vertex 
	center->addFaceId(meshId);
	center->addFaceId(faceList.size());
	center->addFaceId(faceList.size()+1);
	vertexList.push_back(center);

	fprintf(mFile, "NEW VERTEX\n");
	fprintf(mFile, "%d: %f %f %f\n", meshId, center->x, center->y, center->z);

	//same color
	m2.colorId = m.colorId;
	m3.colorId = m.colorId;

	faceList.push_back(m2);
	faceList.push_back(m3);

}

//assign share faceId of v1, v2 to v12 
int findNeighbor(vertex v1, vertex v2, int itself){
	int shareface =-1;
	for(int i=0; i< v1.nface; i++){
		for(int j=0; j< v2.nface; j++){
			if(v1.faceId[i] == v2.faceId[j] ){
				if(v1.faceId[i] != itself)
					shareface = v1.faceId[i];
				//else 
					//printf("find it self %d", itself);
			}
		}
	}
	return shareface;
}

//divide a mesh into 4 new mesh
//handle the operation and restoration in the deque
//return list of new point 12, 23, 31 respectively
int* subDivideMesh(int meshId, bool do_normalize){

	mesh m = faceList.at(meshId);

	//store the old index
	int old1 = m.ind1;
	int old2 = m.ind2;
	int old3 = m.ind3;

	vertex v1 = vertexList.at(old1);
	vertex v2 = vertexList.at(old2);
	vertex v3 = vertexList.at(old3);
	//vertexList.at(m.ind1).removeFaceId(meshId);
	vertexList.at(m.ind2).removeFaceId(meshId);
	vertexList.at(m.ind3).removeFaceId(meshId);

	vertex *v12 = new vertex((v1.x+v2.x)/2.0, (v1.y+v2.y)/2.0, (v1.z+v2.z)/2.0);
	vertex *v23 = new vertex((v2.x+v3.x)/2.0, (v2.y+v3.y)/2.0, (v2.z+v3.z)/2.0);  
	vertex *v31 = new vertex((v3.x+v1.x)/2.0, (v3.y+v1.y)/2.0, (v3.z+v1.z)/2.0);

	//only normalize to  generate sphare
	//do not normalize when regeneration 
	if(do_normalize){
		v12 = normalizeV(v12);
		v23 = normalizeV(v23);
		v31 = normalizeV(v31);
	}

	//add faceId of v12, v23, v31
	int fn12 = findNeighbor(v1, v2, meshId);
	int fn23 = findNeighbor(v2, v3, meshId);
	int fn31 = findNeighbor(v3, v1, meshId);
	v12->addFaceId(fn12);
	v23->addFaceId(fn23);
	v31->addFaceId(fn31);


	//only check the vector once 
	bool v12repete = false, v23repete = false, v31repete = false;

	//assign index
	int size = vertexList.size();
	int indexv12;
	int indexv23;
	int indexv31;

	//check if the vertices already existed?
	//add only the new vertices
	//change index if neccessary
	//add face id 
	//*************

	for(int k=0; k< size; k++){
		vertex* v = &(vertexList.at(k));
		if(!v12repete && sameVertex(*v12, *v)){
			v12 = v;
			indexv12 = k;
			v12repete = true;
			break;
		} 
	}

	for(int k=0; k< size; k++){
		vertex* v = &(vertexList.at(k));
		if(!v23repete && sameVertex(*v23, *v)){
			v23 = v;
			indexv23 = k;
			v23repete = true;
			break;
		} 
	}

	for(int k=0; k< size; k++){
		vertex* v = &(vertexList.at(k));
		if(!v31repete && sameVertex(*v31, *v)){
			v31 = v;
			indexv31 = k;
			v31repete = true;
			break;
		}
	}

	if(!v12repete) {
		vertexList.push_back(*v12);		//index = begin_index	
		indexv12 = vertexList.size()-1;
	}

	if(!v23repete) {
		vertexList.push_back(*v23);		//index = begin_index+1
		indexv23 = vertexList.size()-1;
	}

	if(!v31repete) {
		vertexList.push_back(*v31);		//index = begine_index+2
		indexv31 = vertexList.size()-1;
	}




	//add the first triangle (replace the old one)
	//v1, v12, v31
	faceList.at(meshId).ind1 = old1;
	faceList.at(meshId).ind2 = indexv12;
	faceList.at(meshId).ind3 = indexv31;
	//add faceId for first tri 
	//vertexList.at(old1).addFaceId(meshId);
	vertexList.at(indexv12).addFaceId(meshId);
	vertexList.at(indexv31).addFaceId(meshId);


	//find normal
	vertex* v = getFaceNormal(vertexList.at(old1), vertexList.at(indexv12), vertexList.at(indexv31));	
	faceList.at(meshId).normalX = v->x;
	faceList.at(meshId).normalY = v->y;
	faceList.at(meshId).normalZ = v->z;
	delete(v);

	//the rest 
	for(int i=0; i< 3; i++){	
		mesh *m = new mesh(0, 0, 0);
		vertex* v;

		int triId = faceList.size();
		if(i == 0){
			m->ind1 = old2;			//v2
			m->ind2 = indexv23;		//v23
			m->ind3 = indexv12;		//v12

			v = getFaceNormal(vertexList.at(old2), vertexList.at(indexv23), vertexList.at(indexv12));	
			vertexList.at(old2).addFaceId(triId);
			vertexList.at(indexv23).addFaceId(triId);
			vertexList.at(indexv12).addFaceId(triId);

		}
		else if(i==1){
			m->ind1 = old3;			//v3
			m->ind2 = indexv31;		//v31
			m->ind3 = indexv23;		//v23

			v = getFaceNormal(vertexList.at(old3), vertexList.at(indexv31), vertexList.at(indexv23));	
			vertexList.at(old3).addFaceId(triId);
			vertexList.at(indexv31).addFaceId(triId);
			vertexList.at(indexv23).addFaceId(triId);
		}
		else{
			m->ind1 = indexv12;		//v12
			m->ind2 = indexv23;		//v23
			m->ind3 = indexv31;		//v31

			v = getFaceNormal(vertexList.at(indexv12), vertexList.at(indexv23), vertexList.at(indexv31));	
			vertexList.at(indexv12).addFaceId(triId);
			vertexList.at(indexv23).addFaceId(triId);
			vertexList.at(indexv31).addFaceId(triId);
		}

		m->normalX = v->x;
		m->normalY = v->y;
		m->normalZ = v->z;
		m->colorId = faceList.at(meshId).colorId;

		faceList.push_back(m);
		delete(v);
	}

	//allocate memory for return vertex
	int* newv = (int*) malloc(sizeof(int)*3);
	newv[0]= indexv12;
	newv[1]= indexv23;
	newv[2]= indexv31;
	return newv;
}

void divideHalf(int ind1, int ind2, int newind, int meshid){

	int friendid = findNeighbor(vertexList.at(ind1), vertexList.at(ind2), meshid);
	if(friendid <0){
		printf("error: no neighbor @divideHalf\n");
		return;
	}
	mesh tri = faceList.at(friendid); 


	//check if the reference had changed
	if(twohalf[friendid] != NULL){
		//triangle has been modified
		for(int i=0 ; i< 3; i++){
			int check1, check2;
			if(i==0){
				check1 = tri.ind1;
				check2 = tri.ind2;
			}
			else if(i==1){
				check1 = tri.ind2;
				check2 = tri.ind3;
			}
			else{
				check1 = tri.ind3;
				check2 = tri.ind1;
			}

			if((check1 == ind1 && check2 ==ind2) || (check1 == ind2 && check2 == ind1)){
				//still the same triangle	
				friendid = friendid;
			} else{
				//otherwise, triangle is another half
				friendid = twohalf[friendid];
			}
		}
	}
	
	//remove friendid
	vertexList.at(ind2).removeFaceId(friendid);

	//find another vertex of the tri
	int ind3;
	if(tri.ind1 != ind1 && tri.ind1 != ind2) ind3 = tri.ind1;
	else if(tri.ind2 != ind1 && tri.ind2 != ind2) ind3 = tri.ind2;
	else ind3 = tri.ind3;
	fprintf(mFile, "orginal %d |\t", meshid);

	//divide by half 
	//tri1: ind3, newind, ind2 (old)
	faceList.at(friendid).ind1 = ind3;
	faceList.at(friendid).ind2 = newind;
	faceList.at(friendid).ind3 = ind1;
	//calculate face normal
	vertex fn = getFaceNormal(vertexList.at(ind3), vertexList.at(newind), vertexList.at(ind1));	
	faceList.at(friendid).normalX = fn.x;
	faceList.at(friendid).normalY = fn.y;
	faceList.at(friendid).normalZ = fn.z;
	//colorid 
	faceList.at(friendid).colorId = 4;


	fprintf(mFile,"modify tri %d |\t", friendid);

	//tri2 ind3, ind1, newind
	tri = new mesh(ind3, ind2, newind);
	//add faceId
	int triId= faceList.size();
	vertexList.at(ind3).addFaceId(triId);
	vertexList.at(ind2).addFaceId(triId);
	vertexList.at(newind).addFaceId(triId);
	//assign same face normal
	fn = getFaceNormal(vertexList.at(ind3), vertexList.at(ind2), vertexList.at(newind));	
	tri.normalX = fn.x;
	tri.normalY = fn.y;
	tri.normalZ = fn.z;
	//color
	tri.colorId = 3;

	fprintf(mFile, "new tri %d\n", triId);
	faceList.push_back(tri);

	//newpoint add faceId
	vertexList.at(newind).addFaceId(friendid);
	vertexList.at(newind).addFaceId(triId);
	//update map
	twohalf[friendid] = triId;
}

void indiv_subdivide(){
	int fsize = faceList.size();
	//map consists of two tri id (resulted from spliting)
	for(int id=0; id< fsize; id++){
		if(checkSize(id)) {

			mesh m = faceList.at(id);
			int* newv = subDivideMesh(id, false); //return three new vertices
			
			divideHalf(m.ind1, m.ind2, newv[0], id);
			
			divideHalf(m.ind2, m.ind3, newv[1], id);
			
			divideHalf(m.ind3, m.ind1, newv[2], id);
		}
	}
	fprintf(mFile, "\n<<<<<-------------AFTER SUBDIVIDE------------->>>>\n");
	print_debug();

	reassignFaceId();
}

/****************************************************
				Interpolation
*****************************************************/
float rx;
float ry;

float* convertCoordinate(float transx, float transy, float transz){
	//multiply by inverse matrix
	float radianx = rx*2*3.14159265/360;
	float radiany = ry*2*3.14159265/360;
	
	//transx = 0;
	//transy = 0;
	//transz = 0;

	float vectorx = -transx;
	float vectory = -transy;
	float vectorz = -transz;

	
	//rotate around y axis
	vectorx =  vectorx*cos(radianx) - vectorz*sin(radianx);	//x' = xcos0- zsin0
	vectorz =  transx*sin(radianx) + vectorz*cos(radianx);	//z' = xsin0+ zcos0
	
	/*
	//Note:: mistake 
	//make sure the z value changes and it's updated in the second rotation
	//rotate around x axis
	vectory = transy*cos(radiany) - vectorz*sin(radiany);	// y' = ycos0 - zsin0
	vectorz = transy*sin(radiany) + vectorz*cos(radiany);	// z' = ysin0 + zcos0
	*/

	float v[3] = {vectorx, vectory, vectorz};
	return v;
}

float* convertCoordinate2(float transx, float transy, float transz){
	//multiply by inverse matrix
	float radianx = rx*2*3.14159265/360;
	float radiany = 0;
	
	//transx = 0;
	//transy = 0;
	//transz = 0;

	float vectorx = transx;
	float vectory = transy;
	float vectorz = transz;

	
	//rotate around y axis
	vectorx =  transx*cos(radianx) - vectorz*sin(radianx);	//x' = xcos0- zsin0
	vectorz =  -transx*sin(radianx) + vectorz*cos(radianx);	//z' = xsin0+ zcos0
	
	
	//Note:: mistake 
	//make sure the z value changes and it's updated in the second rotation
	//rotate around x axis
	//vectory = transy*cos(radiany) - vectorz*sin(radiany);	// y' = ycos0 - zsin0
	//vectorz = transy*sin(radiany) + vectorz*cos(radiany);	// z' = ysin0 + zcos0
	

	float v[3] = {vectorx, vectory, vectorz};
	return v;
}
void interpolate(int id, float transx, float transy, float transz, int rotx, int roty){
	rx = rotx;
	ry = roty;
	float* v = convertCoordinate(transx, transy, transz);
	
	float vectorx = v[0];
	float vectory = v[1];
	float vectorz = v[2];

	
	//smart angle?
	if (abs(vectorx) > abs(vectory) && abs(vectorx) > abs(vectorz)) {
		vectory = 0;
		vectorz = 0;
	}
	else if(abs(vectory) > abs(vectorx) && abs(vectory) > abs(vectorz)){
		vectorx = 0;
		vectorz = 0;
	}
	else {
		vectorx = 0;
		vectory = 0;
	}
	fprintf(pFile3, "x= %f, y=%f, z=%f\n", vectorx, vectory, vectorz);

	softselection(id, vectorx/200, vectory/200, vectorz/100);

	//check size to subdivide
	int do_divide = 0;
			
	//internalSubDivide(i);
	//indiv_subdivide();
	//subDivideMesh(i, false);
	int fsize = faceList.size();
	//map consists of two tri id (resulted from spliting)
	for(int id=0; id< fsize; id++){
		if(checkSize(id)) {
			do_divide++;
		}
	}

	if(do_divide > 5){
		fprintf(mFile, "\n<<<<<-------------BEFORE SUBDIVIDE------------->>>>\n");
		print_debug();
		subDivide(false);
		fprintf(mFile, "\n<<<<<-------------AFTER SUBDIVIDE------------->>>>\n");
		print_debug();
		printf("subdivide\n");
	}

	//recalculate face normal and vertex normal
	recalNormal();
	
}

void interpolate(int* list, float transx, float transy, float transz, int rotx, int roty){
	printf("GROUP\n");
	
	rx = rotx;
	ry = roty;
	for(int k=0; k< getsListSize(); k++){
		int id = list[k];

		float* v = convertCoordinate(transx, transy, transz);
	
		float vectorx = v[0];
		float vectory = v[1];
		float vectorz = v[2];

		//move only the selected mesh
		for(int i=0; i<3; i++){

			int index = 0;
			if(i == 0){	
				index = faceList.at(id).ind1;
			}else if(i ==1) {
				index = faceList.at(id).ind2;
			}else{
				index = faceList.at(id).ind3;
			}

			float prevx = vertexList.at(index).x;
			float prevy = vertexList.at(index).y;
			float prevz = vertexList.at(index).z;


			vertexList.at(index).x = vectorx/150+ prevx;
			vertexList.at(index).y = vectory/150+ prevy;
			vertexList.at(index).z = vectorz/150+ prevz;
		}
	}
	
}

bool checkSize(int i){

	//consider every mesh

	mesh m= faceList.at(i);
	float *length = new float[3];

	for(int j=0; j< 3; j++){

		int id1, id2;

		if(j ==0) {
			id1 = m.ind1;
			id2 = m.ind2;
		}
		if(j ==1){
			id1 = m.ind2;
			id2 = m.ind3;
		}
		if(j ==2){
			id1 = m.ind3;
			id2 = m.ind1;
		}

		float x1 = vertexList.at(id1).x;
		float y1 = vertexList.at(id1).y;
		float z1 = vertexList.at(id1).z;

		float x2 = vertexList.at(id2).x;
		float y2 = vertexList.at(id2).y;
		float z2 = vertexList.at(id2).z;

		length[j] = sqrt(pow(x1-x2, 2)+ pow(y1-y2, 2)+ pow(z1-z2, 2)); 
		if(length[j] > maxLen) return true;

	}
	//printf("id: %d, l1: %f, l2: %f, l3: %f\n", i, length[0], length[1], length[2]);

	//find the area of triangles
	float p = (length[0]+ length[1]+ length[2])/2;
	float area = sqrt(p*(p-length[0])*(p-length[1])*(p-length[2]));
	
	if(area > maxArea) {
		printf("id: %d, area: %f\n", i,area);
		int nmesh = faceList.size();
		return true;
	}
	return false;
}



//----------------------Softselection zone------------------------------------------
float s = 0.4;
float e= 2.71828183;

void upEffect(){
	s +=0.1;
	printf("s= %f\n", s);
}
void downEffect(){
	s-=0.1;
	printf("s=%f\n", s);
}
float getEffect(){
	return s;
}
void softselection(int id,float tx,float ty,float tz){

	//function
	//f(x,y) = Ae^ -((x-x0)^2/2sx^2  + (y-y0)^2/2sy^2)
	//A = amplitude
	//x0, y0 = center
	//sx, sy = x and y spreads
	float denom = 2*pow(s,2);
	float *f = getCenterSelection();
	float A = 1;

	//center
	float x0 = f[0];	
	float y0 = f[1];
	float z0 = f[2];

	float exp, coef= 1;
	for(int i=0; i< vertexList.size(); i++){
		vertex v = vertexList.at(i);

		exp = (pow(v.x-x0, 2) + pow(v.y-y0, 2) + pow(v.z-z0, 2))/denom;
		if(exp < 0.5){
			coef = A;
		}
			coef = A*pow(e, -exp);

		//printf("v: %d \t| coef = %f\n", i, coef);

		//translate the point
		vertexList.at(i).x = vertexList.at(i).x + tx*coef;	//x
		vertexList.at(i).y = vertexList.at(i).y + ty*coef;	//y
		vertexList.at(i).z = vertexList.at(i).z + tz*coef;	//z
	}
}

void setEffectColor(vertex v){
	//calculate distance 
	float *f = getCenterSelection();
	float denom = 2*pow(s,2);

	//center
	float x0 = f[0];	
	float y0 = f[1];
	float z0 = f[2];

	float exp = (pow(v.x-x0, 2) + pow(v.y-y0, 2) + pow(v.z-z0, 2))/denom;
	float coef = pow(e, -exp);

	//shade effect 
	//red->orange->yellow->green->blue->black
	//1------>0

	float r =0, g=0, b=0; 
	if(coef > 0.5 && coef <1){
		r = (coef-0.5)*2;
	}
	if(coef <0.8 && coef >0.3){
		g = (coef-0.3)*2;
	}
	if(coef >0 && coef < 0.5){
		b = (coef*2);
	}
	
	glColor3f(r, g, b);
}
/*------------------------Extrude---------------------------------------
void extrude(int id,float tx,float ty,float tz, float nx, float ny, float nz){
	//find the center 
	int ind1 = faceList.at(id).ind1;
	int ind2 = faceList.at(id).ind2;
	int ind3 = faceList.at(id).ind3;
	//3 vertex that form base of the extrusion
	vertex v1 = vertexList.at(ind1);
	vertex v2 = vertexList.at(ind2);
	vertex v3 = vertexList.at(ind3);


	//3 vertex that will be the top 
	vertex v11 = new vertex(v1.x+ nx*ty, v1.y+ ny*ty, v1.z +nz*ty);
	vertex v22 = new vertex(v2.x+ nx*ty, v2.y+ ny*ty, v2.z +nz*ty);
	vertex v33 = new vertex(v3.x+ nx*ty, v3.y+ ny*ty, v3.z +nz*ty);

	//contruct rectangle 
	glBegin(GL_QUADS);
	//1 2 22 11
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v22.x, v22.y, v22.z);
	glVertex3f(v11.x, v11.y, v11.z);

	//2 3 33 22
	glVertex3f(v2.x, v2.y, v2.z);
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v33.x, v33.y, v33.z);
	glVertex3f(v22.x, v22.y, v22.z);


	//3 1 11 33
	glVertex3f(v3.x, v3.y, v3.z);
	glVertex3f(v1.x, v1.y, v1.z);
	glVertex3f(v11.x, v11.y, v11.z);
	glVertex3f(v33.x, v33.y, v33.z);
	glEnd();

}
*/

//------------------------Bounding sphere---------------------------------
#define MIN_F -1000000;
#define MAX_F 1000000;

vertex *center;
float radius =0;

void findBoundingSphere(){

	//find the bound
	float top = MIN_F; 
	float right = MIN_F;
	float front= MIN_F;
	float bottom = MAX_F;
	float left = MAX_F;
	float back = MAX_F;

	int numVertex = getVertexListSize();

	for(int i=0; i< numVertex; i++){
		float x = vertexList.at(i).x;
		float y = vertexList.at(i).y; 
		float z = vertexList.at(i).z;

		//find the bounding box
		if(x < left) left = x;
		else if(x > right) right = x;
		
		if(z < back) back = z;
		else if(z > front) front = z;
		
		if(y <bottom) bottom = y;
		else if(y >top) top = y;

		//find the model's center from the bounding box
		center = new vertex((left+right)/2, (bottom+top)/2, (back+front)/2);
	}
	printf("center: %f, %f, %f\n", center->x, center->y, center->z);	


	//calculate the distance from the center
	//and find the radius 

	for(int i =0; i < numVertex; i++){
		float x = vertexList.at(i).x;
		float y = vertexList.at(i).y; 
		float z = vertexList.at(i).z;

		float dis = sqrt(pow(x-center->x, 2)+pow(y-center->y, 2)+pow(z-center->z, 2));
		if(dis >radius) radius = dis;
	}
	printf("radius: %f\n", radius);

	float result[4] = {center->x, center->y, center->z, radius}; 
} 

vertex getCenter(){
	return (*center);
}

float getDiam(){
	return radius*2;
}


//-------------------------gizmo/center-----------------------------------
void setMeshSelection(int k){
	selectedMesh = k;
}


float* getCenterSelection(){
	//find the center 
	int ind1 = faceList.at(selectedMesh).ind1;
	int ind2 = faceList.at(selectedMesh).ind2;
	int ind3 = faceList.at(selectedMesh).ind3;
	//center
	float *f = (float *) malloc(sizeof(float));
	f[0] = (vertexList.at(ind1).x + vertexList.at(ind2).x + vertexList.at(ind3).x )/3;	
	f[1] = (vertexList.at(ind1).y + vertexList.at(ind2).y + vertexList.at(ind3).y )/3; 
	f[2] = (vertexList.at(ind1).z + vertexList.at(ind2).z + vertexList.at(ind3).z )/3; 

	return f; 
	
}

/*******************************************************************
								UNDO
********************************************************************/
//copy to ex-data
void copy_vmmodel(){
	
	//clear the old copy
	//exvertexList.clear();

	//next index 
	if(numMod == UNDORANGE-1){
		numMod = 0;
	}else{
		numMod++;
	}
	countMod = min(countMod+1, UNDORANGE); 

	exvertexList[numMod].clear();
	int nv = vertexList.size();
	for(int i=0 ; i< nv; i++){
		vertex temp = vertexList.at(i);
		vertex* v = new vertex(temp.x, temp.y, temp.z);
		v->vnormx = temp.vnormx;
		v->vnormy = temp.vnormy;
		v->vnormz = temp.vnormz;

		for(int f=0; f<temp.nface; f++){
			v->addFaceId(temp.faceId[f]);
		}
		//fprintf(mFile,"%d\t %f\t %f\t %f\n", i, v->x, v->y, v->z);
		exvertexList[numMod].push_back(*v);
	}
	printf("copy model: %d\n", numMod);

	//fprintf(mFile,"---------------------------------------------------\n");
}

//restore ex-data to the data structure
void undo_vmmodel(){
	
	if(countMod >1) {

		int id = numMod-1;
		if(id < 0) id = UNDORANGE-1;

		vertexList = exvertexList[id];
		printf("undo to model %d: count model = %d\n", id, countMod);

		countMod-=1;
		numMod = id;
	}
}