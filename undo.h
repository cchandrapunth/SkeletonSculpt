


//action encapsulate all neccesry parameter
typedef struct model_st{
	int nPoint;
	int nPoly;
	float* point_st;
	int* poly_st;
}model_state;

typedef struct matrix_state{
	int rollX;
	int rollY;
	int zoomZ;
}matrix_state;


//getter 
int getMatX();
int getMatY();
float getMatZ();

int restoreMatX();
int restoreMatY();
float restoreMatZ();

//storage
void addMatrix(int x, int y, float z);
void pushMatrix();


void undo_action();