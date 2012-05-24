

void import_vm();
void export_vm();

void drawMesh(int i);
void drawMesh(int i, bool shade);

//subdivision
void subDivide(bool do_norm);
int* subDivideMesh(int i, bool do_norm);
void indiv_subdivide();



//Normal
void recalNormal();
vertex* getFaceNormal(vertex v1, vertex v2, vertex v3);
vertex* normalizeV(vertex* norm);
void calVertexNormal();

void interpolate(int id, float transx, float transy, float transz, int rotx, int roty);
void interpolate(int* list, float transx, float transy, float transz, int rotx, int roty);

bool checkSize(int i);

void upEffect();
void downEffect();
float getEffect();
void setColorPaint(int id);
void setEffectColor(vertex v);

void upBrush();
void downBrush();
void paintMesh(int mid, int cid);

void copy_vmmodel();
void undo_vmmodel();

void softselection(int id,float tx,float ty,float tz);
void extrude(int id,float tx,float ty,float tz, float nx, float ny, float nz);
float* convertCoordinate(float transx, float transy, float transz);
float* convertCoordinate2(float transx, float transy, float transz);

//miniball
void findBoundingSphere();
vertex getCenter();
float getDiam();
void setMeshSelection(int k);
void drawGizmo();
float* getCenterSelection();


//helper
int getFaceListSize();
int getVertexListSize();
void print_debug();