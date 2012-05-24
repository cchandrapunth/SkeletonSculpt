


//callback when picking occurs
void picked(GLuint name, int sw);
void processPick(float cursorX, float cursorY);

static int pickMe = -1;	

int getSelection();
void setNullSelection();	//show no mesh response when hand release

//selection list
void clearSelectionList();
void store_selection(int i);
int sListContain(int i);
void printsList();

int* getsList();
int getsListSize();